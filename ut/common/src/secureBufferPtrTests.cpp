/**
 * @file secureBufferPtrTests.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Buffer.h>
#include "unity.h"
#include <cstdint>
#include <time.h>
#include "math.h"

#include <stdio.h>

#define TAB_SIZE    1000000
#define REPEAT_COPY 128

// degrees of freedom: 2*REPEAT_COPY-2
#define PVALUE_LIM  2.0

using namespace GostCrypt;

static uint8_t buffer1[TAB_SIZE] = {0};

static uint8_t buffer2[TAB_SIZE] = {0};

static uint8_t buffer3[TAB_SIZE] = {0};



void test_securebufferptr_compare() {
    clock_t T0_start, T0_end;
    clock_t T1_start, T1_end;

    clock_t T0_measures[REPEAT_COPY] = {0};
    clock_t T1_measures[REPEAT_COPY] = {0};

    // setting up buffers (differences)
    buffer2[0] = 0xFF; // difference at beginning
    buffer3[TAB_SIZE-1] = 0xFF; // difference at end

    SecureBufferPtr bufptr1(buffer1, sizeof(buffer1));
    SecureBufferPtr bufptr2(buffer2, sizeof(buffer2));
    SecureBufferPtr bufptr3(buffer3, sizeof(buffer3));

    // constant-time comparison test
    // to make it obvious, an epic worst case scenario where the first and last byte are different in a HUGE buffer is chosen

    for(int i=0; i<REPEAT_COPY; i++) {
        /* T0 tests */
        T0_start = clock();
        bufptr1.isDataEqual(bufptr2);
        T0_end = clock();
        T0_measures[i] = T0_end - T0_start;

        /* T1 tests */
        T1_start = clock();
        bufptr1.isDataEqual(bufptr3);
        T1_end = clock();
        T1_measures[i] = T1_end - T1_start;
    }

    /* this computing is not perfect but will do for now
     *
     * We want to know if the dataset T1 has any differences with T0
     */

    double T0_std=0, T1_std=0;
    double T0_mean=0, T1_mean=0;

    // computing mean
    for(int i=0; i<REPEAT_COPY; i++) {
        T0_mean += T0_measures[i]/REPEAT_COPY;
        T1_mean += T1_measures[i]/REPEAT_COPY;
    }

    // computing variance and std
    for(int i=0; i<REPEAT_COPY; i++) {
        T0_std += ((T0_measures[i]-T0_mean)*(T0_measures[i]-T0_mean))/(REPEAT_COPY-1);
        T1_std += ((T1_measures[i]-T1_mean)*(T1_measures[i]-T1_mean))/(REPEAT_COPY-1);
    }

    /* using variance for now */
    //T0_std = sqrt(T0_std);
    //T1_std = sqrt(T1_std);

    /* Computing Student t-test to compare branches */
    //printf("T0 | m:%f std:%f\n", T0_mean, T0_std);
    //printf("T1 | m:%f std:%f\n", T1_mean, T1_std);

    /* Checking if STDs are cloe enough */
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE((4.0-0.25)/2.0, (4.0+0.25)/2.0, T0_std/T1_std, "Standard deviations are too different.");

    /* Computing Pooled standard deviation */
    double sp = sqrt( (T0_std+T1_std)/2 );
    //printf("std0:%f std1:%f stdp:%f\n", sqrt(T0_std), sqrt(T1_std), sp);

    /* Computing P-value */
    double pvalue = (T0_mean - T1_mean)/(sp*sqrt((double)2.0/REPEAT_COPY));

    if (pvalue < 0) pvalue *= -1;

    //printf("p-value:%f\n", pvalue);
    TEST_ASSERT_TRUE_MESSAGE(pvalue < PVALUE_LIM,"p-value too high!");

}