//
// Created by badbouille on 15/11/2020.
//

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <argp.h>
#include "CommonTypes.h"

/* Common values */

#define PROGRAM_VERSION "GostCrypt 0.1.0"
#define PROGRAM_ADDRESS "gostcrypt.org"

/* Default values */

/** Default filesystem is no-filesystem */ // TODO change this to ext2
#define DEFAULT_FILESYSTEMID "none"

/** Default sectorsize is 2048bits (256bytes) */
#define DEFAULT_SECTORSIZE (256*B)

/** Default volumesize is 50MB */
#define DEFAULT_VOLUMESIZE (50*MB)

/** Default algorithm is ECB with XOR */ // TODO change this to XTS_{GOST-G}
#define DEFAULT_ALGORITHM "ECB_{XOR128}"

/** Default Key derivation function */ // TODO change this to Streebog or something
#define DEFAULT_KDF "XOR0"

/** Default volumetype is 'standard' */
#define DEFAULT_VOLUMETYPE "standard"

/* Commands */
int cmd_mount(int argc, char **argv);
int cmd_create(int argc, char **argv);
int cmd_umount(int argc, char **argv);
int cmd_list(int argc, char **argv);

#endif //_COMMANDS_H
