/**
 * @file Buffer.h
 * @author badbouille
 * @date 16/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "GostCryptException.h"

#include <cinttypes>
#include <cstring>

namespace GostCrypt
{

class SecureBuffer;

/**
 * @brief Class handling pointer to Buffer data
 *
 */
class BufferPtr
{
 public:
    /**
     * @brief Default constructor creating a null pointer
     *
     */
    BufferPtr() : dataPtrMutable(nullptr), dataPtrReadonly(nullptr), dataSize(0) {}
    /**
     * @brief Constructor initializing the BufferPtr with a raw data pointer and a data size.
     *
     * @param data Pointer to the buffer data
     * @param size Size of the pointed buffer
     * @warning You will have to free the memory yourself
     */
    BufferPtr(uint8_t* data, size_t size)
        : dataPtrMutable(data), dataPtrReadonly(data), dataSize(size) { }
    /**
     * @brief Constructor initializing the BufferPtr with a const raw data pointer and a data size. Used to make sure the data pointed cannot be modified.
     *
     * @param data Pointer to the const buffer data
     * @param size Size of the pointed buffer
     * @warning You will have to free the memory yourself
     * @warning Well initializing with const data the BufferPtr should be const
     */
    BufferPtr(const uint8_t* data, size_t
              size)  // const version. should not be used with a non-const object
        : dataPtrMutable(nullptr), dataPtrReadonly(data), dataSize(size) { }
    /**
     * @brief Default destructor. Doing nothing since the BufferPtr point to a Buffer object in charge of freeing its memory
     *
     */
    virtual ~BufferPtr() = default;

    /**
     * @brief Conversion operator to get the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    explicit operator const uint8_t* () const { return dataPtrReadonly; }
    /**
     * @brief Conversion operator to get the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     * @warning Will throw a DataNotMutableException if the BufferPtr points to const data
     */
    explicit operator uint8_t* () { if (dataPtrReadonly != dataPtrMutable) { throw DATANOTMUTABLEEXCEPTION(); } return dataPtrMutable; }
    /**
     * @brief Compare data stored in the BufferPtr with the data of another BufferPtr
     *
     * @param otherbufferptr Other BufferPtr to compare data with
     * @return bool Return True is the data of BufferPtrs are equals
     */
    virtual bool isDataEqual(const BufferPtr& otherbufferptr) const;

    /**
     * @brief Copy Data from another BufferPtr to this BufferPtr
     * @warning Will throw a IncorrectParameterException if this BufferPtr does have enough space to store the data of the other BufferPtr
     * @warning Will throw a DataNotMutableException if this BufferPtr point to const data
     * @param bufferPtr Other BufferPtr to copy data from
     */
    virtual void copyFrom(const BufferPtr& bufferPtr);

    /**
     * @brief Will set the all data bytes of this BufferPtr to zero.
     * @warning Will throw a DataNotMutableException if this BufferPtr point to const data
     *
     */
    virtual void erase() { if (!dataPtrMutable) { throw DATANOTMUTABLEEXCEPTION(); } memset(dataPtrMutable, 0, dataSize); }

    /**
     * @brief Exchanges the memory form this buffer with the otherbuffer
     * If the sizes of the buffers are different, shortest size is chosen. Only the beginning of the biggest buffer is swapped.
     * @param otherBuffer the buffer to exchange memory with
     */
    virtual void swap(BufferPtr &otherBuffer);

    /**
     * @brief Xors the content of this buffer with the content of the other buffer
     * If the sizes of the buffers are different, shortest size is chosen. Only the beginning of the biggest buffer is swapped.
     * @param otherBuffer the buffer to exchange memory with
     */
    virtual void Xor(const BufferPtr &otherBuffer);

    /**
     * @brief Return the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     * @warning Will throw DataNotMutableException if this BufferPtr point to const data. Should not happened since the
     * const version of this method will be called when using a const BufferPtr
     */
    uint8_t* get() { if (dataPtrReadonly != dataPtrMutable) { throw DATANOTMUTABLEEXCEPTION(); } return dataPtrMutable; }
    /**
     * @brief Return the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    const uint8_t* get() const { return dataPtrReadonly; }  // const version

    /**
     * @brief Gets a BufferPtr corresponding to a subset of the data of this BufferPtr
     *
     * @param dest New const BufferPtr corresponding to a subset of the const data of this BufferPtrPointer to fill with
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     * @return New BufferPtr corresponding to a subset of the data of this BufferPtr
     * @warning Will throw a IncorrectParameterException if requestData is out of the data of this BufferPtr
     * @warning Will throw DataNotMutableException if this BufferPtr point to const data. Should not happened since the const version of this method will be called when using a const BufferPtr
     */
    virtual void getRange(BufferPtr &dest, size_t offset, size_t size);

