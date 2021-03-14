/**
 * @file FuseFileSystemExt2.cpp
 * @author badbouille
 * @date 05/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Volume.h>
#include <wait.h>
#include "FuseFileSystemExt2.h"

#include "blockdev.h"

extern "C" {
#include "ops.h"
#include "lwext4.h"
}

void GostCrypt::FuseFileSystemExt2::create(std::string target) {

    /* Forking and execing the mkfs program */
    pid_t pid = fork();
    if ( pid == 0 ) {
        static char argvT[][256] = { "/sbin/mkfs.ext2", "-q", "", ""};
        static char *argv[] = { argvT[0], argvT[1], argvT[2], argvT[3], nullptr };

        uid_t uid = geteuid();
        snprintf(argv[2], 256, "-Eroot_owner=%d:%d", uid, uid);

        if (strlen(target.c_str()) < 256) {
            strcpy(argv[3], target.c_str());
        }

        /* Executing basic mkfs.ext2 program */
        execv(argv[0], argv);

        /* If execv fails, exit with error code */
        exit(127);

    } else {
        int status;

        /* Waiting for child to build disk image */
        if (waitpid(pid, &status, 0) == -1 ) {
            throw GOSTCRYPTEXCEPTION("waitpid failed.");
        }

        /* Checking return value */
        if ( WIFEXITED(status) ) {
            if (WEXITSTATUS(status) != 0) {
                throw GOSTCRYPTEXCEPTION("Wrong exit status for mkfs.ext2.");
            }
        } else {
            throw GOSTCRYPTEXCEPTION("Child not exited.");
        }

        /* Formatted successfully */

    }
}

// global var containing opened volume
// Our own io_manager will bind to this
// fuses2fs will think it's writing to a file but in reality everything will go through this volume
extern GostCrypt::Volume *already_opened_volume;

static struct fuse_operations e4f_ops = {
        .getattr = op_getattr,
        .readlink = op_readlink,
        .mkdir = op_mkdir,
        .unlink = op_unlink,
        .rmdir = op_rmdir,
        .symlink = op_symlink,
        .rename = op_rename,
        .link = op_link,
        .chmod = op_chmod,
        .chown = op_chown,
        .truncate = op_truncate,
        .utime	  = op_utimes,
        .open = op_open,
        .read = op_read,
        .write = op_write,
        .release = op_release,
        .setxattr = op_setxattr,
        .getxattr = op_getxattr,
        .listxattr = op_listxattr,
        .removexattr = op_removexattr,
        .opendir = op_opendir,
        .readdir = op_readdir,
        .releasedir = op_release,
        .init = op_init,
        .destroy = op_destroy,
        .create = op_create,
        .ftruncate = op_ftruncate,
#if !defined(__FreeBSD__)
        .utimens	= op_utimens,
#endif
};

extern "C" { /* Adding lwext4 options structure in extern C to be able to link it */
struct fuse_lwext4_options fuse_lwext4_options;
}

void GostCrypt::FuseFileSystemExt2::start_fuse(const char * mountpoint, Volume *target) {
    struct ext4_blockdev *bdev;
#ifdef DEBUG
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

    // binding parameters
    already_opened_volume = target;

    if (blockdev_get(mountpoint, &bdev) != LWEXT4_ERRNO(EOK)) {
        throw GOSTCRYPTEXCEPTION("Failed to open the device");
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

