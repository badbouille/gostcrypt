/**
 * @file PRNGSystem.cpp
 * @author badbouille
 * @date 10/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "PRNGSystem.h"

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
