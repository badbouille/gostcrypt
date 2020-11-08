//
// Created by badbouille on 08/11/2020.
//

#include <BlockCipher.h>
#include "unity.h"
#include <cstdint>
#include <string>
#include <BlockCipherXOR.h>
#include "cryptoTests.h"

using namespace GostCrypt;

void test_blockcipher_xor_basic() {

    BlockCipher *b = new BlockCipherXOR();

    stdtest_blockcipher_basic(b, 16, 16, 16, "XOR 128bits", "XOR128");

    delete b;
}
