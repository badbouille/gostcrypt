/**
 * @file KDFTests.cpp
 * @author badbouille
 * @date 17/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <KDF.h>
#include "KDFTests.h"
#include "unity.h"
#include <string>
#include <iostream>

using namespace GostCrypt;

void stdtest_kdf_basic(KDF *k, const std::string& name, const std::string& id) {

    TEST_ASSERT_EQUAL_STRING_MESSAGE(k->GetName().c_str(), name.c_str(), "MAC name is not the one expected");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(k->GetID().c_str(), id.c_str(), "MAC ID is not the one expected");

}

void stdtest_kdf_derivate(KDF *k, size_t test_num, const KDFTestData512 *testvector) {

    for(int i=0; i< test_num; i++) {
        SecureBuffer dk(testvector[i].expectedsize);
        SecureBufferPtr dkptr (dk.get(), dk.size());
        SecureBufferPtr key (testvector[i].key, testvector[i].keysize);
        SecureBufferPtr salt (testvector[i].salt, testvector[i].saltsize);

        k->Derivate(key, salt, dkptr);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, dkptr.get(), dkptr.size(), "Derived key is wrong!");
    }

}
