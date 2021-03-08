/**
 * @file memory.h
 * @author badbouille
 * @date 07/03/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <cinttypes>
#include <cstddef>

#ifdef PLATFORM_LINUX
// self inclusion problem avoided because sub directory platform is not in -I's
#include <endian.h>
#endif

#ifdef PLATFORM_WINDOWS
#include <winsock.h>
#define htobe32(u32_val) htonl(u32_val)
#endif

#endif // _MEMORY_H_