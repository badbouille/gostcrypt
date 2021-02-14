/**
 * @file MACHMACTests.cpp
 * @author badbouille
 * @date 17/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <MAC.h>
#include "unity.h"
#include <string>
#include "HMAC.h"
#include <MACTests.h>
#include "cryptoTests.h"

using namespace GostCrypt;

#define NUM_TEST_VECTORS 3

// TODO: note that testing HMAC with our 'test hash' XOR-0 might be the dumbest shit ever
//       THE KEY HAS NO EFFECT IN ANY WAY ON THE DIGEST, IT JUST XORS ITSELF FFS.

const MACTestData256 mac_hmac_testvectors[NUM_TEST_VECTORS] = {
    {
        .key      = {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf},
        .input    = {0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a},
        .expected = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        .keysize      = 16,
        .inputsize    = 16,
    },
    {
        .key      = {0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36},
        .input    = {0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                     0x5c, 0x5c, 0x5c },
        .expected = {0x6a, 0x6a, 0x6a, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36},
        .keysize      = 13,
        .inputsize    = 19,
    },
    {
        .key      = { 0 },
        .input    = { 0 },
        .expected = {0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a},
        .keysize      = 0,
        .inputsize    = 0,
    }
};

void test_mac_hmac_basic() {

    MAC *m = new HMAC<HashXOR0<16>>();

    stdtest_mac_basic(m, 16, 16, "HMAC XOR-0Padding-16", "HMAC-XOR0-16");

    delete m;
}

void test_mac_hmac_process() {

    MAC *m = new HMAC<HashXOR0<16>>();

    stdtest_mac_process(m, NUM_TEST_VECTORS, mac_hmac_testvectors);

    delete m;
}

void test_mac_hmac_processmult() {

    MAC *m = new HMAC<HashXOR0<16>>();

    stdtest_mac_processmult(m, NUM_TEST_VECTORS, mac_hmac_testvectors);

    delete m;
}

void test_mac_hmac_sizechecks() {

    MAC *m = new HMAC<HashXOR0<16>>();

    stdtest_mac_sizechecks(m);

    delete m;
}