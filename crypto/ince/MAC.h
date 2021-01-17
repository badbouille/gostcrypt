//
// Created by badbouille on 13/01/2021.
//

#ifndef _MAC_H
#define _MAC_H

#include <Buffer.h>
#include <list>

namespace GostCrypt
{

    class MAC;

    typedef std::list<MAC*> MACList;

    class MAC
    {
    public:
        MAC() : currentState(NOT_INIT) {};
        virtual ~MAC() = default;

        /**
         * @brief This function uses the passed key to initialises its context
         * @param key the key of arbitrary size (no GetKeySize() for this class)
         */
        virtual void SetKey(const SecureBufferPtr &key) = 0;

        /**
         * @brief This method processes the given data. Multiple calls to this function can be made before GetDigest is called.
         * @param data the data to pass through the function
         */
        virtual void Process(const SecureBufferPtr &data) = 0;

        /**
         * @brief This method returns the computed digest from the data given to MAC::Process
         * @param[out] an empty buffer of size MAC::GetDigestSize to be filled with the digest
         */
        virtual void GetDigest(SecureBufferPtr &digest) = 0;

        /**
         * @brief This method resets the processed data to nothing.
         * Used when a user wants to reuse the algorithm without having to initialise the class.
         */
        virtual void Reset() = 0;

        /**
         * Static function to get the list of available MAC algorithms
         * @return A list of all available MACs
         */
        static MACList GetAvailableMACs();

        /**
         * Function to get the digest size of this MAC function.
         * @return The MAC's digest size
         */
        virtual size_t GetDigestSize() const = 0;

        /**
         * Function to get the block size of this MAC function.
         * The block size is the size of the underlying state of the function.
         * May be used by other constructions, like HMAC / Pbkdf for example.
         * @return The MAC's block size
         */
        virtual size_t GetBlockSize() const = 0;

        /**
         * Function to get the name of the MAC (ex: HMAC SHA-256)
         * @return name as a string
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the MAC (ex: HMAC-SHA256)
         * @return id as a string
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small, user-friendly description of this MAC.
         * @return description as a string
         */
        virtual std::string GetDescription() const = 0;

    protected:
        /**
         * State of the MAC. Default is NOT_INIT.
         * Uninitialized MACs might be used to get description, ID, name, etc..
         * Initialized MACs are ready to process data, but also more sensitive, since a confidential key is loaded in their memory.
         */
        typedef enum {
            NOT_INIT,
            READY
        } AlgoState;

        /** Current state of the algorithm */
        AlgoState currentState;

        /** Assertion used to check if this hash is ready to process data. */
        void assertInit() const { if(currentState == NOT_INIT) throw ALGORITHMUNITITILIZEDEXCEPTION(); }

    };

}


#endif //_HMAC_H
