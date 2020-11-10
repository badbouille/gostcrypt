//
// Created by badbouille on 09/11/2020.
//

#ifndef _DISKENCRYPTIONALGORITHMTESTS_H
#define _DISKENCRYPTIONALGORITHMTESTS_H

#include "Buffer.h"

namespace GostCrypt
{

    // 128+8bits is a great value to test out the ciphertext stealing mechanism
    typedef struct DiskEncryptionAlgorithmTestData136_s
    {
        uint8_t input[17];
        uint8_t expected[17];
        size_t sectornum;
        size_t sectorsize;
        size_t sectoroffset;
    } DiskEncryptionAlgorithmTestData136;

    // 56 bits is usually not enough to encrypt
    typedef struct DiskEncryptionAlgorithmTestData56_s
    {
        uint8_t input[7];
        uint8_t expected[7];
        size_t sectornum;
        size_t sectorsize;
        size_t sectoroffset;
    } DiskEncryptionAlgorithmTestData56;

}

#endif // _DISKENCRYPTIONALGORITHMTESTS_H
