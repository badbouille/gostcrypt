//
// Created by badbouille on 08/05/2020.
//

#ifndef _FUSEFILESYSTEMNONE_H
#define _FUSEFILESYSTEMNONE_H

#include <Volume.h>
#include "FuseFileSystem.h"

namespace GostCrypt
{

    class FuseFileSystemNone : public FuseFileSystem
    {
    public:

        FuseFileSystemNone(Volume &target) : FuseFileSystem(target), userID(geteuid()), groupID(getegid()) { };
        FuseFileSystemNone() : FuseFileSystem(), userID(geteuid()), groupID(getegid()) { };

        // infos
        std::string getName() override { return "No FileSystem"; };
        std::string getID() override { return "none"; };
        std::string getDescription() override { return "Not a filesystem, just a FUSE mountpoint of the RAW content "
                                                      "of the Volume with no interpretation."; };

        // create
        void create() override {}; // none filesystem does not need to be built

        // Abstract interface that will be bind to FUSE
        int fuse_service_access(const char* path, int mask) override;
        int fuse_service_getattr(const char* path, struct stat* statData) override;
        int fuse_service_open(const char* path, struct fuse_file_info* fi) override;
        int fuse_service_opendir(const char* path, struct fuse_file_info* fi) override;
        int fuse_service_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) override;
        int fuse_service_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) override;
        int fuse_service_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) override;

    private:

        bool checkAccessRights();
        uid_t userID;
        gid_t groupID;

    };

}


#endif //_FUSEFILESYSTEMNONE_H
