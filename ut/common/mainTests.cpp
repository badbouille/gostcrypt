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
    RUN_TEST(test_buffer_compare);
    return UNITY_END();
}