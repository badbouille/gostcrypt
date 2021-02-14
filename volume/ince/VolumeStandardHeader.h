/**
 * @file VolumeStandardHeader.h
 * @author badbouille
 * @date 02/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _VOLUME_STANDARD_HEADER_H
#define _VOLUME_STANDARD_HEADER_H

#include <cstddef>
#include <Buffer.h>
#include "Volume.h"

// -- HEADER DESCRIPTION WITH DEFAULT CONSTANTS --

// | 'G' | 'O' | 'S' | 'T' | Header Version 32bits |
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
// |                                               |
// |                                               |
// |                                               |
// |                                               |
// |           M A S T E R    K E Y (s)            |
// |         ( 1 0 2 4  b i t s   m a x )          |
// |                                               |
// |                                               |
// |                                               |
// |                                               |
// |                                               |
// |                                               |
// |_______________________________________________|

// Total : 192 Bytes / 1536 bits

#define STANDARD_HEADER_PREFIX "GOST"

#define STANDARD_HEADER_VERSION 0x1
#define STANDARD_MIN_PROG_VERSION 0x1

#define STANDARD_HEADER_MASTER_KEY_START 64
#define STANDARD_HEADER_MASTER_KEY_AREASIZE 64

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
                                masterkey(STANDARD_HEADER_MASTER_KEY_AREASIZE) {};

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
    };

}


#endif //_VOLUME_STANDARD_HEADER_H
