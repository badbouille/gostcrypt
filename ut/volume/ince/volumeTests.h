//
// Created by badbouille on 08/11/2020.
//

#ifndef _VOLUMETESTS_H
#define _VOLUMETESTS_H

#include "Buffer.h"
#include "Volume.h"

/* Generic tests */
void stdtests_volume_open(GostCrypt::Volume *v, const std::string& file, GostCrypt::SecureBufferPtr &password, bool expectedresult, bool expectedexception);
void stdtests_volume_read(GostCrypt::Volume *v, size_t sectorsize, size_t volumesize, const uint8_t *volumecontent);

/* Volume standard tests */
void test_volume_standard_open();
void test_volume_standard_read();

#endif //_VOLUMETESTS_H