    /**
     * @brief Gets a const BufferPtr corresponding to a subset of the const data of this BufferPtr
     *
     * @param dest New const BufferPtr corresponding to a subset of the const data of this BufferPtrPointer to fill with
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     */
    virtual void getRange(BufferPtr &dest, size_t offset, size_t size) const;  // const version

    /**
     * @brief Make this BufferPtr points to other data
     *
     * @param data Pointer to the buffer new data
     * @param size Size of the pointed buffer
     */
    void set(uint8_t* data, size_t size) { dataPtrMutable = data; dataPtrReadonly = data; dataSize = size; }

    /**
     * @brief Make this BufferPtr points to constant data
     *
     * @param data Pointer to the buffer new data
     * @param size Size of the pointed buffer
     */
    void set(const uint8_t* data, size_t size) { dataPtrMutable = nullptr; dataPtrReadonly = data; dataSize = size; }

    /**
     * @brief Return the size of this buffer in bytes
     *
     * @return size_t Size of this buffer in bytes
     */
    size_t size() const { return dataSize; }

 protected:
    uint8_t* dataPtrMutable; /**< Raw pointer to the data of this BufferPtr used when the data is not const */
    const uint8_t* dataPtrReadonly; /**< Raw pointer to the data of this BufferPtr */
    size_t dataSize; /**< Size of this buffer in bytes */
};

/**
 * @brief Class handling pointer to Buffer data
 *
 */
class SecureBufferPtr : public BufferPtr
{
    /*
     * Using all base constructors, since this is the exact same object wth a few tweaks on operations
     */
    using BufferPtr::BufferPtr;
public:
    /**
     * @brief Compare data stored in the BufferPtr with the data of another BufferPtr
     *
     * @param otherbufferptr Other BufferPtr to compare data with
     * @return bool Return True is the data of BufferPtrs are equals
     */
    bool isDataEqual(const BufferPtr& otherbufferptr) const override;

    /**
     * @brief Will set the all data bytes of this BufferPtr to zero.
     * @warning Will throw a DataNotMutableException if this BufferPtr point to const data
     *
     */
    void erase() override;

    SecureBufferPtr& operator=(BufferPtr &buf);
};

/**
 * @brief Class handling buffer data
 *
 */
template <typename T = BufferPtr>
class Buffer
{
 public:
    /**
     * @brief Default constructor initializing an empty buffer
     *
     */
    Buffer() { Data = new T(); Usersize = 0; }
    /**
     * @brief Constructor initializing a buffer with given size
     *
     * @param size Size of the new Buffer in bytes
     */
    explicit Buffer(size_t size): Buffer() { allocate(size); };
    /**
     * @brief Deep copy constructor initializing a new Buffer with the same data as the given BufferPtr
     *
     * @param bufferPtr BufferPtr to copy the data from
     */
    Buffer(const BufferPtr& bufferPtr): Buffer()  { copyFrom(bufferPtr); }
    /**
     * @brief Default Destructor freeing the buffer data
     *
     */
    virtual ~Buffer();

    /**
     * @brief Allocate new memory for this Buffer
     * @warning The previsous data stored in the Buffer will not be kept, but may not be erased
     * @param size New size of the buffer in bytes
     */
    virtual void allocate(size_t size);
    /**
     * @brief Copy Data from a BufferPtr to this Buffer
     * @warning Will throw a IncorrectParameterException if this BufferPtr does have enough space to store the data of the other BufferPtr
     * @param bufferPtr BufferPtr to copy data from
     */
    virtual void copyFrom(const BufferPtr& bufferPtr);
    /**
     * @brief Will set the all data bytes of this BufferPtr to zero.
     * Can be weirdly long if a huge size is allocated while a currently small usersize is in use
     */
    virtual void erase() { Data->erase(); };
    /**
     * @brief Free the data of this Buffer and set its size to 0
     *
     */
    virtual void freeData();

