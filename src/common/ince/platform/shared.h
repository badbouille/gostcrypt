/**
 * @file shared.h
 * @author badbouille
 * @date 14/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _SHARED_H_
#define _SHARED_H_

#include <cinttypes>
#include <cstddef>

namespace GostCrypt
{

/**
 * @brief Template class used to transfer information between processes using a shard memory
 *
 * 'Window' refers to the way this class work. 'put' method will fill the shared memory area.
 * This area is divided in ::ELEMENT_NUMBER slots, so the 'get' method may have a little while
 * before fetching the data in the parent process. If too many elements are pushed into the window,
 * older elements will be overriden.
 *
 * @tparam T is the serializable structure transferred between processes.
 */
template<class T>
class SharedWindow
{
public:

    /**
     * Structure used to represent every entry of the window
     */
    struct Element {
        uint32_t id; /**< The id of the element. Is always increasing and lets the processes know where to write or read next in the window */
        T value; /**< Value stored at this index. Will not be erased but overriden. */
    };

    /**
     * SharedWindows are built using IDs. The constructor will build or attach a shared object pointing on this ID.
     * @param id The ID of this SharedWindow to create or attach to. May be the child's PID for example.
     */
    explicit SharedWindow(uint32_t id);
    ~SharedWindow();

    /**
     * @brief Method to push an object to this shared memory.
     * Will loop and even override data over the window when full because it has no information on what has been read or not.
     * @param element The element to push on the window
     * @return 0, this function can't fail
     */
    int put(const T &element);

    /**
     * @brief Method to retrieve the next element from this SharedWindow.
     * This method may be enclosed in a while to read all elements currently available.
     * @param element A valid pointer on an empty element that will be filled with the retreived element
     * @return 0 if an element was retreived, 1 if nothing is available for now.
     */
    int get(T * element);

    /**
     * @brief Number of elements contained by this window.
     * If the window has to dump a lot of data, incrasing this value may give more time to the listening
     * process to retreive all the elements.
     */
    static const size_t ELEMENT_NUMBER = 10;

    /**
     * @brief The size of each element.
     * This is basically the size of the data shared (T) + around 4 bytes
     */
    static const size_t ELEMENT_SIZE = sizeof(Element);

    /**
     * @brief The size of the shared area created and accessed.
     */
    static const size_t AREA_SIZE = ELEMENT_NUMBER*ELEMENT_SIZE;

    /**
     * Maximum length of the name of the shared memory (path)
     */
    static const size_t SHM_NAME_MAX = 255;

private:

    /**
     * Name of the file to identify this shared memory (is system dependent).
     */
    char shm_name[SHM_NAME_MAX];

    /**
     * Current index of last read/write.
     * Be aware: When using this class both ways (read and write), writing will skip the reads of all previous elements.
     * Before writing the program should read all unread elements.
     */
    uint32_t current_index;

    /**
     * Pointer on the memory area shared between processes.
     */
    Element *mem;

    /**
     * Specific elements defined by the implementation, specific to the operating system.
     */
    void *platform_attributes;

};


}

#endif // _SHARED_H_