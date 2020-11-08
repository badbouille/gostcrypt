//
// Created by badbouille on 08/11/2020.
//

#include <Buffer.h>
#include "unity.h"
#include <cstdint>

using namespace GostCrypt;

/* BufferPtr */

const uint8_t cbuffer1[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
const uint8_t cbuffer2[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
const uint8_t cbuffer3[8] = { 0x11, 0x22, 0x33, 0x44, 0x99, 0xAA, 0xBB, 0xCC };
const uint8_t cbuffer4[4] = { 0x11, 0x22, 0x33, 0x44 };

      uint8_t  buffer1[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
      uint8_t  buffer2[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
      uint8_t  buffer3[8] = { 0x11, 0x22, 0x33, 0x44, 0x99, 0xAA, 0xBB, 0xCC };
      uint8_t  buffer4[4] = { 0x11, 0x22, 0x33, 0x44 };

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
    BufferPtr bufptr1(cbuffer1, sizeof(cbuffer1));
    BufferPtr bufptr2( buffer1, sizeof( buffer1));

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

}