/**
 * @file sharedTests.cpp
 * @author badbouille
 * @date 14/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "platform/shared.h"
#include "unity.h"


using namespace GostCrypt;

void test_shared_rw() {
    uint32_t value1 = 0, value2 = 0;
    int err = 0;

    // Creating shared memory object
    SharedWindow<uint32_t> shm1_w(146);
    SharedWindow<uint32_t> shm2_w(147);
    SharedWindow<uint32_t> shm2_r(147);
    SharedWindow<uint32_t> shm1_r(146);

    // Simple Read/Write
    shm1_w.put(12);
    shm1_r.get(&value1);
    err = shm1_r.get(&value2);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, err, "Read operation was allowed after the number of readable elements");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(12, value1, "Simple read/write error");

    // Multiple Read/Write
    shm1_w.put(789);
    shm1_w.put(456);
    shm1_w.put(123);
    shm1_r.get(&value1);
    shm1_r.get(&value2);
    shm1_r.get(&value1);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(123, value1, "Multiple read/write error (1)");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(456, value2, "Multiple read/write error (2)");

    // Looped Read/Write
    for(uint32_t i = 0; i < GostCrypt::SharedWindow<uint32_t>::ELEMENT_NUMBER*2+2; i++)
    {
        shm1_w.put(1000000 + i);
    }
    shm1_r.get(&value1);
    shm1_r.get(&value2);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1000002+GostCrypt::SharedWindow<uint32_t>::ELEMENT_NUMBER, value1, "Could not read oldest value from window (1)");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1000003+GostCrypt::SharedWindow<uint32_t>::ELEMENT_NUMBER, value2, "Could not read oldest value from window (2)");

    // Separated Read/Write
    /* Pre-reads to 'free' the entire array */
    for(uint32_t i = 0; i < GostCrypt::SharedWindow<uint32_t>::ELEMENT_NUMBER; i++)
    {
        shm1_r.get(&value1);
        shm2_r.get(&value1);
    }
    shm2_w.put(20000);
    shm1_w.put(30000);
    shm1_r.get(&value1);
    shm2_r.get(&value2);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(30000, value1, "Could not read/write multiple SHMs at once (1)");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(20000, value2, "Could not read/write multiple SHMs at once (2)");
}