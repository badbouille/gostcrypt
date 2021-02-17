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

// marking extern C or else linker won't create the right symbol
extern "C" {
    // fuse fs main. Will setup everything and call fuse_main.
    int fuse2fs_main(int argc, char *argv[], const char *fuse_additional_params);
}

void GostCrypt::FuseFileSystemExt2::start_fuse(const char * mountpoint, Volume *target) {

    char params[3][256] = {"gostcrypt", "gostcrypt", ""};
    char fuse_params[256] = ",allow_other";

    // strcpy_s not part of c++11. Using good old strlen instead.
    if(strlen(mountpoint) > 255) {
        throw INVALIDPARAMETEREXCEPTION("mountpoint name too long for buffer.");
    }
    strcpy(params[2], mountpoint);

    char* args[3];
    for (int i = 0; i < 3 ; i++)
    {
        args[i] = params[i];
    }

    // binding parameters
    already_opened_volume = target;

    // setup super fuse (info file)
    setupSuperFuse(geteuid(), getegid(), target, mountpoint);

    // calling fuse2fs
    fuse2fs_main(3, args, fuse_params);
}
