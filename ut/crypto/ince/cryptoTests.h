//
// Created by badbouille on 08/11/2020.
//

#ifndef _COMMONTESTS_H
#define _COMMONTESTS_H

#include "BlockCipher.h"
#include "BlockCipherTests.h"
#include <string>

/* Standard tests */
void stdtest_blockcipher_basic(GostCrypt::BlockCipher *b, size_t bs, size_t ks, size_t ctxs, std::string name, std::string id);
void stdtest_blockcipher_encrypt128(GostCrypt::BlockCipher *b, size_t test_num, const GostCrypt::BlockCipherTestData128 *testvector);
void stdtest_blockcipher_decrypt128(GostCrypt::BlockCipher *b, size_t test_num, const GostCrypt::BlockCipherTestData128 *testvector);

/* XOR Cipher tests */
void test_blockcipher_xor_basic();
void test_blockcipher_xor_encrypt();
void test_blockcipher_xor_decrypt();

#endif //_COMMON_TESTS_H
