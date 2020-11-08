//
// Created by badbouille on 08/11/2020.
//

#include <Buffer.h>
#include "unity.h"
#include <cstdint>

using namespace GostCrypt;

/* BufferPtr */

const uint8_t buffer1[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
      uint8_t buffer2[8] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
      uint8_t buffer3[8] = { 0x11, 0x22, 0x33, 0x44, 0x99, 0xAA, 0xBB, 0xCC };
      uint8_t buffer4[4] = { 0x11, 0x22, 0x33, 0x44 };

void test_buffer_compare() {
    const BufferPtr bufptr1(buffer1, 8);
    const BufferPtr bufptr2(buffer2, 8);
    const BufferPtr bufptr3(buffer3, 8);
    const BufferPtr bufptr4(buffer4, 8);

    TEST_ASSERT_MESSAGE(bufptr1.isDataEqual(bufptr2) == true,  "Buffers 1 and 2 should be equals");
    TEST_ASSERT_MESSAGE(bufptr1.isDataEqual(bufptr3) == false, "Buffers 1 and 3 should not be equals (data)");
    TEST_ASSERT_MESSAGE(bufptr1.isDataEqual(bufptr4) == false, "Buffers 1 and 3 should not be equals (length A) ");
    TEST_ASSERT_MESSAGE(bufptr4.isDataEqual(bufptr1) == false, "Buffers 1 and 3 should not be equals (length B) ");

}