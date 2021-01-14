#include "Buffer.h"

#include <sys/mman.h>

namespace GostCrypt
{

    /* BufferPtr Implementation */

bool BufferPtr::isDataEqual(const BufferPtr& otherbufferptr) const
{
    if (dataSize != otherbufferptr.size())
    {
        return false;
    }
    return memcmp(dataPtrReadonly, otherbufferptr.get(), dataSize) == 0;
}

void BufferPtr::copyFrom(const BufferPtr& bufferPtr)
{
    if (bufferPtr.size() > dataSize)
    {
        throw INVALIDPARAMETEREXCEPTION("bufferPtr.size > DataSize");
    }
    if (!dataPtrMutable)
    {
        throw DATANOTMUTABLEEXCEPTION();
    }
    memcpy(dataPtrMutable, bufferPtr.get(), bufferPtr.size());
}

void BufferPtr::getRange(BufferPtr &dest, size_t offset, size_t size)
{
    if (offset + size > dataSize)
    {
        throw INVALIDPARAMETEREXCEPTION("offset+size > DataSize");
    }
    if (!dataPtrMutable)
    {
        throw DATANOTMUTABLEEXCEPTION();
    }
    dest.set(dataPtrMutable + offset, size);
}

void BufferPtr::getRange(BufferPtr &dest, size_t offset, size_t size) const
{
    if (offset + size > dataSize)
    {
        throw INVALIDPARAMETEREXCEPTION("offset+size > DataSize");
    }
    dest.set(dataPtrReadonly + offset, size);
}

void BufferPtr::swap(BufferPtr &otherBuffer)
{
    size_t length = otherBuffer.size();
    volatile uint8_t * otherbufferptr = otherBuffer.get();
    volatile size_t i = 0;
    if (length > dataSize) length = dataSize;

    if (!dataPtrMutable)
    {
        throw DATANOTMUTABLEEXCEPTION();
    }
    if (length == 0)
    {
        throw INVALIDPARAMETEREXCEPTION("Length can not be null");
    }

    // TODO : 32 and 64 bit optimization ?
    for (; i < length; i++) {
        dataPtrMutable[i] ^= otherbufferptr[i]; // A' = A  ^ B
        otherbufferptr[i] ^= dataPtrMutable[i]; // B' = B  ^ A' = A
        dataPtrMutable[i] ^= otherbufferptr[i]; // A''= A' ^ B' = B
    }
}

void BufferPtr::Xor(BufferPtr &otherBuffer)
{
    size_t length = otherBuffer.size();
    volatile uint8_t * otherbufferptr = otherBuffer.get();
    volatile size_t i = 0;
    if (length > dataSize) length = dataSize;

    if (!dataPtrMutable)
    {
        throw DATANOTMUTABLEEXCEPTION();
    }
    if (length == 0)
    {
        throw INVALIDPARAMETEREXCEPTION("Length can not be null");
    }

    // TODO : 32 and 64 bit optimization ?
    for (; i < length; i++) {
        dataPtrMutable[i] ^= otherbufferptr[i]; // A' = A ^ B
    }
}

/* SecureBufferPtr Implementation */

    bool SecureBufferPtr::isDataEqual(const BufferPtr &otherbufferptr) const
    {
        size_t          m = 0;
        volatile size_t i = 0;
        volatile size_t j = 0;
        volatile size_t k = 0;

        volatile const uint8_t* p1 = dataPtrReadonly;
        volatile const size_t   s1 = dataSize - 1;
        volatile const uint8_t* p2 = otherbufferptr.get();
        volatile const size_t   s2 = otherbufferptr.size() - 1;

        if (p1 == nullptr || p2 == nullptr) {
            return false;
        }

        // TODO 64bits optimisation maybe
        while (true) {
            m |= p1[i]^p2[j];

            if (i == s1 && j == s2)
                break;
            if (i < s1) {
                i++;
            } else {
                k++;
            }
            if (j < s2) {
                j++;
            } else {
                k++;
            }
        }

        return m == 0;
    }

