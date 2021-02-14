/**
 * @file mainTests.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

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

    /* Hash functions (KDFs) */

    /* XOR0-16 */
    RUN_TEST(test_hash_xor0_basic);
    RUN_TEST(test_hash_xor0_process);
    RUN_TEST(test_hash_xor0_processmult);
    RUN_TEST(test_hash_xor0_sizechecks);

    /* MAC functions */

    /* HMAC */
    RUN_TEST(test_mac_hmac_basic);
    RUN_TEST(test_mac_hmac_process);
    RUN_TEST(test_mac_hmac_processmult);
    RUN_TEST(test_mac_hmac_sizechecks);

    /* KDF functions */

    /* Pbkdf2 */
    RUN_TEST(test_kdf_pbkdf2_basic);
    RUN_TEST(test_kdf_pbkdf2_derivate);

    /* PRNG functions */

    /* System PRNG */
    RUN_TEST(test_prng_prngsystem_basic);
    RUN_TEST(test_prng_prngsystem_entropy);

    /* Secure PRNG */
    RUN_TEST(test_prng_prngsecure_basic);
    RUN_TEST(test_prng_prngsecure_entropy);

    return UNITY_END();
}