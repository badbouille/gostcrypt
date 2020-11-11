//
// Created by badbouille on 08/11/2020.
//

#include "volumeTests.h"
#include "VolumeCreator.h"
#include "unity.h"

void setUp(void) {
    // creating volumes for analysis
    stdtests_createvolumes();
}

void tearDown(void) {
    // deleting volumes on cleanup
    // note: delete this to keep volumes for debug purposes
    stdtests_deletevolumes();
}

void test_ignore(void) {
    TEST_IGNORE();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_ignore);

    return UNITY_END();
}