    void SecureBufferPtr::erase(){
        if (!dataPtrMutable)
        {
            throw DATANOTMUTABLEEXCEPTION();
        }
        {
#ifdef GST_NO_BURN_OPTIMISATION
            volatile size_t number_to_erase8 = dataSize;
        volatile uint8_t* burnm8 = (volatile uint8_t*)(dataPtrMutable);
        while (number_to_erase8--) { *burnm8++ = 0; }
#else
#ifndef GST_NO_COMPILER_INT64
            // optimisation doesn't care about memory alignment
            volatile size_t number_to_erase64 = dataSize >> 3;
            volatile size_t number_to_erase8 = dataSize % 8;
            volatile uint64_t* burnm64 = (volatile uint64_t*)(dataPtrMutable);
            volatile uint8_t* burnm8;
            while (number_to_erase64--) { *burnm64++ = 0; }
            burnm8 = (volatile uint8_t*) burnm64;
            while (number_to_erase8--) { *burnm8++ = 0; }
#else
            // optimisation doesn't care about memory alignement
    volatile size_t number_to_erase32 = Data.Size() >> 2;
    volatile size_t number_to_erase8 = Data.Size() % 4;
    volatile uint32_t* burnm32 = (volatile uint32_t*)(Data.Get());
    volatile uint8_t* burnm8;
    while (number_to_erase32--) { *burnm32++ = 0; }
    burnm8 = (volatile uint8_t*) burnm32;
    while (number_to_erase8--) { *burnm8++ = 0; }
#endif
#endif
        }
    }

/* Buffer implementation */

template<typename T>
Buffer<T>::~Buffer()
{
    if (Data->get() != nullptr)
    {
        freeData();
    }
    delete Data;
}

template<typename T>
void Buffer<T>::allocate(size_t size)
{
    if (size < 1)
    {
        throw INVALIDPARAMETEREXCEPTION("size can not be null");
    }

    Usersize = size;

    if (size <= Data->size())
    {
        return;    // everything is fine and allocated already
    }

    if (Data->size() != 0)
    {
        freeData();
        Usersize = size; // was erased by the free function
    }

    try
    {
        Data->set(static_cast<uint8_t*>(calloc(1, size)), size);
        if (!Data->get())
        {
            throw FAILEDMEMORYALLOCATIONEXCEPTION();
        }
    }
    catch (...)
    {
        Data->set(static_cast<uint8_t*>(nullptr), 0);
        Usersize = 0;
        throw; //rethrow
    }
}

template<typename T>
void Buffer<T>::copyFrom(const BufferPtr& bufferPtr)
{
    if (!isAllocated())
    {
        allocate(bufferPtr.size());
    }
    else if (bufferPtr.size() > Usersize)
    {
        throw INVALIDPARAMETEREXCEPTION("offset+size > Usersize");
    }
    memcpy(Data->get(), bufferPtr.get(), bufferPtr.size());
}

template<typename T>
void Buffer<T>::freeData()
{
    if (Data->get() == nullptr)
    {
        throw BUFFERALREADYFREEDEXCEPTION();
    }
    free(Data->get());
    Data->set(static_cast<uint8_t*>(nullptr), 0);
    Usersize = 0;
}

template<typename T>
void Buffer<T>::getRange(BufferPtr& dest, size_t offset, size_t size)
{
    if (offset + size > Usersize)
    {
        throw INVALIDPARAMETEREXCEPTION("offset+size > Usersize");
    }
    return Data->getRange(dest, offset, size);
}

template<typename T>
void Buffer<T>::getRange(BufferPtr& dest,size_t offset, size_t size) const
{
    if (offset + size > Usersize)
    {
        throw INVALIDPARAMETEREXCEPTION("offset+size > Usersize");
    }
    return Data->getRange(dest, offset, size);
}

/* SecureBuffer Implementation */

void SecureBuffer::allocate(size_t size)
{
    erase();
    Buffer::allocate(size);
    erase(); // setting initial data to 0
    mlock(Data->get(), Data->size()); // locking mem to RAM
}

void SecureBuffer::freeData()
{
    if (Data->get() == nullptr)
    {
        throw BUFFERALREADYFREEDEXCEPTION();
    }

    erase();
    munlock(Data->get(), Data->size()); // do not forget to unlock memory
    Buffer::freeData();
}

SecureBufferPtr &SecureBufferPtr::operator=(BufferPtr &buf)
{
    copyFrom(buf);
    return *this;
}

}
