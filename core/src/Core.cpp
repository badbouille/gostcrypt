//
// Created by badbouille on 06/05/2020.
//

#include <iostream>
#include <wait.h>
#include "Core.h"
#include "FuseFileSystem.h"

GostCrypt::Core::CallBackFunction_t GostCrypt::Core::callback_function = nullptr;
float GostCrypt::Core::callback_superbound_high = 1.0f;
float GostCrypt::Core::callback_superbound_low = 0.0f;
bool GostCrypt::Core::callback_enable = true;

GostCrypt::DiskEncryptionAlgorithmList GostCrypt::Core::GetEncryptionAlgorithms()
{
    return GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
}

GostCrypt::KDFList GostCrypt::Core::GetDerivationFunctions()
{
    return GostCrypt::KDF::GetAvailableHashes();
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
    // TODO check input

    // Pointer on final Volume object
    GostCrypt::Volume *volume = nullptr;
    GostCrypt::FuseFileSystem *interface = nullptr;
    bool volumeOpened = false;
    float i = 0.0;

    callback("Finding interface", 0.05);

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

        callback("Trying to open the volume", 0.05f + ((i+1)/volumeList.size())*0.75f);

        // trying to open volume
        if (volumeIterator->open(p->volumePath, p->password)) {
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
    callback("Creating the FUSE mountpoint", 0.85);

    interface->start_fuse(p->mountPoint.c_str(), volume);

    callback("Done mounting", 1.0);
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

    /* Calling fusermount to kill the volume gracefully */
    /* Forking to call fusermount */
    pid_t pid = fork();
    int status;

    if ( pid == 0 ) {
        // calling fusermount
        static char argvT[][256] = { "/bin/fusermount", "-uz", "" };
        static char *argv[] = { argvT[0], argvT[1], argvT[2], nullptr };

        if (strlen(mountPoint.c_str()) < 256) {
            strcpy(argv[2], mountPoint.c_str());
        }

        /* Executing fusermount program */
        execv(argv[0], argv);

        /* If fusermount fails, exit with error code */
        exit(127);
    }

    /* Waiting for child to mount the raw volume */
    if (waitpid(pid, &status, 0) == -1 ) {
        throw GOSTCRYPTEXCEPTION("waitpid failed.");
    }

    /* Checking return value */
    if ( WIFEXITED(status) ) {
        if (WEXITSTATUS(status) != 0) {
            throw GOSTCRYPTEXCEPTION("fusermount operation failed.");
        }
    } else {
        throw GOSTCRYPTEXCEPTION("Child not exited.");
    }

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
    callback("Finding interface", 0.02);
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
    callback("Finding volume type", 0.04);
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
    callback("Creating volume", 0.06);
    try {
        callback_superbound_high = 0.80f;
        callback_superbound_low = 0.06f;
        volume->setCallBack(super_callback);
        volume->create(p->volumePath, p->dataSize, p->algorithmID, p->keyDerivationFunctionID, p->sectorSize, p->password);
    } catch (GostCryptException &e) {
        delete volume;
        throw;
    }

    // filesystem init

    // closing volume
    callback("Closing created volume", 0.81f);
    volume->close();
    delete volume;

    // mounting using the 'none' filesystem (loop device to create the fs)
    p->afterCreationMount.fileSystemID = "none";

    // Mounting raw volume
    callback("Mounting raw volume", 0.85f);
    disableCallback();
    mount(&p->afterCreationMount);
    enableCallback();

    // creating filesystem in raw file
    callback("Creating filesystem", 0.90f);
    interface->create(p->afterCreationMount.mountPoint + "/volume");

    // unmounting volume
    callback("Closing and reopening final volume", 0.95f);
    disableCallback();
    umount(p->afterCreationMount.mountPoint);
    enableCallback();

    delete interface;

    // restoring real filesystem asked by the user
    p->afterCreationMount.fileSystemID = realfs;

    // post-creation mount (with real filesystem this time)
    disableCallback();
    mount(&p->afterCreationMount);
    enableCallback();
    callback("Done creating", 1.0);
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
            // extract info from folder
            VolumeInfoFile_t infos;
            std::ifstream gostinfo(folder + INFO_FILE);

            if(!gostinfo.is_open()) {
                throw FILENOTFOUNDEXCEPTION(folder + INFO_FILE);
            }

            gostinfo.read((char *)&infos, sizeof(infos));

            if(gostinfo.fail()) { // TODO consider a better exception
                throw FILENOTFOUNDEXCEPTION(folder + INFO_FILE);
            }

            // creating entry
            VolumeInfo vi;
            vi.file = infos.file;
            vi.mountPoint = infos.mountPoint;
            vi.algorithmID = infos.algorithmID;
            vi.keyDerivationFunctionID = infos.keyDerivationFunctionID;
            vi.volumeTypeID = infos.volumeTypeID;
            vi.dataSize = be64toh(infos.dataSize);

            // adding entry
            volumes.push_back(vi);

        }
    }

    return volumes;
}
