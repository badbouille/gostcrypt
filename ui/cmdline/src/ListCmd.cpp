//
// Created by badbouille on 15/11/2020.
//

#include <argp.h>
#include <iostream>
#include <vector>
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

#define MAX_OPTION_NUMBER 50
static struct argp_option list_options[MAX_OPTION_NUMBER] = {
        {nullptr }
};

/* Custom parser */
static error_t parse_opt_list (int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    std::string *arguments = (std::string *)state->input;

    switch (key)
    {
        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                *arguments = std::string(arg);
                break;
            }
            /* Too many arguments. */
            argp_usage(state);
            break;
        case ARGP_KEY_NO_ARGS:
        case ARGP_KEY_END:
            if (state->arg_num < 1) {
                /* No arguments means listing volumes */
                *arguments = std::string("mounted");
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* argp custom params */
static struct argp argp_list = { list_options, parse_opt_list, args_doc_list, doc_list };

std::string formatSize(size_t s) {
    /*if (s > TB) {
        s /= TB;
        return std::to_string(s) + "TB";
    }*/
    if (s > GB) {
        s /= GB;
        return std::to_string(s) + "GB";
    }
    if (s > MB) {
        s /= MB;
        return std::to_string(s) + "MB";
    }
    if (s > KB) {
        s /= KB;
        return std::to_string(s) + "KB";
    }
    return std::to_string(s) + "B";
}

/* Real command */
int cmd_list(int argc, char **argv) {
    std::string item;
    uint32_t index = 0;

    item = "";

    argp_parse (&argp_list, argc, argv, 0, 0, &item);

    if (item == "algorithms") {
        DiskEncryptionAlgorithmList dealist = Core::GetEncryptionAlgorithms();
        // Using vector<string> to store temporary strings, so c_str() stays valid after object deletion
        std::vector<std::string> names;
        std::vector<std::string> descs;
        for (auto dea : dealist) {
            names.push_back(dea->GetID());
            descs.push_back("[" + dea->GetName() + "]\n" + dea->GetDescription());

            // impossible to compute a list this long.
            // list can be longer but current limit is already very high
            if(index >= MAX_OPTION_NUMBER) {
                abort();
            }

            list_options[index].name = names.back().c_str();
            list_options[index].key = 0;
            list_options[index].arg = 0;
            list_options[index].flags = OPTION_DOC;
            list_options[index].doc = descs.back().c_str();
            index++;

            delete dea;
        }
        argp_help(&argp_list, stdout, ARGP_HELP_LONG, argv[0]);
        return 0;
    }

    if (item == "kdfs") {
        HashList hlist = Core::GetDerivationFunctions();
        // Using vector<string> to store temporary strings, so c_str() stays valid after object deletion
        std::vector<std::string> names;
        std::vector<std::string> descs;
        for (auto h : hlist) {
            names.push_back(h->GetID());
            descs.push_back("[" + h->GetName() + "]\n" + h->GetDescription());

            if(index >= MAX_OPTION_NUMBER) {
                abort();
            }

            list_options[index].name = names.back().c_str();
            list_options[index].key = 0;
            list_options[index].arg = 0;
            list_options[index].flags = OPTION_DOC;
            list_options[index].doc = descs.back().c_str();
            index++;

            delete h;
        }
        argp_help(&argp_list, stdout, ARGP_HELP_LONG, argv[0]);
        return 0;
    }

    if (item == "types") {
        VolumeList vlist = Core::GetVolumeTypes();
        // Using vector<string> to store temporary strings, so c_str() stays valid after object deletion
        std::vector<std::string> names;
        std::vector<std::string> descs;
        for (auto v : vlist) { // TODO display error!
            names.push_back(v->GetID());
            descs.push_back("[" + v->GetName() + "]\n" + v->GetDescription());

            if(index >= MAX_OPTION_NUMBER) {
                abort();
            }

            list_options[index].name = names.back().c_str();
            list_options[index].key = 0;
            list_options[index].arg = 0;
            list_options[index].flags = OPTION_DOC;
            list_options[index].doc = descs.back().c_str();
            index++;

            delete v;
        }
        argp_help(&argp_list, stdout, ARGP_HELP_LONG, argv[0]);
        return 0;
    }

    if (item == "filesystems") {
        FuseFileSystemList fslist = Core::GetFileSystems();
        // Using vector<string> to store temporary strings, so c_str() stays valid after object deletion
        std::vector<std::string> names;
        std::vector<std::string> descs;
        for (auto fs : fslist) {
            names.push_back(fs->getID());
            descs.push_back("[" + fs->getName() + "]\n" + fs->getDescription());

            if(index >= MAX_OPTION_NUMBER) {
                abort();
            }

            list_options[index].name = names.back().c_str();
            list_options[index].key = 0;
            list_options[index].arg = 0;
            list_options[index].flags = OPTION_DOC;
            list_options[index].doc = descs.back().c_str();
            index++;

            delete fs;
        }
        argp_help(&argp_list, stdout, ARGP_HELP_LONG, argv[0]);
        return 0;
    }

    if (item == "mounted") {
        Core::VolumeInfoList vlist = Core::list();

        for (auto v : vlist) {
            std::cout << v.mountPoint << ": " << v.file << " " << v.algorithmID << ":" << v.keyDerivationFunctionID << " (" << formatSize(v.dataSize) << ")" << std::endl;
        }
        return 0;
    }

    argp_help(&argp_list, stdout, ARGP_HELP_STD_USAGE, argv[0]);
    return 1;
}
