//
// Created by badbouille on 10/12/2020.
//

#include "PRNGSecure.h"

GostCrypt::PRNGSecure::PRNGSecure(Hash *hash, uint32_t factor)
{
    h = hash;
    compressionFactor = factor;

    if (hash == nullptr) {
        throw INVALIDPARAMETEREXCEPTION("empty hash");
    }

    if (factor < 1 || factor > 1000) {
        throw INVALIDPARAMETEREXCEPTION("Impossible compressionfactor out of [1-1000]");
    }

    randombuffer = new SecureBuffer(h->GetDigestSize());
    prandombuffer.set(randombuffer->get(), randombuffer->size());
}

GostCrypt::PRNGSecure::~PRNGSecure()
{
    delete randombuffer;
}

void GostCrypt::PRNGSecure::Get(GostCrypt::SecureBufferPtr &data)
{
    uint32_t i,j;
    /* For every block of data to get */
    for (i=0; i < data.size(); i+=randombuffer->size()) {

        /* Resetting Hash */
        h->Reset();

        /* Processing N times random values in Hash function */
        for (j=0; j < compressionFactor; j++) {
            PRNGSystem::Get(prandombuffer);
            h->Process(prandombuffer);
        }

        /* Getting final digest */
        h->GetDigest(prandombuffer);

        /* Copying secure random numbers in user buffer */
        for (j=0; j < std::min(randombuffer->size(), data.size() - i); j++) {
            data.get()[i+j] = prandombuffer.get()[j];
        }
    }

    /* Erasing Local buffer and hash */
    prandombuffer.erase();
    h->Reset();
}
