//
// Created by badbouille on 05/05/2020.
//

#include <Volume.h>
#include <VolumeStandard.h>
#include <iostream>
#include <zconf.h>
#include "FuseFileSystem.h"
#include "FuseFileSystemNone.h"
#include "FuseFileSystemExt2.h"
#include "fuse.h"



extern "C" {

    struct fuse_operations new_op;
    struct fuse_operations real_op;

    uid_t mount_uid;
    gid_t mount_gid;

    VolumeInfoFile_t fileinfo;

    /* Overrides default getattr callback
     * When our 'special file' (i.e. .gostfile is called, it will always return a nice result to the user
     */
    int fusefs_super_getattr(const char *path, struct stat *statData)
    {
        /* If this call concerns our special file */
        if (strcmp(path, INFO_FILE) == 0)
        {
            memset(statData, 0, sizeof(*statData));

            /* Returning standard info */
            statData->st_uid = mount_uid;
            statData->st_gid = mount_gid;
            statData->st_atime = time(NULL);
            statData->st_ctime = time(NULL);
            statData->st_mtime = time(NULL);

            /* Our file is readonly, obviously */
            statData->st_mode = S_IFREG | 0400;
            statData->st_nlink = 1;
            statData->st_size = sizeof(fileinfo);

            return 0;
        }

        /* If it's not our file, let the real filesystem do its job */
        if(real_op.getattr)
            return real_op.getattr(path, statData);
        /* If not implemented in the real filesystem, a simple -ENOENT is enough */
        return -ENOENT;
    }

    int fusefs_super_open(const char *path, struct fuse_file_info *fi)
    {
        /* If this call concerns our special file */
        if (strcmp(path, INFO_FILE) == 0)
        {
            /* We only let the user or root open this file */
            if (fuse_get_context()->uid == 0 || fuse_get_context()->uid == mount_uid) {
                /* Simply returning an empty structure */
                memset(fi, 0, sizeof(*fi));
                fi->direct_io = 1; // no pages, cache, better for a small file
                fi->fh = 0; // my own handle. Will be used to know I am trying to read from my special file
                return 0;
            }
            return -EACCES;
        }
        /* If it's not our file, let the real filesystem do its job */
        if(real_op.open)
            return real_op.open(path, fi);
        /* If not implemented in the real filesystem, a simple -ENOENT is enough */
        return -ENOENT;
    }

    int fusefs_super_read(const char *path,
                         char *buf,
                         size_t size,
                         off_t offset,
                         struct fuse_file_info *fi)
    {
        /* Checking if this is our file we are reading:
         *  - Checking path value (equals to INFO_FILE?)
         *  - Checking fh value (only gostcrypt sets it to zero)
         */
        if ( (path != nullptr && strcmp(path, INFO_FILE) == 0) ||
             (path == nullptr && fi->fh == 0) )
        {
            /* We only let the user or root read this file */
            if (fuse_get_context()->uid == 0 || fuse_get_context()->uid == mount_uid)
            {
                /* No reads after end */
                if (offset > sizeof(fileinfo))
                {
                    return 0;
                }
                /* Adjusting size if too large */
                if (size + offset > sizeof(fileinfo))
                {
                    size = sizeof(fileinfo) - offset;
                }
                /* Copying and returning */
                memcpy(buf, ((uint8_t *) &fileinfo) + offset, size);
                return size;
            }
            return -EACCES;
        }
        /* If it's not our file, let the real filesystem do its job */
        if(real_op.read)
            return real_op.read(path, buf, size, offset, fi);
        /* If not implemented in the real filesystem, a simple -ENOENT is enough */
        return -ENOENT;
    }

    int fusefs_super_release(const char *path,
                             struct fuse_file_info *fp)
    {
        /* Checking if this is our file we are releasing:
         *  - Checking path value (equals to INFO_FILE?)
         *  - Checking fh value (only gostcrypt sets it to zero)
         */
        if ( (path != nullptr && strcmp(path, INFO_FILE) == 0) ||
             (path == nullptr && fp->fh == 0) )
        {
            /* We are catching this case to NOT let the real filesystem try to release
             * our special file. Since we did nothing, the filesystem may try to
             * free unallocated memory in the structure fp.
             */
            return 0;
        }
        /* If it's not our file, let the real filesystem do its job */
        if(real_op.release)
            return real_op.release(path, fp);
        /* If not implemented in the real filesystem, a simple -ENOENT is enough */
        return -ENOENT;
    }

    int super_fuse_main(int argc, char *argv[], const struct fuse_operations *op, void *private_data) {

        real_op = *op;
        new_op = *op;

        /* adding overriden functions */
        new_op.getattr = fusefs_super_getattr;
        new_op.open = fusefs_super_open;
        new_op.read = fusefs_super_read;
        new_op.release = fusefs_super_release;

        /* Calling real main */
        return fuse_main(argc, argv, &new_op, private_data);
    }

}

GostCrypt::FuseFileSystemList GostCrypt::FuseFileSystem::GetFileSystems()
{
    GostCrypt::FuseFileSystemList l;

    // Default volume type
    l.push_back(new FuseFileSystemNone());
    l.push_back(new FuseFileSystemExt2());

    return l;
}

void GostCrypt::FuseFileSystem::setupSuperFuse(uid_t uid, gid_t gid, GostCrypt::Volume *volume, const char *mountpoint)
{
    mount_uid = uid;
    mount_gid = gid;

    memset(&fileinfo, 0, sizeof(fileinfo));

    // NOTE: use GetFullPath on windows
    char absmountpoint[PATH_MAX];
    if (realpath(mountpoint, absmountpoint) != absmountpoint)
        throw FILENOTFOUNDEXCEPTION(std::string(mountpoint));

    // NOTE: use GetFullPath on windows
    char abssource[PATH_MAX];
    if (realpath(volume->getVolumeSource().c_str(), abssource) != abssource)
        throw FILENOTFOUNDEXCEPTION(volume->getVolumeSource());

    // TODO goto error in else
    if(strlen(abssource) < VOLUMEINFO_STR_SIZE) {
        strcpy(fileinfo.file, abssource);
    }

    if(strlen(absmountpoint) < VOLUMEINFO_STR_SIZE) {
        strcpy(fileinfo.mountPoint, absmountpoint);
    }

    if(strlen(volume->getAlgorithmID().c_str()) < VOLUMEINFO_SHORTSTR_SIZE) {
        strcpy(fileinfo.algorithmID, volume->getAlgorithmID().c_str());
    }

    // TODO use kdf
    if(strlen(volume->getAlgorithmID().c_str()) < VOLUMEINFO_SHORTSTR_SIZE) {
        strcpy(fileinfo.keyDerivationFunctionID, volume->getAlgorithmID().c_str());
    }

    if(strlen(volume->GetID().c_str()) < VOLUMEINFO_SHORTSTR_SIZE) {
        strcpy(fileinfo.volumeTypeID, volume->GetID().c_str());
    }

    fileinfo.dataSize = htobe64(volume->getSize());

}
