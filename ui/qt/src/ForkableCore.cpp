//
// Created by badbouille on 19/12/2020.
//

#include <wait.h>
#include <iostream>
#include "ForkableCore.h"
#include "Buffer.h"
#include "Core.h"
#include "GraphicInterface.h"

QSharedMemory g_currentprogress;

int ForkableCore_api_handler(int argc, char **argv) {
    int ret = 8;

    if (argc < 3 || argc > 4) return ret;
    if (argc == 4) {
        g_currentprogress.setKey(argv[3]);
        g_currentprogress.attach();
        GostCrypt::Core::setCallBack(ForkableCore_uicallback);
    }

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
        ret = 0;
    }
    if (std::string(argv[1]) == "create") {
        // calling mount command
        GostCrypt::SecureBuffer pass(64);
        GostCrypt::Core::CreateParams_t p;

        pass.getRange(p.afterCreationMount.password, 0, pass.size());
        pass.getRange(p.password, 0, pass.size());
        ForkableCore_api_DeserializeCreate(&p, argv[2]);

        // erasing all sensible data
        GostCrypt::SecureBufferPtr dataptr((uint8_t *)argv[2], strlen(argv[2]));
        dataptr.erase();

        try {
            GostCrypt::Core::create(&p);
        } catch (GostCrypt::GostCryptException &e) {
            qDebug() << e.what();
            pass.erase();
            return 1;
        }
        pass.erase();
        ret = 0;
    }
    if (argc == 4) {
        g_currentprogress.detach();
    }
    return ret;
}

int ForkableCore_api_callMount(const GostCrypt::Core::MountParams_t *p, DoneCallBackFunction_t final, UICallBackFunction_t f, const uint32_t progress_id) {

    pid_t pid;
    static char argvT[][256] = { "api", "mount", "", "" };
    static char *argv[] = { argvT[0], argvT[1], nullptr, nullptr, nullptr };
    uint32_t len;
    QSharedMemory currentprogress;

    if (f){
        strncpy(argvT[3], (SHARED_KEY_PREFIX + std::to_string(progress_id)).c_str(), 255 );
        currentprogress.setKey(argvT[3]); // Note : ForkableCore uses QSharedMemory, but can be POSIX-functions instead.
        currentprogress.create(sizeof(float));
        *((float*)currentprogress.data()) = 0.0f;
        argv[3] = argvT[3];
    }

    pid = fork();

    if (pid == 0) {
        ForkableCore_api_SerializeMount(p, &argv[2], &len);
        execv(g_prog_path, argv);
        exit(127);
    }

    ForkExecutionControl *fec = new ForkExecutionControl(argvT[3], pid, progress_id, final, f);
    fec->start(); // TODO never freed

    currentprogress.detach();

    return 0;
}

int ForkableCore_api_callCreate(const GostCrypt::Core::CreateParams_t *p, DoneCallBackFunction_t final, UICallBackFunction_t f, const uint32_t progress_id) {

    pid_t pid;
    static char argvT[][256] = { "api", "create", "", "" };
    static char *argv[] = { argvT[0], argvT[1], nullptr, nullptr, nullptr };
    uint32_t len;
    QSharedMemory currentprogress;

    if (f){
        strncpy(argvT[3], (SHARED_KEY_PREFIX + std::to_string(progress_id)).c_str(), 255 );
        currentprogress.setKey(argvT[3]); // Note : ForkableCore uses QSharedMemory, but can be POSIX-functions instead.
        currentprogress.create(sizeof(float));
        *((float*)currentprogress.data()) = 0.0f;
        argv[3] = argvT[3];
    }

    pid = fork();

    if (pid == 0) {
        ForkableCore_api_SerializeCreate(p, &argv[2], &len);
        execv(g_prog_path, argv);
        exit(127);
    }

    ForkExecutionControl *fec = new ForkExecutionControl(argvT[3], pid, progress_id, final, f);
    fec->start(); // TODO never freed

    currentprogress.detach();

    return 0;
}

