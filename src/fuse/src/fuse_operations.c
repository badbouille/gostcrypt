/**
 * @file fuse_operations.c
 * @author badbouille
 * @date 14/03/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "ops.h"
#include "lwext4.h"

// undefining ftruncate for this file because it is defined in a .h and breaks the structure
#ifdef ftruncate
#undef ftruncate
#endif

struct fuse_operations e4f_ops = {
        .getattr = op_getattr,
        .readlink = op_readlink,
        .mkdir = op_mkdir,
        .unlink = op_unlink,
        .rmdir = op_rmdir,
        .symlink = op_symlink,
        .rename = op_rename,
        .link = op_link,
        .chmod = op_chmod,
        .chown = op_chown,
        .truncate = op_truncate,
        .utime	  = op_utimes,
        .open = op_open,
        .read = op_read,
        .write = op_write,
        .release = op_release,
        .setxattr = op_setxattr,
        .getxattr = op_getxattr,
        .listxattr = op_listxattr,
        .removexattr = op_removexattr,
        .opendir = op_opendir,
        .readdir = op_readdir,
        .releasedir = op_release,
        .init = op_init,
        .destroy = op_destroy,
        .create = op_create,
        .ftruncate = op_ftruncate,
#if !defined(__FreeBSD__)
        .utimens	= op_utimens,
#endif
};
