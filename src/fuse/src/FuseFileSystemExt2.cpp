/**
 * @file FuseFileSystemExt2.cpp
 * @author badbouille
 * @date 05/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Volume.h>
#include "platform/usermode.h"
#include <ext4_mkfs.h>
#include "FuseFileSystemExt2.h"

//#include "blockdev.h"
// this function should be in blockdev.h, but isn't since blockdev.h is not a GostCrypt file but a fuse-ext4 file.
extern int blockdev_get_volume(GostCrypt::Volume *target, struct ext4_blockdev **pbdev);

extern "C" {
#include "ops.h"
#include "lwext4.h"
}

void GostCrypt::FuseFileSystemExt2::create(Volume *target) {
    struct ext4_blockdev *bdev;

    int fs_type = F_SET_EXT4;
    int r = 1;

    struct ext4_fs fs = {0};
    struct ext4_mkfs_info info = {
            .block_size = 1024,
            .journal = true,
    };

    if (blockdev_get_volume(target, &bdev) != LWEXT4_ERRNO(EOK)) {
        throw GOSTCRYPTEXCEPTION("Failed to load target device");
    }

    r = ext4_mkfs(&fs, bdev, &info, fs_type);

    if (r != EOK) {
        throw GOSTCRYPTEXCEPTION("ext4_mkfs failed.");
    }

}

extern struct fuse_operations e4f_ops;

extern "C" { /* Adding lwext4 options structure in extern C to be able to link it */
struct fuse_lwext4_options fuse_lwext4_options;
}

void GostCrypt::FuseFileSystemExt2::start_fuse(const char * mountpoint, Volume *target) {
    struct ext4_blockdev *bdev;
#ifdef TARGET_DEBUG
    #define ARG_NUM 5
    char params[ARG_NUM][256] = {"gostcrypt", "", "-oallow_other", "-f", "-s"};
#else
#define ARG_NUM 4
    char params[ARG_NUM][256] = {"gostcrypt", "", "-oallow_other", "-s"};
#endif

    // strcpy_s not part of c++11. Using good old strlen instead.
    if(strlen(mountpoint) > 255) {
        throw INVALIDPARAMETEREXCEPTION("mountpoint name too long for buffer.");
    }
    strcpy(params[1], mountpoint);

    char* args[ARG_NUM];
    for (int i = 0; i < ARG_NUM; i++)
    {
        args[i] = params[i];
    }

    if (blockdev_get_volume(target, &bdev) != LWEXT4_ERRNO(EOK)) {
        throw GOSTCRYPTEXCEPTION("Failed to load target device");
    }

    // setup super fuse (info file)
    setupSuperFuse(geteuid(), getegid(), target, mountpoint);

    // setup lwext4 options (nothing for now)
    fuse_lwext4_options.cache = 0;
    fuse_lwext4_options.debug = 0;
    fuse_lwext4_options.journal = 0;
    fuse_lwext4_options.disk = nullptr;
    fuse_lwext4_options.logfile = nullptr;

    /* Waiting for child to mount the raw volume */
    if (super_fuse_main(ARG_NUM, args, &e4f_ops, bdev) != 0 ) {
        throw GOSTCRYPTEXCEPTION("fuse_main failed.");
    }
}

