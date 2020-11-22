//
// Created by badbouille on 05/05/2020.
//

#include <Volume.h>
#include <VolumeStandard.h>
#include "FuseFileSystemExt2.h"

void GostCrypt::FuseFileSystemExt2::create() {

    // not implemented yet
    // should use mke2fs and init the current target.

}

// global var containing opened volume
// Our own io_manager will bind to this
// fuses2fs will think it's writing to a file but in reality everything will go through this volume
extern GostCrypt::Volume *already_opened_volume;

// marking extern C or else linker won't create the right symbol
extern "C" {
    // fuse fs main. Will setup everything and call fuse_main.
    int fuse2fs_main(int argc, char *argv[]);
}

void GostCrypt::FuseFileSystemExt2::start_fuse(const char * mountpoint) {

    char params[5][256] = {"gostcrypt", "nofile", "", "-o", "fuse2fs_debug"};

    // strcpy_s not part of c++11. Using good old strlen instead.
    if(strlen(mountpoint) > 255) {
        throw INVALIDPARAMETEREXCEPTION("mountpoint name too long for buffer.");
    }
    strcpy(params[1], mountpoint);

    char* args[5];
    for (int i = 0; i < 5 ; i++)
    {
        args[i] = params[i];
    }

    // binding parameters
    already_opened_volume = target;

    // calling fuse2fs
    fuse2fs_main(5, args);
}

