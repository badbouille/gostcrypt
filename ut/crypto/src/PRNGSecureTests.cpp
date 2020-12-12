//
// Created by badbouille on 12/12/2020.
//

#include "PRNGSecure.h"
#include "HashXOR0.h"
#include "unity.h"
#include <string>
#include <cryptoTests.h>

using namespace GostCrypt;

void test_prng_prngsecure_basic() {
    // TODO maybe change used Hash. Note: why is name/id not hash-dependent?
    Hash * h = new HashXOR0<16>();
    PRNG * p = new PRNGSecure(h, 4);

    stdtest_prng_basic(p, "Secure PRNG", "CSPRNG");

    delete p;
    delete h;
}

void test_prng_prngsecure_entropy() {
    Hash * h = new HashXOR0<16>();
    PRNG * p = new PRNGSecure(h, 4);

    stdtest_prng_entropy(p, 100, 0.01f);

    delete p;
    delete h;
}
