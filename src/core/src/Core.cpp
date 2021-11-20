/**
 * @file Core.cpp
 * @author badbouille
 * @date 06/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <iostream>
#include <wait.h>
#include <ContainerFile.h>
#include "Core.h"
#include "FuseFileSystem.h"
#include "RequestSerializer.h"

/* Defining static variables */
GostCrypt::Progress GostCrypt::Core::progress;

const char * GostCrypt::Core::g_prog_path = nullptr;

int GostCrypt::Core::main_api_handler(int argc, char **argv)
{
    int ret = 8;

    if (argv == nullptr) {
        return 1;
    }

    /* Updating program location */
    g_prog_path = argv[0];

    if (argv[0] == nullptr || std::string(argv[0]) != "api") {
        return 0;
    }

    if (argc < 3 || argc > 4) exit(ret);

    /* Binding progress shm */
    SharedWindow<Progress::ProgressInfo_t> shm_send(getpid());
    progress.setSharedMemory(&shm_send);

    /* Only mount can be called for now */
    if (std::string(argv[1]) == "mount") {
        // calling mount command
        GostCrypt::SecureBuffer pass(64); // TODO: why 64
        GostCrypt::Core::MountParams_t p;

        pass.getRange(p.password, 0, pass.size());
        RequestSerializer_api_DeserializeMount(&p, argv[2]);

        // erasing all sensible data
        GostCrypt::SecureBufferPtr dataptr((uint8_t *)argv[2], strlen(argv[2]));
        dataptr.erase();

        try {
            /* Calling directmount, the true mount function */
            GostCrypt::Core::directmount(&p);
            ret = 0;
        } catch (GostCrypt::GostCryptException &e) {
            ret = 1;
            progress.reportException(e);
        }
        pass.erase();
    }

    exit(ret);
}

GostCrypt::DiskEncryptionAlgorithmList GostCrypt::Core::GetEncryptionAlgorithms()
{
    return GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
}

GostCrypt::KDFList GostCrypt::Core::GetDerivationFunctions()
{
    return GostCrypt::KDF::GetAvailableKDFs();
}

GostCrypt::VolumeList GostCrypt::Core::GetVolumeTypes()
{
    return GostCrypt::Volume::GetAvailableVolumeTypes();
}

GostCrypt::FuseFileSystemList GostCrypt::Core::GetFileSystems()
{
    return GostCrypt::FuseFileSystem::GetFileSystems();
}

void GostCrypt::Core::mount(GostCrypt::Core::MountParams_t *p)
{
#ifdef DEBUG
    return GostCrypt::Core::directmount(p);
#else
    pid_t pid;
    static char argvT[][256] = { "api", "mount", "", "" };
    static char *argv[] = { argvT[0], argvT[1], nullptr, nullptr, nullptr };
    uint32_t len;
    int ret = 0, status;

    pid = vfork();

    if (pid == 0) {
        RequestSerializer_api_SerializeMount(p, &argv[2], &len);
        execv(g_prog_path, argv);
        exit(127);
    }

    /* Binding progress shm to child's pid */
    SharedWindow<Progress::ProgressInfo_t> shm_listen(pid);

    /* While child is running */
    while((ret = waitpid(pid, &status, WNOHANG)) == 0) {
        /* Getting notifications */
        progress.listenSharedMemory(&shm_listen);
    }

    /* Getting final unread notifications (maybe an exception?) */
    progress.listenSharedMemory(&shm_listen);

    /* Child is dead, analysing */
    if ((ret == -1) || (WIFEXITED(status) == false)) {
        throw GOSTCRYPTEXCEPTION("forked process failed to execute");
    } else {
        if (WEXITSTATUS(status) == 0) {
            progress.report("Done mounting", 1.0);
        }
    }

#endif
}

void GostCrypt::Core::directmount(GostCrypt::Core::MountParams_t *p)
{
    // TODO check input

    // Pointer on final Volume object
    GostCrypt::Volume *volume = nullptr;
    GostCrypt::FuseFileSystem *interface = nullptr;
    bool volumeOpened = false;
    float i = 0.0;

    progress.report("Finding interface", 0.05);

    // finding interface requested
    FuseFileSystemList fileSystemList = GostCrypt::FuseFileSystem::GetFileSystems();
    for (auto & fileSystemIterator : fileSystemList) {
        // checking name
        if (fileSystemIterator->getID() == p->fileSystemID) {
            interface = fileSystemIterator;
        }
    }

    // cleaning unused structures
    for (auto & fileSystemIterator : fileSystemList) {
        if (interface != fileSystemIterator) {
            delete fileSystemIterator;
        }
    }

    if (interface == nullptr) {
        throw INVALIDPARAMETEREXCEPTION("Could not find the requested interface (filesystem).");
    }

    // TODO : try catch

    // trying all different volume types on input
    VolumeList volumeList = GostCrypt::Volume::GetAvailableVolumeTypes();
    for (auto & volumeIterator : volumeList) {

        progress.report("Trying to open the volume", 0.05f + ((i+1)/volumeList.size())*0.75f);

        // trying to open volume
        if (volumeIterator->open(new ContainerFile(p->volumePath), p->password)) {
            // Worked! Header was successfully decrypted
            volume = volumeIterator;
            volumeOpened = true;
            break;
        }

        // didn't work, let's try to open it with the next algorithm
    }

    // cleaning unused structures
    for (auto & volumeIterator : volumeList) {
        if (volume != volumeIterator) {
            delete volumeIterator;
        }
    }

    if (!volumeOpened) {
        // Password error OR volume corrupted
        throw VOLUMEPASSWORDEXCEPTION();
    }

    // Volume has been opened successfully
    // Starting fuse
    progress.report("Creating the FUSE mountpoint", 0.85);

    interface->start_fuse(p->mountPoint.c_str(), volume);
}

