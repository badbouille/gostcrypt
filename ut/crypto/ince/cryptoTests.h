//
// Created by badbouille on 08/11/2020.
//

#ifndef _COMMONTESTS_H
#define _COMMONTESTS_H

#include "BlockCipher.h"
#include <string>

/* Standard tests */
void stdtest_blockcipher_basic(GostCrypt::BlockCipher *b, size_t bs, size_t ks, size_t ctxs, std::string name, std::string id);

/* XOR Cipher tests */
void test_blockcipher_xor_basic();

#endif //_COMMON_TESTS_H
