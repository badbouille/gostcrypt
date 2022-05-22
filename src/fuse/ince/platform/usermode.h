/**
 * @file usermode.h
 * @author badbouille
 * @date 22/05/2022
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _USERMODE_H_
#define _USERMODE_H_

#include <cinttypes>
#include <cstddef>

#ifdef PLATFORM_LINUX
#include <zconf.h>
#include <wait.h>

// redefinitions of winfsp variables
#define fuse_stat stat
#define fuse_uid_t uid_t
#define fuse_gid_t gid_t
#define fuse_off_t off_t
#define fuse_timespec time_t

#endif

#ifdef PLATFORM_WINDOWS
// binding euid to root (windows does not care about access)
#define geteuid() 0
#define getegid() 0
#endif

#endif // _USERMODE_H_
