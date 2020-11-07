//
// Created by badbouille on 30/04/2020.
//

#ifndef _BLOCKCIPHERXOR_H
#define _BLOCKCIPHERXOR_H

#include <cstddef>
#include "BlockCipher.h"
#include <Buffer.h>

namespace GostCrypt
{
    class BlockCipherXOR : BlockCipher
    {
    public:
        BlockCipherXOR() : ctx(keySize) {};

        virtual void SetKey(SecureBufferPtr &key) { ctx.copyFrom(key); currentState = READY; };
        virtual void Encrypt(SecureBufferPtr &buffer) const;
        virtual void Decrypt(SecureBufferPtr &buffer) const { return Encrypt(buffer); };

        virtual size_t GetBlockSize() const { return keySize; };
        virtual size_t GetKeySize() const { return keySize; };
        virtual size_t GetCtxSize() const { return keySize; };

        virtual std::string GetName() const { return "XOR 128bits"; };
        virtual std::string GetID() const { return "XOR128"; };
        virtual std::string GetDescription() const { return "XOR Cipher is one of the most simple ciphers available."
                                                            "The key is the same size of a block. Decryption is the same"
                                                            "as encryption: the key is XORed with the data for every block."; };

    protected:

        /** The keysize of this algorithm. Default is 128 bits */
        const size_t keySize = 16;

        SecureBuffer ctx;

    };

}


#endif //_BLOCKCIPHERXOR_H
