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

#include "Volume.h"

/**@brief   Image block size.*/
#define EXT4_BLOCKDEV_BSIZE 1024

struct block_dev {
	struct ext4_blockdev bdev;
	struct ext4_blockdev_iface bdif;
    GostCrypt::Volume *volume;
	unsigned char block_buf[EXT4_BLOCKDEV_BSIZE];
};

#define ALLOC_BDEV() calloc(1, sizeof(struct block_dev))
#define FREE_BDEV(p) free(p)

/**********************BLOCKDEV INTERFACE**************************************/
int blockdev_get(const char *fname, struct ext4_blockdev **pbdev) { return LWEXT4_ERRNO(EIO); } // unimplemented in GostCrypt
int blockdev_get_volume(GostCrypt::Volume *target, struct ext4_blockdev **pbdev);

static int blockdev_open(struct ext4_blockdev *bdev);
static int blockdev_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
						 uint32_t blk_cnt);
static int blockdev_bwrite(struct ext4_blockdev *bdev, const void *buf,
						  uint64_t blk_id, uint32_t blk_cnt);
static int blockdev_close(struct ext4_blockdev *bdev);

/******************************************************************************/

int blockdev_get_volume(GostCrypt::Volume *target, struct ext4_blockdev **pbdev)
{
	struct block_dev *bdev;
	int64_t block_cnt = 0;

	if (target == nullptr)
		return LWEXT4_ERRNO(EIO);

	bdev = (struct block_dev *)ALLOC_BDEV();
	if (!bdev) {
		return LWEXT4_ERRNO(ENOMEM);
	}
	bdev->bdif.ph_bsize = EXT4_BLOCKDEV_BSIZE;

    block_cnt = target->getSize() / EXT4_BLOCKDEV_BSIZE;

	bdev->bdif.ph_bcnt = block_cnt;
	bdev->bdif.ph_bbuf = bdev->block_buf;

	bdev->bdif.open = blockdev_open;
	bdev->bdif.bread = blockdev_bread;
	bdev->bdif.bwrite = blockdev_bwrite;
	bdev->bdif.close = blockdev_close;

	bdev->bdev.bdif = &bdev->bdif;
	bdev->bdev.part_offset = 0;
	bdev->bdev.part_size = block_cnt * EXT4_BLOCKDEV_BSIZE;

    bdev->volume = target;

	*pbdev = (struct ext4_blockdev *)bdev;

	return LWEXT4_ERRNO(EOK);
}

extern "C" { /* Declared extern C to be able to link it to the one in  */
void blockdev_put(struct ext4_blockdev *bdev)
{
    FREE_BDEV(bdev);
}
}

static int blockdev_open(struct ext4_blockdev *bdev)
{
	return LWEXT4_ERRNO(EOK);
}

/******************************************************************************/

static int blockdev_bread(struct ext4_blockdev *bdev, void *buf, uint64_t blk_id,
						 uint32_t blk_cnt)
{
    GostCrypt::SecureBufferPtr sbuf;
    struct block_dev *fbdev = (struct block_dev *)bdev;

    try
    {
        // hoping computed size is alright
        sbuf.set((uint8_t *) buf, bdev->bdif->ph_bsize * blk_cnt);

        // reading
        fbdev->volume->read(sbuf, blk_id * bdev->bdif->ph_bsize);
    } catch(GostCrypt::GostCryptException &e)
    {
        // TODO handle more errors, like shortread for example.
        return LWEXT4_ERRNO(EIO);
    }

    return LWEXT4_ERRNO(EOK);
}

/******************************************************************************/
static int blockdev_bwrite(struct ext4_blockdev *bdev, const void *buf,
						  uint64_t blk_id, uint32_t blk_cnt)
{
    GostCrypt::SecureBufferPtr sbuf;
    struct block_dev *fbdev = (struct block_dev *)bdev;

    try
    {
        // hoping computed size is alright
        sbuf.set((uint8_t *) buf, bdev->bdif->ph_bsize * blk_cnt);

        // reading
        fbdev->volume->write(sbuf, blk_id * bdev->bdif->ph_bsize);
    } catch(GostCrypt::GostCryptException &e)
    {
        // TODO handle more errors, like shortread for example.
        return LWEXT4_ERRNO(EIO);
    }

    return LWEXT4_ERRNO(EOK);
}
/******************************************************************************/
static int blockdev_close(struct ext4_blockdev *bdev)
{
	return LWEXT4_ERRNO(EOK);
}


/******************************************************************************/
