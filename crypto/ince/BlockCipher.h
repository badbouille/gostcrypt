//
// Created by badbouille on 08/08/2019.
//

#ifndef _BLOCKCIPHER_H
#define _BLOCKCIPHER_H

#include <cstddef>
#include <Buffer.h>

namespace GostCrypt
{
    class BlockCipher
    {
    public:
        BlockCipher() : currentState(NOT_INIT) {};

        /**
         * @brief This function uses the passed key to initialises its context
         * @param key the key of size #GetKeySize()
         */
        virtual void SetKey(SecureBufferPtr &key) = 0;

        /**
         * Encrypts a single block
         * @param buffer of size #GetBlockSize()
         */
        virtual void Encrypt(SecureBufferPtr &buffer) const = 0;

        /**
         * Decrypts a single block
         * @param buffer of size #GetBlockSize()
         */
        virtual void Decrypt(SecureBufferPtr &buffer) const = 0;

        /**
         * Function to get the Block size of the cipher to use as input size when encrypting and decrypting
         * @return The BlockCipher's block size
         */
        virtual size_t GetBlockSize() const = 0;

        /**
         * Function to get the key size of this cipher
         * @return The BlockCipher's key size
         */
        virtual size_t GetKeySize() const = 0;

        /**
         * Function to get the ctx size of this cipher (amount of sensitive memory needed for it to run)
         * @return The BlockCipher's ctx size
         */
        virtual size_t GetCtxSize() const = 0;

        /**
         * Function to get the name of the cipher (ex: AES 128bits)
         * @return name as a string
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the cipher (ex: AES128)
         * @return id as a string
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small, user-friendly description of this cipher.
         * @return description as a string
         */
        virtual std::string GetDescription() const = 0;

    protected:

        /**
         * State of the cipher. Default is NOT_INIT.
         * Uninitialized ciphers might be used to get description, ID, name, etc..
         * Initialized ciphers are ready to encrypt, but also more sensitive, since a confidential key is loaded in their memory.
         */
        typedef enum {
            NOT_INIT,
            READY
        } AlgoState;

        /** Current state of the algorithm */
        AlgoState currentState;

        /** Assertion used to check if this cipher is ready to encrypt or decrypt. */
        void assertInit() const { if(currentState == NOT_INIT) throw ALGORITHMUNITITILIZEDEXCEPTION(); }
    };

}


#endif //_BLOCKCIPHER_H
