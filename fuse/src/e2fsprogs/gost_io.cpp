/*
 * gost_io.c --- This is the GostCrypt implementation
 *	of the I/O manager. This way, the fuse2fs from e2fsprogs can be
 *	flawlessly integrated into gostcrypt with minimum work.
 *
 * Copyright (C) 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001,
 *	2002 by Theodore Ts'o.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Library
 * General Public License, version 2.
 * %End-Header%
 */

#if !defined(__FreeBSD__) && !defined(__NetBSD__) && !defined(__OpenBSD__)
#define _XOPEN_SOURCE 600
#define _DARWIN_C_SOURCE
#define _FILE_OFFSET_BITS 64
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <stdio.h>
#include <string.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_ERRNO_H
#include <errno.h>
#endif
#include <fcntl.h>
#include <time.h>
#ifdef __linux__
#include <sys/utsname.h>
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
#ifdef HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#else
#define PR_GET_DUMPABLE 3
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#if HAVE_LINUX_FALLOC_H
#include <linux/falloc.h>
#endif

#if defined(__linux__) && defined(_IO) && !defined(BLKROGET)
#define BLKROGET   _IO(0x12, 94) /* Get read-only status (0 = read_write).  */
#endif

#undef ALIGN_DEBUG

#include "ext2fs/ext2_fs.h"
#include "ext2fs/ext2fs.h"

#define EXT2_CHECK_MAGIC(struct, code) \
	  if ((struct)->magic != (code)) return (code)

#include "Volume.h"

// weird way of giving current volume
// TODO comment this
GostCrypt::Volume *already_opened_volume;

// custom io manager
/* gost_io.cpp */
extern io_manager gost_io_manager;
extern io_manager gostfd_io_manager;

// My own magic number
#define EXT2_ET_MAGIC_GOST_IO_CHANNEL            (2133572334L)

struct gostio_private_data {
	int	magic;
	int	flags;
    GostCrypt::Volume *v;
	struct struct_io_stats io_stats;
};

#define IS_ALIGNED(n, align) ((((uintptr_t) n) & \
			       ((uintptr_t) ((align)-1))) == 0)

// TODO
static errcode_t unix_get_stats(io_channel channel, io_stats *stats)
{
	errcode_t	retval = 0;

	struct gostio_private_data *data;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	if (stats)
		*stats = &data->io_stats;

	return retval;
}

/*
 * Here are the raw I/O functions
 */
static errcode_t raw_read_blk(io_channel channel,
			      struct gostio_private_data *data,
			      unsigned long long block,
			      int count, void *bufv)
{
	ssize_t		size;
	ext2_loff_t	location;
	GostCrypt::SecureBufferPtr buf;

	size = (count < 0) ? -count : (ext2_loff_t) count * channel->block_size;
	data->io_stats.bytes_read += size;
	location = ((ext2_loff_t) block * channel->block_size);

	try
    {
        // hoping computed size is alright
        buf.set((uint8_t *) bufv, size);

        // reading
        data->v->read(buf, location);
    } catch(GostCrypt::GostCryptException &e)
    {
	    // TODO handle more errors, like shortread for example.
	    return 1;
    }

	return 0;
}

static errcode_t raw_write_blk(io_channel channel,
			       struct gostio_private_data *data,
			       unsigned long long block,
			       int count, const void *bufv)
{
    ssize_t		size;
    ext2_loff_t	location;
    GostCrypt::SecureBufferPtr buf;

    size = (count < 0) ? -count : (ext2_loff_t) count * channel->block_size;
    data->io_stats.bytes_written += size;
    location = ((ext2_loff_t) block * channel->block_size);

    try
    {
        // hoping computed size is alright
        buf.set((uint8_t *) bufv, size);

        // reading
        data->v->write(buf, location);
    } catch(GostCrypt::GostCryptException &e)
    {
        // TODO handle more errors, like shortwrite for example.
        return 1;
    }

    return 0;
}


