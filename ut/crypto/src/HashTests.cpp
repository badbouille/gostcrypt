//
// Created by badbouille on 09/12/2020.
//

#include <HashTests.h>
#include "unity.h"
#include <string>

using namespace GostCrypt;

void stdtest_hash_basic(Hash *h, size_t ds, size_t bs, const std::string& name, const std::string& id) {

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(h->GetDigestSize(), ds, "Given digest size was not the one expected");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(h->GetBlockSize(), bs, "Given block size was not the one expected");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(h->GetName().c_str(), name.c_str(), "Hash name is not the one expected");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(h->GetID().c_str(), id.c_str(), "Hash ID is not the one expected");

}

void stdtest_hash_process(Hash *h, size_t test_num, const HashTestData128 *testvector) {

    SecureBuffer digest(h->GetDigestSize());

    for(int i=0; i< test_num; i++) {
        SecureBufferPtr content (testvector[i].text, testvector[i].length);
        SecureBufferPtr pdigest (digest.get(), digest.size());

        h->Reset();
        h->Process(content);
        h->GetDigest(pdigest);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, digest.get(), digest.size(), "Computed digest is wrong!");
    }

}

void stdtest_hash_processmult(Hash *h, size_t test_num, const HashTestData128 *testvector) {

    SecureBuffer digest(h->GetDigestSize());

    for(int i=0; i< test_num; i++) {
        SecureBufferPtr content (testvector[i].text, testvector[i].length);
        SecureBufferPtr pdigest (digest.get(), digest.size());

        h->Reset();

        for (int j=0; j < testvector[i].length; j++) {
            content.set(testvector[i].text + j, 1);
            h->Process(content);
        }

        h->GetDigest(pdigest);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, digest.get(), digest.size(), "Computed digest is wrong!");
    }

}

void stdtest_hash_sizechecks(Hash *h) {

    SecureBuffer wbufa(h->GetDigestSize() + 1); // buffer of the wrong size
    SecureBuffer wbufb(h->GetDigestSize() - 1); // buffer of the wrong size
    SecureBufferPtr wbufaptr(wbufa.get(), wbufa.size());
    SecureBufferPtr wbufbptr(wbufb.get(), wbufb.size());
    wbufa.erase();
    wbufb.erase();

    /* Wrong digest size test */
    try {
        h->GetDigest(wbufaptr);
        TEST_FAIL_MESSAGE("Should not be able to get a digest of too large size");
    } catch (InvalidParameterException &e) {
        // expected
    }

    /* Wrong digest size test */
    try {
        h->GetDigest(wbufbptr);
        TEST_FAIL_MESSAGE("Should not be able to get a digest of too small size");
    } catch (InvalidParameterException &e) {
        // expected
    }

}