void ForkableCore_uicallback(const char *message, float percent) {
    (void)message;
    g_currentprogress.lock();
    *((float*)g_currentprogress.data()) = percent;
    g_currentprogress.unlock();
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

int ForkableCore_api_SerializeCreate(const GostCrypt::Core::CreateParams_t *p, char **d, uint32_t *len) {
    uint8_t *tmp = nullptr;
    uint8_t *tmpid = nullptr;
    uint32_t tmplen;
    uint32_t size;
    *d = new char[1024];
    tmp = new uint8_t[512];
    tmpid = tmp;
    *len = 0;

    if(*d == nullptr || tmpid == nullptr) {
        return 1;
    }

    /* storing all bytes in tmp */

    /* Mount_Params_t */
    // volumePath
    ForkableCore_private_SerializeString(&p->afterCreationMount.volumePath, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // mountpoint
    ForkableCore_private_SerializeString(&p->afterCreationMount.mountPoint, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // password
    ForkableCore_private_SerializeBuffer(&p->afterCreationMount.password, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // filesystemID
    ForkableCore_private_SerializeString(&p->afterCreationMount.fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    /* Other */

    // volumePath
    ForkableCore_private_SerializeString(&p->volumePath, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // password
    ForkableCore_private_SerializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // dataSize
    size = htobe32(p->dataSize);
    *len += 4;
    *(tmpid++) = (size >> 24) & 0xFF;
    *(tmpid++) = (size >> 16) & 0xFF;
    *(tmpid++) = (size >>  8) & 0xFF;
    *(tmpid++) = (size      ) & 0xFF;

    // sectorSize
    size = htobe32(p->sectorSize);
    *len += 4;
    *(tmpid++) = (size >> 24) & 0xFF;
    *(tmpid++) = (size >> 16) & 0xFF;
    *(tmpid++) = (size >>  8) & 0xFF;
    *(tmpid++) = (size      ) & 0xFF;

    // algorithmID
    ForkableCore_private_SerializeString(&p->algorithmID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // keyDerivationFunctionID
    ForkableCore_private_SerializeString(&p->keyDerivationFunctionID, tmpid, &tmplen);
    tmpid += tmplen;
    *len += tmplen;

    // volumeTypeID
    ForkableCore_private_SerializeString(&p->volumeTypeID, tmpid, &tmplen);
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

int ForkableCore_api_DeserializeCreate(GostCrypt::Core::CreateParams_t *p, const char *d) {
    uint8_t *tmp = nullptr;
    uint8_t *tmpid = nullptr;
    uint32_t tmplen;
    uint32_t size;

    tmp = new uint8_t[512];
    tmpid = tmp;

    if(tmpid == nullptr) {
        return 1;
    }

    /* converting all char to bytes */
    for (uint32_t i = 0; d[i] != '\0'; i+=2) {
        if (d[i + 1] == '\0') {
            return 1;
        }
        getUint8(d + i, tmpid++);
    }
    tmpid = tmp;

    /* analysing all bytes */

    /* Mount_Params_t */
    // volumePath
    ForkableCore_private_DeserializeString(&p->afterCreationMount.volumePath, tmpid, &tmplen);
    tmpid += tmplen;

    // mountpoint
    ForkableCore_private_DeserializeString(&p->afterCreationMount.mountPoint, tmpid, &tmplen);
    tmpid += tmplen;

    // password
    ForkableCore_private_DeserializeBuffer(&p->afterCreationMount.password, tmpid, &tmplen);
    tmpid += tmplen;

    // filesystemID
    ForkableCore_private_DeserializeString(&p->afterCreationMount.fileSystemID, tmpid, &tmplen);
    tmpid += tmplen;

    /* Other */

    // volumePath
    ForkableCore_private_DeserializeString(&p->volumePath, tmpid, &tmplen);
    tmpid += tmplen;

    // password
    ForkableCore_private_SerializeBuffer(&p->password, tmpid, &tmplen);
    tmpid += tmplen;

    // dataSize
    size =               *(tmpid++);
    size = (size << 8) | *(tmpid++);
    size = (size << 8) | *(tmpid++);
    size = (size << 8) | *(tmpid++);
    p->dataSize = be32toh(size);

    // sectorSize
    size =               *(tmpid++);
    size = (size << 8) | *(tmpid++);
    size = (size << 8) | *(tmpid++);
    size = (size << 8) | *(tmpid++);
    p->sectorSize = be32toh(size);

    // algorithmID
    ForkableCore_private_DeserializeString(&p->algorithmID, tmpid, &tmplen);
    tmpid += tmplen;

    // keyDerivationFunctionID
    ForkableCore_private_DeserializeString(&p->keyDerivationFunctionID, tmpid, &tmplen);
    tmpid += tmplen;

    // volumeTypeID
    ForkableCore_private_DeserializeString(&p->volumeTypeID, tmpid, &tmplen);
    tmpid += tmplen;

    GostCrypt::SecureBufferPtr eraser(tmp, tmpid - tmp);
    eraser.erase();

    return 0;
}

ForkExecutionControl::ForkExecutionControl(const char *mem_key, pid_t pid, uint32_t id, DoneCallBackFunction_t callback_final, UICallBackFunction_t callback_function)
{
    mem.setKey(mem_key);
    monitored = pid;
    notif_id = id;
    final = callback_final;
    callback = callback_function;
}

void ForkExecutionControl::run()
{
    float lastdata = 0.0f;
    int ret = 0, status;

    if (callback)
    {
        mem.attach(QSharedMemory::ReadOnly);
        do
        {
            mem.lock();
            if (lastdata != *((const float *) mem.constData()))
            {
                lastdata = *((const float *) mem.constData());
                // Note: Forkablecore does not support messages for now
                callback("", lastdata, notif_id);
            }
            mem.unlock();

            ret = waitpid(monitored, &status, WNOHANG);
        } while (ret == 0);
        mem.detach();
    } else {
        ret = waitpid(monitored, &status, 0);
    }

    /* Waiting for child to build disk image */
    if ((ret == -1) || (WIFEXITED(status) == false)) {
        final(-1, notif_id);
    }

    final(WEXITSTATUS(status), notif_id);
}
