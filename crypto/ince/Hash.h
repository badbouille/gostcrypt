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

    class Hash
    {
    public:
        Hash() = default;
        virtual ~Hash() = default;

        /**
         * @brief This method processes the given data. Multiple calls to this function can be made before GetDigest is called.
         * @param data the data to pass through the function
         */
        virtual void Process(const SecureBufferPtr &data) = 0;

        /**
         * @brief This method returns the computed digest from the data given to Hash::Process
         * @param[out] an empty buffer of size Hash::GetDigestSize
         */
        virtual void GetDigest(SecureBufferPtr &digest) = 0;

        /**
         * @brief This method resets the processed data to nothing.
         * Used when a user wants to reuse the algorithm without having to initialise the class
         */
        virtual void Reset() = 0;

        /**
         * Static function to get the list of available Hash functions
         * @return A list of all available hashes
         */
        static HashList GetAvailableHashes();

        /**
         * Function to get the digest size of this Hash function.
         * @return The hash's digest size
         */
        virtual size_t GetDigestSize() const = 0;

        /**
         * Function to get the block size of this Hash function.
         * The block size is the size of the underlying state of the function.
         * May be used by other constructions, like HMAC for example.
         * @return The hash's block size
         */
        virtual size_t GetBlockSize() const = 0;

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
