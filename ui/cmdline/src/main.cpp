//
// Created by badbouille on 15/11/2020.
//

#include "Commands.h"
#include <string>

/* Common values */

const char *argp_program_version = PROGRAM_NAME " " PROGRAM_VERSION;
const char *argp_program_bug_address = PROGRAM_BUG_ADDRESS;

/* Program documentation. */
static char doc[] = "Command to mount an encrypted file on a virtual folder";

/* A description of the arguments we accept. */
static char args_doc[] = "<mount|create|umount|list>";

/* argp custom params */
static struct argp argp = { nullptr, nullptr, args_doc, doc };

int main(int argc, char **argv) {
    std::string cmd = "";

    if (argc < 2) {
        argp_help(&argp, stdout, ARGP_HELP_STD_USAGE, argv[0]);
        return 0;
    }

    cmd = argv[1];

    if (cmd == "mount") {
        return cmd_mount(argc - 1, argv + 1);
    }

    if (cmd == "create") {
        return cmd_create(argc - 1, argv + 1);
    }

    if (cmd == "umount") {
        return cmd_umount(argc - 1, argv + 1);
    }

    if (cmd == "list") {
        return cmd_list(argc - 1, argv + 1);
    }

    argp_help(&argp, stdout, ARGP_HELP_STD_USAGE, argv[0]);
    return 1;
}

