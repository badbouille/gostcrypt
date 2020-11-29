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

// fuse superfunctions
extern "C" {

    /** File containing the structure */
    #define INFO_FILE "/.gostinfo"

    /** Size of longer fields like names, etc (watch out for overflows) */
    #define VOLUMEINFO_STR_SIZE 256

    /** Size of shorter fields like IDs, etc */
    #define VOLUMEINFO_SHORTSTR_SIZE 32

    /**
     * @brief content of the info file
     */
    typedef struct VolumeInfoFile {
        char file[VOLUMEINFO_STR_SIZE]; /** file mounted (volume source) */
        char mountPoint[VOLUMEINFO_STR_SIZE]; /** mountPoint of the volume */
        char algorithmID[VOLUMEINFO_SHORTSTR_SIZE]; /** Algorithm used to encrypt the volume */
        char keyDerivationFunctionID[VOLUMEINFO_SHORTSTR_SIZE]; /** Key derivation function used to compute the password */
        char volumeTypeID[VOLUMEINFO_SHORTSTR_SIZE]; /** Type of volume */
        size_t dataSize; /** Size of this volume */
    } VolumeInfoFile_t;

    /**
     * @brief Function that replaces fuse_main in gostcrypt.
     *
     * This functions overrides a few callbacks with its own, to serve an hidden file '.gostinfo'.
     * All other calls will be redirected to the real fuse_operations from the implementation.
     *
     * @note This function is in C code to be linked with C files like fuse2fs.c from e2fsprogs
     *
     * All parameters are the same as the real fuse_main function, and only the callbacks (op) is altered.
     */
    int super_fuse_main(int argc, char *argv[], const struct fuse_operations *op, void *private_data);
};

namespace GostCrypt
{

    class FuseFileSystem;

    typedef std::list<FuseFileSystem*> FuseFileSystemList;

    /**
     * @brief Abstract structure to define a FileSystem.
     *
     * Filesystems are used to interpret the decrypted data of the user to give it back to them in a more usable way
     * filesystems are bound to fuse, and aim to give back the content as a virtual folder on the desktop (this is the
     * 'Virtual' part of this vde system!).
     *
     * This abstract interface is less intuitive than the rest of the code since it's less important.
     * The first implementation, FuseFileSystemNone is for usage by custom tools of the raw data as a block device;
     * and the second implementation, FuseFileSystemExt2, aims to ease the user's life when mounting its volume.
     * Other filesystems may be useful, but will often use an external library, like e2fsprogs for ext2.
     *
     * Since FuseFileSystemNone gives access to the block devices, users can mount other file systems so no
     * need to have them hardcoded in there.
     *
     * Since Ext2 can be portable (e2fsprogs for mingw maybe... ? need to work on that.), there is little to no need for
     * an other filesystem.
     *
     */
    class FuseFileSystem
    {
    public:

        /**
         * Constructor. No parameters
         */
        FuseFileSystem() {};

        /**
         * Function to get the available implementations of this class
         * @return List of the implemented filesystems
         */
        static FuseFileSystemList GetFileSystems();

        // infos
        /**
         * Function to get the name of this implementation of FuseFileSystem, in a fancy and displayable way.
         * @return name of the filesystem type
         */
        virtual std::string getName() = 0;

        /**
         * Function to get the ID of the filesystem. Note that this name is used in the command line.
         * @return The id of this filesystem
         */
        virtual std::string getID() = 0;

        /**
         * Function to get a small description of this filesystem for the user.
         * @return description of this filesystem.
         */
        virtual std::string getDescription() = 0;

        /**
         * @brief Function to create a filesystem on a volume. equivalent to mkfs.
         * @note The target volume must be opened and mounted at the target location (as none).
         *
         * For FuseFileSystemNone, does nothing (nothing to init).
         * For FuseFileSystemExt2, calls mkfs.ext2.
         *
         * @param target the volume to initialise.
         */
        virtual void create(std::string target) = 0;

        /**
         * @brief Starts fuse on this volume
         *
         * This method will call fuse_main (super_fuse_main) on the mountpoint given, to interface the volume.
         *
         * Usually, the class implementation has to implement all (or most of fuse's primitives) and interface
         * them with the given volume object. These primitives must handle all readdir, write, etc operations
         * the user needs, and write the result in the target volume.
         *
         * After all primitives are ready and local data (global variables) initialized, this function will call
         * setupSuperFuse to setup the true fuse handler and finally super_fuse_main to call fuse.
         *
         * The final fuse_main call will fork the program and listen on any calls on the directory. Only then the
         * function will return.
         *
         * @param mountpoint the directory where to mount the volume. must be empty.
         * @param target the target volume to interface on this directory.
         */
        virtual void start_fuse(const char * mountpoint, Volume *target) = 0;

        /**
         * Function to setup the super fuse environnement
         *
         * The super fuse environnement is composed of callbacks that intercept user requests on the folder, to serve
         * a 'special file' to give information on this mountpoint, regardless of the underlying FuseFileSystem
         * implementation.
         *
         * @param uid The uid allowed to get information from the .gostfile
         * @param gid The uid allowed to get information from the .gostfile (not working now)
         * @param volume the volume mounted. Only infos like name, id, etc will be extracted from it.
         * @param mountpoint The mountpoint of this filesystem.
         */
        void setupSuperFuse(uid_t uid, gid_t gid, Volume *volume, const char *mountpoint);

    };

}


#endif //_BLOCKCIPHER_H
