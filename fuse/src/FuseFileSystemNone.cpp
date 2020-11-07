//
// Created by badbouille on 05/05/2020.
//

#include <Volume.h>
#include <VolumeStandard.h>
#include <zconf.h>
#include "../ince/FuseFileSystemNone.h"

bool GostCrypt::FuseFileSystemNone::checkAccessRights()
{
    return fuse_get_context()->uid == 0 || fuse_get_context()->uid == userID;
}

int GostCrypt::FuseFileSystemNone::fuse_service_access(const char *path, int mask)
{
    // parameters are ignored
    (void)path;
    (void)mask;
    if (!checkAccessRights()) {
        return -EACCES;
    }
    return 0;
}

int GostCrypt::FuseFileSystemNone::fuse_service_getattr(const char *path, struct stat *statData)
{
    memset(statData, 0, sizeof(*statData));

    statData->st_uid = userID;
    statData->st_gid = groupID;
    statData->st_atime = time(NULL);
    statData->st_ctime = time(NULL);
    statData->st_mtime = time(NULL);

    if (strcmp(path, "/") == 0) {
        statData->st_mode = S_IFDIR | 0500;
        statData->st_nlink = 2;
    } else {
        if (!checkAccessRights())
        {
            return -EACCES;
        }

        if (strcmp(path, "/volume") == 0)
        {
            statData->st_mode = S_IFREG | 0600;
            statData->st_nlink = 1;
            statData->st_size = target->getSize();
        }
        else
        {
            return -ENOENT;
        }
    }
    return 0;
}

int GostCrypt::FuseFileSystemNone::fuse_service_open(const char *path, struct fuse_file_info *fi)
{
    (void)fi;
    if (!checkAccessRights())
    {
        return -EACCES;
    }
    if (strcmp(path, "/volume") != 0)
    {
        return -ENOENT;
    }
    return 0;
}

int GostCrypt::FuseFileSystemNone::fuse_service_opendir(const char *path, struct fuse_file_info *fi)
{
    (void)fi;
    if (!checkAccessRights())
    {
        return -EACCES;
    }
    if (strcmp(path, "/") != 0)
    {
        return -ENOENT;
    }
    return 0;
}

int GostCrypt::FuseFileSystemNone::fuse_service_read(const char *path,
                                                     char *buf,
                                                     size_t size,
                                                     off_t offset,
                                                     struct fuse_file_info *fi)
{
    (void)fi;
    if (!checkAccessRights())
    {
        return -EACCES;
    }
    if (strcmp(path, "/volume") != 0)
    {
        return -ENOENT;
    }

    // prevent reads after end
    if (size + offset > target->getSize()) {
        size = target->getSize();
    }

    SecureBufferPtr ptr((uint8_t *)(buf), size);

    target->read(ptr, offset);

    return 0;
}

int GostCrypt::FuseFileSystemNone::fuse_service_readdir(const char *path,
                                                        void *buf,
                                                        fuse_fill_dir_t filler,
                                                        off_t offset,
                                                        struct fuse_file_info *fi)
{
    (void)fi;
    (void)offset;
    if (!checkAccessRights())
    {
        return -EACCES;
    }

    if (strcmp(path, "/") != 0)
    {
        return -ENOENT;
    }

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    filler(buf, "volume", NULL, 0);

    return 0;
}

int GostCrypt::FuseFileSystemNone::fuse_service_write(const char *path,
                                                      const char *buf,
                                                      size_t size,
                                                      off_t offset,
                                                      struct fuse_file_info *fi)
{

    (void)fi;
    if (!checkAccessRights())
    {
        return -EACCES;
    }
    if (strcmp(path, "/volume") != 0)
    {
        return -ENOENT;
    }

    // prevent writes after end
    if (size + offset > target->getSize()) {
        size = target->getSize();
    }

    SecureBufferPtr ptr((uint8_t *)(buf), size);

    target->write(ptr, offset);

    return 0;
}
