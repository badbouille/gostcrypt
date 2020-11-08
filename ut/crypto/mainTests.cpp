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
    /* XOR */
    RUN_TEST(test_blockcipher_xor_basic);
    return UNITY_END();
}