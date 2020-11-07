//
// Created by badbouille on 06/05/2020.
//

#include "../ince/Core.h"
#include "../../fuse/inci/FuseService.h"

DiskEncryptionAlgorithmList GostCrypt::Core::GetEncryptionAlgorithms()
{
    return GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
}

DiskEncryptionAlgorithmList GostCrypt::Core::GetDerivationFunctions()
{
    // TODO
    return GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
}

VolumeList GostCrypt::Core::GetVolumeTypes()
{
    return GostCrypt::Volume::GetAvailableVolumeTypes();
}

DFuseFileSystemList GostCrypt::Core::GetFileSystems()
{
    return GostCrypt::FuseFileSystem::GetFileSystems();
}

void GostCrypt::Core::mount(GostCrypt::Core::MountParams_t *p)
{
    // TODO check input

    // Pointer on final Volume object
    GostCrypt::Volume *volume = nullptr;
    bool volumeOpened = false;

    // TODO : try catch

    // trying all different volume types on input
    VolumeList volumeList = GostCrypt::Volume::GetAvailableVolumeTypes();
    for (auto & volumeIterator : volumeList) {
        // trying to open volume
        if (volumeIterator->open(p->volumePath, p->password)) {
            // Worked! Header was successfully decrypted
            // TODO : must make a copy of this shite
            volume = volumeIterator;
            volumeOpened = true;
            break;
        }

        // didn't work, let's try to open it with the next algorithm
    }

    if (!volumeOpened) {
        // Password error OR volume corrupted
        throw VOLUMEPASSWORDEXCEPTION();
    }

    // Volume has been opened successfully
    // Starting fuse

    start_fuse(p->mountPoint, )

}

void GostCrypt::Core::umount(std::string mountPoint)
{
    // TODO
}

void GostCrypt::Core::create(GostCrypt::Core::CreateParams_t *p)
{
    // TODO
}
