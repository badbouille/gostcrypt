//
// Created by badbouille on 08/11/2020.
//

#include "Buffer.h"
#include "Volume.h"
#include "unity.h"

using namespace GostCrypt;

void stdtests_volume_open(Volume *v, const std::string& file, SecureBufferPtr &password, bool expectedresult, bool expectedexception) {

    bool opened = false;

    try {
        opened = v->open(file, password);

        if (opened && !expectedresult) {
            TEST_FAIL_MESSAGE("Volume successfully opened but it shouldn't have");
        }

        if (!opened && expectedresult) {
            TEST_FAIL_MESSAGE("A well-formed volume could not be opened");
        }

    } catch (GostCryptException &e) {
        if (!expectedexception) {
            TEST_FAIL_MESSAGE(e.what());
        }
    }

}



