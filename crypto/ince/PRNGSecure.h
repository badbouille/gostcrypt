//
// Created by badbouille on 10/12/2020.
//

#ifndef _PRNGSECURE_H
#define _PRNGSECURE_H

#include <Buffer.h>
#include <fstream>
#include "PRNG.h"
#include "Hash.h"
#include "PRNGSystem.h"

namespace GostCrypt
{
    class PRNGSecure : PRNGSystem
    {
    public:
        PRNGSecure(Hash *hash, uint32_t factor);
        ~PRNGSecure();

        void Get(SecureBufferPtr &data) override;

        std::string GetName() const override { return "Secure PRNG"; };
        std::string GetID() const override { return "CSPRNG"; };
        std::string GetDescription() const override { return "The Secure PRNG extracts random values from the pseudo-random "
                                                            "generator of the current system. This data is considered random but "
                                                            "with low entropy. The generator gets N times the amount of needed data "
                                                            "before passing it through a hash function to get a perfectly random output."; };
    protected:
        SecureBuffer *randombuffer;
        SecureBufferPtr prandombuffer;
        Hash *h;
        uint32_t compressionFactor;
    };

}

#endif //_PRNGSECURE_H
