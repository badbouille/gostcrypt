/**
 * @file PRNGSystemTests.cpp
 * @author badbouille
 * @date 12/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "PRNGSystem.h"
#include "unity.h"
#include <string>
#include <cryptoTests.h>

using namespace GostCrypt;

void test_prng_prngsystem_basic() {
    PRNG * p = new PRNGSystem();

    stdtest_prng_basic(p, "System PRNG", "SPRNG");

    delete p;
}

void test_prng_prngsystem_entropy() {
    PRNG * p = new PRNGSystem();

    stdtest_prng_entropy(p, 100, 0.01f);

    delete p;

}
