/**
 * @file DiskEncryptionAlgorithm.cpp
 * @author badbouille
 * @date 06/08/2019
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "../ince/DiskEncryptionAlgorithm.h"
#include "GostCryptException.h"
#include "../ince/DiskEncryptionAlgorithmECB.h"
#include "../ince/BlockCipherXOR.h"

GostCrypt::DiskEncryptionAlgorithmList GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms()
{
    GostCrypt::DiskEncryptionAlgorithmList l;

    // ECB - XOR (most simple algorithm for test purposes)
    l.push_back(new DiskEncryptionAlgorithmECB<BlockCipherXOR>());

    return l;
}

void GostCrypt::DiskEncryptionAlgorithm::Decrypt(GostCrypt::SecureBufferPtr &data,
                                                 size_t sectorIndex,
                                                 size_t sectorCount,
                                                 size_t sectorSize) const
{
    size_t i = 0;
    GostCrypt::SecureBufferPtr currentsector;

    // Checking input size
    if (data.size() != sectorCount*sectorSize) {
        throw INVALIDPARAMETEREXCEPTION("Cannot encrypt half sector.");
    }

    // Encrypting every sector separately
    for(;i < sectorCount; i++){
        data.getRange(currentsector, i*sectorSize, sectorSize);
        this->Decrypt(currentsector, sectorIndex + i);
    }

}

void GostCrypt::DiskEncryptionAlgorithm::Encrypt(GostCrypt::SecureBufferPtr &data,
                                                 size_t sectorIndex,
                                                 size_t sectorCount,
                                                 size_t sectorSize) const
{
    size_t i = 0;
    GostCrypt::SecureBufferPtr currentsector;

    // Checking input size
    if (data.size() != sectorCount*sectorSize) {
        throw INVALIDPARAMETEREXCEPTION("Cannot decrypt half sector.");
    }

    // Encrypting every sector separately
    for(;i < sectorCount; i++){
        data.getRange(currentsector, i*sectorSize, sectorSize);
        this->Encrypt(currentsector, sectorIndex + i);
    }
}