/*
 * Here we implement the cache functions
 */

/* Allocate the cache buffers */
static errcode_t alloc_cache(io_channel channel,
			     struct gostio_private_data *data)
{
	return 0; // no cache implemented
}

/* Free the cache buffers */
static void free_cache(struct gostio_private_data *data)
{
}

#ifdef __linux__
#ifndef BLKDISCARDZEROES
#define BLKDISCARDZEROES _IO(0x12,124)
#endif
#endif

/*
 * This function is a bit weird.
 * classic usage:
 *  [User] -> ext2fs_open2("monfichier", io_mgr) -> [e2fs]
 *                                                  [e2fs] -> io_mgr.open(monfichier, ctx) -> [io_mgr]
 *                                                  [e2fs] -> io_mgr.read(monfichier, ctx) -> [io_mgr]
 *
 * my usage:
 *  [GOST]   -> ext2fs_open2("bullshit", gio_mgr) -> [e2fs]
 *  [gio_mgr]    <- gio_mgr.open(monfichier, ctx) <- [e2fs]
 *  [gio_mgr] -> return already opened volume ->     [e2fs]
 *  [gio_mgr]    <- gio_mgr.read(monfichier, ctx) <- [e2fs]
 *  [gio_mgr] -> return ctx.volume.read ->           [e2fs]
 */
static errcode_t unix_open_channel(const char *name, int fd,
				   int flags, io_channel *channel,
				   io_manager io_mgr)
{
	io_channel	io = NULL;
	struct gostio_private_data *data = NULL;
	errcode_t	retval;
	ext2fs_struct_stat st;
#ifdef __linux__
	struct		utsname ut;
#endif

	retval = ext2fs_get_mem(sizeof(struct struct_io_channel), &io);
	if (retval)
		goto cleanup;
	memset(io, 0, sizeof(struct struct_io_channel));
	io->magic = EXT2_ET_MAGIC_IO_CHANNEL;
	retval = ext2fs_get_mem(sizeof(struct gostio_private_data), &data);
	if (retval)
		goto cleanup;

	io->manager = io_mgr;
	retval = ext2fs_get_mem(strlen(name)+1, &io->name);
	if (retval)
		goto cleanup;

	strcpy(io->name, name);
	io->private_data = data;
	io->block_size = 1024;
	io->read_error = 0;
	io->write_error = 0;
	io->refcount = 1;

	memset(data, 0, sizeof(struct gostio_private_data));
	data->magic = EXT2_ET_MAGIC_GOST_IO_CHANNEL;
	data->io_stats.num_fields = 2;
	data->flags = flags;
	data->v = already_opened_volume; // getting already opened volume handle

    // TODO : compute this.. or not
	io->align = io->block_size;

#if defined(__CYGWIN__)
	/*
	 * Some operating systems require that the buffers be aligned,
	 * regardless of O_DIRECT
	 */
	if (!io->align)
		io->align = 512;
#endif

#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
	if (io->flags & CHANNEL_FLAGS_BLOCK_DEVICE) {
		int dio_align = ext2fs_get_dio_alignment(fd);

		if (io->align < dio_align)
			io->align = dio_align;
	}
#endif

	if ((retval = alloc_cache(io, data)))
		goto cleanup;

#ifdef BLKROGET
	if (flags & IO_FLAG_RW) {
		int error;
		int readonly = 0;

		/* Is the block device actually writable? */
		error = ioctl(data->dev, BLKROGET, &readonly);
		if (!error && readonly) {
			retval = EPERM;
			goto cleanup;
		}
	}
#endif
	*channel = io;
	return 0;

cleanup:
	if (data) {
		free_cache(data);
		ext2fs_free_mem(&data);
	}
	if (io) {
		if (io->name) {
			ext2fs_free_mem(&io->name);
		}
		ext2fs_free_mem(&io);
	}
	return retval;
}

