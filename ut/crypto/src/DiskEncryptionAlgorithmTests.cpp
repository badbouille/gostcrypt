/**
 * @file DiskEncryptionAlgorithmTests.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

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

void stdtest_diskencryptionalgorithm_encrypt(DiskEncryptionAlgorithm *a, size_t test_num, const DiskEncryptionAlgorithmTestData520 *testvector) {

    SecureBuffer keybuf(16);
    SecureBufferPtr key(keybuf.get(), keybuf.size());

    for(int i=0; i< test_num; i++) {
        SecureBuffer inputbuf(testvector[i].inputsize);
        SecureBufferPtr input(inputbuf.get(), inputbuf.size());

        SecureBufferPtr ckey  (testvector[i].key,   16);
        SecureBufferPtr cinput(testvector[i].input, testvector[i].inputsize);

        key.copyFrom(ckey);
        input.copyFrom((cinput));

        a->SetKey(key);
        a->Encrypt(input, testvector[i].sectoroffset, testvector[i].sectornum, testvector[i].sectorsize);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, input.get(), testvector[i].inputsize, "Encryption result is wrong!");
    }
}

void stdtest_diskencryptionalgorithm_decrypt(DiskEncryptionAlgorithm *a, size_t test_num, const DiskEncryptionAlgorithmTestData520 *testvector) {

    SecureBuffer keybuf(16);
    SecureBufferPtr key(keybuf.get(), keybuf.size());

    for(int i=0; i< test_num; i++) {
        SecureBuffer inputbuf(testvector[i].inputsize);
        SecureBufferPtr input(inputbuf.get(), inputbuf.size());

        SecureBufferPtr ckey  (testvector[i].key,   16);
        SecureBufferPtr cinput(testvector[i].input, testvector[i].inputsize);

        key.copyFrom(ckey);
        input.copyFrom((cinput));

        a->SetKey(key);
        a->Decrypt(input, testvector[i].sectoroffset, testvector[i].sectornum, testvector[i].sectorsize);

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(testvector[i].expected, input.get(), testvector[i].inputsize, "Decryption result is wrong!");
    }
}

void stdtest_diskencryptionalgorithm_sizechecks(DiskEncryptionAlgorithm *a, size_t wrongblocksize) {

    SecureBuffer wbuf(wrongblocksize);      // buffer of the wrong size
    SecureBuffer rbuf(64);                  // buffer good for encryption
    SecureBuffer kbuf(a->GetKeySize());     // buffer good as a key
    SecureBufferPtr wbufptr(wbuf.get(), wbuf.size());
    SecureBufferPtr rbufptr(rbuf.get(), rbuf.size());
    SecureBufferPtr kbufptr(kbuf.get(), kbuf.size());

    wbuf.erase();
    rbuf.erase();
    kbuf.erase();

    /* Wrong key size test */
    try {
        a->SetKey(wbufptr);
        TEST_FAIL_MESSAGE("Should not be able to set a key of the wrong size");
    } catch (InvalidParameterException &e) {
        // expected
    }

    /* Sequencing test (..crypt without key) */
    try {
        a->Encrypt(rbufptr); // good encryption
        TEST_FAIL_MESSAGE("Should not be able to encrypt without setting a key first");
    } catch (AlgorithmUnititializedException &e) {
        // expected
    }
    try {
        a->Decrypt(rbufptr); // good decryption
        TEST_FAIL_MESSAGE("Should not be able to decrypt without setting a key first");
    } catch (AlgorithmUnititializedException &e) {
        // expected
    }

    /* Setting a key to allow ..cryption */
    a->SetKey(kbufptr);

    /* Wrong ..cryption buffer size */
    try {
        a->Encrypt(wbufptr);
        TEST_FAIL_MESSAGE("Should not be able to encrypt a buffer of the wrong size");
    } catch (InvalidParameterException &e) {
        // expected
    }
    try {
        a->Decrypt(wbufptr);
        TEST_FAIL_MESSAGE("Should not be able to decrypt a buffer of the wrong size");
    } catch (InvalidParameterException &e) {
        // expected
    }

    /* Wrong ..cryption sector size */
    try {
        a->Encrypt(rbufptr, 0, 8, 8);
        TEST_FAIL_MESSAGE("Should not be able to encrypt too small sectors");
    } catch (InvalidParameterException &e) {
        // expected
    }
    try {
        a->Decrypt(rbufptr, 0, 8, 8);
        TEST_FAIL_MESSAGE("Should not be able to decrypt too small sectors");
    } catch (InvalidParameterException &e) {
        // expected
    }

    /* Wrong ..cryption sector count */
    try {
        a->Encrypt(rbufptr, 0, 8, 16);
        TEST_FAIL_MESSAGE("Should not be able to encrypt too small sectors");
    } catch (InvalidParameterException &e) {
        // expected
    }
    try {
        a->Decrypt(rbufptr, 0, 8, 16);
        TEST_FAIL_MESSAGE("Should not be able to decrypt too small sectors");
    } catch (InvalidParameterException &e) {
        // expected
    }

    /* Good ..cryption sector num */
    try {
        a->Encrypt(rbufptr, 0xFFFFFFFF, 4, 16);
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE("Encryption with a huge sector number sould be possible");
    }
    try {
        a->Decrypt(rbufptr, 0xFFFFFFFF, 4, 16);
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE("Decryption with a huge sector number sould be possible");
    }

}
