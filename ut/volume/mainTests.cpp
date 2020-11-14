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

int main(int argc, char **argv) {
    UNITY_BEGIN();

    /* VolumeStandard tests */
    RUN_TEST(test_volume_standard_open);
    RUN_TEST(test_volume_standard_read);
    RUN_TEST(test_volume_standard_write);
    RUN_TEST(test_volume_standard_create);
    RUN_TEST(test_volume_standard_checks);

    return UNITY_END();
}