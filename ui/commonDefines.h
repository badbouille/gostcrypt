//
// Created by badbouille on 16/12/2020.
//

#ifndef _COMMONDEFINES_H
#define _COMMONDEFINES_H

#include "CommonTypes.h"

/* Common values */

#define PROGRAM_NAME "GostCrypt"
#define PROGRAM_VERSION "2.1.0"
#define PROGRAM_ADDRESS "gostcrypt.org"
#define PROGRAM_BUG_ADDRESS "badbouille@" PROGRAM_ADDRESS

/* Default values */

/** Default filesystem is no-filesystem */ // TODO change this to ext2
#define DEFAULT_FILESYSTEMID "ext2"

/** Default sectorsize is 2048bits (256bytes) */
#define DEFAULT_SECTORSIZE (256*B)

/** Default volumesize is 50MB */
#define DEFAULT_VOLUMESIZE (50*MB)

/** Default algorithm is ECB with XOR */ // TODO change this to XTS_{GOST-G}
#define DEFAULT_ALGORITHM "ECB_{XOR128}"

/** Default Key derivation function */ // TODO change this to Streebog or something
#define DEFAULT_KDF "Pbkdf2-HMAC-XOR0-16"

/** Default volumetype is 'standard' */
#define DEFAULT_VOLUMETYPE "standard"

#endif // _COMMONDEFINES_H
