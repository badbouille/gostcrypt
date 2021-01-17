//
// Created by badbouille on 17/01/2021.
//

#include <MAC.h>
#include "MACTests.h"
#include "unity.h"
#include <string>

using namespace GostCrypt;

void stdtest_mac_basic(MAC *m, size_t ds, size_t bs, const std::string& name, const std::string& id) {

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(m->GetDigestSize(), ds, "Given digest size was not the one expected");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(m->GetBlockSize(), bs, "Given block size was not the one expected");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(m->GetName().c_str(), name.c_str(), "MAC name is not the one expected");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(m->GetID().c_str(), id.c_str(), "MAC ID is not the one expected");

}

void stdtest_mac_process(MAC *m, size_t test_num, const MACTestData256 *testvector) {

    SecureBuffer digest(m->GetDigestSize());

    for(int i=0; i< test_num; i++) {
        SecureBufferPtr key (testvector[i].key, testvector[i].keysize);
        SecureBufferPtr input (testvector[i].input, testvector[i].inputsize);
        SecureBufferPtr pdigest (digest.get(), digest.size());

        m->SetKey(key);
        m->Process(input);
        m->GetDigest(pdigest);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, digest.get(), digest.size(), "Computed digest is wrong!");
    }

}

void stdtest_mac_processmult(MAC *m, size_t test_num, const MACTestData256 *testvector) {

    SecureBuffer digest(m->GetDigestSize());

    for(int i=0; i< test_num; i++) {
        SecureBufferPtr key (testvector[i].key, testvector[i].keysize);
        SecureBufferPtr input (testvector[i].input, testvector[i].inputsize);
        SecureBufferPtr pdigest (digest.get(), digest.size());

        m->SetKey(key);

        for (int j=0; j < testvector[i].inputsize; j++) {
            input.set(testvector[i].input + j, 1);
            m->Process(input);
        }

        m->GetDigest(pdigest);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, digest.get(), digest.size(), "Computed digest is wrong!");
    }

}

void stdtest_mac_sizechecks(MAC *m) {

    SecureBuffer wbufa(m->GetDigestSize() + 1); // buffer of the wrong size
    SecureBuffer wbufb(m->GetDigestSize() - 1); // buffer of the wrong size
    SecureBufferPtr wbufaptr(wbufa.get(), wbufa.size());
    SecureBufferPtr wbufbptr(wbufb.get(), wbufb.size());
    wbufa.erase();
    wbufb.erase();

    /* Sequencing test (process without key) */
    try {
        m->Process(wbufaptr);
        TEST_FAIL_MESSAGE("Should not be able to process data without setting a key first");
    } catch (AlgorithmUnititializedException &e) {
        // expected
    }

    // Setting key for next tests
    m->SetKey(wbufbptr);

    /* Wrong digest size test */
    try {
        m->GetDigest(wbufaptr);
        TEST_FAIL_MESSAGE("Should not be able to get a digest of too large size");
    } catch (InvalidParameterException &e) {
        // expected
    }

    /* Wrong digest size test */
    try {
        m->GetDigest(wbufbptr);
        TEST_FAIL_MESSAGE("Should not be able to get a digest of too small size");
    } catch (InvalidParameterException &e) {
        // expected
    }

}