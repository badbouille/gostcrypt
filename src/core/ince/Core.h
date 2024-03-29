/**
 * @file Core.h
 * @author badbouille
 * @date 06/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _CORE_H
#define _CORE_H


#include <Volume.h>
#include <KDF.h>
#include "DiskEncryptionAlgorithm.h"
#include "FuseFileSystem.h"
#include "Progress.h"

namespace GostCrypt
{

    class Core
    {
    public:

        // Structures (for long requests)

        /**
         * @brief structure holding the parameters for a Core::mount request
         */
        typedef struct MountParams {
            std::string volumePath; /** Path of the file to mount (must be a valid gostcrypt file) */
            std::string mountPoint; /** Folder where to mount it (will not be created) */
            SecureBufferPtr password; /** Pointer on a secure buffer where the password is stored */
            std::string fileSystemID; /** ID of the filesystem used by the volume (how to interpred the unciphered content) */
            // keyfiles
            // readonly ?
            // customHeader or backup or other header from file
        } MountParams_t;

        /**
         * @brief structure holding the parameters for a Core::create request
         */
        typedef struct CreateParams {
            MountParams_t afterCreationMount; /** Mount params to mount the volume (creation also does a mount request after) */
            std::string volumePath; /** Path of the file to create (may be a device file) */
            SecureBufferPtr password; /** Password used to create the volume */
            // keyfiles
            size_t dataSize; /** Size of the CONTENT of the volume. Can be zero if file exists */
            size_t sectorSize; /** Size of every sector of the volume. */
            std::string algorithmID; /** Algorithm to use to encrypt the volume */
            std::string keyDerivationFunctionID; /** Key derivation function used to compute the password */
            std::string volumeTypeID; /** Type of volume to use */
        } CreateParams_t;

        /**
         * @brief Structure used to give info about the mounted volumes
         */
        typedef struct VolumeInfo {
            std::string file; /** file mounted */
            std::string mountPoint; /** mountPoint of the volume */
            std::string algorithmID; /** Algorithm used to encrypt the volume */
            std::string keyDerivationFunctionID; /** Key derivation function used to compute the password */
            std::string volumeTypeID; /** Type of volume */
            size_t dataSize; /** Size of this volume */
        } VolumeInfo_t;

        /**
         * @brief Structure to return the currently mounted volumes
         */
        typedef std::list<VolumeInfo_t> VolumeInfoList;

        // everything static. Only one instance of this class exists
        Core() = delete;

        // Function to catch exec-ed requests
        static int main_api_handler(int argc, char **argv);

        // Getting info
        static DiskEncryptionAlgorithmList GetEncryptionAlgorithms();
        static KDFList GetDerivationFunctions();
        static VolumeList GetVolumeTypes();
        static FuseFileSystemList GetFileSystems();

        // MountPoints management
        static void mount(MountParams_t *p);
        static void directmount(MountParams_t *p); /* Allowed only in detached process */
        static void umount(std::string mountPoint);
        static void umountAll();
        static void create(CreateParams_t *p);
        static void directcreate(CreateParams_t *p); /* Allowed only in detached process */
        static VolumeInfoList list();

        // Volume management

        /**
         * Current object used to report progress by Core.
         * Can be altered anytime to add a callback, a master, etc
         */
        static Progress progress;

    private:

        // Path of the current program
        static const char *g_prog_path;

        // Tool functions
        static VolumeInfo readVolumeInfo(std::string gostinfofile);

    };

}


#endif //_CORE_H
