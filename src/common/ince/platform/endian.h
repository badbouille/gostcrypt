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
#include <winsock2.h>
#include <winsock.h>
#define htobe32(u32_val) htonl(u32_val)
#define be32toh(u32_val) ntohl(u32_val)
#define htobe64(u64_val) ((1==htonl(1)) ? (u64_val) : (((uint64_t)htonl((u64_val) & 0xFFFFFFFFUL)) << 32) | htonl((uint32_t)((u64_val) >> 32)))
#define be64toh(u64_val) ((1==htonl(1)) ? (u64_val) : (((uint64_t)ntohl((u64_val) & 0xFFFFFFFFUL)) << 32) | ntohl((uint32_t)((u64_val) >> 32)))
#endif

#endif // _MEMORY_H_