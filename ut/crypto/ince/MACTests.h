/**
 * @file MACTests.h
 * @author badbouille
 * @date 17/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _MACTESTS_H
#define _MACTESTS_H

#include "MAC.h"
#include "Buffer.h"

namespace GostCrypt
{

    typedef struct MACTestData256_s
    {
        uint8_t key[32];
        uint8_t input[32];
        uint8_t expected[16];
        uint32_t keysize;
        uint32_t inputsize;
    } MACTestData256;

}

#endif //_MACTESTS_H
