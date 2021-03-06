/**
 * @file HashTests.h
 * @author badbouille
 * @date 09/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _HASHTESTS_H
#define _HASHTESTS_H

#include "Hash.h"
#include "Buffer.h"

namespace GostCrypt
{

    /* 128 is the length of the expected digest */
    typedef struct HashTestData128_s
    {
        uint8_t text[1004];
        uint32_t length;
        uint8_t expected[16];
    } HashTestData128;

}

#endif //_HASHTESTS_H
