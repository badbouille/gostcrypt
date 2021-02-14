/**
 * @file BlockCipherTests.h
 * @author badbouille
 * @date 09/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

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
