/**
 * @file FuseFileSystem.cpp
 * @author badbouille
 * @date 05/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Volume.h>
#include <iostream>
#include <sys/stat.h>
#include "platform/fullpath.h"
#include "platform/endian.h"
#include "FuseFileSystem.h"
#include "FuseFileSystemNone.h"
#include "FuseFileSystemExt2.h"
#include "GostCryptException.h"
#include "fuse.h"



extern "C" {

    struct fuse_operations new_op;
    struct fuse_operations real_op;

    fuse_uid_t mount_uid;
    fuse_gid_t mount_gid;

    VolumeInfoFile_t fileinfo;

    /* Overrides default getattr callback
     * When our 'special file' (i.e. .gostfile is called, it will always return a nice result to the user
     */
    int fusefs_super_getattr(const char *path, struct fuse_stat *statData)
    {
        /* If this call concerns our special file */
        if (strncmp(path, INFO_FILE, INFO_FILE_PREFIX_LEN) == 0)
        {
            memset(statData, 0, sizeof(*statData));

            /* Returning standard info */
            statData->st_uid = mount_uid;
            statData->st_gid = mount_gid;

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
        if (strncmp(path, INFO_FILE, INFO_FILE_PREFIX_LEN) == 0)
        {
            /* We only let the user or root open this file */
            if (fuse_get_context()->uid == 0 || fuse_get_context()->uid == mount_uid) {
                /* Simply returning an empty structure */
                memset(fi, 0, sizeof(*fi));
                fi->direct_io = 1; // no pages, cache, better for a small file
                fi->fh = 0; // my own handle. Will be used to know I am trying to read from my special file

                /* If the exit file is opened, we register this as the fuse_exit file.
                 * When released, this file will close the fuse mountpoint gracefully
                 */
                if (strcmp(path, INFO_FILE_EXIT) == 0) {
                    fi->fh = -1;
                }

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
        if ( (path != nullptr && strncmp(path, INFO_FILE, INFO_FILE_PREFIX_LEN) == 0) ||
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
        if ( (path != nullptr && strncmp(path, INFO_FILE, INFO_FILE_PREFIX_LEN) == 0) ||
             (path == nullptr && fp->fh == 0) )
        {
            /* We are catching this case to NOT let the real filesystem try to release
             * our special file. Since we did nothing, the filesystem may try to
             * free unallocated memory in the structure fp.
             */
            if ( (path != nullptr && strcmp(path, INFO_FILE_EXIT) == 0) ||
                 (path == nullptr && fp->fh == -1) )
            {
                /* If the exit file was used, closing fuse gracefully */
                fuse_exit(fuse_get_context()->fuse);
            }
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

        fuse_main(argc, argv, &new_op, private_data);

        throw GOSTCRYPTEXCEPTION("Could not start fuse.");
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

void GostCrypt::FuseFileSystem::setupSuperFuse(fuse_uid_t uid, fuse_gid_t gid, GostCrypt::Volume *volume, const char *mountpoint)
{
    mount_uid = uid;
    mount_gid = gid;

    memset(&fileinfo, 0, sizeof(fileinfo));

    // NOTE: use GetFullPath on windows
    char absmountpoint[PATH_MAX];
    if (fullpath_getpath(mountpoint, absmountpoint, PATH_MAX) != 0)
        throw FILENOTFOUNDEXCEPTION(std::string(mountpoint));

    // NOTE: use GetFullPath on windows
    char abssource[PATH_MAX];
    if (fullpath_getpath(volume->getVolumeSource().c_str(), abssource, PATH_MAX) != 0)
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

    if(strlen(volume->getKdfID().c_str()) < VOLUMEINFO_SHORTSTR_SIZE) {
        strcpy(fileinfo.keyDerivationFunctionID, volume->getKdfID().c_str());
    }

    if(strlen(volume->GetID().c_str()) < VOLUMEINFO_SHORTSTR_SIZE) {
        strcpy(fileinfo.volumeTypeID, volume->GetID().c_str());
    }

    fileinfo.dataSize = htobe64(volume->getSize());

}
