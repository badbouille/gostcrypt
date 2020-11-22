//
// Created by badbouille on 06/05/2020.
//

#ifndef _CORE_H
#define _CORE_H


#include <Volume.h>
#include "DiskEncryptionAlgorithm.h"
#include "FuseFileSystem.h"

namespace GostCrypt
{

    class Core
    {
    public:

        // Structures (for long requests)
        typedef struct MountParams {
            std::string volumePath;
            std::string mountPoint;
            SecureBufferPtr password;
            std::string fileSystemID;
            // keyfiles
            // readonly ?
            // customHeader or backup or other header from file
        } MountParams_t;

        typedef struct CreateParams {
            MountParams_t afterCreationMount;
            std::string volumePath;
            SecureBufferPtr password;
            // keyfiles
            size_t dataSize;
            size_t sectorSize;
            std::string algorithmID;
            std::string keyDerivationFunctionID;
            std::string volumeTypeID;
        } CreateParams_t;

        // everything static. Only one instance of this class exists
        Core() = delete;

        // Getting info
        static DiskEncryptionAlgorithmList GetEncryptionAlgorithms();
        static DiskEncryptionAlgorithmList GetDerivationFunctions();
        static VolumeList GetVolumeTypes();
        static FuseFileSystemList GetFileSystems();

        // MountPoints management
        static void mount(MountParams_t *p);
        static void umount(std::string mountPoint);
        static void create(CreateParams_t *p);

        // Volume management

    };

}


#endif //_BLOCKCIPHER_H
