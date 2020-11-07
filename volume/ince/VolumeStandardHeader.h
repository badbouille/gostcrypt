//
// Created by badbouille on 02/05/2020.
//

#ifndef _VOLUME_STANDARD_HEADER_H
#define _VOLUME_STANDARD_HEADER_H

#include <cstddef>
#include <Buffer.h>
#include "Volume.h"

// -- HEADER DESCRIPTION WITH DEFAULT CONSTANTS --

// | 'G' | 'O' | 'S' | 'T' | Header Version 32bits |
// |_ Min program Version _|_  Header CRC 32bits  _|
// |___        Sector Size       ( 64bits )     ___|
// |___        Data Start Offset ( 64bits )     ___|
// |___        Data Size         ( 64bits )     ___|
// |           Reserved for future updates         |
// |                 ( 192 bits )                  |
// |_______________________________________________|
// |                                               |
// |                                               |
// |                                               |
// |             M A S T E R    K E Y              |
// |         ( 5 1 2   b i t s   m a x )           |
// |                                               |
// |                                               |
// |_______________________________________________|
// |                                               |
// |                                               |
// |                                               |
// |                   S A L T                     |
// |         ( 5 1 2   b i t s   m a x )           |
// |                                               |
// |                                               |
// |_______________________________________________|

// Total : 192 Bytes / 1536 bits

#define STANDARD_HEADER_PREFIX "GOST"

#define STANDARD_HEADER_VERSION 0x1
#define STANDARD_MIN_PROG_VERSION 0x1

#define STANDARD_HEADER_MASTER_KEY_START 64
#define STANDARD_HEADER_MASTER_KEY_AREASIZE 64

#define STANDARD_HEADER_SALT_START 128
#define STANDARD_HEADER_SALT_AREASIZE 64

#define STANDARD_HEADER_SIZE 192

namespace GostCrypt
{
    /**
     * Standard Header used by VolumeStandard and VolumeStandardHidden
     */
    class VolumeStandardHeader
    {
    public:

        VolumeStandardHeader() : headerVersion(STANDARD_HEADER_VERSION),
                                minProgramVersion(STANDARD_MIN_PROG_VERSION),
                                sectorsize(0),
                                dataStartOffset(0),
                                dataSize(0),
                                masterkey(STANDARD_HEADER_MASTER_KEY_AREASIZE),
                                salt(STANDARD_HEADER_SALT_AREASIZE) {};

        void Serialize(SecureBufferPtr &dest);
        static void SerializeFake(SecureBufferPtr &dest);
        bool Deserialize(const SecureBufferPtr &src);

        // -- HEADER CONTENT --
        // public : this class can even be a struct

        uint32_t headerVersion;
        uint32_t minProgramVersion;

        size_t sectorsize;
        size_t dataStartOffset;
        size_t dataSize;

        SecureBuffer masterkey;
        SecureBuffer salt;

    };

}


#endif //_VOLUME_STANDARD_HEADER_H
