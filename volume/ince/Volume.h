//
// Created by badbouille on 02/05/2020.
//

#ifndef _VOLUME_H
#define _VOLUME_H

#include <cstddef>
#include <list>
#include <memory>
#include <Buffer.h>
#include <fstream>

namespace GostCrypt
{

    class Volume;

    typedef std::list<Volume*> VolumeList;

    /**
     * @brief Abstract structure used to define a Volume, the entity representing an encrypted memory space.
     *
     * A volume is usually a file, consisting of an (encrypted) header describing how the data is stored in it.
     * It represents the layer between a filesystem and the disk it writes on. The volume is divided into
     * independently encrypted sectors.
     *
     * Default implementation is #GostCrypt::VolumeStandard, which is a simple encrypted volume.
     * This class may lead to few other implementations in the future:
     * - VolumeStandardHidden, which is a volume stored inside the standard one, to enforce deniability of the presence of the volume.
     * - VolumeLegacyGOST and VolumeLegacyVeracrypt, for compatibility with old versions of this program
     * - VolumeOnline, an implementation that uses an url instead of a file: no need for a local media
     * - VolumeSteganographic, a volume entirely written inside a video, audio, or image file using steganography techniques on top of encryption.
     * - ..., This class is abstract, implement your own!
     */
    class Volume
    {
    public:

        // ----- INIT -----
        Volume() = default;
        ~Volume() = default;

        /**
         * @brief Function to open an existing volume from a file on the disk.
         *
         * What is done depends on the implementation of this class, please see #GostCrypt::VolumeStandard for an example.
         *
         * @param file the path of the volume file to open
         * @param password the user's password used to open the volume
         * @return a boolean indicating the success (or not) of this action
         */
        virtual bool open(std::string file, SecureBufferPtr password) = 0;

        /**
         * @brief Function to create a volume on the disk.
         *
         * What is done depends on the implementation of this class, please see #GostCrypt::VolumeStandard for an example.
         *
         * @param file the path of the file to create. Note that some implementations might take a directory or even a url instead.
         * @param datasize Exact size of the volume to create. Must be a multiple of sectorsize.
         * @param algorithmID ID of the algorithm to use. Algorithm must be in #DiskEncryptionAlgorithm::GetAvailableAlgorithms(). See #DiskEncryptionAlgorithm::GetID() for details.
         * @param sectorsize size of a sector on the disk. It's the smallest size that can be read from the disk.
         * Cannot be smaller than the BlockCipher's block size. Some algorithms only allow multiples of the cipher's blocksize.
         * @param password User's password to use to open this volume
         */
        virtual void create(std::string file, size_t datasize, std::string algorithmID, size_t sectorsize, SecureBufferPtr password) = 0;

        // ----- RUNTIME -----
        // Filesystem interface
        /**
         * @brief Function to write plain data to the disk.
         *
         * This function supports all kinds of parameters, and has to figure out how to write only a part of a sector, etc.
         * Having parameters aligned on the sector size might still speed up the process a little.
         * For example, writing to only a part of a sector means the Volume has to decrypt the sector,
         * to change only the concerned bytes before re-encrypting the sector and writing it on the disk.
         *
         * @param buffer data to encrypt and write on disk
         * @param offset offset to write to on the disk. 0 = start of data area, not start of volume file.
         */
        virtual void write(SecureBufferPtr buffer, size_t offset) = 0;

        /**
         * @brief Function to read plain data from the disk.
         *
         * This function supports all kinds of parameters, and has to figure out how to read only a part of a sector, etc.
         * Having parameters aligned on the sector size might still speed up the process a little.
         *
         * @param buffer the buffer to fill with data from the disk
         * @param offset offset to read from the disk. 0 = start of data area, not start of volume file.
         */
        virtual void read(SecureBufferPtr buffer, size_t offset) = 0;

        // Core interface (control)
        /**
         * Closes the file, deletes the read buffer and the stored keys.
         */
        virtual void close() = 0;

        /**
         * Static function to get the list of available volume types
         * They are uninitialized and need a call to open or create.
         * @return A list of all available volume types
         */
        static VolumeList GetAvailableVolumeTypes();


        // Core interface (static display)
        /**
         * Function to get the name of this implementation of Volume, in a fancy and displayable way.
         * @return name of the volume type
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the volume type. Usually just the name of the class without 'Volume'
         * @return The id of this volume type
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small description of this volume type for the user.
         * @return description of this volume type.
         */
        virtual std::string GetDescription() const = 0;

        // Core interface (display while running)
        /**
         * Returns information about the current algorithm used in this Volume
         * @return Algorithm's name
         */
        virtual std::string getAlgorithmName() const = 0;

        /**
         * Returns information about the current algorithm used in this Volume
         * @return Algorithm's ID
         */
        virtual std::string getAlgorithmID() const = 0;

        /**
         * Returns information about the current algorithm used in this Volume
         * @return Algorithm's description
         */
        virtual std::string getAlgorithmDescription() const = 0;

        /**
         * Returns the source of the data of this volume.
         * Usually a file but can be a folder, url, etc. It is only for UI purposes.
         * @return the volume's source path
         */
        virtual std::string getVolumeSource() const = 0;

        /**
         * Returns the size in bytes of this Volume
         * @return Size in bytes
         */
        virtual size_t getSize() const = 0;

    };

}


#endif //_BLOCKCIPHER_H
