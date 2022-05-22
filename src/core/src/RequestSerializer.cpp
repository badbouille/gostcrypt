/**
 * @file RequestSerializer.cpp
 * @author badbouille
 * @date 19/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "platform/endian.h"
#include <iostream>
#include "RequestSerializer.h"
#include "Buffer.h"

void serializeByte(char *c, uint8_t b) {
    static const char chartab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    c[0] = chartab[b >> 4];
    c[1] = chartab[b & 0x0F];
}

void deserializeByte(const char *c, uint8_t *b) {
    if (c[0] <= '9') {
        *b = (c[0] - '0');
    } else {
        *b = (c[0] - 'a' + 10);
    }
    *b <<= 4;
    if (c[1] <= '9') {
        *b |= (c[1] - '0');
    } else {
        *b |= (c[1] - 'a' + 10);
    }
}

int RequestSerializer_private_SerializeSize(const size_t *s, uint8_t *b, uint32_t *len) {
    *len = 8;

    *(b++) = (s[0] >> 56) & 0xFF;
    *(b++) = (s[0] >> 48) & 0xFF;
    *(b++) = (s[0] >> 40) & 0xFF;
    *(b++) = (s[0] >> 32) & 0xFF;
    *(b++) = (s[0] >> 24) & 0xFF;
    *(b++) = (s[0] >> 16) & 0xFF;
    *(b++) = (s[0] >>  8) & 0xFF;
    *(b++) = (s[0]      ) & 0xFF;

    return 0;
}

int RequestSerializer_private_DeserializeSize(size_t *s, uint8_t *b, uint32_t *len) {
    *len = 8;

    s[0] =               *(b++);
    s[0] = (s[0] << 8) | *(b++);
    s[0] = (s[0] << 8) | *(b++);
    s[0] = (s[0] << 8) | *(b++);
    s[0] = (s[0] << 8) | *(b++);
    s[0] = (s[0] << 8) | *(b++);
    s[0] = (s[0] << 8) | *(b++);
    s[0] = (s[0] << 8) | *(b++);

    return 0;
}

int RequestSerializer_private_SerializeString(const std::string *s, uint8_t *b, uint32_t *len) {
    uint32_t size = htobe32(s->size());

    *len = 4;

    *(b++) = (size >> 24) & 0xFF;
    *(b++) = (size >> 16) & 0xFF;
    *(b++) = (size >>  8) & 0xFF;
    *(b++) = (size      ) & 0xFF;

    for (char c : *s) {
        *(b++) = c;
    }

    *len += s->size();
    return 0;
}

int RequestSerializer_private_DeserializeString(std::string *s, const uint8_t *b, uint32_t *len)
{
    uint32_t size = 0;

    *len = 4;

    size =               *(b++);
    size = (size << 8) | *(b++);
    size = (size << 8) | *(b++);
    size = (size << 8) | *(b++);

    size = be32toh(size);

    s->resize(size);
    for (uint32_t i = 0; i < size; i++) {
        (*s)[i] = *(b++);
    }

    *len += size;
    return 0;
}

int RequestSerializer_private_SerializeBuffer(const GostCrypt::BufferPtr *s, uint8_t *b, uint32_t *len) {
    uint32_t size = htobe32(s->size());

    *len = 4;

    *(b++) = (size >> 24) & 0xFF;
    *(b++) = (size >> 16) & 0xFF;
    *(b++) = (size >>  8) & 0xFF;
    *(b++) = (size      ) & 0xFF;

    for (uint32_t i =0 ; i < s->size(); i++) {
        *(b++) = s->get()[i];
    }

    *len += s->size();
    return 0;
}

int RequestSerializer_private_DeserializeBuffer(GostCrypt::BufferPtr *s, const uint8_t *b, uint32_t *len)
{
    uint32_t size = 0; // TODO 64 bits

    *len = 4;

    size =               *(b++);
    size = (size << 8) | *(b++);
    size = (size << 8) | *(b++);
    size = (size << 8) | *(b++);

    size = be32toh(size);

    s->getRange(*s, 0, size);

    for (uint32_t i = 0; i < size; i++) {
        s->get()[i] = *(b++);
    }

    *len += size;
    return 0;
}

int RequestSerializer_api_SerializeMount(const GostCrypt::Core::MountParams_t *p, char **d, uint32_t *len) {
    uint8_t *tmp = nullptr;
    uint8_t *tmpid = nullptr;
    uint32_t tmplen;
    *d = new char[1024];
    tmp = new uint8_t[512];
    tmpid = tmp;
    *len = 0;

    if(*d == nullptr || tmpid == nullptr) {
        return 1;
    }

    /* storing all bytes in tmp */

    // volumePath
    RequestSerializer_private_SerializeString(&p->volumePath, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // mountpoint
    RequestSerializer_private_SerializeString(&p->mountPoint, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // password
    RequestSerializer_private_SerializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // filesystemID
    RequestSerializer_private_SerializeString(&p->fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    /* converting all bytes to char */
    uint32_t i;
    for (i = 0; i < *len; i++) {
        serializeByte((*d) + 2 * i, tmp[i]);
    }
    *((*d) + 2*i) = '\0';

    GostCrypt::SecureBufferPtr eraser(tmp, *len);
    eraser.erase();

    *len *= 2;

    return 0;
}

int RequestSerializer_api_DeserializeMount(GostCrypt::Core::MountParams_t *p, const char *d) {
    uint8_t *tmp = nullptr;
    uint8_t *tmpid = nullptr;
    uint32_t tmplen;

    tmp = new uint8_t[512];
    tmpid = tmp;

    if(tmpid == nullptr) {
        return 1;
    }

    /* converting all bytes to char */
    for (uint32_t i = 0; d[i] != '\0'; i+=2) {
        if (d[i + 1] == '\0') {
            return 1;
        }
        deserializeByte(d + i, tmpid++);
    }
    tmpid = tmp;

    /* storing all bytes in tmp */

    // volumePath
    RequestSerializer_private_DeserializeString(&p->volumePath, tmpid, &tmplen);

    tmpid += tmplen;

    // mountpoint
    RequestSerializer_private_DeserializeString(&p->mountPoint, tmpid, &tmplen);
    tmpid += tmplen;

    // password
    RequestSerializer_private_DeserializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;

    // filesystemID
    RequestSerializer_private_DeserializeString(&p->fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;

    GostCrypt::SecureBufferPtr eraser(tmp, tmpid - tmp);
    eraser.erase();

    return 0;
}

int RequestSerializer_api_SerializeCreate(const GostCrypt::Core::CreateParams_t *p, char **d, uint32_t *len) {
    uint8_t *tmp = nullptr;
    uint8_t *tmpid = nullptr;
    uint32_t tmplen;
    *d = new char[2048];
    tmp = new uint8_t[1024];
    tmpid = tmp;
    *len = 0;

    if(*d == nullptr || tmpid == nullptr) {
        return 1;
    }

    /* storing all bytes in tmp */

    // // MountParams

    // volumePath
    RequestSerializer_private_SerializeString(&p->afterCreationMount.volumePath, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // mountpoint
    RequestSerializer_private_SerializeString(&p->afterCreationMount.mountPoint, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // password
    RequestSerializer_private_SerializeBuffer(&p->afterCreationMount.password, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // filesystemID
    RequestSerializer_private_SerializeString(&p->afterCreationMount.fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // // CreateParams

    // algorithmID
    RequestSerializer_private_SerializeString(&p->algorithmID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // keyDerivationFunctionID
    RequestSerializer_private_SerializeString(&p->keyDerivationFunctionID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // volumeTypeID
    RequestSerializer_private_SerializeString(&p->volumeTypeID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // volumePath
    RequestSerializer_private_SerializeString(&p->volumePath, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // password
    RequestSerializer_private_SerializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // dataSize
    RequestSerializer_private_SerializeSize(&p->dataSize, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // sectorSize
    RequestSerializer_private_SerializeSize(&p->sectorSize, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    /* converting all bytes to char */
    uint32_t i;
    for (i = 0; i < *len; i++) {
        serializeByte((*d) + 2 * i, tmp[i]);
    }
    *((*d) + 2*i) = '\0';

    GostCrypt::SecureBufferPtr eraser(tmp, *len);
    eraser.erase();

    *len *= 2;

    return 0;
}

int RequestSerializer_api_DeserializeCreate(GostCrypt::Core::CreateParams_t *p, const char *d) {
    uint8_t *tmp = nullptr;
    uint8_t *tmpid = nullptr;
    uint32_t tmplen;

    tmp = new uint8_t[512];
    tmpid = tmp;

    if(tmpid == nullptr) {
        return 1;
    }

    /* converting all bytes to char */
    for (uint32_t i = 0; d[i] != '\0'; i+=2) {
        if (d[i + 1] == '\0') {
            return 1;
        }
        deserializeByte(d + i, tmpid++);
    }
    tmpid = tmp;

    /* storing all bytes in tmp */

    // // MountParams

    // volumePath
    RequestSerializer_private_DeserializeString(&p->afterCreationMount.volumePath, tmpid, &tmplen);
    tmpid += tmplen;

    // mountpoint
    RequestSerializer_private_DeserializeString(&p->afterCreationMount.mountPoint, tmpid, &tmplen);
    tmpid += tmplen;

    // password
    RequestSerializer_private_DeserializeBuffer(&p->afterCreationMount.password, tmpid, &tmplen);
    tmpid += tmplen;

    // filesystemID
    RequestSerializer_private_DeserializeString(&p->afterCreationMount.fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;

    // // CreateParams

    // algorithmID
    RequestSerializer_private_DeserializeString(&p->algorithmID, tmpid, &tmplen);
    tmpid += tmplen;

    // keyDerivationFunctionID
    RequestSerializer_private_DeserializeString(&p->keyDerivationFunctionID, tmpid, &tmplen);
    tmpid += tmplen;

    // volumeTypeID
    RequestSerializer_private_DeserializeString(&p->volumeTypeID, tmpid, &tmplen);
    tmpid += tmplen;

    // volumePath
    RequestSerializer_private_DeserializeString(&p->volumePath, tmpid, &tmplen);
    tmpid += tmplen;

    // password
    RequestSerializer_private_DeserializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;

    // dataSize
    RequestSerializer_private_DeserializeSize(&p->dataSize, tmpid, &tmplen);
    tmpid += tmplen;

    // sectorSize
    RequestSerializer_private_DeserializeSize(&p->sectorSize, tmpid, &tmplen);
    tmpid += tmplen;

    GostCrypt::SecureBufferPtr eraser(tmp, tmpid - tmp);
    eraser.erase();

    return 0;
}
