//
// Created by badbouille on 08/11/2020.
//

#include "Buffer.h"
#include "Volume.h"
#include "unity.h"

using namespace GostCrypt;

void stdtests_volume_open(Volume *v, const std::string& file, SecureBufferPtr &password, bool expectedresult, bool expectedexception) {

    bool opened = false;

    try {
        opened = v->open(file, password);

        if (opened && !expectedresult) {
            TEST_FAIL_MESSAGE("Volume successfully opened but it shouldn't have");
        }

        if (!opened && expectedresult) {
            TEST_FAIL_MESSAGE("A well-formed volume could not be opened");
        }

    } catch (GostCryptException &e) {
        if (!expectedexception) {
            TEST_FAIL_MESSAGE(e.what());
        }
    }

}

void stdtests_volume_read(Volume *v, size_t sectorsize, size_t volumesize, const uint8_t *volumecontent) {
    SecureBuffer bb(3*sectorsize + 2);
    SecureBufferPtr b;
    size_t sectornum, i;

    /* Assuming v is correctly initialized */

    if (volumesize % sectorsize != 0) {
        TEST_FAIL_MESSAGE("Wrong input data - the volume is not an even number of sectors.");
    }

    // getting sector number to have targeted tests on specific features
    sectornum = volumesize / sectorsize;

    /* Aligned reading test (case 4) */
    bb.getRange(b, 0, sectorsize); // aligned means we read the exact amount needed
    for (i=0; i<sectornum; i++) {
        v->read(b, i*sectorsize);
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(volumecontent+i*sectorsize, b.get(), b.size(), "Case 4 read error.");
    }

    /* smaller reading test (case 1) */
    bb.getRange(b, 0, 1); // reading only one byte
    for (i=0; i<sectornum; i++) {
        v->read(b, i*sectorsize + (i % sectorsize)); // (i % sectorsize) let us read something different from the first byte only
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(volumecontent+(i*sectorsize)+(i % sectorsize), b.get(), b.size(), "Case 1 read error.");
    }

    /* block end reading test (case 2) */
    bb.getRange(b, 0, sectorsize - 1); // reading all minus one
    for (i=0; i<sectornum; i++) {
        v->read(b, i*sectorsize + 1);
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(volumecontent+(i*sectorsize)+1, b.get(), b.size(), "Case 2 read error.");
    }

    /* block start reading test (case 3) */
    bb.getRange(b, 0, sectorsize - 1); // reading all minus one
    for (i=0; i<sectornum; i++) {
        v->read(b, i*sectorsize);
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(volumecontent+(i*sectorsize), b.get(), b.size(), "Case 3 read error.");
    }

    /* Complex reading test (case 2 + 4 + 3) */
    bb.getRange(b, 0, bb.size()); // reading max
    for (i=1; i<sectornum-3; i++) {
        v->read(b, i*sectorsize - 1); // minus one = reading one byte from previous and one byte from next
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(volumecontent+(i*sectorsize)-1, b.get(), b.size(), "Case 2 + 4 + 3 read error.");
    }

}
