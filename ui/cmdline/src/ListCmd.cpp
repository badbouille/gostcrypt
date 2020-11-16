//
// Created by badbouille on 15/11/2020.
//

#include <argp.h>
#include <iostream>
#include "Commands.h"
#include "Buffer.h"
#include "Core.h"

using namespace GostCrypt;

/* ---------------------------- List command ---------------------------- */

/* Program documentation. */
static char doc_list[] = "Command to list different options supported by GostCrypt";

/* A description of the arguments we accept. */
static char args_doc_list[] = "<algorithms|kdfs|types|filesystems>";

/* Options */
static struct argp_option list_options[] = {
        {nullptr }
};

/* Custom parser */
static error_t parse_opt_list (int key, char *arg, struct argp_state *state) {
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
static struct argp argp_list = { list_options, parse_opt_list, args_doc_list, doc_list };

/* Real command */
int cmd_list(int argc, char **argv) {
    std::string item;

    item = "";

    argp_parse (&argp_list, argc, argv, 0, 0, &item);

    argp_help(&argp_list, stdout, ARGP_HELP_STD_USAGE, argv[0]);

    return 0;
}
