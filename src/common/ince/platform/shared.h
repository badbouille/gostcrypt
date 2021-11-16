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

template<class T>
class SharedWindow
{
public:
    struct Element {
        uint32_t id;
        T value;
    };

    explicit SharedWindow(uint32_t id);
    ~SharedWindow();

    int put(const T &element);
    int get(T * element);

    static const size_t ELEMENT_NUMBER = 10;
    static const size_t ELEMENT_SIZE = sizeof(Element);
    static const size_t AREA_SIZE = ELEMENT_NUMBER*ELEMENT_SIZE;
    static const size_t SHM_NAME_MAX = 255;

private:

    char shm_name[SHM_NAME_MAX];
    uint32_t current_index;
    Element *mem;

    void *platform_attributes;

};

    template class SharedWindow<uint32_t>;


}

#endif // _SHARED_H_