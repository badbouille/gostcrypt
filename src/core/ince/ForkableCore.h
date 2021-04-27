/**
 * @file ForkableCore.h
 * @author badbouille
 * @date 19/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef GOSTCRYPT_FORKABLECORE_H
#define GOSTCRYPT_FORKABLECORE_H

#include <Core.h>

/* Current program path */
extern const char *g_prog_path;

#define SHARED_KEY_PREFIX "gostcrypt-shared-mem-"

/* Callback used in forked process to send info across shared memory */
void ForkableCore_uicallback(const char *message, float percent);

/* Callback used in main process to show received info... since main process may (obviously) be forked multiple times, id is necessary */
typedef void (*UICallBackFunction_t)(const char *, float, uint32_t);

/* Callback used for after the execution, to analyse the results */
typedef int (*DoneCallBackFunction_t)(uint32_t, uint32_t);

int ForkableCore_api_handler(int argc, char **argv);

int ForkableCore_api_SerializeMount(const GostCrypt::Core::MountParams_t *p, char **d, uint32_t *len);
int ForkableCore_api_DeserializeMount(GostCrypt::Core::MountParams_t *p, const char *d);

#endif //GOSTCRYPT_FORKABLECORE_H