void GostCrypt::Core::umount(std::string mountPoint)
{
    /* Checking if mountpoint present */
    if (mountPoint.empty()) {
        throw INVALIDPARAMETEREXCEPTION("empty mountpoint");
    }

    /* removing trailing slash to add INFO_FILE constant */
    if (mountPoint.back() == '/') {
        mountPoint.pop_back();
    }

    /* Reading volume info with the .exit suffix will read it and ask for closure of the volume */
    readVolumeInfo(mountPoint + INFO_FILE_EXIT);

}

void GostCrypt::Core::umountAll()
{
    VolumeInfoList vl = Core::list();

    for (VolumeInfo& v : vl) {
        Core::umount(v.mountPoint);
    }
}

void GostCrypt::Core::create(GostCrypt::Core::CreateParams_t *p)
{
    // TODO check input

    // Pointer on final Volume object
    GostCrypt::Volume *volume = nullptr;
    GostCrypt::FuseFileSystem *interface = nullptr;
    bool volumeOpened = false;

    std::string realfs = p->afterCreationMount.fileSystemID;

    // finding interface requested
    progress.report("Finding interface", 0.02);
    FuseFileSystemList fileSystemList = GostCrypt::FuseFileSystem::GetFileSystems();
    for (auto & fileSystemIterator : fileSystemList) {
        // checking name
        if (fileSystemIterator->getID() == p->afterCreationMount.fileSystemID) {
            interface = fileSystemIterator;
        }
    }

    // cleaning unused structures
    for (auto & fileSystemIterator : fileSystemList) {
        if (interface != fileSystemIterator) {
            delete fileSystemIterator;
        }
    }

    if (interface == nullptr) {
        throw INVALIDPARAMETEREXCEPTION("Could not find the requested interface (filesystem).");
    }

    // finding volume type requested
    progress.report("Finding volume type", 0.04);
    VolumeList volumeList = GostCrypt::Volume::GetAvailableVolumeTypes();
    for (auto & volumeIterator : volumeList) {
        // checking name
        if (volumeIterator->GetID() == p->volumeTypeID) {
            volume = volumeIterator;
        }
    }

    // cleaning unused structures
    for (auto & volumeIterator : volumeList) {
        if (volume != volumeIterator) {
            delete volumeIterator;
        }
    }

    if (volume == nullptr) {
        throw INVALIDPARAMETEREXCEPTION("Could not find the requested volume type.");
    }

    // volume creation
    progress.report("Creating volume", 0.06);
    try {
        progress.setChildBounds(0.06, 0.80);
        //volume->setCallBack(super_callback);
        volume->create(new ContainerFile(p->volumePath), p->dataSize, p->algorithmID, p->keyDerivationFunctionID, p->sectorSize, p->password);
    } catch (GostCryptException &e) {
        delete volume;
        throw;
    }

    // filesystem init

    // creating filesystem in raw file
    progress.report("Creating filesystem", 0.90f);
    interface->create(volume);

    delete interface;

    // post-creation mount

    // reporting here is tough and needs a second object
    progress.report("Mounting volume at target", 0.90f);
    Progress tmp;
    tmp.setCallBack(progress.getCallBack(), progress.getCallBackCtx());
    tmp.setChildBounds(0.90, 0.99);
    progress.removeCallBack();
    progress.setMaster(&tmp);

    // actually mounting
    mount(&p->afterCreationMount);

    // resetting progress to normal state
    progress.setCallBack(tmp.getCallBack(), tmp.getCallBackCtx());
    progress.removeMaster();

    progress.report("Done creating", 1.0);
}

GostCrypt::Core::VolumeInfoList GostCrypt::Core::list()
{
    std::string sourcefile = "/proc/mounts";
    std::ifstream mounts(sourcefile);
    std::string line;

    size_t first_space = 0;
    std::string mountType;
    std::string folder;

    GostCrypt::Core::VolumeInfoList volumes;

    if(!mounts.is_open()) {
        throw FILENOTFOUNDEXCEPTION(sourcefile);
    }

    while (std::getline(mounts, line)) {
        first_space = line.find(' ');
        mountType = line.substr(0, first_space);
        folder = line.substr(first_space+1, line.find(' ', first_space+1) - first_space - 1);

        // TODO add constant
        if (mountType == "gostcrypt") {

            // reading infos and adding all entries
            try {
                volumes.push_back(readVolumeInfo(folder + INFO_FILE));
            } catch (FileNotFoundException &e) {
                VolumeInfo vi;
                vi.mountPoint = folder;
                volumes.push_back(vi);
            }
        }
    }

    return volumes;
}

GostCrypt::Core::VolumeInfo GostCrypt::Core::readVolumeInfo(std::string gostinfofile)
{
    // extract info from folder
    VolumeInfoFile_t infos;
    std::ifstream gostinfo(gostinfofile);

    if(!gostinfo.is_open()) {
        throw FILENOTFOUNDEXCEPTION(gostinfofile);
    }

    gostinfo.read((char *)&infos, sizeof(infos));

    if(gostinfo.fail()) { // TODO consider a better exception
        throw FILENOTFOUNDEXCEPTION(gostinfofile);
    }

    // creating entry
    VolumeInfo vi;
    vi.file = infos.file;
    vi.mountPoint = infos.mountPoint;
    vi.algorithmID = infos.algorithmID;
    vi.keyDerivationFunctionID = infos.keyDerivationFunctionID;
    vi.volumeTypeID = infos.volumeTypeID;
    vi.dataSize = be64toh(infos.dataSize);

    gostinfo.close();

    return vi;
}
