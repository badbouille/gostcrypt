//
// Created by badbouille on 07/12/2020.
//

#ifndef _HASH_H
#define _HASH_H

#include <Buffer.h>
#include <list>

namespace GostCrypt
{
    class Hash;

    typedef std::list<Hash*> HashList;

    /** Key derivation functions are just Hashes honestly, let's not waste time redefining an interface */
    typedef Hash KDF;

    class Hash
    {
    public:
        Hash() {};

        /**
         * @brief This method processes the given data. Multiple calls to this function can be made before GetDigest is called.
         * @param data the data to pass through the function
         */
        virtual void Process(SecureBufferPtr &data) = 0;

        /**
         * @brief This method returns the computed digest from the data given to Hash::Process
         * @param[out] an empty buffer of size Hash::GetDigestSize
         */
        virtual void GetDigest(SecureBufferPtr &digest) = 0;

        /**
         * Static function to get the list of available Hash functions
         * @return A list of all available hashes
         */
        static HashList GetAvailableHashes();

        /**
         * Function to get the digest size of this Hash function.
         * @return The BlockCipher's block size
         */
        virtual size_t GetDigestSize() const = 0;

        /**
         * Function to get the name of the Hash (ex: SHA-256)
         * @return name as a string
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the Hash (ex: SHA256)
         * @return id as a string
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small, user-friendly description of this hash.
         * @return description as a string
         */
        virtual std::string GetDescription() const = 0;

    protected:

    };

}


#endif //_HASH_H
