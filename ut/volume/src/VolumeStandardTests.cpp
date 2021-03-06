/**
 * @file VolumeStandardTests.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <ContainerFile.h>
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
        if(!v->open(new ContainerFile(creator_files[1].filename), pass)) {
            TEST_FAIL_MESSAGE("Could not open volume");
        }
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE(e.what());
    }

    // read tests (these tests are hardcoded, making them generic would require a lot more time)
    stdtests_volume_read(v, 32, STANDARD_HEADER_SIZE, volume1_content+(STANDARD_HEADER_SIZE+STANDARD_HEADER_SALT_AREASIZE)*2);

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

    stdtests_volume_write(v, pass, STANDARD_HEADER_SIZE+STANDARD_HEADER_SALT_AREASIZE, 32, &creator_files[1]);

    // finish
    delete v;
}

void test_volume_standard_create() {
    Volume *v = nullptr;
    SecureBuffer password(16);
    SecureBuffer bc(16);
    SecureBuffer br(16);
    SecureBufferPtr pass(password.get(), password.size());
    std::fstream rfile;
    int i;

    pass.erase();

    // opening a good volume
    v = new VolumeStandard();
    v->create(new ContainerFile(creator_files[1].filename), STANDARD_HEADER_SIZE, "ECB_{XOR128}", "Pbkdf2-HMAC-XOR0-16", 32, pass);
    v->close();

    // checking file
    rfile.open(creator_files[1].filename, std::ios::binary | std::ios::in);
    // first two headers
    bc.allocate(2*(STANDARD_HEADER_SIZE+STANDARD_HEADER_SALT_AREASIZE));
    rfile.read((char *)bc.get(), bc.size());
    // only the first 64 elements (after the salt) are not random and can be verified
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(creator_files[1].content+64, bc.get()+64, 64, "Wrong first header");

    // Not checking second header (random data)
    //TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(creator_files[1].content, bc.get(), bc.size(), "Wrong second header");

    bc.allocate(32);
    br.allocate(32);
    br.erase(); // all content should be empty.

    // volume content (full)
    for (i=0; i < STANDARD_HEADER_SIZE/32; i++) {
        rfile.read((char *)bc.get(), bc.size()); // reading from file
        // TODO: Not checking content because it's random data. Maybe check something else ?
        //TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(br.get(), bc.get(), bc.size(), "Wrong content written");
    }

    // last two headers
    bc.allocate(2*(STANDARD_HEADER_SIZE+STANDARD_HEADER_SALT_AREASIZE));
    rfile.read((char *)bc.get(), bc.size());
    // Not checking second header (random data)
    //TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(creator_files[1].content + 3*STANDARD_HEADER_SIZE, bc.get(), bc.size()/2, "Wrong first backup header");
    // only the first 64 elements are not random and can be verified
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(creator_files[1].content + 2*(STANDARD_HEADER_SIZE+STANDARD_HEADER_SALT_AREASIZE) + STANDARD_HEADER_SIZE + bc.size()/2 + 64, bc.get()+ bc.size()/2 + 64, 64, "Wrong second backup header");

    // checking eof
    if (rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file reached too early");
    }
    rfile.read((char *)bc.get(), 1); // one byte is enough
    if (!rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file not reached");
    }

    // finish
    delete v;
}

void test_volume_standard_checks() {
    Volume *v = nullptr;
    SecureBuffer password(16);
    SecureBuffer rbuf(16);
    SecureBufferPtr pass(password.get(), password.size());
    SecureBufferPtr rb(rbuf.get(), rbuf.size());

    pass.erase();

    // Testing all error cases

    // TODO: nothing checked for now. Currently no checks are implemented and anything illegal WILL crash.

    // create
    v = new VolumeStandard();
    v->create(new ContainerFile(creator_files[1].filename), STANDARD_HEADER_SIZE, "ECB_{XOR128}", "Pbkdf2-HMAC-XOR0-16", 32, pass);
    v->close();

    // open
    v->open(new ContainerFile(creator_files[1].filename), pass);

    // write
    v->write(pass, 0);

    // read
    v->read(rb, 0);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(pass.get(), rb.get(), rb.size(), "Wrong read operation");

    // close
    v->close();
}
