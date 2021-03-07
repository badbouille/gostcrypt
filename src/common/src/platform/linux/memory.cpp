/**
 * @file memory.c
 * @author badbouille
 * @date 07/03/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <platform/memory.h>
#include "sys/mman.h"

int memory_lock(uint8_t *address, size_t size) {
    return mlock(address, size);
}

int memory_unlock(uint8_t *address, size_t size) {
    return munlock(address, size);
}
