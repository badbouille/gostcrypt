//
// Created by badbouille on 08/11/2020.
//

#include <BlockCipher.h>
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
