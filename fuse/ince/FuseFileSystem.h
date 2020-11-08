//
// Created by badbouille on 02/05/2020.
//

#ifndef _FUSEFILESYSTEM_H
#define _FUSEFILESYSTEM_H

#include <cstddef>
#include <list>
#include <memory>
#include <Buffer.h>
#include <fstream>
#include <unistd.h>
#include <fuse.h>

namespace GostCrypt
{

    class FuseFileSystem;

    typedef std::list<std::shared_ptr<FuseFileSystem>> FuseFileSystemList;

    class FuseFileSystem
    {
    public:

        /* WARNING: Will crash if used with nullptr */
        FuseFileSystem() : target(nullptr) {};
        FuseFileSystem(Volume &target) : target(&target) {};

        void setTarget(Volume *newTarget) { target = newTarget; };

        // getter
        static FuseFileSystemList GetFileSystems();

        // infos
        virtual std::string getName() = 0;
        virtual std::string getID() = 0;
        virtual std::string getDescription() = 0;

        // Abstract interface that will be bind to FUSE
        virtual int fuse_service_access(const char* path, int mask) = 0;
        virtual int fuse_service_getattr(const char* path, struct stat* statData) = 0;
        virtual int fuse_service_open(const char* path, struct fuse_file_info* fi) = 0;
        virtual int fuse_service_opendir(const char* path, struct fuse_file_info* fi) = 0;
        virtual int fuse_service_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) = 0;
        virtual int fuse_service_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) = 0;
        virtual int fuse_service_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) = 0;

    protected:
        Volume *target;

    };

}


#endif //_BLOCKCIPHER_H
