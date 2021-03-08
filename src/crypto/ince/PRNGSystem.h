/**
 * @file PRNGSystem.h
 * @author badbouille
 * @date 10/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _PRNGSYSTEM_H
#define _PRNGSYSTEM_H

#ifdef PLATFORM_WINDOWS
#define _CRT_RAND_S
#include <stdlib.h>
#endif

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
                                                            "generator of the current system, (/dev/urandom on linux, rand_s on windows). "
                                                            "This may not be a secure way of generating random numbers, "
                                                            "but a very fast way of generating random data across the disk."; };
#ifdef PLATFORM_LINUX
    protected:
        std::ifstream randsource;
#endif
    };

}


#endif //_PRNGSYSTEM_H
