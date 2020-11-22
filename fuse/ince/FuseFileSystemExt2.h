//
// Created by badbouille on 08/05/2020.
//

#ifndef _FUSEFILESYSTEMEXT2_H
#define _FUSEFILESYSTEMEXT2_H

#include <Volume.h>
#include "FuseFileSystem.h"

namespace GostCrypt
{

    class FuseFileSystemExt2 : public FuseFileSystem
    {
    public:

        FuseFileSystemExt2(Volume &target) : FuseFileSystem(target) { };
        FuseFileSystemExt2() : FuseFileSystem() { };

        // infos
        std::string getName() override { return "Ext2 FileSystem"; };
        std::string getID() override { return "ext2"; };
        std::string getDescription() override { return "Implementation of the ext2 filesystem, based on e2fsprogs and "
                                                      "its own fuse implementation, fuse2fs."; };

        // create
        void create() override;

        // Fuse entrypoint
        void start_fuse(const char * mountpoint);

    };

}


#endif //_FUSEFILESYSTEMEXT2_H
