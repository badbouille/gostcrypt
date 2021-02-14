/**
 * @file KDFTests.h
 * @author badbouille
 * @date 17/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _KDFTESTS_H
#define _KDFTESTS_H

#include "KDF.h"
#include "Buffer.h"

namespace GostCrypt
{

    typedef struct KDFTestData512_s
    {
        uint8_t key[64];
        uint8_t salt[64];
        uint8_t expected[64];
        uint32_t keysize;
        uint32_t saltsize;
        uint32_t expectedsize;
    } KDFTestData512;

}

#endif //_KDFTESTS_H
