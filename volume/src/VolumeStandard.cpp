//
// Created by badbouille on 02/05/2020.
//

#include <VolumeStandard.h>
#include <iostream>
#include <utility>
#include "../../crypto/ince/DiskEncryptionAlgorithm.h"
#include "../../crypto/ince/Hash.h"
#include "../../crypto/ince/PRNGSecure.h"
#include <cstring>

/** Compression factor of the CSPRNG: How many bytes of system random for one byte of secure random */
#define PRNG_COMPRESSION_FACTOR 4

bool GostCrypt::VolumeStandard::open(std::string file, GostCrypt::SecureBufferPtr password)
{
    SecureBuffer encryptedHeader(STANDARD_HEADER_SIZE);
    SecureBuffer tempDecryptedHeader(STANDARD_HEADER_SIZE);
    SecureBufferPtr encryptedHeaderPtr;
    SecureBufferPtr tempDecryptedHeaderPtr;
    DiskEncryptionAlgorithm * algorithm = nullptr;
    KDF * kdf = nullptr;
    bool headerRead = false;

    // getting pointers on secure buffers
    encryptedHeader.getRange(encryptedHeaderPtr, 0, STANDARD_HEADER_SIZE);
    tempDecryptedHeader.getRange(tempDecryptedHeaderPtr, 0, STANDARD_HEADER_SIZE);

    // TODO : error handling
    // opening volume in readonly mode
    volumefile.open(file, std::ios::binary | std::ios::in | std::ios::out);

    // remembering requested file
    volumefilepath = file;

    // going to header offset (usually 0 : beginning of file)
    volumefile.seekg(getHeaderOffset(), std::ios::beg);
    volumefile.read((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    // Getting list of all available algorithms to open the volume
    DiskEncryptionAlgorithmList algorithmList = GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
    KDFList kdfList = GostCrypt::KDF::GetAvailableHashes();
    for (auto & algorithmIterator : algorithmList) {
        algorithm = algorithmIterator;

        SecureBuffer derivedKey(algorithm->GetKeySize());
        SecureBufferPtr derivedKeyPtr(derivedKey.get(), derivedKey.size());

        for (auto & kdfIterator : kdfList) {
            kdf = kdfIterator;

            /* Checking key size against digest size */
            if (kdf->GetDigestSize() != algorithm->GetKeySize()) {
                /* in this case, this kdf can not be used with this algorithm,
                 * since it doesn't generate keys fit for the algorithm. */
                // TODO maybe check for algorithms that do not have any fitting hash
                continue;
            }

            /* Copying the header to try to decrypt it "in-place" */
            tempDecryptedHeaderPtr.copyFrom(encryptedHeaderPtr);

            /* Deriving Key using KDF */
            kdf->Reset();
            kdf->Process(password);
            kdf->GetDigest(derivedKeyPtr);

            /* initialisation of algorithm with user password */
            algorithm->SetKey(derivedKeyPtr);

            /* trying to decrypt header */
            algorithm->Decrypt(tempDecryptedHeaderPtr);

            /* trying to read header */
            if (header.Deserialize(tempDecryptedHeaderPtr))
            {
                // Worked! Header was successfully decrypted
                headerRead = true;
                break;
            }

            /* didn't work, let's try to open it with the next kdf */
            kdf = nullptr;
        }

        /* Double break to outside the loop when header is read */
        if (headerRead) {
            break;
        }

        // didn't work, let's try to open it with the next algorithm
        algorithm = nullptr;
    }

    // cleaning unused structures
    for (auto & algorithmIterator : algorithmList) {
        if (algorithm != algorithmIterator) {
            delete algorithmIterator;
        }
    }

    for (auto & kdfIterator : kdfList) {
        if (kdf != kdfIterator) {
            delete kdfIterator;
        }
    }

    if(!headerRead) {
        // TODO : try backup Header
        return false;
    }

    // TODO reopening as rw instead of readonly

    // setting up class
    setUpVolumeFromHeader(algorithm, kdf);

    return true;

}

void GostCrypt::VolumeStandard::create(std::string file,
                                       size_t datasize,
                                       std::string algorithmID,
                                       std::string kdfID,
                                       size_t sectorsize,
                                       GostCrypt::SecureBufferPtr password)
{
    DiskEncryptionAlgorithm *algorithm = nullptr;
    bool algorithmFound = false;
    KDF *pkdf = nullptr;
    bool kdfFound = false;
    SecureBuffer encryptedHeader(STANDARD_HEADER_SIZE);
    SecureBufferPtr encryptedHeaderPtr;
    SecureBufferPtr tmpKeyPtr;

    encryptedHeader.getRange(encryptedHeaderPtr, 0, STANDARD_HEADER_SIZE);

    // ----------------  FEASABILITY CHECKS ----------------
    if (datasize % sectorsize != 0) {
        throw INVALIDPARAMETEREXCEPTION("Datasize % sectorsize != 0");
    }

    //  ---------------  OPENING FILE  ---------------
    callback("Opening target file", 0.01);
    // TODO : error handling / checks / readonly
    // TODO : handle device files
    volumefile.open(file, std::fstream::out | std::fstream::binary);

    if (volumefile.fail()) {
        throw CANTCREATEFILEEXCEPTION(file);
    }

    //  ---------------  FINDING ALGORITHM  ---------------
    callback("Finding target algorithm", 0.03);
    DiskEncryptionAlgorithmList algorithmList = GostCrypt::DiskEncryptionAlgorithm::GetAvailableAlgorithms();
    for (auto & algorithmIterator : algorithmList) {
        // Checking ID
        if (algorithmIterator->GetID() == algorithmID) {
            algorithm = algorithmIterator;
            algorithmFound = true;
            break;
        }
    }

    // cleaning unused structures
    for (auto & algorithmIterator : algorithmList) {
        if (algorithm != algorithmIterator) {
            delete algorithmIterator;
        }
    }

    if(!algorithmFound) {
        throw INVALIDPARAMETEREXCEPTION("Given algorithm ID was not found. Is this a supported algorithm?");
    }

    //  ---------------  FINDING KDF  ---------------
    callback("Finding target KDF", 0.05);
    KDFList hList = GostCrypt::KDF::GetAvailableHashes();
    for (auto & hIterator : hList) {
        // Checking ID
        if (hIterator->GetID() == kdfID) {
            pkdf = hIterator;
            kdfFound = true;
            break;
        }
    }

    // cleaning unused structures
    for (auto & hIterator : hList) {
        if (pkdf != hIterator) {
            delete hIterator;
        }
    }

    if(!kdfFound) {
        throw INVALIDPARAMETEREXCEPTION("Given kdf ID was not found. Is this a supported kdf?");
    }

    // Checking if given KDF is fit
    if (pkdf->GetDigestSize() != algorithm->GetKeySize()) {
        // kdf was found but can't generate data fit for the wanted algorithm
        throw INVALIDPARAMETEREXCEPTION("Given kdf has a digest size different from the algorithm's key size.");
    }

    //  ---------------  FILLING AND ENCRYPTING HEADER  ---------------
    callback("Creating header", 0.07);

    header.sectorsize = sectorsize;
    header.dataStartOffset = 2*STANDARD_HEADER_SIZE; // normal + hidden
    header.dataSize = datasize;

    PRNGSecure csprng(pkdf, PRNG_COMPRESSION_FACTOR);
    SecureBufferPtr randTarget;

    randTarget.set(header.masterkey.get(), header.masterkey.size());
    csprng.Get(randTarget);

    randTarget.set(header.salt.get(), header.salt.size());
    csprng.Get(randTarget);

    // fill header
    header.Serialize(encryptedHeaderPtr);

    // deriving given password using the kdf
    SecureBuffer derivedKey(algorithm->GetKeySize());
    SecureBufferPtr derivedKeyPtr(derivedKey.get(), derivedKey.size());

    /* Deriving Key using KDF */
    pkdf->Reset();
    pkdf->Process(password);
    pkdf->GetDigest(derivedKeyPtr);

    // setting password and encrypting header
    algorithm->SetKey(derivedKeyPtr);
    algorithm->Encrypt(encryptedHeaderPtr);

    // Setting up EA
    setUpVolumeFromHeader(algorithm, pkdf);

    //  ---------------  WRITING HEADERS TO DISK  ---------------
    callback("Writing headers to volume", 0.09);

    // standard header
    volumefile.seekg(getHeaderOffset(), std::ios::beg);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    // TODO encrypt with different sector num
    // backup header
    volumefile.seekg(4*STANDARD_HEADER_SIZE + header.dataSize + getHeaderOffsetBackup(), std::ios::beg);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    //  ---------------  Writing random data across data area and encrypting it  ---------------

    // TODO skip step if fast creation
    // TODO watch out for crazy values of sectorsize
    // TODO buffer would be better in this case
    SecureBuffer randomBuffer(header.sectorsize);
    SecureBufferPtr randomBufferPtr;
    randomBuffer.getRange(randomBufferPtr, 0, header.sectorsize);

    PRNGSystem fastprng;

    for (size_t i = 0; i < header.dataSize; i+=header.sectorsize) {
        // TODO Use rwBuffer instead of randomBuffer
        if ((i / header.sectorsize) % 65536 == 0) // modulo to not flood UI
            callback("Writing data across volume", 0.10f + 0.80f*(i/(float)header.dataSize));

        // erasing buffer
        fastprng.Get(randomBufferPtr);

        // encrypting with sector num
        EA->Encrypt(randomBufferPtr, i/header.sectorsize, 1, header.sectorsize);

        // writing sector
        volumefile.seekg(header.dataStartOffset + (i/header.sectorsize)*header.sectorsize);
        volumefile.write((char*)randomBufferPtr.get(), header.sectorsize);
    }

    randomBuffer.erase();

    //  ---------------  WRITING FAKE HEADERS TO VOLUME  ---------------
    callback("Creating and writing fake headers", 0.90);

    // creation and encryption of fake headers
    VolumeStandardHeader::SerializeFake(encryptedHeaderPtr);

    // creating random key to encrypt those differently
    SecureBuffer randomKey(EA->GetKeySize());
    SecureBufferPtr prandomKey(randomKey.get(), randomKey.size());

    // generating random key
    csprng.Get(prandomKey);

    // seting random key and encrypting fake header
    EA->SetKey(prandomKey);
    EA->Encrypt(encryptedHeaderPtr);

    // writing fake headers

    // TODO link those offsets to class VolumeStandardHidden ?

    // standard hidden header
    volumefile.seekg(getHeaderOffset() + STANDARD_HEADER_SIZE, std::ios::beg);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    // TODO encrypt with different sector num
    // backup header
    volumefile.seekg(4*STANDARD_HEADER_SIZE + header.dataSize + getHeaderOffsetBackup() - STANDARD_HEADER_SIZE, std::ios::beg);
    volumefile.write((char *)(encryptedHeaderPtr.get()), STANDARD_HEADER_SIZE);

    //  ---------------  CLOSING AND REOPENING VOLUME  ---------------

    callback("Closing file and reopening it as a volume", 0.92);
    close();
    open(file, password);

    // TODO : check for errors

    // volume is now ready to use
    callback("Done creating volume", 1.0);
}

void GostCrypt::VolumeStandard::setUpVolumeFromHeader(DiskEncryptionAlgorithm *algorithm, KDF *pkdf)
{
    SecureBufferPtr tempKeysPtr;

    // Setting up algorithm
    EA = algorithm;
    header.masterkey.getRange(tempKeysPtr, 0, EA->GetKeySize());
    EA->SetKey(tempKeysPtr);

    // Setting up KDF
    kdf = pkdf;

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

    // TODO : erase rwBuffer after use ?
    // TODO : check if volume exists
    // TODO : check if read operation is possible (in bounds)

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
        memcpy(rwBufferPtr_char, rBufferPtr_char, maxReadable*sectorSize);

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
    if (volumefile.is_open()) {
        volumefile.close();
    }

    // Deleting the algorithm (and keys)
    if (EA) {
        delete EA;
        EA = nullptr;
    }

    // Deleting the KDF
    if (kdf) {
        delete kdf;
        kdf = nullptr;
    }

    // deleting rw buffer
    if (rwBuffer) {
        delete rwBuffer;
        rwBuffer = nullptr;
        rwBufferSectorNum = 0;
    }

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

std::string GostCrypt::VolumeStandard::getKdfName() const
{
    if (kdf == nullptr) throw ALGORITHMUNITITILIZEDEXCEPTION();
    return kdf->GetName();
}

std::string GostCrypt::VolumeStandard::getKdfID() const
{
    if (kdf == nullptr) throw ALGORITHMUNITITILIZEDEXCEPTION();
    return kdf->GetID();
}

std::string GostCrypt::VolumeStandard::getKdfDescription() const
{
    if (kdf == nullptr) throw ALGORITHMUNITITILIZEDEXCEPTION();
    return kdf->GetDescription();
}

size_t GostCrypt::VolumeStandard::getSize() const
{
    return header.dataSize;
}

std::string GostCrypt::VolumeStandard::getVolumeSource() const
{
    return volumefilepath;
}
