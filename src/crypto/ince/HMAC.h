/**
 * @file HMAC.h
 * @author badbouille
 * @date 12/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _HMAC_H
#define _HMAC_H

#include <Buffer.h>
#include <list>
#include "Hash.h"
#include "MAC.h"

#include "HashXOR0.h"

namespace GostCrypt
{

    template <class H>
    class HMAC : public MAC
    {
    public:
        HMAC() : IHash(new H), OHash(new H), initvalue(new SecureBuffer(IHash->GetDigestSize())) {};
        virtual ~HMAC() { delete IHash; delete OHash; };

        /**
         * @brief This function uses the passed key to initialises its context
         * @param key the key of arbitrary size (no GetKeySize() for this class)
         */
        void SetKey(const SecureBufferPtr &key) override;

        /**
         * @brief This method processes the given data. Multiple calls to this function can be made before GetDigest is called.
         * @param data the data to pass through the function
         */
        void Process(const SecureBufferPtr &data) override;

        /**
         * @brief This method returns the computed digest from the data given to Hash::Process
         * @param[out] an empty buffer of size Hash::GetDigestSize
         */
        void GetDigest(SecureBufferPtr &digest) override;

        /**
         * @brief This method resets the processed data to nothing.
         * Used when a user wants to reuse the algorithm without having to initialise the class.
         */
        void Reset() override;

        /**
         * Function to get the digest size of this Hash function.
         * @return The Digest size in bytes
         */
        size_t GetDigestSize() const override { return IHash->GetDigestSize(); };

        /**
         * Function to get the block size of this MAC function.
         * The block size is the size of the underlying state of the function.
         * May be used by other constructions, like HMAC / Pbkdf for example.
         * @return The MAC's block size
         */
        size_t GetBlockSize() const override { return IHash->GetBlockSize(); };

        /**
         * Function to get the name of the HMAC
         * @return name as a string
         */
        std::string GetName() const override { return "HMAC "+IHash->GetName(); };

        /**
         * Function to get the ID of the HMAC
         * @return id as a string
         */
        std::string GetID() const override { return "HMAC-"+IHash->GetID(); };

        /** // TODO add references
         * Function to get a small, user-friendly description of this HMAC.
         * @return description as a string
         */
        std::string GetDescription() const override { return "HMAC is a Keyed-Hash function (or MAC function) "
                                                    "that generates hashes (from the underlying hash function) "
                                                    "and combines them with a secret key."
                                                    "\n\n"
                                                    + IHash->GetDescription(); };

    protected:

        /** First instantiation of the Hash function (applied to ipad / m) */
        Hash *IHash;

        /** Second instantiation of the Hash function (applied to opad / IhashDigest) */
        Hash *OHash;

        /** Saved key, needed to be able to reset the algorithm */
        SecureBuffer *initvalue;

        /** Initialisation from saved key */
        void InitFromCurrentKey();
    };

}

namespace GostCrypt {

    /* explicit instanciation of all existing HMAC classes */

    template class HMAC<HashXOR0<16>>;

}

#endif //_HMAC_H
