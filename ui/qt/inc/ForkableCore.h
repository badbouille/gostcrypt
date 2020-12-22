//
// Created by badbouille on 19/12/2020.
//

#ifndef GOSTCRYPT_FORKABLECORE_H
#define GOSTCRYPT_FORKABLECORE_H

#include <Core.h>

int ForkableCore_api_handler(int argc, char **argv);
int ForkableCore_api_callMount(const GostCrypt::Core::MountParams_t *p);
int ForkableCore_api_callCreate(const GostCrypt::Core::CreateParams_t *p);

int ForkableCore_api_SerializeMount(const GostCrypt::Core::MountParams_t *p, char **d, uint32_t *len);
int ForkableCore_api_DeserializeMount(GostCrypt::Core::MountParams_t *p, const char *d);

int ForkableCore_api_SerializeCreate(const GostCrypt::Core::CreateParams_t *p, char **d, uint32_t *len);
int ForkableCore_api_DeserializeCreate(GostCrypt::Core::CreateParams_t *p, const char *d);

#endif //GOSTCRYPT_FORKABLECORE_H