static errcode_t unixfd_open(const char *str_fd, int flags,
			     io_channel *channel)
{
	int fd;
	int fd_flags;

	fd = atoi(str_fd);
#if defined(HAVE_FCNTL)
	fd_flags = fcntl(fd, F_GETFD);
	if (fd_flags == -1)
		return -EBADF;

	flags = 0;
	if (fd_flags & O_RDWR)
		flags |= IO_FLAG_RW;
	if (fd_flags & O_EXCL)
		flags |= IO_FLAG_EXCLUSIVE;
#if defined(O_DIRECT)
	if (fd_flags & O_DIRECT)
		flags |= IO_FLAG_DIRECT_IO;
#endif
#endif  /* HAVE_FCNTL */

	return unix_open_channel(str_fd, fd, flags, channel, gostfd_io_manager);
}

static errcode_t unix_open(const char *name, int flags,
			   io_channel *channel)
{
	int fd = -1;
	int open_flags;

	if (name == 0)
		return EXT2_ET_BAD_DEVICE_NAME;

	open_flags = (flags & IO_FLAG_RW) ? O_RDWR : O_RDONLY;
	if (flags & IO_FLAG_EXCLUSIVE)
		open_flags |= O_EXCL;
#if defined(O_DIRECT)
	if (flags & IO_FLAG_DIRECT_IO)
		open_flags |= O_DIRECT;
#endif

#if defined(F_NOCACHE) && !defined(IO_DIRECT)
	if (flags & IO_FLAG_DIRECT_IO) {
		if (fcntl(fd, F_NOCACHE, 1) < 0)
			return errno;
	}
#endif
	return unix_open_channel(name, fd, flags, channel, gost_io_manager);
}

static errcode_t unix_close(io_channel channel)
{
	struct gostio_private_data *data;
	errcode_t	retval = 0;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	if (--channel->refcount > 0)
		return 0;

	// TODO not closing the volume. It's FUSE's work to close it.

	free_cache(data);

	ext2fs_free_mem(&channel->private_data);
	if (channel->name)
		ext2fs_free_mem(&channel->name);
	ext2fs_free_mem(&channel);
	return retval;
}

static errcode_t unix_set_blksize(io_channel channel, int blksize)
{
	struct gostio_private_data *data;
	errcode_t		retval;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	if (channel->block_size != blksize) {
		channel->block_size = blksize;
		free_cache(data);
		if ((retval = alloc_cache(channel, data)))
			return retval;
	}
	return 0;
}

static errcode_t unix_read_blk64(io_channel channel, unsigned long long block,
			       int count, void *buf)
{
	struct gostio_private_data *data;;
	errcode_t	retval;
	char		*cp;
	int		i, j;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	return raw_read_blk(channel, data, block, count, buf);
}

static errcode_t unix_read_blk(io_channel channel, unsigned long block,
			       int count, void *buf)
{
	return unix_read_blk64(channel, block, count, buf);
}

static errcode_t unix_write_blk64(io_channel channel, unsigned long long block,
				int count, const void *buf)
{
	struct gostio_private_data *data;
	errcode_t	retval = 0;
	const char	*cp;
	int		writethrough;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	return raw_write_blk(channel, data, block, count, buf);
}

static errcode_t unix_cache_readahead(io_channel channel,
				      unsigned long long block,
				      unsigned long long count)
{
    // TODO amazing feature probably needed for our app
	return EXT2_ET_OP_NOT_SUPPORTED;
}

static errcode_t unix_write_blk(io_channel channel, unsigned long block,
				int count, const void *buf)
{
	return unix_write_blk64(channel, block, count, buf);
}

