//
// Created by badbouille on 02/05/2020.
//

#include <VolumeStandardHeader.h>
#include <PRNGSystem.h>

void GostCrypt::VolumeStandardHeader::Serialize(GostCrypt::SecureBufferPtr &dest)
{
    uint8_t *dataptr = dest.get();
    SecureBufferPtr tempBufferPtr;
    uint32_t temp32bit;
    uint64_t temp64bit;
    uint32_t *crc_offset;

    // Input params

    if (dest.size() != STANDARD_HEADER_SIZE) {
        throw INVALIDPARAMETEREXCEPTION("Header size is too short! Should be TOTAL_HEADER_SIZE.");
    }

    // Prefix (32bits)
    tempBufferPtr.set((const uint8_t *)(STANDARD_HEADER_PREFIX), sizeof(STANDARD_HEADER_PREFIX));
    dest.copyFrom(tempBufferPtr);
    dataptr += 4;

    // Header Version + Min program version (2x32 bits)
    // TODO : watch out for alignment ?

    temp32bit = htobe32(headerVersion);
    *((uint32_t*)dataptr) = temp32bit;
    dataptr += 4;

    temp32bit = htobe32(minProgramVersion);
    *((uint32_t*)dataptr) = temp32bit;
    dataptr += 4;

    // CRC (save for later)
    crc_offset = (uint32_t *)dataptr;
    dataptr += 4;

    // Sector size, data start, data size
    temp64bit = htobe64(sectorsize);
    *((uint64_t*)dataptr) = temp64bit;
    dataptr += 8;

    temp64bit = htobe64(dataStartOffset);
    *((uint64_t*)dataptr) = temp64bit;
    dataptr += 8;

    temp64bit = htobe64(dataSize);
    *((uint64_t*)dataptr) = temp64bit;
    dataptr += 8;

    // Skipping reserved area
    temp64bit = 0;
    *((uint64_t*)dataptr) = temp64bit;
    dataptr += 8;
    *((uint64_t*)dataptr) = temp64bit;
    dataptr += 8;
    *((uint64_t*)dataptr) = temp64bit;
    dataptr += 8;

    // master key

    dest.getRange(tempBufferPtr, STANDARD_HEADER_MASTER_KEY_START, STANDARD_HEADER_MASTER_KEY_AREASIZE);
    tempBufferPtr.erase();
    tempBufferPtr.copyFrom((BufferPtr)masterkey);

    // salt
    dest.getRange(tempBufferPtr, STANDARD_HEADER_SALT_START, STANDARD_HEADER_SALT_AREASIZE);
    tempBufferPtr.erase();
    tempBufferPtr.copyFrom((BufferPtr)salt);

    // CRC computation

    // TODO add crc module to compute this.
    // Compute it on firts 64 bytes of input + both secure buffers (post copy) to ensure no errors.
    temp32bit = htobe32(0x01020304);
    *crc_offset = temp32bit;

}


bool GostCrypt::VolumeStandardHeader::Deserialize(const GostCrypt::SecureBufferPtr &src)
{
    const uint8_t *dataptr = src.get();
    SecureBufferPtr tempBufferPtr;
    uint32_t temp32bit;
    uint64_t temp64bit;

    // Input params

    if (src.size() != STANDARD_HEADER_SIZE) {
        throw INVALIDPARAMETEREXCEPTION("Header size is too short! Should be TOTAL_HEADER_SIZE.");
    }

    // Prefix (32bits)

    if (memcmp(dataptr, STANDARD_HEADER_PREFIX, sizeof(STANDARD_HEADER_PREFIX)) != 0) {
        // no exception when a header doesn't match since it's very common. we simply return false.
        return false;
    }

    dataptr += 4;

    // Header Version + Min program version (2x32 bits)
    // TODO : watch out for alignment ?

    temp32bit = *((uint32_t*)dataptr);
    headerVersion = be32toh(temp32bit);
    dataptr += 4;

    temp32bit = *((uint32_t*)dataptr);
    minProgramVersion = be32toh(temp32bit);
    dataptr += 4;

    // since this is version 1, there is no header older than this one : headerVersion is useless for now.

    if (minProgramVersion > STANDARD_MIN_PROG_VERSION) {
        // program has been updated, and this update tells us specifically that we shouldn't read this volume.
        throw PROGRAMTOOOLDEXCEPTION();
    }

    // CRC (save for later)
    temp32bit = be32toh(*((uint32_t*)dataptr));
    dataptr += 4;

    // Sector size, data start, data size
    temp64bit = *((uint64_t*)dataptr);
    sectorsize = be64toh(temp64bit);
    dataptr += 8;

    if (((uint64_t)sectorsize) != be64toh(temp64bit)) {
        throw UNSUPPORED64BITSEXCEPTION();
    }

    temp64bit = *((uint64_t*)dataptr);
    dataStartOffset = be64toh(temp64bit);
    dataptr += 8;

    if (((uint64_t)dataStartOffset) != be64toh(temp64bit)) {
        throw UNSUPPORED64BITSEXCEPTION();
    }

    temp64bit = *((uint64_t*)dataptr);
    dataSize = be64toh(temp64bit);
    dataptr += 8;

    if (((uint64_t)dataSize) != be64toh(temp64bit)) {
        throw UNSUPPORED64BITSEXCEPTION();
    }

    // Skipping reserved area
    dataptr += 8*3;

    // master key
    src.getRange(tempBufferPtr, STANDARD_HEADER_MASTER_KEY_START, STANDARD_HEADER_MASTER_KEY_AREASIZE);
    masterkey.copyFrom(tempBufferPtr);

    // salt
    src.getRange(tempBufferPtr, STANDARD_HEADER_SALT_START, STANDARD_HEADER_SALT_AREASIZE);
    salt.copyFrom(tempBufferPtr);

    // CRC computation

    // TODO add crc module to compute and verify this.
    // Compute it on firts 64 bytes of input + both secure buffers (post copy) to ensure no errors.

    // success of input parsing
    return true;
}

void GostCrypt::VolumeStandardHeader::SerializeFake(GostCrypt::SecureBufferPtr &dest)
{
    VolumeStandardHeader fakeHeader;

    fakeHeader.headerVersion = STANDARD_HEADER_VERSION;
    fakeHeader.minProgramVersion = STANDARD_MIN_PROG_VERSION;

    fakeHeader.sectorsize = 512;
    fakeHeader.dataStartOffset = 2*STANDARD_HEADER_SIZE;
    fakeHeader.dataSize = 100*STANDARD_HEADER_SIZE;

    PRNGSystem fastprng;
    SecureBufferPtr sptr;

    fakeHeader.masterkey.getRange(sptr, 0, fakeHeader.masterkey.size());
    fastprng.Get(sptr);

    fakeHeader.salt.getRange(sptr, 0, fakeHeader.salt.size());
    fastprng.Get(sptr);

    return fakeHeader.Serialize(dest);

}

