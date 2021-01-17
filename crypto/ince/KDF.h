//
// Created by badbouille on 13/01/2021.
//

#ifndef _KDF_H
#define _KDF_H

#include <Buffer.h>
#include <list>

namespace GostCrypt
{
    class KDF;

    typedef std::list<KDF*> KDFList;

    /**
     * @brief Class template to implement a Key Derivation Function.
     * Key derivation functions are functions used to derivate a low entropy password,
     * using a salt to generate a secure key of a target size (that will fit the target algorithm nicely)
     */
    class KDF
    {
    public:
        KDF() = default;
        virtual ~KDF() = default;

        /**
         * @brief This function derivates a key using a salt, and stores the result in key
         * @param[in] password The user password (probably ASCII characters)
         * @param[in] salt random bytes probably given along with the target encrypted data
         * @param[in,out] key the generated key this function will return (set this buffer size accordingly with the algorithm key size).
         */
        virtual void Derivate(const SecureBufferPtr &password, const SecureBufferPtr &salt, SecureBufferPtr &key) = 0;

        /**
         * Static function to get the list of available KDF functions
         * @return A list of all available KDFs
         */
        static KDFList GetAvailableKDFs();

        /**
         * Function to get the name of the KDF
         * @return name as a string
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the KDF
         * @return id as a string
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small, user-friendly description of this KDF.
         * @return description as a string
         */
        virtual std::string GetDescription() const = 0;

    protected:

    };

}


#endif //_KDF_H
