/**
 * @file Commands.h
 * @author badbouille
 * @date 15/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <argp.h>
#include "commonDefines.h"

/* Commands */
int cmd_mount(int argc, char **argv);
int cmd_create(int argc, char **argv);
int cmd_umount(int argc, char **argv);
int cmd_list(int argc, char **argv);

#endif //_COMMANDS_H
