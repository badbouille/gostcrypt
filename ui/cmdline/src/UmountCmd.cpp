//
// Created by badbouille on 15/11/2020.
//

#include <argp.h>
#include <iostream>
#include "Commands.h"
#include "Buffer.h"
#include "Core.h"

using namespace GostCrypt;

/* ---------------------------- Umount command ---------------------------- */

/* Program documentation. */
static char doc_umount[] = "Command to unmount a previously mounted volume to close it.";

/* A description of the arguments we accept. */
static char args_doc_umount[] = "FOLDER";

/* Options */
static struct argp_option umount_options[] = {
        {nullptr }
};

/* Custom parser */
static error_t parse_opt_umount (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    std::string *arguments = (std::string *)state->input;

    switch (key)
    {
        case ARGP_KEY_NO_ARGS:
            argp_usage (state);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                *arguments = std::string(arg);
                break;
            }
            /* Too many arguments. */
            argp_usage(state);
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1) {
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
static struct argp argp_umount = { umount_options, parse_opt_umount, args_doc_umount, doc_umount };

/* Real command */
int cmd_umount(int argc, char **argv) {
    std::string folder;
    SecureBuffer pass(64);

    folder = "";

    argp_parse (&argp_umount, argc, argv, 0, 0, &folder);

    std::cout << "Unmounting folder: " << folder << std::endl;

    return 0;
}
