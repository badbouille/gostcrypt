//
// Created by badbouille on 08/11/2020.
//

#ifndef _COMMONTESTS_H
#define _COMMONTESTS_H

#include "BlockCipher.h"
#include "DiskEncryptionAlgorithm.h"
#include "Hash.h"
#include "PRNG.h"
#include "BlockCipherTests.h"
#include "DiskEncryptionAlgorithmTests.h"
#include "HashTests.h"
#include <string>


/* Standard tests */
void stdtest_blockcipher_basic(GostCrypt::BlockCipher *b, size_t bs, size_t ks, size_t ctxs, const std::string &name, const std::string &id);
void stdtest_blockcipher_encrypt128(GostCrypt::BlockCipher *b, size_t test_num, const GostCrypt::BlockCipherTestData128 *testvector);
void stdtest_blockcipher_decrypt128(GostCrypt::BlockCipher *b, size_t test_num, const GostCrypt::BlockCipherTestData128 *testvector);
void stdtest_blockcipher_sizechecks(GostCrypt::BlockCipher *b, size_t wrongsize);

void stdtest_diskencryptionalgorithm_basic(GostCrypt::DiskEncryptionAlgorithm *a, size_t ks, const std::string& id);
void stdtest_diskencryptionalgorithm_encrypt(GostCrypt::DiskEncryptionAlgorithm *a, size_t test_num, const GostCrypt::DiskEncryptionAlgorithmTestData520 *testvector);
void stdtest_diskencryptionalgorithm_decrypt(GostCrypt::DiskEncryptionAlgorithm *a, size_t test_num, const GostCrypt::DiskEncryptionAlgorithmTestData520 *testvector);
void stdtest_diskencryptionalgorithm_sizechecks(GostCrypt::DiskEncryptionAlgorithm *a, size_t wrongblocksize);

void stdtest_hash_basic(GostCrypt::Hash *h, size_t ds, const std::string& name, const std::string& id);
void stdtest_hash_process(GostCrypt::Hash *h, size_t test_num, const GostCrypt::HashTestData128 *testvector);
void stdtest_hash_processmult(GostCrypt::Hash *h, size_t test_num, const GostCrypt::HashTestData128 *testvector);
void stdtest_hash_sizechecks(GostCrypt::Hash *h);

void stdtest_prng_basic(GostCrypt::PRNG *p, const std::string& name, const std::string& id);
void stdtest_prng_entropy(GostCrypt::PRNG *p, size_t cyclecount, float precision);

/* XOR Cipher tests */
void test_blockcipher_xor_basic();
void test_blockcipher_xor_encrypt();
void test_blockcipher_xor_decrypt();
void test_blockcipher_xor_sizechecks();

/* ECB (XOR) DiskEA tests */
void test_diskencryptionalgorithm_ecb_xor_basic();
void test_diskencryptionalgorithm_ecb_xor_encrypt();
void test_diskencryptionalgorithm_ecb_xor_decrypt();
void test_diskencryptionalgorithm_ecb_xor_sizechecks();

/* XOR-0Padding (XOR0) Hash tests */
void test_hash_xor0_basic();
void test_hash_xor0_process();
void test_hash_xor0_processmult();
void test_hash_xor0_sizechecks();

/* System PRNG (SPRNG) tests */
void test_prng_prngsystem_basic();
void test_prng_prngsystem_entropy();

/* Secure PRNG (CSPRNG) tests */
void test_prng_prngsecure_basic();
void test_prng_prngsecure_entropy();

#endif //_COMMON_TESTS_H
