//
// Created by badbouille on 06/08/2019.
//

#include "../ince/DiskEncryptionAlgorithmECB.h"

template<class T>
void GostCrypt::DiskEncryptionAlgorithmECB<T>::Encrypt(SecureBufferPtr &data, size_t sectorIndex) const
{
    const size_t blockSize = bc->GetBlockSize();
    const size_t maxSize = data.size();
    size_t i = 0;

    SecureBufferPtr block;
    SecureBufferPtr finalBlock;

    assertInit();

    // Ignoring sector index (ECB)
    (void)sectorIndex;

    // encrypting what we can
    for(;i + blockSize <= maxSize; i += blockSize){
        data.getRange(block, i, blockSize);
        bc->Encrypt(block);
    }

    if (i == maxSize) {
        // everything is encrypted
        // returning
        return;
    }

    // some data has not been encrypted at the end
    // using ciphertext stealing (CS3)

    // block is the last complete block
    // i is now the index of the last uncomplete block

    // getting the last block (of size < blockSize)
    data.getRange(finalBlock, i, maxSize - i);

    // swapping encrypted and last uncomplete block
    // block is now filled with the last uncomplete block + ciphertext of last complete block at the end
    // this is the ciphertext stealing part
    // we can also assume that the entire buffer was aligned, so the previous complete block is also aligned
    block.swap(finalBlock);

    // final encryption part
    bc->Encrypt(block);
}

template<class T>
void GostCrypt::DiskEncryptionAlgorithmECB<T>::Decrypt(SecureBufferPtr &data, size_t sectorIndex) const
{
    const size_t blockSize = bc->GetBlockSize();
    const size_t maxSize = data.size();
    size_t i = 0;

    SecureBufferPtr block;
    SecureBufferPtr finalBlock;

    assertInit();

    // Ignoring sector index (ECB)
    (void)sectorIndex;

    // decrypting what we can
    for(;i + blockSize <= maxSize; i += blockSize){
        data.getRange(block, i, blockSize);
        bc->Decrypt(block);
    }

    if (i == maxSize) {
        // everything is decrypted
        // returning
        return;
    }

    // getting the last block (of size < blockSize)
    data.getRange(finalBlock, i, maxSize - i);

    // swapping partially decrypted block with encrypted block at the end
    block.swap(finalBlock);

    // final decryption part
    bc->Decrypt(block);
}