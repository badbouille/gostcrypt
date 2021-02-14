/**
 * @file bufferPtrTests.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Buffer.h>
#include "unity.h"
#include <cstdint>

using namespace GostCrypt;

/* BufferPtr */

const uint8_t cbuffere[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
const uint8_t cbuffer1[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
const uint8_t cbuffer2[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
const uint8_t cbuffer3[8] = { 0x11, 0x22, 0x33, 0x44, 0x99, 0xAA, 0xBB, 0xCC };
const uint8_t cbuffer4[4] = { 0x11, 0x22, 0x33, 0x44 };

static uint8_t  buffer1[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
static uint8_t  buffer2[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
static uint8_t  buffer3[8] = { 0x11, 0x22, 0x33, 0x44, 0x99, 0xAA, 0xBB, 0xCC };
static uint8_t  buffer4[4] = { 0x11, 0x22, 0x33, 0x44 };

const uint8_t csbuffer1[8] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
const uint8_t csbuffer2[8] = { 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
const uint8_t csbuffer3[8] = { 0x00, 0x11, 0x22, 0x33, 0xCC, 0xDD, 0xEE, 0xFF };
const uint8_t csbuffer4[8] = { 0x88, 0x99, 0xAA, 0xBB, 0x44, 0x55, 0x66, 0x77 };
static uint8_t  sbuffer1[8] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
static uint8_t  sbuffer2[8] = { 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

static uint8_t  xbuffer1[8] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
const uint8_t  cxbuffer1[8] = { 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88 };
const uint8_t  cxbuffer2[8] = { 0x99, 0xAA, 0xBB, 0xCC, 0x88, 0x88, 0x88, 0x88 };

void test_bufferptr_compare() {
    const BufferPtr bufptr1(cbuffer1, sizeof(cbuffer1));
    const BufferPtr bufptr2(cbuffer2, sizeof(cbuffer2));
    const BufferPtr bufptr3(cbuffer3, sizeof(cbuffer3));
    const BufferPtr bufptr4(cbuffer4, sizeof(cbuffer4));

    TEST_ASSERT_MESSAGE(bufptr1.isDataEqual(bufptr2) == true,  "Buffers 1 and 2 should be equals");
    TEST_ASSERT_MESSAGE(bufptr1.isDataEqual(bufptr3) == false, "Buffers 1 and 3 should not be equals (data)");
    TEST_ASSERT_MESSAGE(bufptr1.isDataEqual(bufptr4) == false, "Buffers 1 and 3 should not be equals (length A)");
    TEST_ASSERT_MESSAGE(bufptr4.isDataEqual(bufptr1) == false, "Buffers 1 and 3 should not be equals (length B)");

}

void test_bufferptr_mutability() {
    BufferPtr bufptr1(cbuffer1, sizeof(cbuffer1)); // this is illegal and may generate runtime exceptions
    BufferPtr bufptr2( buffer1, sizeof( buffer1));
    uint8_t *ptr;

    try {
        ((uint8_t *)bufptr1)[0] = 0x22;
        TEST_FAIL_MESSAGE("Buffer 1 should not be mutable");
    } catch (DataNotMutableException &e) {
        // test ok
    }

    try {
        ((uint8_t *)bufptr2)[0] = 0x22;
        // should work
    } catch (DataNotMutableException &e) {
        TEST_FAIL_MESSAGE("Buffer 2 should be mutable");
    }

    try {
        bufptr1.erase();
        TEST_FAIL_MESSAGE("Buffer 1 should not be erasable");
    } catch (DataNotMutableException &e) {
        // test ok
    }

    try {
        bufptr1.swap(bufptr2);
        TEST_FAIL_MESSAGE("Buffer 1 should not be swappable");
    } catch (DataNotMutableException &e) {
        // test ok
    }

}

void test_bufferptr_copyfrom() {
    const BufferPtr bufptr1(cbuffer1, sizeof(cbuffer1));
    BufferPtr bufptr2( buffer1, sizeof( buffer1));

    for(unsigned char & i : buffer1) {
        i = 0x00; // making sure buffer1 and cbuffer1 are differents
    }

    // function under test
    bufptr2.copyFrom(bufptr1);

    // buffers should now be equals
    TEST_ASSERT_EQUAL_UINT8_ARRAY(cbuffer1, buffer1, sizeof(buffer1));
}

void test_bufferptr_erase() {
    BufferPtr bufptr1(buffer1, sizeof(buffer1));

    for(unsigned char & i : buffer1) {
        i = 0xAB; // making sure buffer1 is not null
    }

    // function under test
    bufptr1.erase();

    // buffer1 should now be empty
    TEST_ASSERT_EQUAL_UINT8_ARRAY(cbuffere, buffer1, sizeof(buffer1));

}

void test_bufferptr_swap() {
    BufferPtr bufptr1(sbuffer1, sizeof(sbuffer1));
    BufferPtr bufptr2(sbuffer2, sizeof(sbuffer2));
    BufferPtr bufptr3(sbuffer2, sizeof(sbuffer2)/2);

    // making sure buffers were not altered
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(csbuffer1, sbuffer1, sizeof(csbuffer2), "Buffers were altered");
    TEST_ASSERT_EQUAL_UINT8_ARRAY(csbuffer2, sbuffer2, sizeof(csbuffer2));

    // function under test
    bufptr2.swap(bufptr1);

    // buffers should be swapped
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(csbuffer2, sbuffer1, sizeof(csbuffer2), "Could not swap the buffers!");
    TEST_ASSERT_EQUAL_UINT8_ARRAY(csbuffer1, sbuffer2, sizeof(csbuffer2));

    // function under test
    bufptr1.swap(bufptr3);

    // buffers should be half-swapped
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(csbuffer3, sbuffer1, sizeof(csbuffer3), "Big buffer not half-swapped!");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(csbuffer4, sbuffer2, sizeof(csbuffer4), "Small buffer not half-swapped!");

}

void test_bufferptr_xor() {
    BufferPtr bufptr1(xbuffer1, sizeof(xbuffer1));
    BufferPtr bufptr2(csbuffer2, sizeof(csbuffer2));
    BufferPtr bufptr3(cbuffer4, sizeof(cbuffer4));

    // making sure buffers were not altered
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(csbuffer1, xbuffer1, sizeof(csbuffer1), "Buffers were altered");

    // function under test
    bufptr1.Xor(bufptr2);

    // buffers should be swapped
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(cxbuffer1, xbuffer1, sizeof(csbuffer2), "Xor result is wrong!");

    // function under test
    bufptr1.Xor(bufptr3);

    // buffers should be half-swapped
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(cxbuffer2, xbuffer1, sizeof(csbuffer3), "Half Xor result is wrong!");

}

void test_bufferptr_illegalinit() {
    const BufferPtr bufptr1(cbuffer4, sizeof(cbuffer4));
          BufferPtr bufptr2(cbuffer4, sizeof(cbuffer4)); // buffers containing const data should be made CONST, or else -> runtime error

    try
    {
        TEST_ASSERT_EQUAL_MESSAGE((const uint8_t *) bufptr1.get(), cbuffer4, "Buffer loaded with wrong pointer");
    } catch (DataNotMutableException &e) {
        TEST_FAIL_MESSAGE("A const BufferPtr should be able to give away its const pointer.");
    }

    try
    {
        TEST_ASSERT_EQUAL_MESSAGE((const uint8_t *) bufptr2.get(), cbuffer4, "Buffer loaded with wrong pointer");
        TEST_FAIL_MESSAGE("Reading a const value from a non-const BufferPtr is NOT ALLOWED and the buffer should not exist in the first place");
    } catch (DataNotMutableException &e) {
        // this error should be detected
    }

}

void test_bufferptr_range() {
    BufferPtr mbufptr1(buffer4, sizeof(buffer4));
    const BufferPtr bufptr1(cbuffer1, sizeof(cbuffer1));
    BufferPtr bufptr2(buffer3, sizeof(buffer3));
    const BufferPtr &cbufptr2 = bufptr2;

    /* testing basic SET method (only exists in non-const version) */

    TEST_ASSERT_EQUAL_HEX64_MESSAGE((const uint8_t*)mbufptr1.get(), buffer4, "Buffer loaded with wrong pointer");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(mbufptr1.size(), sizeof(buffer4), "Buffer loaded with wrong size");

    mbufptr1.set(buffer1, sizeof(buffer1));

    TEST_ASSERT_EQUAL_HEX64_MESSAGE(mbufptr1.get(), buffer1, "Buffer setted with wrong pointer");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(mbufptr1.size(), sizeof(buffer1), "Buffer setted with wrong size");

    /* Sometimes, a user wants to get ranges from a constant buffer.
     * The problem is: range will change the target bufferptr object to put the new range value
     * This is the only case when a BufferPtr can handle const data.
     * WARNING: when trying to acces const data from non-const BufferPtr, an exception is raised!
     * It may look tricky but it is a safelock. The BufferPtr must be re-interpreted as a const object to be able to read const data
     */

    /* Good getrange on const object */
    try {
        bufptr1.getRange(bufptr2, bufptr1.size()/2, bufptr1.size()/2);

        TEST_ASSERT_EQUAL_MESSAGE((const uint8_t*)cbufptr2.get(), cbuffer1+sizeof(cbuffer1)/2, "Wrong range recovered");
        TEST_ASSERT_EQUAL_MESSAGE(bufptr2.size(), sizeof(cbuffer1)/2, "Wrong range recovered");

    } catch (InvalidParameterException &e) {
        TEST_FAIL_MESSAGE("Valid range should not throw an error");
    } catch (DataNotMutableException &e) {
        TEST_FAIL_MESSAGE("Ranges can not be recovered from not mutable data");
    }

    /* Bad getRange on const object */
    try {
        bufptr1.getRange(bufptr2, bufptr1.size()/2, bufptr1.size());
        TEST_FAIL_MESSAGE("Invalid range should have thrown an error");
    } catch (InvalidParameterException &e) {
        // error was detected
    } catch (DataNotMutableException &e) {
        TEST_FAIL_MESSAGE("Ranges can not be recovered from not mutable data");
    }

}