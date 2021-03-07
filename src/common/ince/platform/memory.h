/**
 * @file memory.h
 * @author badbouille
 * @date 07/03/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <cinttypes>
#include <cstddef>

/**
 * Function to forbid a memory area to be swapped.
 * Used as a common interface for mlock and mVirtualLock
 */
int memory_lock(uint8_t *address, size_t size);

/**
 * Function to un-forbid a memory area to be swapped.
 * Used as a common interface for munlock and mVirtualUnlock
 */
int memory_unlock(uint8_t *address, size_t size);

#endif // _MEMORY_H_