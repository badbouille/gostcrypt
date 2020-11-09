//
// Created by badbouille on 08/11/2020.
//

#include <BlockCipher.h>
#include "BlockCipherTests.h"
#include "unity.h"
#include <cstdint>
#include <string>

using namespace GostCrypt;

void stdtest_blockcipher_basic(BlockCipher *b, size_t bs, size_t ks, size_t ctxs, std::string name, std::string id) {

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(b->GetBlockSize(), bs, "Given block size was not the one expected");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(b->GetKeySize(), ks, "Given key size was not the one expected");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(b->GetCtxSize(), ctxs, "Given ctx size was not the one expected");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(b->GetName().c_str(), name.c_str(), "Algorithm name is not the one expected");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(b->GetID().c_str(), id.c_str(), "Algorithm ID is not the one expected");

}

void stdtest_blockcipher_encrypt128(BlockCipher *b, size_t test_num, const BlockCipherTestData128 *testvector) {

    SecureBuffer keybuf(16);
    SecureBuffer inputbuf(16);

    SecureBufferPtr key(keybuf.get(), keybuf.size());
    SecureBufferPtr input(inputbuf.get(), inputbuf.size());

    for(int i=0; i< test_num; i++) {
        SecureBufferPtr ckey  (testvector[i].key,   16);
        SecureBufferPtr cinput(testvector[i].input, 16);

        key.copyFrom(ckey);
        input.copyFrom((cinput));

        b->SetKey(key);
        b->Encrypt(input);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, input.get(), 16, "Encryption result is wrong!");
    }

}

void stdtest_blockcipher_decrypt128(BlockCipher *b, size_t test_num, const BlockCipherTestData128 *testvector) {

    SecureBuffer keybuf(16);
    SecureBuffer inputbuf(16);

    SecureBufferPtr key(keybuf.get(), keybuf.size());
    SecureBufferPtr input(inputbuf.get(), inputbuf.size());

    for(int i=0; i< test_num; i++) {
        SecureBufferPtr ckey  (testvector[i].key,   16);
        SecureBufferPtr cinput(testvector[i].input, 16);

        key.copyFrom(ckey);
        input.copyFrom((cinput));

        b->SetKey(key);
        b->Decrypt(input);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, input.get(), 16, "Decryption result is wrong!");
    }

}
