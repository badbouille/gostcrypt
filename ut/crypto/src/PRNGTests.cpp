//
// Created by badbouille on 12/12/2020.
//

#include "PRNG.h"
#include "unity.h"
#include <string>

using namespace GostCrypt;

void stdtest_prng_basic(PRNG *p, const std::string& name, const std::string& id) {

    TEST_ASSERT_EQUAL_STRING_MESSAGE(p->GetName().c_str(), name.c_str(), "PRNG name is not the one expected");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(p->GetID().c_str(), id.c_str(), "PRNG ID is not the one expected");

}

void stdtest_prng_entropy(PRNG *p, size_t cyclecount, float precision) {

    SecureBuffer source(64);
    SecureBufferPtr psource(source.get(), source.size());

    uint32_t count[256] = {0};

    for(int i=0; i < cyclecount; i++) {
        // getting alea
        p->Get(psource);

        // counting values
        for(int j=0; j < psource.size(); j++) {
            count[psource.get()[j]] += 1;
        }

    }

    // computing entropy
    float entropy = 0;
    for(unsigned int i : count) {
        if(i == 0)
            continue;
        float prob = (float)i / (float)(cyclecount * 64);
        entropy -= prob*log2(prob);
    }

    entropy /= 8;

    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(precision, 1.0f, entropy, "Entropy too low!");

}
