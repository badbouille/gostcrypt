//
// Created by badbouille on 10/12/2020.
//

#ifndef _PRNGSYSTEM_H
#define _PRNGSYSTEM_H

#include <Buffer.h>
#include <fstream>
#include "PRNG.h"

namespace GostCrypt
{
    class PRNGSystem : public PRNG
    {
    public:
        PRNGSystem();
        ~PRNGSystem();

        void Get(SecureBufferPtr &data) override;

        std::string GetName() const override { return "System PRNG"; };
        std::string GetID() const override { return "SPRNG"; };
        std::string GetDescription() const override { return "The System PRNG extracts random values from the pseudo-random "
                                                            "generator of the current system, (/dev/urandom on linux). "
                                                            "This may not be a secure way of generating random numbers, "
                                                            "but a very fast way of generating random data across the disk."; };
    protected:
        std::ifstream randsource;
    };

}


#endif //_PRNGSYSTEM_H
