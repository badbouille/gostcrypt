//
// Created by badbouille on 02/05/2020.
//

#ifndef _VOLUME_STANDARD_H
#define _VOLUME_STANDARD_H

#include <cstddef>
#include <Buffer.h>
#include "Volume.h"
#include "VolumeStandardHeader.h"
#include "../../crypto/ince/DiskEncryptionAlgorithm.h"
#include "../../crypto/ince/Hash.h"

//         ------ VOLUME DESCRIPTION ------
//  _______________________________________________
// |               STANDARD HEADER                 |
// |________________  192 Bytes  __________________|
// |           UNUSED SPACE (RANDOM BYTES)         |
// |__  192 Bytes (Hypothetical Hidden header) ____|
// |                                               |
// |                                               |
// |                                               |
// |            STANDARD VOLUME SPACE              |
// |          ( header.dataSize Bytes )            |
// |                                               |
// |                                               |
// |_______________________________________________|
// |           UNUSED SPACE (RANDOM BYTES)         |
// |___  192 Bytes (Hypothetical Hidden backup) ___|
// |           STANDARD BACKUP HEADER              |
// |________________  192 Bytes  __________________|

#define STANDARD_VOLUME_FILL_SIZE 16

#define STANDARD_VOLUME_RWBUFFER_SIZE (32*32)

namespace GostCrypt
{
    class VolumeStandard : public Volume
    {
    public:

        // ----- INIT -----
        VolumeStandard() : rwBufferSectorNum(0), rwBuffer(nullptr), EA(nullptr), kdf(nullptr), container(nullptr) {};
        ~VolumeStandard() { close(); delete container; };

        /**
         * Opens a standard volume.
         *
         * Steps:
         *  - The header is read (backup header too in case of error).
         *  - Every DiskEncryptionAlgorithm is tested with every KeyDerivationAlgorithm on the password to decrypt the header
         *  - If the decrypted header can be parsed, the Algorithm is found and the password is good! See #VolumeStandardHeader::Deserialize()
         *  - Informations from the header (salt, masterkey, data area, sector size) are used to set up local buffers
         *  - Ready!
         */
        bool open(Container *source, SecureBufferPtr password) override;

        /**
         * @brief Creates a standard volume
         *
         * Steps:
         *  - Parameters are parsed (algorithm ID, kdfID, sector size, etc)
         *  - Header is created and written to disk at correct offsets: Header at the beginning, Backup header at the end
         *  - Random data is written across disk
         *  - Fake headers are written on the disk -> There must be no noticeable difference between a Standard and StandardHidden Volume
         *  - Written file is closed then reopened using VolumeStandard::open(), to ensure everything was done correctly.
         *  - Ready! (from open)
         */
        void create(Container *source, size_t datasize, std::string algorithmID, std::string kdfID, size_t sectorsize, SecureBufferPtr password) override;

        // ----- RUNTIME -----
        // Filesystem interface
        /**
         * @brief Writes a chunk of data to the volume
         *
         * The data might not be aligned with sectors on the disk.
         * In this case, this method decrypts the needed sectors in its local buffer,
         * copies the data at the right offset and reencrypts it.
         * Aligned and unaligned writing of any kind are authorized, But please note that in any way,
         * aligned writing is ALWAYS faster, since no decryption is needed.
         */
        void write(SecureBufferPtr buffer, size_t offset) override;

        /**
         * @brief Reads a chunk of data from the volume
         *
         * The data might not be aligned with sectors on the disk.
         * In this case, this method decrypts the needed sectors in its local buffer,
         * and copies only the needed data back to the user.
         * Aligned and unaligned writing of any kind are authorized,
         * but reading one byte will ALWAYS need to unecrypt the whole sector of the byte.
         */
        void read(SecureBufferPtr buffer, size_t offset) override;

        // Core interface (control)
        void close() override;

        std::string GetName() const override { return "Standard Volume"; };
        std::string GetID() const override { return "standard"; };
        std::string GetDescription() const override { return "Standard Volume stores your files using the chosen "
                                                             "encryption algorithm in an encrypted file on the disk.\n"
                                                             "The password is used to encrypt the header. This header "
                                                             "contains the master key, a key stronger than the password "
                                                             "which encrypts the rest of the volume."; };

        // Core interface (display)
        std::string getAlgorithmName() const override;
        std::string getAlgorithmID() const override;
        std::string getAlgorithmDescription() const override;
        std::string getKdfName() const override;
        std::string getKdfID() const override;
        std::string getKdfDescription() const override;
        std::string getVolumeSource() const override;

        size_t getSize() const override;

    protected:

        /**
         * Function to setup the whole class from a decrypted header stored in #VolumeStandard::header
         * Will setup: R/W Buffer, Encryption Algorithm, etc
         * @param algorithm the algorithm used to decrypt the header: Same algorithm as the one used to decrypt the content
         * @param pkdf the kdf used to decrypt the header: only used at this step.
         */
        virtual void setUpVolumeFromHeader(DiskEncryptionAlgorithm *algorithm, KDF *pkdf);

        /**
         * Function to get normal header offset. (Overriden in StandardVolumeHidden to use a different header location)
         * @return the normal header offset
         */
        virtual size_t getHeaderOffset() { return 0; };

        /**
         * Function to get backup header offset. (Overriden in StandardVolumeHidden to use a different header location)
         * @return the backup header offset
         */
        virtual size_t getHeaderOffsetBackup() { return -STANDARD_HEADER_SIZE; };

        /**
         * Header of this volume. Contains all the necessary data to understand the layout.
         */
        VolumeStandardHeader header;

        /**
         * Encryption Algorithm of this volume. Loaded with the master key, and used on the fly for encryption/decryption
         */
        DiskEncryptionAlgorithm *EA;

        /**
         * Source of this volume. Given by the user, this is the target of the volume.
         * @warning Needs to be deleted by this class, not the user.
         */
        Container *container;

        /**
         * Key Derivation Function of this volume. Not really used after the volume is opened.
         */
        KDF *kdf;

        /**
         * Buffer used in reading/writing operations
         *  - Stores complete sectors for decryption when user buffer asks only for a few bytes
         *  - Aligned, permits faster decryption times if the algorithm is well-implemented.
         */
        SecureBuffer *rwBuffer;

        /**
         * Number of sectors the buffer can hold at once.
         * Is computed using user's sector size and #STANDARD_VOLUME_RWBUFFER_SIZE.
         * The rwBuffer can hold multiple sectors at once to ease the number of read/write operations
         */
        size_t rwBufferSectorNum;

    };

}


#endif //_VOLUME_STANDARD_H
