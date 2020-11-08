//
// Created by badbouille on 08/11/2020.
//

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
    RUN_TEST(test_bufferptr_illegalinit);
    RUN_TEST(test_bufferptr_range);
    return UNITY_END();
}