//
// Created by badbouille on 06/05/2020.
//

#include <iostream>
#include "Core.h"
#include "FuseFileSystem.h"

GostCrypt::DiskEncryptionAlgorithmList GostCrypt::Core::GetEncryptionAlgorithms()
{
    return GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
}

GostCrypt::DiskEncryptionAlgorithmList GostCrypt::Core::GetDerivationFunctions()
{
    // TODO
    return GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
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

    interface->start_fuse(p->mountPoint.c_str(), volume);

}

void GostCrypt::Core::umount(std::string mountPoint)
{
    std::fstream gostinfo;
    const std::string umountcmd = "umount";

    /* Checking if mountpoint present */
    if (mountPoint.empty()) {
        throw INVALIDPARAMETEREXCEPTION("empty mountpoint");
    }

    /* removing trailing slash to add INFO_FILE constant */
    if (mountPoint.back() == '/') {
        mountPoint.pop_back();
    }

    /* opening special file to check presence of volume */
    gostinfo.open(mountPoint + INFO_FILE, std::ios_base::in );

    /* Checking */
    if (!gostinfo.is_open()) {
        throw MOUNTPOINTNOTFOUNDEXCEPTION(mountPoint);
    }

    /* Closing read-only file */
    gostinfo.close();

    /* Opening special file */
    /* Append mode is the only mode where the 'write' callback is called in fuse.
     * Other modes just recreate the file, etc
     */
    gostinfo.open(mountPoint + INFO_FILE, std::ios_base::app );
    if (!gostinfo.is_open()) {
        throw GOSTCRYPTEXCEPTION("Mountpoint found but cannot be accessed: " + mountPoint);
    }

    /* writing command to it to close it */
    gostinfo.write(umountcmd.c_str(), umountcmd.length());

    /* Closing file */
    gostinfo.close();

    /* reopening file to check for success */
    gostinfo.open(mountPoint + INFO_FILE, std::ios_base::in );

    /* Checking */
    if (gostinfo.is_open()) {
        gostinfo.close();
        throw UMOUNTFAILEDEXCEPTION(mountPoint);
    }

}

void GostCrypt::Core::create(GostCrypt::Core::CreateParams_t *p)
{
    // TODO check input

    // Pointer on final Volume object
    GostCrypt::Volume *volume = nullptr;
    GostCrypt::FuseFileSystem *interface = nullptr;
    bool volumeOpened = false;

    // finding interface requested
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
    try {
        volume->create(p->volumePath, p->dataSize, p->algorithmID, p->sectorSize, p->password);
    } catch (GostCryptException &e) {
        delete volume;
        throw;
    }

    // filesystem init
    interface->create(volume);

    // closing volume
    volume->close();

    delete volume;
    delete interface;

    // post-creation mount
    mount(&p->afterCreationMount);
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
