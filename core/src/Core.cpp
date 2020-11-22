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

    interface->setTarget(volume);
    interface->start_fuse(p->mountPoint.c_str());

}

void GostCrypt::Core::umount(std::string mountPoint)
{
    // TODO
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
    interface->setTarget(volume);
    interface->create();

    // closing volume
    volume->close();

    delete volume;
    delete interface;

    // post-creation mount
    mount(&p->afterCreationMount);
}
