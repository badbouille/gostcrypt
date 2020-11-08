//
// Created by badbouille on 08/05/2020.
//

#include "FuseService.h"

fuse_operations fuse_service_oper;

GostCrypt::Volume * mountedVolume;
GostCrypt::FuseFileSystem * interface;

void * fuse_service_init(struct fuse_conn_info * cf) {
    // nothing
    return nullptr;
}

void fuse_service_destroy(void* userdata) {
    // nothing
    mountedVolume->close();
}

int fuse_service_access(const char* path, int mask) {
    return interface->fuse_service_access(path, mask);
}

int fuse_service_getattr(const char* path, struct stat* statData) {
    return interface->fuse_service_getattr(path, statData);
}
int fuse_service_open(const char* path, struct fuse_file_info* fi) {
    return interface->fuse_service_open(path, fi);
}

int fuse_service_opendir(const char* path, struct fuse_file_info* fi) {
    return interface->fuse_service_opendir(path, fi);
}

int fuse_service_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    return interface->fuse_service_read(path, buf, size, offset, fi);
}

int fuse_service_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
    return interface->fuse_service_readdir(path, buf, filler, offset, fi);
}

int fuse_service_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    return interface->fuse_service_write(path, buf, size, offset, fi);
}

void start_fuse(char * mountpoint, GostCrypt::Volume * l_volume, GostCrypt::FuseFileSystem *l_interface) {

    interface = l_interface;
    mountedVolume = l_volume;

    char params[4][256] = {"gostcrypt", "", "-o", "allow_other"};

    // memcpy_s (256o max)
    memcpy(params[1], mountpoint, strlen(mountpoint));

    char* args[4];
    for (int i = 0; i < 4 ; i++)
    {
        args[i] = params[i];
    }

    /* Setting up fuse_service callbacks */
    // note: it could be a constant, but in c++xx>11 C like structures can't be easily initialised
    fuse_service_oper.getattr = fuse_service_getattr;
    fuse_service_oper.open = fuse_service_open;
    fuse_service_oper.read = fuse_service_read;
    fuse_service_oper.write = fuse_service_write;
    fuse_service_oper.opendir = fuse_service_opendir;
    fuse_service_oper.readdir = fuse_service_readdir;
    fuse_service_oper.init = fuse_service_init;
    fuse_service_oper.destroy = fuse_service_destroy;
    fuse_service_oper.access = fuse_service_access;

    fuse_main(4, args, &fuse_service_oper, nullptr);
}