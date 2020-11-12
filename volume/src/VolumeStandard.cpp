//
// Created by badbouille on 02/05/2020.
//

#include <VolumeStandard.h>
#include <iostream>
#include <utility>
#include "../../crypto/ince/DiskEncryptionAlgorithm.h"
#include <cstring>

bool GostCrypt::VolumeStandard::open(std::string file, GostCrypt::SecureBufferPtr password)
{
    SecureBuffer encryptedHeader(STANDARD_HEADER_SIZE);
    SecureBuffer tempDecryptedHeader(STANDARD_HEADER_SIZE);
    SecureBufferPtr encryptedHeaderPtr;
    SecureBufferPtr tempDecryptedHeaderPtr;
    std::shared_ptr<DiskEncryptionAlgorithm> algorithm(nullptr);
    bool headerRead = false;

    // getting pointers on secure buffers
    encryptedHeader.getRange(encryptedHeaderPtr, 0, STANDARD_HEADER_SIZE);
    tempDecryptedHeader.getRange(tempDecryptedHeaderPtr, 0, STANDARD_HEADER_SIZE);

    // TODO : error handling
    // opening volume in readonly mode
    volumefile.open(file, std::ios::binary | std::ios::in | std::ios::out);

    // going to header offset (usually 0 : beginning of file)
    volumefile.seekg(getHeaderOffset(), std::ios::beg);
    volumefile.read((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    // Getting list of all available algorithms to open the volume
    DiskEncryptionAlgorithmList algorithmList = GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
    for (auto & algorithmIterator : algorithmList) {
        tempDecryptedHeaderPtr.copyFrom(encryptedHeaderPtr);
        algorithm = algorithmIterator;

        // TODO : Add key derivation function !!!!!
        // initialisation of algorithm with user password
        algorithm->SetKey(password);

        // trying to decrypt header
        algorithm->Decrypt(tempDecryptedHeaderPtr);

        // trying to read header
        if (header.Deserialize(tempDecryptedHeaderPtr)) {
            // Worked! Header was successfully decrypted
            headerRead = true;
            break;
        }

        // didn't work, let's try to open it with the next algorithm
    }

    if(!headerRead) {
        // TODO : try backup Header
        return false;
    }

    // TODO reopening as rw instead of readonly

    // setting up class
    setUpVolumeFromHeader(algorithm);

    return true;

}

void GostCrypt::VolumeStandard::create(std::string file,
                                       size_t datasize,
                                       std::string algorithmID,
                                       size_t sectorsize,
                                       GostCrypt::SecureBufferPtr password)
{
    std::shared_ptr<DiskEncryptionAlgorithm> algorithm(nullptr);
    bool algorithmFound = false;
    SecureBuffer encryptedHeader(STANDARD_HEADER_SIZE);
    SecureBufferPtr encryptedHeaderPtr;

    encryptedHeader.getRange(encryptedHeaderPtr, 0, STANDARD_HEADER_SIZE);

    //  ---------------  OPENING FILE  ---------------
    // TODO : error handling / checks / readonly
    volumefile.open(file, std::ios::binary | std::ios::in | std::ios::out);

    //  ---------------  FINDING ALGORITHM  ---------------
    DiskEncryptionAlgorithmList algorithmList = GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
    for (auto & algorithmIterator : algorithmList) {
        algorithm = algorithmIterator;

        // trying to read header
        if (algorithm->GetID() == algorithmID) {
            // Worked! Header was successfully decrypted
            algorithmFound = true;
            break;
        }
    }

    if(!algorithmFound) {
        throw INVALIDPARAMETEREXCEPTION("Given algorithm ID was not found. Is this a supported algorithm?");
    }

    //  ---------------  FILLING AND ENCRYPTING HEADER  ---------------
    header.sectorsize = sectorsize;
    header.dataStartOffset = 2*STANDARD_HEADER_SIZE; // normal + hidden
    header.dataSize = datasize;

    // TODO : use derivation function

    header.masterkey.erase();
    header.masterkey.copyFrom(password);

    // TODO : use PRNG

    header.salt.erase();
    header.salt.copyFrom(password);

    // Setting up EA
    setUpVolumeFromHeader(algorithm);

    // fill header
    header.Serialize(encryptedHeaderPtr);
    EA->Encrypt(encryptedHeaderPtr);

    //  ---------------  WRITING HEADERS TO DISK  ---------------

    // standard header
    volumefile.seekg(getHeaderOffset(), std::ios::beg);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    // backup header
    // TODO: wrong seek! Use real offset since end is not final end
    volumefile.seekg(getHeaderOffsetBackup(), std::ios::end);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    //  ---------------  Writing random data across data area and encrypting it  ---------------

    // TODO use prng across whole disk
    // TODO skip step if fast creation
    SecureBuffer randomBuffer(STANDARD_VOLUME_FILL_SIZE);
    SecureBufferPtr randomBufferPtr;
    randomBuffer.getRange(randomBufferPtr, 0, STANDARD_VOLUME_FILL_SIZE);

    for (size_t i = 0; i < header.dataSize; i+=STANDARD_VOLUME_FILL_SIZE) {
        // TODO filling buffer with random data
        // TODO Use rwBuffer instead of randomBuffer

        // writing buffer
        write(randomBufferPtr, i);
    }


    //  ---------------  WRITING FAKE HEADERS TO VOLUME  ---------------

    // creation and encryption of fake headers
    VolumeStandardHeader::SerializeFake(encryptedHeaderPtr);

    // TODO creating new random key and encrypting area
    //EA->SetKey()
    EA->Encrypt(encryptedHeaderPtr);

    // writing fake headers

    // TODO link those offsets to class VolumeStandardHidden ?

    // standard hidden header
    volumefile.seekg(getHeaderOffset() + STANDARD_HEADER_SIZE, std::ios::beg);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    // backup header
    volumefile.seekg(getHeaderOffsetBackup() - STANDARD_HEADER_SIZE, std::ios::end);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    //  ---------------  CLOSING AND REOPENING VOLUME  ---------------

    close();
    open(file, password);

    // TODO : check for errors

    // volume is now ready to use
}

void GostCrypt::VolumeStandard::setUpVolumeFromHeader(std::shared_ptr<DiskEncryptionAlgorithm> algorithm)
{
    SecureBufferPtr tempKeysPtr;

    // Setting up algorithm
    EA = std::move(algorithm);
    header.masterkey.getRange(tempKeysPtr, 0, EA->GetKeySize());
    EA->SetKey(tempKeysPtr);

    // TODO : do the same with salt

    // TODO: check volume delimiters against whole file size

    // Setting up R/W Buffer
    rwBufferSectorNum = STANDARD_VOLUME_RWBUFFER_SIZE/header.sectorsize;
    if (rwBufferSectorNum >= 1)
    {
        rwBuffer = new SecureBuffer(rwBufferSectorNum * header.sectorsize);
    } else {
        /*
         * This case might happen when using one-sector algorithms.
         * (algorithms that have to decrypt the WHOLE volume everytime anything is written on it)
         * These volumes have only one sector the size of the volume.
         * The writing buffer must be the size of the volume..
         */
        // TODO : put some kind of safe lock on this value (do not allocate huge buffers!)
        rwBufferSectorNum = 1;
        rwBuffer = new SecureBuffer(header.sectorsize);
    }
}

void GostCrypt::VolumeStandard::read(GostCrypt::SecureBufferPtr buffer, size_t offset)
{
    SecureBufferPtr rwBufferPtr;
    SecureBufferPtr wBufferPtr;
    SecureBufferPtr rBufferPtr;
    char * rwBufferPtr_char = (char *)rwBuffer->get();
    char * wBufferPtr_char = nullptr;

    // TODO : erase rwBuffer after use ?
    // TODO : check if volume exists
    // TODO : check if read operation is possible (in bounds)

    // multiple cases
    // Case 1: ..xxx|rrwwwwrr|xxx..
    // Case 2: ..xxx|rrwwwwww|www..
    // Case 3: ..www|wwwwwwrr|xxx..
    // Case 4: ..www|wwwwwwww|www..

    // Example : Read operation (w) impacting 4 sectors with unused data (r)
    // Multiple cases are present
    //
    //                          2x Case 4
    //              Case 2     ____|_____      Case 3
    //                V        V        V        V
    //     ...xxx|rrrwwwww|wwwwwwww|wwwwwwww|wwrrrrrr|xxx...

    // find sectors to decrypt from offset and buffer size
    size_t sectorSize = header.sectorsize;
    size_t sectorIndexBegin = offset / sectorSize;
    size_t sectorIndexEnd = (offset+buffer.size()) / sectorSize;

    // checking if we have to deal with uncomplete sectors
    size_t firstSectorOffset = offset % sectorSize;
    size_t lastSectorOffset = (offset+buffer.size()) % sectorSize;

    // Case 1
    if (sectorIndexBegin == sectorIndexEnd) {
        // reading FIRST sector
        volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
        volumefile.read(rwBufferPtr_char, sectorSize);
        // decrypting sector
        rwBuffer->getRange(rwBufferPtr, 0, sectorSize);
        EA->Decrypt(rwBufferPtr, sectorIndexBegin, 1, sectorSize);
        // getting part to read from, from sector
        rwBufferPtr.getRange(rBufferPtr, firstSectorOffset, buffer.size());
        // copy to user output
        buffer.copyFrom(rBufferPtr);
        // everything written, returning
        return;
    }

    // dealing with incomplete sectors first
    // Case 2
    if (firstSectorOffset != 0) {
        // reading FIRST sector
        volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
        volumefile.read(rwBufferPtr_char, sectorSize);
        // decrypting sector
        rwBuffer->getRange(rwBufferPtr, 0, sectorSize);
        EA->Decrypt(rwBufferPtr, sectorIndexBegin, 1, sectorSize);
        // getting part to read from, from sector
        rwBufferPtr.getRange(rBufferPtr, firstSectorOffset, sectorSize - firstSectorOffset);
        // getting part to write to
        buffer.getRange(wBufferPtr, 0, sectorSize - firstSectorOffset);
        // copy
        wBufferPtr.copyFrom(rBufferPtr);
        // adjusting input to forget what has already be done
        buffer.getRange(buffer, sectorSize - firstSectorOffset, buffer.size() - sectorSize + firstSectorOffset);
        offset += sectorSize - firstSectorOffset;
        sectorIndexBegin += 1;
    }

    // Case 3
    if (lastSectorOffset != 0) {
        // reading LAST sector
        volumefile.seekg(header.dataStartOffset + sectorIndexEnd*sectorSize);
        volumefile.read(rwBufferPtr_char, sectorSize);
        // decrypting sector
        rwBuffer->getRange(rwBufferPtr, 0, sectorSize);
        EA->Decrypt(rwBufferPtr, sectorIndexEnd, 1, sectorSize);
        // getting part to read from, from sector
        rwBufferPtr.getRange(rBufferPtr, 0, lastSectorOffset);
        // getting part to write to
        buffer.getRange(wBufferPtr, buffer.size() - lastSectorOffset, lastSectorOffset);
        // copy
        wBufferPtr.copyFrom(rBufferPtr);
        // adjusting input to forget what has already be done
        buffer.getRange(buffer, 0, buffer.size() - lastSectorOffset);
    }

    // in some cases, everything is done! (case 2 + 3 only)
    if (sectorIndexBegin == sectorIndexEnd) {
        return;
    }

    // Case 4
    // erasing and encrypting above all other sectors
    volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
    wBufferPtr_char = (char *)buffer.get();
    rwBuffer->getRange(rwBufferPtr, 0, rwBuffer->size());
    do {
        size_t maxReadable = sectorIndexEnd - sectorIndexBegin;
        if (maxReadable > rwBufferSectorNum) {
            maxReadable = rwBufferSectorNum;
        }

        // Applying size to rwBufferPtr
        rwBuffer->getRange(rwBufferPtr, 0, maxReadable*sectorSize);

        // copying data into rwBuffer
        volumefile.read(rwBufferPtr_char, maxReadable*sectorSize);

        // Decrypting data
        EA->Decrypt(rwBufferPtr, sectorIndexBegin, maxReadable, sectorSize);

        // copy to output buffer
        memcpy(wBufferPtr_char, rwBufferPtr_char, maxReadable*sectorSize);

        // adjusting input
        sectorIndexBegin += maxReadable;
        wBufferPtr_char += maxReadable*sectorSize;
    } while (sectorIndexBegin != sectorIndexEnd);

    // Finally done!
}

void GostCrypt::VolumeStandard::write(GostCrypt::SecureBufferPtr buffer, size_t offset)
{
    SecureBufferPtr rwBufferPtr;
    SecureBufferPtr wBufferPtr;
    SecureBufferPtr rBufferPtr;
    char * rwBufferPtr_char = (char *)rwBuffer->get();
    char * rBufferPtr_char = nullptr;

    // multiple cases
    // Case 1: xxx|rrrrwwwwrr|xxx
    // Case 2: xxx|rrrrwwwwww|www
    // Case 3: www|wwwwwwwwrr|xxx
    // Case 4: xxx|wwwwwwwwww|xxx

    // Example : Write operation (w) impacting 4 sectors with unchanged data (r)
    // Multiple cases are present, unchanged data must be decrypted then encrypted again
    //
    //                          2x Case 4
    //              Case 2     ____|_____      Case 3
    //                V        V        V        V
    //     ...xxx|rrrwwwww|wwwwwwww|wwwwwwww|wwrrrrrr|xxx...

    // find sectors to decrypt from offset and buffer size
    size_t sectorSize = header.sectorsize;
    size_t sectorIndexBegin = offset / sectorSize;
    size_t sectorIndexEnd = (offset+buffer.size()) / sectorSize;

    // checking if we have to deal with uncomplete sectors
    size_t firstSectorOffset = offset % sectorSize;
    size_t lastSectorOffset = (offset+buffer.size()) % sectorSize;

    // Case 1
    if (sectorIndexBegin == sectorIndexEnd) {
        // reading FIRST sector
        volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
        volumefile.read(rwBufferPtr_char, sectorSize);
        // decrypting sector
        rwBuffer->getRange(rwBufferPtr, 0, sectorSize);
        EA->Decrypt(rwBufferPtr, sectorIndexBegin, 1, sectorSize);
        // getting part to write to from sector
        rwBufferPtr.getRange(wBufferPtr, firstSectorOffset, buffer.size());
        // copy
        wBufferPtr.copyFrom(buffer);
        // encrypting
        EA->Encrypt(rwBufferPtr, sectorIndexBegin, 1, sectorSize);
        // writing sector
        volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
        volumefile.write(rwBufferPtr_char, sectorSize);
        // everything written, returning
        return;
    }

    // dealing with incomplete sectors first
    // Case 2
    if (firstSectorOffset != 0) {
        // reading FIRST sector
        volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
        volumefile.read(rwBufferPtr_char, sectorSize);
        // decrypting sector
        rwBuffer->getRange(rwBufferPtr, 0, sectorSize);
        EA->Decrypt(rwBufferPtr, sectorIndexBegin, 1, sectorSize);
        // getting part to write to from sector
        rwBufferPtr.getRange(wBufferPtr, firstSectorOffset, sectorSize - firstSectorOffset);
        // getting part to read from
        buffer.getRange(rBufferPtr, 0, sectorSize - firstSectorOffset);
        // copy
        wBufferPtr.copyFrom(rBufferPtr);
        // encrypting
        EA->Encrypt(rwBufferPtr, sectorIndexBegin, 1, sectorSize);
        // writing sector
        volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
        volumefile.write(rwBufferPtr_char, sectorSize);
        // adjusting input to forget what has already be done
        buffer.getRange(buffer, sectorSize - firstSectorOffset, buffer.size() - sectorSize + firstSectorOffset);
        offset += sectorSize - firstSectorOffset;
        sectorIndexBegin += 1;
    }

    // Case 3
    if (lastSectorOffset != 0) {
        // reading LAST sector
        volumefile.seekg(header.dataStartOffset + sectorIndexEnd*sectorSize);
        volumefile.read(rwBufferPtr_char, sectorSize);
        // decrypting sector
        rwBuffer->getRange(rwBufferPtr, 0, sectorSize);
        EA->Decrypt(rwBufferPtr, sectorIndexEnd, 1, sectorSize);
        // getting part to write to from sector
        rwBufferPtr.getRange(wBufferPtr, 0, lastSectorOffset);
        // getting part to read from
        buffer.getRange(rBufferPtr, buffer.size() - lastSectorOffset, lastSectorOffset);
        // copy
        wBufferPtr.copyFrom(rBufferPtr);
        // encrypting
        EA->Encrypt(rwBufferPtr, sectorIndexEnd, 1, sectorSize);
        // writing sector
        volumefile.seekg(header.dataStartOffset + sectorIndexEnd*sectorSize);
        volumefile.write(rwBufferPtr_char, sectorSize);
        // adjusting input to forget what has already be done
        buffer.getRange(buffer, 0, buffer.size() - lastSectorOffset);
    }

    // in some cases, everything is done! (case 2 + 3)
    if (sectorIndexBegin == sectorIndexEnd) {
        return;
    }

    // Case 4
    // erasing and encrypting above all other sectors
    volumefile.seekg(header.dataStartOffset + sectorIndexBegin*sectorSize);
    rBufferPtr_char = (char *)buffer.get();
    do {
        size_t maxReadable = sectorIndexEnd - sectorIndexBegin;
        if (maxReadable > rwBufferSectorNum) {
            maxReadable = rwBufferSectorNum;
        }

        // Applying size to rwBufferPtr
        rwBuffer->getRange(rwBufferPtr, 0, maxReadable*sectorSize);

        // copying data into rwBuffer
        memcpy(rBufferPtr_char, rwBufferPtr_char, maxReadable*sectorSize);

        // Encrypting data
        EA->Encrypt(rwBufferPtr, sectorIndexBegin, maxReadable, sectorSize);

        // Writing to disk
        volumefile.write(rwBufferPtr_char, maxReadable*sectorSize);

        // adjusting input
        sectorIndexBegin += maxReadable;
        rBufferPtr_char += maxReadable*sectorSize;
    } while (sectorIndexBegin != sectorIndexEnd);

    // Finally done!
}

void GostCrypt::VolumeStandard::close()
{
    // closing file
    volumefile.close();

    // deleting Algoritm (and all contexts that might contain keys
    delete EA.get();
    EA.reset();

    // deleting rw buffer
    delete rwBuffer;
    rwBuffer = nullptr;
    rwBufferSectorNum = 0;

}

std::string GostCrypt::VolumeStandard::getAlgorithmName() const
{
    if (EA == nullptr) throw ALGORITHMUNITITILIZEDEXCEPTION();
    return EA->GetName();
}

std::string GostCrypt::VolumeStandard::getAlgorithmID() const
{
    if (EA == nullptr) throw ALGORITHMUNITITILIZEDEXCEPTION();
    return EA->GetID();
}

std::string GostCrypt::VolumeStandard::getAlgorithmDescription() const
{
    if (EA == nullptr) throw ALGORITHMUNITITILIZEDEXCEPTION();
    return EA->GetDescription();
}

size_t GostCrypt::VolumeStandard::getSize() const
{
    return header.dataSize;
}
