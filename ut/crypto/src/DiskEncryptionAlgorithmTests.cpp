//
// Created by badbouille on 08/11/2020.
//

#include "DiskEncryptionAlgorithm.h"
#include "DiskEncryptionAlgorithmTests.h"
#include "unity.h"
#include <cstdint>
#include <string>

using namespace GostCrypt;

void stdtest_diskencryptionalgorithm_basic(DiskEncryptionAlgorithm *a, size_t ks, const std::string& id) {

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(a->GetKeySize(), ks, "Given key size was not the one expected");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(a->GetID().c_str(), id.c_str(), "Algorithm ID is not the one expected");

}

void stdtest_diskencryptionalgorithm_encrypt(DiskEncryptionAlgorithm *a, size_t test_num, DiskEncryptionAlgorithmTestData136 *test) {


}
