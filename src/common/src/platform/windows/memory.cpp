/**
 * @file memory.c
 * @author badbouille
 * @date 07/03/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <platform/memory.h>
#include <windows.h>
#include <memoryapi.h>

int memory_lock(uint8_t *address, size_t size) {
    return VirtualLock(address, size);
}

int memory_unlock(uint8_t *address, size_t size) {
    return VirtualUnlock(address, size);
}
