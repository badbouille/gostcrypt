//
// Created by badbouille on 08/11/2020.
//

#include "cryptoTests.h"
#include "unity.h"

void setUp(void) {
    // nothing to setup
}

void tearDown(void) {
    // nothing to remove
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    /* Ciphers */

    /* XOR */
    RUN_TEST(test_blockcipher_xor_basic);
    RUN_TEST(test_blockcipher_xor_encrypt);
    RUN_TEST(test_blockcipher_xor_decrypt);
    RUN_TEST(test_blockcipher_xor_sizechecks);

    /* Encryption Algorithms */

    /* ECB */
    RUN_TEST(test_diskencryptionalgorithm_ecb_xor_basic);
    RUN_TEST(test_diskencryptionalgorithm_ecb_xor_encrypt);
    RUN_TEST(test_diskencryptionalgorithm_ecb_xor_decrypt);
    RUN_TEST(test_diskencryptionalgorithm_ecb_xor_sizechecks);

    return UNITY_END();
}