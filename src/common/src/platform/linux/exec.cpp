/**
 * @file exec.cpp
 * @author badbouille
 * @date 19/05/2022
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <platform/exec.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

void * process_exec(const char * path, char * const * argv) {
    int pid;
    int *ppid = static_cast<int *>(malloc(sizeof(int)));

    pid = fork();

    if (pid == 0) {
        execv(path, argv);
        exit(127);
    }

    ppid[0] = pid;

    return ppid;
}

int process_waitpid(void ** pid, int * status) {
    int stat = 0;
    int ret;
    int *ppid;

    if (pid == nullptr || pid[0] == nullptr) {
        return -1;
    }

    ppid = static_cast<int *>(pid[0]);

    ret = waitpid(ppid[0], &stat, WNOHANG);

    if(ret != 0) {
        status[0] = WEXITSTATUS(stat);
        free(pid[0]);
        pid[0] = nullptr;
    } else {
        status[0] = 0;
    }

    return ret;

}

int process_getpid(void * pid) {
    int *ppid;

    if (pid == nullptr) {
        return getpid();
    }

    ppid = static_cast<int *>(pid);

    return ppid[0];
}
