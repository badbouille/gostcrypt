//
// Created by badbouille on 15/11/2020.
//

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
