/**
 * @file mainTests.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "commonTests.h"
#include "unity.h"

void setUp(void) {
    // nothing to setup
}

void tearDown(void) {
    // nothing to remove
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_bufferptr_compare);
    RUN_TEST(test_bufferptr_mutability);
    RUN_TEST(test_bufferptr_copyfrom);
    RUN_TEST(test_bufferptr_erase);
    RUN_TEST(test_bufferptr_swap);
    RUN_TEST(test_bufferptr_xor);
    RUN_TEST(test_bufferptr_illegalinit);
    RUN_TEST(test_bufferptr_range);
    RUN_TEST(test_securebufferptr_compare);
    RUN_TEST(test_shared_rw);
    RUN_TEST(test_progress_callback);
    RUN_TEST(test_progress_master);
    RUN_TEST(test_progress_shm);
    return UNITY_END();
}