static errcode_t unix_write_byte(io_channel channel, unsigned long offset,
				 int size, const void *buf)
{
	struct gostio_private_data *data;
    GostCrypt::SecureBufferPtr b;
	errcode_t	retval = 0;
	ssize_t		actual;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	if (channel->align != 0) {
#ifdef ALIGN_DEBUG
		printf("unix_write_byte: O_DIRECT fallback\n");
#endif
		return EXT2_ET_UNIMPLEMENTED;
	}

    try
    {
        b.set((uint8_t *) buf, size);

        // reading
        data->v->write(b, offset);
    } catch(GostCrypt::GostCryptException &e)
    {
        // TODO handle more errors, like shortread for example.
        retval = 1;
    }

	return retval;
}

/*
 * Flush data buffers to disk.
 */
static errcode_t unix_flush(io_channel channel)
{
	struct gostio_private_data *data;
	errcode_t retval = 0;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

#ifdef HAVE_FSYNC
	if (!retval && fsync(data->dev) != 0)
		return errno;
#endif
	return retval;
}

static errcode_t unix_set_option(io_channel channel, const char *option,
				 const char *arg)
{
	struct gostio_private_data *data;
	unsigned long long tmp;
	char *end;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	// file offset is not supported.

	return EXT2_ET_INVALID_ARGUMENT;
}

#if defined(__linux__) && !defined(BLKDISCARD)
#define BLKDISCARD		_IO(0x12,119)
#endif

static errcode_t unix_discard(io_channel channel, unsigned long long block,
			      unsigned long long count)
{
	struct gostio_private_data *data;
	int		ret;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

	// TODO : is this needed ?
	// maybe write zeroes to discarded area ?

	return EXT2_ET_UNIMPLEMENTED;
}

/* parameters might not be used if OS doesn't support zeroout */
#if __GNUC_PREREQ (4, 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
static errcode_t unix_zeroout(io_channel channel, unsigned long long block,
			      unsigned long long count)
{
	struct gostio_private_data *data;
	int		ret;

	EXT2_CHECK_MAGIC(channel, EXT2_ET_MAGIC_IO_CHANNEL);
	data = (struct gostio_private_data *) channel->private_data;
	EXT2_CHECK_MAGIC(data, EXT2_ET_MAGIC_GOST_IO_CHANNEL);

    // TODO : is this needed ?
    // maybe write zeroes ?

	return EXT2_ET_UNIMPLEMENTED;
}
#if __GNUC_PREREQ (4, 6)
#pragma GCC diagnostic pop
#endif

static struct struct_io_manager struct_gostio_manager = {
	.magic		= EXT2_ET_MAGIC_IO_MANAGER,
	.name		= "GostCrypt I/O Manager",
	.open		= unix_open,
	.close		= unix_close,
	.set_blksize	= unix_set_blksize,
	.read_blk	= unix_read_blk,
	.write_blk	= unix_write_blk,
	.flush		= unix_flush,
	.write_byte	= unix_write_byte,
	.set_option	= unix_set_option,
	.get_stats	= unix_get_stats,
	.read_blk64	= unix_read_blk64,
	.write_blk64	= unix_write_blk64,
	.discard	= unix_discard,
	.cache_readahead	= unix_cache_readahead,
	.zeroout	= unix_zeroout,
};

io_manager gost_io_manager = &struct_gostio_manager;

static struct struct_io_manager struct_gostiofd_manager = {
	.magic		= EXT2_ET_MAGIC_IO_MANAGER,
	.name		= "GostCrypt fd I/O Manager",
	.open		= unixfd_open,
	.close		= unix_close,
	.set_blksize	= unix_set_blksize,
	.read_blk	= unix_read_blk,
	.write_blk	= unix_write_blk,
	.flush		= unix_flush,
	.write_byte	= unix_write_byte,
	.set_option	= unix_set_option,
	.get_stats	= unix_get_stats,
	.read_blk64	= unix_read_blk64,
	.write_blk64	= unix_write_blk64,
	.discard	= unix_discard,
	.cache_readahead	= unix_cache_readahead,
	.zeroout	= unix_zeroout,
};

io_manager gostfd_io_manager = &struct_gostiofd_manager;
