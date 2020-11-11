//
// Created by badbouille on 09/11/2020.
//

#ifndef _DISKENCRYPTIONALGORITHMTESTS_H
#define _DISKENCRYPTIONALGORITHMTESTS_H

#include "Buffer.h"

namespace GostCrypt
{

    // huge vector that can store smaller values
    typedef struct DiskEncryptionAlgorithmTestData520_s
    {
        uint8_t key[16]; // ecryption modes are tested with a standard algorithm (128bit key)
        uint8_t input[65];
        uint8_t expected[65];
        size_t inputsize;
        size_t sectornum;
        size_t sectorsize;
        size_t sectoroffset;
    } DiskEncryptionAlgorithmTestData520;

}

#endif // _DISKENCRYPTIONALGORITHMTESTS_H
