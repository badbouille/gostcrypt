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

    typedef std::list<FuseFileSystem*> FuseFileSystemList;

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

        // creation
        virtual void create() = 0;

        // Abstract interface that will be bind to FUSE
        virtual void start_fuse(const char * mountpoint) = 0;

    protected:
        Volume *target;

    };

}


#endif //_BLOCKCIPHER_H
