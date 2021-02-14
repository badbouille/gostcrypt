/**
 * @file VolumeTests.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "Buffer.h"
#include "Volume.h"
#include "unity.h"

#include <fstream>
#include <VolumeCreatorContents.h>
#include <VolumeCreator.h>
#include <ContainerFile.h>

using namespace GostCrypt;

void stdtests_volume_open(Volume *v, const std::string& file, SecureBufferPtr &password, bool expectedresult, bool expectedexception) {

    bool opened = false;

    try {
        opened = v->open(new ContainerFile(file), password);

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

/* will write to the volume then check if file has changed */
void stdtests_volume_write(Volume *v, SecureBufferPtr &password, size_t headersize, size_t sectorsize, const VolumeFileCreator *file) {
    SecureBuffer bb(3*sectorsize + 2); // buffer de test
    SecureBuffer bc(headersize*2); // buffer de constantes à vérifier
    SecureBuffer br(headersize*2); // buffer lu dans le fichier
    SecureBufferPtr b;
    size_t sectornum, i, j;
    size_t volumesize = file->size - headersize * 4;

    std::fstream rfile;

    /* Assuming v is correctly initialized */

    if (volumesize % sectorsize != 0) {
        TEST_FAIL_MESSAGE("Wrong input data - the volume is not an even number of sectors.");
    }

    // getting sector number to have targeted tests on specific features
    sectornum = volumesize / sectorsize;

    // preparing writing value
    bb.getRange(b, 0, bb.size());
    for (i=0; i< b.size(); i++) {
        b.get()[i] = 0xFF;
    }

    /* ------------------------ CASE 4 : aligned writing ------------------------ */

    // opening volume
    try {
        if(!v->open(new ContainerFile(file->filename), password)) {
            TEST_FAIL_MESSAGE("Could not open volume (Case 4)");
        }
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE(e.what());
    }

    // writing to volume (loop)
    bb.getRange(b, 0, sectorsize); // one sector
    for (i=0; i<sectornum; i++) {
        v->write(b, i*sectorsize);
    }

    // closing volume
    v->close();

    // checking volume content
    // opening volume
    rfile.open(file->filename, std::ios::binary | std::ios::in);

    /* Checking file content */
    // TODO only works for StandardVolume ? Does this function make sense ?

    // first two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size()/2, "Wrong first header (Case 4)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size(), "Wrong second header (Case 4)");

    // volume content
    bc.allocate(sectorsize);
    br.allocate(sectorsize);
    for (i=0; i < sectornum; i++) {
        rfile.read((char *)bc.get(), bc.size()); // reading from file
        memcpy(br.get(), file->content + 2*headersize + i*sectorsize, sectorsize); // getting original value
        for (j=0; j< br.size(); j++) { // adding expected component
            br.get()[j] = 0xFF;
        }
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(br.get(), bc.get(), bc.size(), "Wrong content written (Case 4)");
    }

    // last two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size()/2, "Wrong first backup header (Case 4)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size(), "Wrong second backup header (Case 4)");

    // checking eof
    if (rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file reached too early (Case 4)");
    }
    rfile.read((char *)bc.get(), 1); // one byte is enough
    if (!rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file not reached (Case 4)");
    }

    // closing volume
    rfile.close();

    // resetting volume to original content
    stdtests_createvolume(file->filename, file->size, (char *)file->content);


    /* ------------------------ CASE 1 : smaller writing test ------------------------ */

    // opening volume
    try {
        if(!v->open(new ContainerFile(file->filename), password)) {
            TEST_FAIL_MESSAGE("Could not open volume (Case 1)");
        }
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE(e.what());
    }

    // writing to volume (loop)
    bb.getRange(b, 0, 1); // one byte
    for (i=0; i<sectornum; i++) {
        v->write(b, i*sectorsize+i%sectorsize);
    }

    // closing volume
    v->close();

    // checking volume content
    // opening volume
    rfile.open(file->filename, std::ios::binary | std::ios::in);

    /* Checking file content */
    // TODO only works for StandardVolume ? Does this function make sense ?

    // first two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size()/2, "Wrong first header (Case 1)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size(), "Wrong second header (Case 1)");

    // volume content
    bc.allocate(sectorsize);
    br.allocate(sectorsize);
    for (i=0; i < sectornum; i++) {
        rfile.read((char *)bc.get(), bc.size()); // reading from file
        memcpy(br.get(), file->content + 2*headersize + i*sectorsize, sectorsize); // getting original value
        br.get()[i] = 0xFF; // adding expected component
        // NOTE : THIS PART ASSUMES THE CONTAINER IS 'NOT' ENCRYPTED (or is xor encrypted using key=0x00..)
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(br.get(), bc.get(), bc.size(), "Wrong content written (Case 1)");
    }

    // last two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size()/2, "Wrong first backup header (Case 1)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size(), "Wrong second backup header (Case 1)");

    // checking eof
    if (rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file reached too early (Case 1)");
    }
    rfile.read((char *)bc.get(), 1); // one byte is enough
    if (!rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file not reached (Case 1)");
    }

    // closing volume
    rfile.close();

    // resetting volume to original content
    stdtests_createvolume(file->filename, file->size, (char *)file->content);

    /* ------------------------ CASE 2 : block end writing test ------------------------ */

    // opening volume
    try {
        if(!v->open(new ContainerFile(file->filename), password)) {
            TEST_FAIL_MESSAGE("Could not open volume (Case 2)");
        }
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE(e.what());
    }

    // writing to volume (loop)
    bb.getRange(b, 0, sectorsize-1); // one sector
    for (i=0; i<sectornum; i++) {
        v->write(b, i*sectorsize+1);
    }

    // closing volume
    v->close();

    // checking volume content
    // opening volume
    rfile.open(file->filename, std::ios::binary | std::ios::in);

    /* Checking file content */
    // TODO only works for StandardVolume ? Does this function make sense ?

    // first two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size()/2, "Wrong first header (Case 2)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size(), "Wrong second header (Case 2)");

    // volume content
    bc.allocate(sectorsize);
    br.allocate(sectorsize);
    for (i=0; i < sectornum; i++) {
        rfile.read((char *)bc.get(), bc.size()); // reading from file
        memcpy(br.get(), file->content + 2*headersize + i*sectorsize, sectorsize); // getting original value
        for (j=1; j < br.size(); j++) { // adding expected component
            br.get()[j] = 0xFF;
        }
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(br.get(), bc.get(), bc.size(), "Wrong content written (Case 2)");
    }

    // last two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size()/2, "Wrong first backup header (Case 2)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size(), "Wrong second backup header (Case 2)");

    // checking eof
    if (rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file reached too early (Case 2)");
    }
    rfile.read((char *)bc.get(), 1); // one byte is enough
    if (!rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file not reached (Case 2)");
    }

    // closing volume
    rfile.close();

    // resetting volume to original content
    stdtests_createvolume(file->filename, file->size, (char *)file->content);

    /* ------------------------ CASE 3 : block start writing test ------------------------ */

    // opening volume
    try {
        if(!v->open(new ContainerFile(file->filename), password)) {
            TEST_FAIL_MESSAGE("Could not open volume (Case 3)");
        }
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE(e.what());
    }

    // writing to volume (loop)
    bb.getRange(b, 0, sectorsize-1); // one sector
    for (i=0; i<sectornum; i++) {
        v->write(b, i*sectorsize);
    }

    // closing volume
    v->close();

    // checking volume content
    // opening volume
    rfile.open(file->filename, std::ios::binary | std::ios::in);

    /* Checking file content */
    // TODO only works for StandardVolume ? Does this function make sense ?

    // first two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size()/2, "Wrong first header (Case 3)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size(), "Wrong second header (Case 3)");

    // volume content
    bc.allocate(sectorsize);
    br.allocate(sectorsize);
    for (i=0; i < sectornum; i++) {
        rfile.read((char *)bc.get(), bc.size()); // reading from file
        memcpy(br.get(), file->content + 2*headersize + i*sectorsize, sectorsize); // getting original value
        for (j=0; j< br.size() - 1; j++) { // adding expected component
            br.get()[j] = 0xFF;
        }
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(br.get(), bc.get(), bc.size(), "Wrong content written (Case 3)");
    }

    // last two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size()/2, "Wrong first backup header (Case 3)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size(), "Wrong second backup header (Case 3)");

    // checking eof
    if (rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file reached too early (Case 3)");
    }
    rfile.read((char *)bc.get(), 1); // one byte is enough
    if (!rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file not reached (Case 3)");
    }

    // closing volume
    rfile.close();

    // resetting volume to original content
    stdtests_createvolume(file->filename, file->size, (char *)file->content);

    /* ------------------------ CASE ALL : Complex writing test ------------------------ */

    // opening volume
    try {
        if(!v->open(new ContainerFile(file->filename), password)) {
            TEST_FAIL_MESSAGE("Could not open volume (all Cases)");
        }
    } catch (GostCryptException &e) {
        TEST_FAIL_MESSAGE(e.what());
    }

    // writing to volume (loop)
    bb.getRange(b, 0, 1); // first sector
    v->write(b, 0);

    bb.getRange(b, 0, bb.size()); // one sector
    for (i=1; i+3<sectornum; i+=4) {
        v->write(b, i*sectorsize-1);
    }

    // writing missing byte
    bb.getRange(b, 0, 1); // first sector
    v->write(b, i*sectorsize-1);

    // writing missing sector(s)
    bb.getRange(b, 0, sectorsize); // one sector
    for (; i<sectornum; i++) {
        v->write(b, i*sectorsize);
    }

    // closing volume
    v->close();

    // checking volume content
    // opening volume
    rfile.open(file->filename, std::ios::binary | std::ios::in);

    /* Checking file content */
    // TODO only works for StandardVolume ? Does this function make sense ?

    // first two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size()/2, "Wrong first header (all Cases)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content, bc.get(), bc.size(), "Wrong second header (all Cases)");

    bc.allocate(sectorsize);
    br.allocate(sectorsize);
    // volume content (full)
    for (i=0; i < sectornum; i++) {
        rfile.read((char *)bc.get(), bc.size()); // reading from file
        memcpy(br.get(), file->content + 2*headersize + i*sectorsize, sectorsize); // getting original value
        if (i % 4) { // full block
            for (j=0; j< br.size(); j++) { // adding expected component
                br.get()[j] = 0xFF;
            }
        } else { // only first and last
            br.get()[0] = 0xFF;
            br.get()[sectorsize-1] = 0xFF;
        }
        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(br.get(), bc.get(), bc.size(), "Wrong content written (all Cases)");
    }

    // last two headers
    bc.allocate(2*headersize);
    rfile.read((char *)bc.get(), bc.size());
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size()/2, "Wrong first backup header (all Cases)");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(file->content + 2*headersize + sectornum*sectorsize, bc.get(), bc.size(), "Wrong second backup header (all Cases)");

    // checking eof
    if (rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file reached too early (all Cases)");
    }
    rfile.read((char *)bc.get(), 1); // one byte is enough
    if (!rfile.eof()) {
        TEST_FAIL_MESSAGE("End of file not reached (all Cases)");
    }

    // closing volume
    rfile.close();

    // resetting volume to original content
    stdtests_createvolume(file->filename, file->size, (char *)file->content);

    // success of writing test!
}
