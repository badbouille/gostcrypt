/**
 * @file shared.c
 * @author badbouille
 * @date 15/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <platform/shared.h>
#include <GostCryptException.h>

#include <windows.h>
#include <fcntl.h>

/* Forward declaration of templates */
#include "Progress.h"

template class GostCrypt::SharedWindow<uint32_t>;
template class GostCrypt::SharedWindow<GostCrypt::Progress::ProgressInfo_t>;

#define SHM_PREFIX "gc_shm_"

struct shm_windows_attributes {
    HANDLE hMapFile;
};

template<class T>
GostCrypt::SharedWindow<T>::SharedWindow(uint32_t id)
{
    /* Default values */
    platform_attributes = new shm_windows_attributes;
    auto* p_attr = static_cast<shm_windows_attributes *>(platform_attributes);
    current_index = 0;
    mem = nullptr;

    /* Checking malloc */
    if (p_attr == nullptr) {
        throw MEMORYALLOCATIONEXCEPTION(sizeof(shm_windows_attributes));
    }

    /* Computing SHM name */
    if (snprintf(shm_name, SHM_NAME_MAX, "%s%d", SHM_PREFIX, id) < 0) {
        delete p_attr;
        throw GOSTCRYPTEXCEPTION("snprintf unknown error");
    }

    /* Opening SHM */
    //p_attr->fd = shm_open(shm_name, O_RDWR, S_IRUSR | S_IWUSR);
    p_attr->hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, shm_name);

    /* In case of error, the file is created instead */
    if (p_attr->hMapFile == nullptr) {

        /* Creating SHM */
        p_attr->hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, AREA_SIZE, shm_name);

        if (p_attr->hMapFile == nullptr) {
            delete p_attr;
            throw GOSTCRYPTEXCEPTION("Could not create shm with path " + std::string(shm_name) + ". Errno = " + std::to_string(errno));
        }
    }

    /* Mapping memory area */
    mem = (Element *)MapViewOfFile(p_attr->hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, AREA_SIZE);

    if (mem == nullptr) {
        CloseHandle(p_attr->hMapFile);
        delete p_attr;
        throw GOSTCRYPTEXCEPTION("Could not map shared memory");
    }

    /* setting it to zero */
    memset((uint8_t*)mem, 0, AREA_SIZE);
}

template<class T>
GostCrypt::SharedWindow<T>::~SharedWindow()
{
    auto* p_attr = static_cast<shm_windows_attributes *>(platform_attributes);
    UnmapViewOfFile(mem);
    CloseHandle(p_attr->hMapFile);
    delete p_attr;
}

template<class T>
int GostCrypt::SharedWindow<T>::put(const T &element)
{
    /* Finding next cell */
    while(mem[current_index % ELEMENT_NUMBER].id >= current_index + 1) {
        current_index++;
    }

    /* Writing value at current index */
    mem[current_index % ELEMENT_NUMBER].id = current_index + 1;
    mem[current_index % ELEMENT_NUMBER].value = element;

    return 0;
}

template<class T>
int GostCrypt::SharedWindow<T>::get(T * element)
{
    if(element == nullptr) {
        throw INVALIDPARAMETEREXCEPTION("element is NULL");
    }

    /* Finding next cell if available */
    while(mem[current_index % ELEMENT_NUMBER].id > current_index + 1) {
        current_index++;
    }

    /* Checking if it's the one we are looking for */
    if (mem[current_index % ELEMENT_NUMBER].id == current_index + 1) {
        /* Reading the value */
        element[0] = mem[current_index % ELEMENT_NUMBER].value;
        // forget it by increasing current_index
        current_index+=1;
        return 0;
    } else {
        /* Nothing to read for now */
        return 1;
    }
}
