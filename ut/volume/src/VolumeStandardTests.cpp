//
// Created by badbouille on 08/11/2020.
//

#include "volumeTests.h"
#include "Buffer.h"
#include "Volume.h"
#include "VolumeStandard.h"
#include "VolumeCreatorContents.h"
#include "unity.h"

using namespace GostCrypt;

void test_volume_standard_open() {

    Volume *v = nullptr;
    SecureBuffer password(16);
    SecureBufferPtr pass(password.get(), password.size());

    pass.erase(); // 0

    v = new VolumeStandard();
    stdtests_volume_open(v, creator_files[0].filename, pass, false, false);
    delete v;

    v = new VolumeStandard();
    stdtests_volume_open(v, creator_files[1].filename, pass, true, false);
    delete v;
}


