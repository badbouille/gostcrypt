/**
 * @file exec.cpp
 * @author badbouille
 * @date 19/05/2022
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <platform/exec.h>
#include <windows.h>

void * process_exec(const char * path, char * const * argv) {
    STARTUPINFO si;
    PROCESS_INFORMATION *pi = static_cast<PROCESS_INFORMATION *>(malloc(sizeof(PROCESS_INFORMATION)));
    char cmd_buffer[PATH_MAX] = {0};

    // setting up output
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( pi, sizeof(PROCESS_INFORMATION) );

    // creating cmd buffer
    char *target = &cmd_buffer[0];
    size_t target_size = PATH_MAX;
    int ret = 0;
    size_t strlength = 0;
    char *space_presence = nullptr;
    for (int i=0; argv[i] != nullptr; i++) {
        // analyzing length
        strlength = strnlen(argv[i], PATH_MAX);
        if (strlength >= PATH_MAX) {
            return nullptr; // failure: parameter too long
        }

        // analyzing ' ' presence to concatenate
        space_presence = strchr(argv[i], ' ');
        if (space_presence != nullptr) {
            return nullptr; // failure: space is present meaning the parameter will not fit in the command-line unless quoted?
        }

        // concatenation
        // adding ' '
        if(i != 0) {
            target[0] = ' ';
            target_size-=1;
            target++;
        }
        // adding argument
        ret = strcpy_s(target, target_size, argv[i]);
        if (ret != 0) {
            return nullptr;
        }
        target += strlength;
        target_size -= strlength;
    }

    // creating process
    if( !CreateProcess( NULL,   // No module name (use command line)
                        cmd_buffer,        // Command line
                        NULL,           // Process handle not inheritable
                        NULL,           // Thread handle not inheritable
                        FALSE,          // Set handle inheritance to FALSE
                        0,              // No creation flags
                        NULL,           // Use parent's environment block
                        NULL,           // Use parent's starting directory
                        &si,            // Pointer to STARTUPINFO structure
                        pi )           // Pointer to PROCESS_INFORMATION structure
            )
    {
        return nullptr;
    }

    return pi;
}

int process_waitpid(void ** pid, int * status) {
    DWORD dwExitCode;
    PROCESS_INFORMATION *pi;

    if (pid == nullptr || pid[0] == nullptr) {
        return -1;
    }

    pi = static_cast<PROCESS_INFORMATION *>(pid[0]);

    dwExitCode = WaitForSingleObject(pi->hProcess, 0);

    if (dwExitCode == WAIT_FAILED) {
        return -1;
    }

    if ( (dwExitCode == WAIT_OBJECT_0 )
      || (dwExitCode == WAIT_ABANDONED) ) {

        GetExitCodeProcess(pi->hProcess, &dwExitCode);

        CloseHandle(pi->hProcess);
        CloseHandle(pi->hThread);
        free(pid[0]);
        pid[0] = nullptr;

        status[0] = (int)dwExitCode;
        return 1;
    } else {
        status[0] = 0;
    }

    return 0;
}

int process_getpid(void * pid) {
    PROCESS_INFORMATION *pi;

    if (pid == nullptr ) {
        return (int)GetCurrentProcessId();
    }

    pi = static_cast<PROCESS_INFORMATION *>(pid);

    return (int)GetProcessId(pi->hProcess);
}
