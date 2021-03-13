/*
 * Copyright (c) 2015, Kaho Ng, ngkaho1234@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - The name of the author may not be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _FILE_OFFSET_BITS 64

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#if defined(__linux__)
#include <linux/fs.h>
#elif defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/disk.h>
#endif

#include "lwext4.h"

/**@brief   Image block size.*/
#define EXT4_BLOCKDEV_BSIZE 512

struct block_dev {
	struct ext4_blockdev bdev;
	struct ext4_blockdev_iface bdif;
	unsigned char block_buf[EXT4_BLOCKDEV_BSIZE];
	int fd;
};

#define ALLOC_BDEV() calloc(1, sizeof(struct block_dev))
#define FREE_BDEV(p) free(p)

/**********************BLOCKDEV INTERFACE**************************************/
int blockdev_get(char *fname, struct ext4_blockdev **pbdev);
void blockdev_put(struct ext4_blockdev *bdev);
static int blockdev_open(struct ext4_blockdev *bdev);
static int blockdev_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
						 uint32_t blk_cnt);
static int blockdev_bwrite(struct ext4_blockdev *bdev, const void *buf,
						  uint64_t blk_id, uint32_t blk_cnt);
static int blockdev_close(struct ext4_blockdev *bdev);

/******************************************************************************/
static int __blockdev_get(const char *path)
{
	int disk_fd = open(path, O_RDWR);
	if (disk_fd < 0) {
		return -errno;
	}

	return disk_fd;
}

static void __blockdev_put(int fd)
{
	if (fd)
		close(fd);
}

int blockdev_get(char *fname, struct ext4_blockdev **pbdev)
{
	struct block_dev *bdev;
	int dev_file = __blockdev_get(fname);
	int64_t block_cnt = 0;
	struct stat stat = { 0 };

	if (dev_file < 0)
		return LWEXT4_ERRNO(EIO);

	bdev = ALLOC_BDEV();
	if (!bdev) {
		__blockdev_put(dev_file);
		return LWEXT4_ERRNO(ENOMEM);
	}
	bdev->fd = dev_file;
	bdev->bdif.ph_bsize = EXT4_BLOCKDEV_BSIZE;
	fstat(dev_file, &stat);
	if (S_ISBLK(stat.st_mode)) {
#if defined(__linux__)
		ioctl(dev_file, BLKGETSIZE64, &block_cnt);
		block_cnt /= EXT4_BLOCKDEV_BSIZE;
#elif defined(__APPLE__) 
		ioctl(dev_file, DKIOCGETBLOCKCOUNT, &block_cnt);
#elif defined(__FreeBSD__)
	} else if (S_ISCHR(stat.st_mode)) {
		ioctl(dev_file, DIOCGMEDIASIZE, &block_cnt);
		block_cnt /= EXT4_BLOCKDEV_BSIZE;
#else
 #error "Currently support Linux, FreeBSD and OS X only."
#endif
	} else if (S_ISREG(stat.st_mode & S_IFMT)) {
		block_cnt = lseek(dev_file, 0, SEEK_END) / EXT4_BLOCKDEV_BSIZE;
		lseek(dev_file, 0, SEEK_SET);
	} else {
		FREE_BDEV(bdev);
		__blockdev_put(dev_file);
		return LWEXT4_ERRNO(EINVAL);
	}
	bdev->bdif.ph_bcnt = block_cnt;
	bdev->bdif.ph_bbuf = bdev->block_buf;

	bdev->bdif.open = blockdev_open;
	bdev->bdif.bread = blockdev_bread;
	bdev->bdif.bwrite = blockdev_bwrite;
	bdev->bdif.close = blockdev_close;

	bdev->bdev.bdif = &bdev->bdif;
	bdev->bdev.part_offset = 0;
	bdev->bdev.part_size = block_cnt * EXT4_BLOCKDEV_BSIZE;

	*pbdev = (struct ext4_blockdev *)bdev;

	return LWEXT4_ERRNO(EOK);
}

void blockdev_put(struct ext4_blockdev *bdev)
{
	__blockdev_put(((struct block_dev *)bdev)->fd);
	FREE_BDEV(bdev);
}

static int blockdev_open(struct ext4_blockdev *bdev)
{
	return LWEXT4_ERRNO(EOK);
}

/******************************************************************************/

static int blockdev_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
						 uint32_t blk_cnt)
{
	int ret = pread(((struct block_dev *)bdev)->fd,
					buf,
					bdev->bdif->ph_bsize * blk_cnt,
					blk_id * bdev->bdif->ph_bsize);
	if ((size_t)ret != bdev->bdif->ph_bsize * blk_cnt)
		return LWEXT4_ERRNO(EIO);

	return LWEXT4_ERRNO(EOK);
}

/******************************************************************************/
static int blockdev_bwrite(struct ext4_blockdev *bdev, const void *buf,
						  uint64_t blk_id, uint32_t blk_cnt)
{
	int ret = pwrite(((struct block_dev *)bdev)->fd,
					buf,
					bdev->bdif->ph_bsize * blk_cnt,
					blk_id * bdev->bdif->ph_bsize);
	if ((size_t)ret != bdev->bdif->ph_bsize * blk_cnt)
		return LWEXT4_ERRNO(EIO);

	return LWEXT4_ERRNO(EOK);
}
/******************************************************************************/
static int blockdev_close(struct ext4_blockdev *bdev)
{
	__blockdev_put(((struct block_dev *)bdev)->fd);
	((struct block_dev *)bdev)->fd = 0;
	return LWEXT4_ERRNO(EOK);
}


/******************************************************************************/
