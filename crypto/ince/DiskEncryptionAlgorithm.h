
#ifndef _ENCRYPTIONALGORITHM_H_
#define _ENCRYPTIONALGORITHM_H_

#include <list>
#include <memory>
#include "Buffer.h"
#include "GostCryptException.h"

namespace GostCrypt
{

    class DiskEncryptionAlgorithm;

    typedef std::list<std::shared_ptr<DiskEncryptionAlgorithm>> DiskEncryptionAlgorithmList;

    /**
     * @brief Abstract disk encryption algorithm class used to encrypt files, or just abstract buffers of data.
     *
     * This class is meant to be the highest level interface for the crypto library.
     * At this level, the user doesn't have to care about buffer size or anything,
     * we just want to choose an algorithm and apply it on the data.
     */
    class DiskEncryptionAlgorithm
    {
    public:
        /**
         * Default constructor. Nothing is initialised and everything is deprecated.
         */
        DiskEncryptionAlgorithm() : Deprecated(true), currentState(NOT_INIT) {};
        virtual ~DiskEncryptionAlgorithm() = default;

        /**
         * Function to set the key of an algorithm.
         * If this is enough, the algorithm will pass in "ready" mode, and ecryption will be possible.
         * @param key the key of size #GetKeySize() to pass to this algorithm.
         */
        virtual void SetKey(SecureBufferPtr &key) = 0;

        /**
         * Function to decrypt data of a sector. Will call the basic decryption function on every sector
         * @param data a sector to decrypt. Must be exactly of size sectorCount*sectorSize
         * @param sectorIndex the index of the first sector contained in this buffer.
         * @param sectorCount number of sectors contained in this buffer
         * @param sectorSize size of one sector
         */
        virtual void Decrypt(SecureBufferPtr &data, size_t sectorIndex, size_t sectorCount, size_t sectorSize) const;

        /**
         * Function to decrypt data. Buffer can be of any size (greater than bloc cipher size).
         * This function is only called in cases when Sectors are not important, for example in headers, test vectors, etc.
         * @warning This function should only be called in cases where sectors are not relevant.
         * @param data Buffer containing data to decrypt.
         */
        virtual void Decrypt(SecureBufferPtr &data) const { return Decrypt(data, 0); };

        /**
         * Function to decrypt a sector.
         * This function is called on every full sector to decrypt parts of the volume.
         * It can also be called on test vectors, headers, or anything, assuming a sectorIndex of 0.
         * @param data Buffer containing data to decrypt.
         * @param sectorIndex The index of this sector. Sometimes used to generate an initialisation vector
         */
        virtual void Decrypt(SecureBufferPtr &data, size_t sectorIndex) const = 0;

        /**
         * Function to encrypt data of a sector. Will call the basic encryption function on every sector
         * @param data a sector to encrypt. Must be exactly of size sectorCount*sectorSize
         * @param sectorIndex the index of the first sector contained in this buffer.
         * @param sectorCount number of sectors contained in this buffer
         * @param sectorSize size of one sector
         */
        virtual void Encrypt(SecureBufferPtr &data, size_t sectorIndex, size_t sectorCount, size_t sectorSize) const;

        /**
         * Function to encrypt data. Buffer can be of any size (greater than bloc cipher size).
         * This function is only called in cases when Sectors are not important, for example in headers, test vectors, etc.
         * @warning This function should only be called in cases where sectors are not relevant.
         * @param data Buffer containing data to encrypt.
         */
        virtual void Encrypt(SecureBufferPtr &data) const { return Encrypt(data, 0); };

        /**
         * Function to encrypt a sector.
         * This function is called on every full sector to encrypt parts of the volume.
         * It can also be called on test vectors, headers, or anything, assuming a sectorIndex of 0.
         * @param data Buffer containing data to encrypt.
         * @param sectorIndex The index of this sector. Sometimes used to generate an initialisation vector
         */
        virtual void Encrypt(SecureBufferPtr &data, size_t sectorIndex) const = 0;

        /**
         * Static function to get the list of available algorithms
         * They are initialised but SetKey is not called, meaning they all are in NOT_INIT state
         * @return A list of all available algorithms
         */
        static DiskEncryptionAlgorithmList GetAvailableAlgorithms();

        /**
         * Function to get Key size of this algorithm.
         * Usually, key size is equal to the the underlying block cipher key size,
         * but in some cases, more keys are requested to encrypt more than one time the data
         * @return The size of the needed key for this algorithm.
         */
        virtual size_t GetKeySize() const = 0;

        /**
         * Function to get the name of the algorithm, in a fancy and displayable way.
         * @return name of the algorithm
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the algorithm. Something like 'CBC_{AES128}'
         * @return The id of this algorithm
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small description of this algorithm for the user.
         * @return description of this algorithm.
         */
        virtual std::string GetDescription() const = 0;

        /**
         * Function to know if this algorithm is deprecated.
         * The user should not (and can not) create a volume with a deprecated algorithm.
         * However, this algorithm might still be used to open an old volume (old volumes are still supported)
         * @return Deprecated value
         */
        bool IsDeprecated() const
        { return Deprecated; } // TODO : Mix of Mode depreciation and Cipher depreciation

        // deleted methods
        DiskEncryptionAlgorithm(const DiskEncryptionAlgorithm &) = delete;
        DiskEncryptionAlgorithm &operator=(const DiskEncryptionAlgorithm &) = delete;

    protected:

        /**
         * State of the algorithm. Default is NOT_INIT.
         * Uninitialized algorithms might be used to get description, ID, name, etc..
         * Initialized algorithms are ready to encrypt, but also more sensitive, since a confidential key is loaded in their memory.
         */
        typedef enum {
            NOT_INIT,
            READY
        } AlgoState;

        /** Current state of the algorithm */
        AlgoState currentState;

        /** Deprecated value of this algorithm. Should not change during execution. */
        bool Deprecated;

        /** Assertion used to check if this algorithm is ready to encrypt or decrypt. */
        void assertInit() const { if(currentState == NOT_INIT) throw ALGORITHMUNITITILIZEDEXCEPTION(); }
    };

}


#endif //_ENCRYPTIONALGORITHM_H_
