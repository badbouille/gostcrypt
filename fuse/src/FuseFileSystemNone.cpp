//
// Created by badbouille on 05/05/2020.
//

#include <Volume.h>
#include <VolumeStandard.h>
#include <zconf.h>
#include <iostream>
#include "../ince/FuseFileSystemNone.h"

fuse_operations fuse_service_oper;

GostCrypt::Volume * mountedVolume;
GostCrypt::FuseFileSystemNone * interface;

void * fusefs_none_init(struct fuse_conn_info * cf) {
    // nothing
    return nullptr;
}

void fusefs_none_destroy(void* userdata) {
    // nothing
    mountedVolume->close();
}

bool checkAccessRights()
{
    return fuse_get_context()->uid == 0 || fuse_get_context()->uid == interface->getUID();
}

int fusefs_none_access(const char *path, int mask)
{
    // parameters are ignored
    (void)path;
    (void)mask;
    if (!checkAccessRights()) {
        return -EACCES;
    }
    return 0;
}

int fusefs_none_getattr(const char *path, struct stat *statData)
{
    memset(statData, 0, sizeof(*statData));

    statData->st_uid = interface->getUID();
    statData->st_gid = interface->getGID();
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
            statData->st_size = mountedVolume->getSize();
        }
        else
        {
            return -ENOENT;
        }
    }
    return 0;
}

int fusefs_none_open(const char *path, struct fuse_file_info *fi)
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

int fusefs_none_opendir(const char *path, struct fuse_file_info *fi)
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

int fusefs_none_read(const char *path,
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
    if (size + offset > mountedVolume->getSize()) {
        size = mountedVolume->getSize() - offset;
    }

    GostCrypt::SecureBufferPtr ptr((uint8_t *)(buf), size);

    mountedVolume->read(ptr, offset);

    return size;
}

int fusefs_none_readdir(const char *path,
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

int fusefs_none_write(const char *path,
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
    if (size + offset > mountedVolume->getSize()) {
        size = mountedVolume->getSize() - offset;
    }

    GostCrypt::SecureBufferPtr ptr((uint8_t *)(buf), size);

    mountedVolume->write(ptr, offset);

    return size;
}


void GostCrypt::FuseFileSystemNone::start_fuse(const char * mountpoint, Volume *target) {

#ifdef DEBUG
#define ARG_NUM 5
    char params[ARG_NUM][256] = {"gostcrypt", "", "-oallow_other", "-f", "-s"};
#else
#define ARG_NUM 4
    char params[ARG_NUM][256] = {"gostcrypt", "", "-oallow_other", "-s"};
#endif

    // TODO option allow_other only allowed if 'user_allow_other' is set in /etc/fuse.conf

    // strcpy_s not part of c++11. Using good old strlen instead.
    if(strlen(mountpoint) > 255) {
        throw INVALIDPARAMETEREXCEPTION("mountpoint name too long for buffer.");
    }
    strcpy(params[1], mountpoint);

    char* args[ARG_NUM];
    for (int i = 0; i < ARG_NUM; i++)
    {
        args[i] = params[i];
    }

    // making sure interface points on the volume
    mountedVolume = target;
    interface = this;

    /* Setting up fuse_service callbacks */
    // note: it could be a constant, but in c++xx>11 C like structures can't be easily initialised
    fuse_service_oper.getattr = fusefs_none_getattr;
    fuse_service_oper.open = fusefs_none_open;
    fuse_service_oper.read = fusefs_none_read;
    fuse_service_oper.write = fusefs_none_write;
    fuse_service_oper.opendir = fusefs_none_opendir;
    fuse_service_oper.readdir = fusefs_none_readdir;
    fuse_service_oper.init = fusefs_none_init;
    fuse_service_oper.destroy = fusefs_none_destroy;
    fuse_service_oper.access = fusefs_none_access;

    // setup super fuse (info file)
    setupSuperFuse(geteuid(), getegid(), target, mountpoint);

    super_fuse_main(ARG_NUM, args, &fuse_service_oper, nullptr);
}
