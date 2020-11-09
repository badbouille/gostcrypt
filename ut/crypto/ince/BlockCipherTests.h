//
// Created by badbouille on 09/11/2020.
//

#ifndef _BLOCKCIPHERTESTS_H
#define _BLOCKCIPHERTESTS_H

#include "BlockCipher.h"
#include "Buffer.h"

namespace GostCrypt
{

    typedef struct BlockCipherTestData128_s
    {
        uint8_t key[16];
        uint8_t input[16];
        uint8_t expected[16];
    } BlockCipherTestData128;

}

#endif //_BLOCKCIPHERTESTS_H
