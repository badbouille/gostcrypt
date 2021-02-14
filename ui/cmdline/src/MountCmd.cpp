/**
 * @file MountCmd.cpp
 * @author badbouille
 * @date 15/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <argp.h>
#include <iostream>
#include "Commands.h"
#include "Buffer.h"
#include "Core.h"

using namespace GostCrypt;

/* ---------------------------- Mount command ---------------------------- */

/* Program documentation. */
static char doc_mount[] = "Command to mount an encrypted file on a virtual folder";

/* A description of the arguments we accept. */
static char args_doc_mount[] = "VOLUME FOLDER";

/* Options */
static struct argp_option mount_options[] = {
        {"password", 'p', "<pass>", 0, "Manually enter password (deprecated)" },
        {"filesystem", 'f', "<fs>", 0, "Filesystem to use when mounting this volume. Type 'gostcrypt list filesystems' for list of supported filesystems" },
        {nullptr }
};

/* Custom parser */
static error_t parse_opt_mount (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    Core::MountParams_t *arguments = (Core::MountParams_t *)state->input;
    int tmp;

    switch (key)
    {
        case 'p':
            tmp = strlen(arg);
            if (tmp > 64) { // Checking against buffer size
                return 1;
            }
            strcpy((char *)arguments->password.get(), arg);
            arguments->password.set(arguments->password.get(), tmp); // Usually very illegal but here we know the buffer is big enough
            break;
        case 'f':
            arguments->fileSystemID = arg;
            break;
        case ARGP_KEY_NO_ARGS:
            argp_usage (state);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                arguments->volumePath = std::string(arg);
                break;
            }
            if (state->arg_num == 1) {
                arguments->mountPoint = std::string(arg);
                break;
            }
            /* Too many arguments. */
            argp_usage(state);
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 2) {
                /* Not enough arguments. */
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* argp custom params */
static struct argp argp_mount = { mount_options, parse_opt_mount, args_doc_mount, doc_mount };

/* Real command */
int cmd_mount(int argc, char **argv) {
    Core::MountParams_t arguments;
    SecureBuffer pass(64);

    arguments.mountPoint = "";
    arguments.password = SecureBufferPtr(pass.get(), 0);
    arguments.fileSystemID = DEFAULT_FILESYSTEMID;
    arguments.volumePath = "";

    argp_parse (&argp_mount, argc, argv, 0, 0, &arguments);

    std::cout << "Mounting volume:" << std::endl;
    std::cout << "Volume Path: " << arguments.volumePath << std::endl;
    std::cout << "MountPoint: " << arguments.mountPoint << std::endl;
    std::cout << "Password: " << std::string((char*)arguments.password.get()) << std::endl;
    std::cout << "FileSystem: " << arguments.fileSystemID << std::endl;

    try {
        Core::mount(&arguments);
    } catch (GostCryptException &e) {
        std::cout << "Cannot mount volume. " << e.what() << std::endl;
    }

    return 0;
}
