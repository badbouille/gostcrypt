/**
 * @file exec.h
 * @author badbouille
 * @date 19/05/2022
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _EXEC_H_
#define _EXEC_H_

#include <cinttypes>
#include <cstddef>

/**
 * Function to execute a given program from its path and arguments
 * Works using execv or Createprocess in backend
 * Returns the process id to wait
 * TODO comment this
 */
void * process_exec(const char * path, char * const * argv);

/**
 * Function to wait for a previously created process to end
 * TODO comment this
 * @param pid
 * @return
 */
int process_waitpid(void ** pid, int * status);

int process_getpid(void * pid);

#endif // _EXEC_H_