/**
 * @file RequestSerializer.h
 * @author badbouille
 * @date 19/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef GOSTCRYPT_REQUESTSERIALIZER_H
#define GOSTCRYPT_REQUESTSERIALIZER_H

#include <Core.h>

int RequestSerializer_api_SerializeMount(const GostCrypt::Core::MountParams_t *p, char **d, uint32_t *len);
int RequestSerializer_api_DeserializeMount(GostCrypt::Core::MountParams_t *p, const char *d);

#endif //GOSTCRYPT_REQUESTSERIALIZER_H
