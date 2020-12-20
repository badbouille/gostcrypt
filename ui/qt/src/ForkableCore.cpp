//
// Created by badbouille on 19/12/2020.
//

#include <wait.h>
#include "ForkableCore.h"
#include "Buffer.h"
#include "Core.h"
#include "GraphicInterface.h"

int ForkableCore_api_handler(int argc, char **argv) {

    if (argc != 3) return 8;

    if (std::string(argv[1]) == "mount") {
        // calling mount command
        GostCrypt::SecureBuffer pass(64);
        GostCrypt::Core::MountParams_t p;

        pass.getRange(p.password, 0, pass.size());
        ForkableCore_api_DeserializeMount(&p, argv[2]);

        // erasing all sensible data
        GostCrypt::SecureBufferPtr dataptr((uint8_t *)argv[2], strlen(argv[2]));
        dataptr.erase();

        try {
            GostCrypt::Core::mount(&p);
        } catch (GostCrypt::VolumePasswordException &e) {
            pass.erase();
            return 2;
        } catch (GostCrypt::GostCryptException &e) {
            qDebug() << e.what();
            pass.erase();
            return 1;
        }
        pass.erase();
        return 0;

    }
    return 8;
}

int ForkableCore_api_callMount(const GostCrypt::Core::MountParams_t *p) {

    pid_t pid;
    int status;
    static char argvT[][256] = { "api", "mount", "" };
    static char *argv[] = { argvT[0], argvT[1], nullptr, nullptr };
    uint32_t len;

    pid = fork();

    if (pid == 0) {
        ForkableCore_api_SerializeMount(p, &argv[2], &len);
        execv(g_prog_path, argv);
        exit(127);
    }

    /* Waiting for child to build disk image */
    if (waitpid(pid, &status, 0) == -1 ) {
        throw GOSTCRYPTEXCEPTION("waitpid failed.");
    }

    /* Checking return value */
    if ( WIFEXITED(status) ) {
        if (WEXITSTATUS(status) != 0) {
            if (WEXITSTATUS(status) == 2) {
                throw VOLUMEPASSWORDEXCEPTION();
            } else {
                throw GOSTCRYPTEXCEPTION("Unknown exception");
            }
        }
    } else {
        throw GOSTCRYPTEXCEPTION("Child not exited.");
    }

    return 0;
}

void getChars(char *c, uint8_t b) {
    static const char chartab[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    c[0] = chartab[b >> 4];
    c[1] = chartab[b & 0x0F];
}

void getUint8(const char *c, uint8_t *b) {
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

int ForkableCore_private_SerializeString(const std::string *s, uint8_t *b, uint32_t *len) {
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

int ForkableCore_private_DeserializeString(std::string *s, const uint8_t *b, uint32_t *len)
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

int ForkableCore_private_SerializeBuffer(const GostCrypt::BufferPtr *s, uint8_t *b, uint32_t *len) {
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

int ForkableCore_private_DeserializeBuffer(GostCrypt::BufferPtr *s, const uint8_t *b, uint32_t *len)
{
    uint32_t size = 0;

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

int ForkableCore_api_SerializeMount(const GostCrypt::Core::MountParams_t *p, char **d, uint32_t *len) {
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
    ForkableCore_private_SerializeString(&p->volumePath, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // mountpoint
    ForkableCore_private_SerializeString(&p->mountPoint, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // password
    ForkableCore_private_SerializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // filesystemID
    ForkableCore_private_SerializeString(&p->fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    /* converting all bytes to char */
    uint32_t i;
    for (i = 0; i < *len; i++) {
        getChars((*d) + 2*i, tmp[i]);
    }
    *((*d) + 2*i) = '\0';

    GostCrypt::SecureBufferPtr eraser(tmp, *len);
    eraser.erase();

    *len *= 2;

    return 0;
}

int ForkableCore_api_DeserializeMount(GostCrypt::Core::MountParams_t *p, const char *d) {
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
        getUint8(d + i, tmpid++);
    }
    tmpid = tmp;

    /* storing all bytes in tmp */

    // volumePath
    ForkableCore_private_DeserializeString(&p->volumePath, tmpid, &tmplen);

    tmpid += tmplen;

    // mountpoint
    ForkableCore_private_DeserializeString(&p->mountPoint, tmpid, &tmplen);
    tmpid += tmplen;

    // password
    ForkableCore_private_DeserializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;

    // filesystemID
    ForkableCore_private_DeserializeString(&p->fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;

    GostCrypt::SecureBufferPtr eraser(tmp, tmpid - tmp);
    eraser.erase();

    return 0;
}
