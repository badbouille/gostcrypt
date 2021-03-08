/**
 * @file PRNGSystem.cpp
 * @author badbouille
 * @date 10/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "PRNGSystem.h"

/* Linux implementation using "/dev/urandom" */
#ifdef PLATFORM_LINUX

#define RANDOM_FILE "/dev/urandom"

GostCrypt::PRNGSystem::PRNGSystem()
{
    randsource.open(RANDOM_FILE, std::ios::in | std::ios::binary);
    if (!randsource.is_open()) {
        throw FILENOTFOUNDEXCEPTION(RANDOM_FILE);
    }
}

GostCrypt::PRNGSystem::~PRNGSystem()
{
    randsource.close();
}

void GostCrypt::PRNGSystem::Get(GostCrypt::SecureBufferPtr &data)
{
    if (data.size() == 0) {
        return;
    }
    if (data.get() == nullptr) {
        throw INVALIDPARAMETEREXCEPTION("empty buffer");
    }
    randsource.read((char *)data.get(), data.size());
}

#endif

/* Windows implementation using rand_s */
#ifdef PLATFORM_WINDOWS

GostCrypt::PRNGSystem::PRNGSystem() {}

GostCrypt::PRNGSystem::~PRNGSystem() {}

void GostCrypt::PRNGSystem::Get(GostCrypt::SecureBufferPtr &data)
{
    uint32_t i = 0;
    uint32_t randomvalue;
    uint8_t randomvalue8[4];
    if (!(((size_t)data.get()) & 0x03)) { // alignement
        for ( ; i < (data.size()/4); i++ ) {
            if (rand_s(&randomvalue) != 0) {
                throw GOSTCRYPTEXCEPTION("rand_s failed to acquire data");
            }
            ((uint32_t *)data.get())[i] = randomvalue;
        }
        i*=4;
    }
    for ( ; i < data.size(); i+=4 ) {
        if ( (i%4 == 0) && (rand_s((unsigned int *)&randomvalue8) != 0)) {
            throw GOSTCRYPTEXCEPTION("rand_s failed to acquire data");
        }
        data.get()[i  ] = randomvalue8[0];
        if (i+1 >= data.size()) break;
        data.get()[i+1] = randomvalue8[0];
        if (i+2 >= data.size()) break;
        data.get()[i+2] = randomvalue8[0];
        if (i+3 >= data.size()) break;
        data.get()[i+3] = randomvalue8[0];
    }
    randomvalue = 0;
    randomvalue8[0] = randomvalue8[1] = randomvalue8[2] = randomvalue8[3] = 0;
}

#endif
