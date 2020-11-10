//
// Created by badbouille on 08/11/2020.
//

#include "DiskEncryptionAlgorithm.h"
#include "DiskEncryptionAlgorithmECB.h"
#include "DiskEncryptionAlgorithmTests.h"
#include "BlockCipherXOR.h"
#include "unity.h"
#include <cstdint>
#include <string>
#include "cryptoTests.h"

using namespace GostCrypt;

void test_diskencryptionalgorithm_ecb_xor_basic() {

    DiskEncryptionAlgorithm *a = new DiskEncryptionAlgorithmECB<BlockCipherXOR>();

    stdtest_diskencryptionalgorithm_basic(a, 16, "ECB_{XOR128}");

    delete a;
}
