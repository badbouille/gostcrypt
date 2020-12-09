//
// Created by badbouille on 07/12/2020.
//

#ifndef _HASHXOR0_H
#define _HASHXOR0_H

#include <Hash.h>
#include "GostCryptException.h"

namespace GostCrypt
{
    template<uint32_t SIZE>
    class HashXOR0 : public Hash
    {
    public:
        HashXOR0() : digest(SIZE), pdigest(digest.get(), digest.size()), offset(0) {};


        void Process(const SecureBufferPtr &data) override;
        void GetDigest(SecureBufferPtr &result) override { if(result.size() != SIZE) throw INVALIDPARAMETEREXCEPTION("Wrong input buffer size"); result.copyFrom(pdigest); };
        void Reset() override { pdigest.erase(); offset = 0; };

        size_t GetDigestSize() const override { return SIZE; };

        std::string GetName() const override { return "XOR-0Padding-"+std::to_string(SIZE); };
        std::string GetID() const override { return "XOR0-"+std::to_string(SIZE); };
        std::string GetDescription() const override { return "XOR-0Padding (XOR0) hash function is a very simple hash "
                                                             "That XORs all given data on a 0-filled buffer."
                                                             "This is a very simple hash that should only be used for test purposes. "
                                                             "\n"
                                                             "When data is bigger than the digest size, the function acts as a compression "
                                                             "where all subsequent blocks are XORed together."
                                                             "\n"
                                                             "When data is smaller than the digest size, the function acts as a 0-padding "
                                                             "function that simply appends zeroes to the end of the string."; };

    protected:
        uint32_t offset;
        SecureBuffer digest;
        SecureBufferPtr pdigest;

    };

}

/* Inclusion of all possible implementations of this template */
#include "HashXOR0Impl.h"

#endif //_HASHXOR0_H
