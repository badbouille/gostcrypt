//
// Created by badbouille on 08/11/2020.
//

#include "volumeTests.h"
#include "Buffer.h"
#include "Volume.h"
#include "VolumeStandard.h"
#include "VolumeCreatorContents.h"
#include "unity.h"

using namespace GostCrypt;

void test_volume_standard_open() {

    Volume *v = nullptr;
    SecureBuffer password(16);
    SecureBufferPtr pass(password.get(), password.size());

    pass.erase(); // 0

    v = new VolumeStandard();
    stdtests_volume_open(v, creator_files[0].filename, pass, false, false);
    delete v;

    v = new VolumeStandard();
    stdtests_volume_open(v, creator_files[1].filename, pass, true, false);
    delete v;
}

void test_volume_standard_read() {
    Volume *v = nullptr;
    SecureBuffer password(16);
    SecureBufferPtr pass(password.get(), password.size());

    pass.erase();

    // opening a good volume
    v = new VolumeStandard();
    try {
        if(!v->open(creator_files[1].filename, pass)) {
            TEST_FAIL_MESSAGE("Could not open volume");
        }
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE(e.what());
    }

    // read tests (these tests are hardcoded, making them generic would require a lot more time)
    stdtests_volume_read(v, 32, STANDARD_HEADER_SIZE, volume1_content+STANDARD_HEADER_SIZE*2);

    // finish
    delete v;
}

void test_volume_standard_write() {
    Volume *v = nullptr;
    SecureBuffer password(16);
    SecureBufferPtr pass(password.get(), password.size());

    pass.erase();

    // opening a good volume
    v = new VolumeStandard();

    stdtests_volume_write(v, pass, STANDARD_HEADER_SIZE, 32, &creator_files[1]);

    // finish
    delete v;
}