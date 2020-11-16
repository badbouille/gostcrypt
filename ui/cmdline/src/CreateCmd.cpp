//
// Created by badbouille on 15/11/2020.
//

#include <argp.h>
#include <iostream>
#include "Commands.h"
#include "Buffer.h"
#include "Core.h"

using namespace GostCrypt;

/* ---------------------------- Create command ---------------------------- */

/* Program documentation. */
static char doc_create[] = "Command to create an encrypted volume";

/* A description of the arguments we accept. */
static char args_doc_create[] = "VOLUME FOLDER";

/* Options */
static struct argp_option create_options[] = {
        {"password", 'p', "<pass>", 0, "Manually enter password (deprecated)" },
        {"filesystem", 'f', "<fs>", 0, "Filesystem to use when mounting this volume. Type 'gostcrypt list filesystems' for list of supported filesystems" },
        {"size", 0, "<size>", 0, "Volume size in bytes" },
        {"sector-size", 0, "<size>", 0, "Sector size in bytes" },
        {"type", 't', "<type>", 0, "Volume type. Type 'gostcrypt list types' for list of supported volume types." },
        {"algorithm", 'a', "<algo>", 0, "Encryption algorithm to use. Type 'gostcrypt list algorithms' for list of supported algorithms." },
        {"kdf", 'k', "<algo>", 0, "Key derivation algorithm to use. Type 'gostcrypt list kdfs' for list of supported derivation functions." },
        {nullptr }
};

/* Custom parser */
static error_t parse_opt_create (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    Core::CreateParams_t *arguments = (Core::CreateParams_t *)state->input;

    switch (key)
    {
        case 'p':
            strcpy((char *)arguments->password.get(), arg);
            break;
        case 'f':
            arguments->afterCreationMount.fileSystemID = arg;
            break;
        case ARGP_KEY_NO_ARGS:
            argp_usage (state);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                arguments->volumePath = std::string(arg);
                arguments->afterCreationMount.volumePath = arguments->volumePath;
                break;
            }
            if (state->arg_num == 1) {
                arguments->afterCreationMount.mountPoint = std::string(arg);
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
static struct argp argp_create = { create_options, parse_opt_create, args_doc_create, doc_create };

/* Real command */
int cmd_create(int argc, char **argv) {
    Core::CreateParams_t arguments;
    SecureBuffer pass(64);

    /* Default init */
    arguments.afterCreationMount.password = SecureBufferPtr(pass.get(), pass.size());
    arguments.afterCreationMount.volumePath = "";
    arguments.afterCreationMount.mountPoint = "";
    arguments.afterCreationMount.fileSystemID = DEFAULT_FILESYSTEMID;

    arguments.password = SecureBufferPtr(pass.get(), pass.size());
    arguments.volumePath = "";

    arguments.dataSize = DEFAULT_VOLUMESIZE;
    arguments.sectorSize = DEFAULT_SECTORSIZE;

    arguments.volumeTypeID = DEFAULT_VOLUMETYPE;
    arguments.algorithmID = DEFAULT_ALGORITHM;
    arguments.keyDerivationFunctionID = DEFAULT_KDF;

    argp_parse (&argp_create, argc, argv, 0, 0, &arguments);

    std::cout << "Creating volume:" << std::endl;
    std::cout << "Volume Path: " << arguments.volumePath << std::endl;
    std::cout << "MountPoint: " << arguments.afterCreationMount.mountPoint << std::endl;
    std::cout << "Password: " << std::string((char*)arguments.password.get()) << std::endl;
    std::cout << "FileSystem: " << arguments.afterCreationMount.fileSystemID << std::endl;
    std::cout << "Volume size: " << arguments.dataSize << " B" << std::endl;
    std::cout << "Sector size: " << arguments.sectorSize << " B" << std::endl;
    std::cout << "Volume type: " << arguments.volumeTypeID << std::endl;
    std::cout << "Algorithm: " << arguments.algorithmID << std::endl;
    std::cout << "Key derivation function: " << arguments.keyDerivationFunctionID << std::endl;

    return 0;
}
