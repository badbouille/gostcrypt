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
#include <QString>
#include <QThread>
#include <QSharedMemory>

#define SHARED_KEY_PREFIX "gostcrypt-shared-mem-"

/* Callback used in forked process to send info across shared memory */
void ForkableCore_uicallback(const char *message, float percent);

/* Callback used in main process to show received info... since main process may (obviously) be forked multiple times, id is necessary */
typedef void (*UICallBackFunction_t)(const char *, float, uint32_t);

/* Callback used for after the execution, to analyse the results */
typedef int (*DoneCallBackFunction_t)(uint32_t, uint32_t);

int ForkableCore_api_handler(int argc, char **argv);
int ForkableCore_api_callMount(const GostCrypt::Core::MountParams_t *p, DoneCallBackFunction_t final, UICallBackFunction_t function = nullptr, uint32_t progress_id = 0);
int ForkableCore_api_callCreate(const GostCrypt::Core::CreateParams_t *p, DoneCallBackFunction_t final, UICallBackFunction_t function = nullptr, uint32_t progress_id = 0);

int ForkableCore_api_SerializeMount(const GostCrypt::Core::MountParams_t *p, char **d, uint32_t *len);
int ForkableCore_api_DeserializeMount(GostCrypt::Core::MountParams_t *p, const char *d);

int ForkableCore_api_SerializeCreate(const GostCrypt::Core::CreateParams_t *p, char **d, uint32_t *len);
int ForkableCore_api_DeserializeCreate(GostCrypt::Core::CreateParams_t *p, const char *d);

class ForkExecutionControl : public QThread {
public:
    ForkExecutionControl(const char *mem_key, pid_t pid, uint32_t id, DoneCallBackFunction_t callback_final, UICallBackFunction_t callback_function);
private:
    void run() override;
    QSharedMemory mem;
    pid_t monitored;
    UICallBackFunction_t callback;
    DoneCallBackFunction_t final;
    uint32_t notif_id;
};

#endif //GOSTCRYPT_FORKABLECORE_H