    /**
     * @brief Return the size of this buffer in bytes
     *
     * @return size_t Size of this buffer in bytes
     */
    virtual size_t size() const { return Usersize; }
    /**
     * @brief Return true if this Buffer size is not zero
     *
     * @return True if this Buffer size is not zero
     */
    virtual bool isAllocated() const { return Data->size() != 0; }

    /**
     * @brief Return the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     */
    virtual uint8_t* get() { return Data->get(); }
    /**
     * @brief Return the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    virtual const uint8_t* get() const { return Data->get(); }  // const version

    /**
     * @brief Return BufferPtr corresponding to a subset of the data of this Buffer
     *
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     * @return BufferPtr corresponding to a subset of the data of this Buffer
     * @warning Will throw a IncorrectParameterException if requestData is out of the data of this BufferPtr
     * @warning Will throw DataNotMutableException if this BufferPtr point to const data. Should not happened since the const version of this method will be called when using a const BufferPtr
     */
    virtual void getRange(BufferPtr& dest, size_t offset, size_t size);

    /**
     * @brief Return const BufferPtr corresponding to a subset of the const data of this Buffer
     *
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     * @return const BufferPtr corresponding to a subset of the const data of this Buffer
     */
    virtual void getRange(BufferPtr& dest, size_t offset, size_t size) const;


    /**
     * @brief Conversion operator to get the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     */
    virtual explicit operator uint8_t* () { return Data->get(); }
    /**
     * @brief Conversion operator to get the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    virtual explicit operator const uint8_t* () const { return Data->get(); }

    /**
     * @brief Conversion operator to get the corresponding BufferPtr
     *
     * @return corresponding BufferPtr
     */
    virtual explicit operator BufferPtr& () { return *Data; }
    /**
     * @brief Conversion operator to get the corresponding const BufferPtr
     *
     * @return corresponding const BufferPtr
     */
    virtual explicit operator const BufferPtr& () const { return *Data; }

 protected:
    BufferPtr *Data; /**< Pointer to the buffer data handling allocated memory for the buffer */
    size_t Usersize{}; /**< Actual used size. Allow not to realloc when reducing the size of the Buffer*/
 private:
    /**
     * @brief The copy constructor is disabled to prevent heavy deep copy
     */
    explicit Buffer(const Buffer&);

    /**
     * @brief The equal operator is disabled to prevent heavy deep copy
     */
    Buffer& operator= (const Buffer&);
};

/**
 * @brief Declaring Buffer<SecureBufferPtr> in advance so SecureBuffer can inherit from it.
 * GCC was able to do it itself, while mingw needs this declaration.
 */
template class Buffer<SecureBufferPtr>;

/**
 * @brief Class handling buffer data when data is sensitive. The data will be locked into RAM using mlock and erased before beeing freed
 *
 */
class SecureBuffer : public Buffer<SecureBufferPtr>
{
 public:
    /**
     * @brief Default constructor initializing an empty buffer
     *
     */
    SecureBuffer(): Buffer() {};
    /**
     * @brief Constructor initializing a buffer with given size
     *
     * @param size Size of the new Buffer in bytes
     */
    explicit SecureBuffer(size_t size): Buffer(size) {};
    /**
     * @brief Deep copy constructor initializing a new Buffer with the same data as the given BufferPtr
     *
     * @param bufferPtr BufferPtr to copy the data from
     */
    SecureBuffer(const BufferPtr& bufferPtr): Buffer(bufferPtr) {};

    /**
     * @brief Allocate new memory for this Buffer and lock it in RAM using mlock
     * @warning The previsous data stored in the Buffer will not be kept and will be erased
     * @param size New size of the buffer in bytes
     */
    void allocate(size_t size) override;
    /**
     * @brief Free the data of this Buffer after having erased it and set its size to 0
     *
     */
    void freeData() override;

     // disabled methods

    /**
     * @brief The copy constructor is disabled to prevent accidental heavy deep copy
     */
    SecureBuffer(const SecureBuffer&) = delete;
    /**
     * @brief The equal operator is disabled to prevent accidental heavy deep copy
     */
    SecureBuffer& operator= (const SecureBuffer&) = delete;
};

}

#endif
