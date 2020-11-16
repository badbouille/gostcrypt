//
// Created by badbouille on 07/05/2020.
//

#ifndef _FUSESERVICE_H
#define _FUSESERVICE_H


#include <Volume.h>
#include "FuseFileSystem.h"

#include <fuse.h>

void * fuse_service_init(struct fuse_conn_info * cf);
int fuse_service_access(const char* path, int mask);
void fuse_service_destroy(void* userdata);
int fuse_service_getattr(const char* path, struct stat* statData);
int fuse_service_open(const char* path, struct fuse_file_info* fi);
int fuse_service_opendir(const char* path, struct fuse_file_info* fi);
int fuse_service_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi);
int fuse_service_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi);
int fuse_service_write(const char* path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi);

void start_fuse(const char * mountpoint, GostCrypt::Volume * volume, GostCrypt::FuseFileSystem *interface);

#endif //_FUSESERVICE_H
