/**
 * @file main.cpp
 * @author badbouille
 * @date 15/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "Commands.h"
#include <string>
#include <Core.h>

/* Common values */

const char *program_name_version = PROGRAM_NAME " " PROGRAM_VERSION;
const char *program_bug_address = PROGRAM_BUG_ADDRESS;

/* Program documentation. */
static char doc[] = "Command-line tool to manage encrypted containers.";

void show_main_help() {
    printf("%s\n", program_name_version);
    printf("%s\n", doc);
    printf("Usage: gc_cmdline <mount|create|umount|list> [--help] [...]\n");
}

int main(int argc, char **argv) {
    std::string cmd = "";

    /* Interception of api calls */
    if (GostCrypt::Core::main_api_handler(argc, argv) != 0) {
        return 1; // no exception needed but can't continue
    }

    if (argc < 2) {
        show_main_help();
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

    show_main_help();
    return 1;
}

