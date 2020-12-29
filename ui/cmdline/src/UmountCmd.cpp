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
        {"all", 'a', nullptr, 0, "Unmount all volumes. Do not give a folder name when using this option." },
        {nullptr }
};

struct UmountOptions_s {
    std::string mountpoint;
    bool all;
};

/* Custom parser */
static error_t parse_opt_umount (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct UmountOptions_s *arguments = (struct UmountOptions_s *)state->input;

    switch (key)
    {
        case 'a':
            arguments->all = true;
            break;
        case ARGP_KEY_NO_ARGS:
            argp_usage (state);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num == 0 && !(arguments->all)) {
                arguments->mountpoint = std::string(arg);
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
    struct UmountOptions_s opts;
    SecureBuffer pass(64);

    opts.mountpoint = "";
    opts.all = false;

    argp_parse (&argp_umount, argc, argv, 0, 0, &opts);

    std::cout << "Unmounting folder: " << opts.mountpoint << std::endl;

    try {
        if (opts.all) {
            Core::umountAll();
        } else {
            Core::umount(opts.mountpoint);
        }
    } catch (GostCryptException &e) {
        std::cout << "Cannot unmount volume. " << e.what() << std::endl;
    }

    return 0;
}
