/**
 * @file ListCmd.cpp
 * @author badbouille
 * @date 15/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <iostream>
#include <vector>
#include "Commands.h"
#include "Buffer.h"
#include "Core.h"

using namespace GostCrypt;

/* ---------------------------- List command ---------------------------- */

void print_info(std::string &name, std::string &desc) {
    int desclen = 80;
    size_t end = 0;
    size_t start = 0;
    std::vector<std::string> lines;

    do {
        end = desc.rfind("\n", start + desclen);
        if (end > start && end != std::string::npos) {
            lines.push_back(desc.substr(start, end-start));
        } else {
            end = desc.find(" ", start + desclen);
            if (end == std::string::npos) {
                lines.push_back(desc.substr(start));
                break;
            } else {
                lines.push_back(desc.substr(start, end-start));
            }
        }
        start = end + 1;
    } while (true);

    for(auto &l : lines) {
        if (l == lines.front()) {
            printf("%-25s %s\n", name.c_str(), l.c_str());
        } else {
            printf("%-25s %s\n", "", l.c_str());
        }
    }
}

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
    int nerrors;

    struct arg_lit *help;
    struct arg_str *category;
    struct arg_end *end;
    void *argtable[] = {
            help     = arg_litn("h", "help", 0, 1, "display this help and exit"),
            category = arg_strn(NULL, NULL, "<(mounted)|algorithms|kdfs|types|filesystems>", 0, 1, "type to list"),
            end      = arg_end(20),
    };

    // default values
    item = "mounted";

    nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0)
    {
        printf("Usage: %s", PROGRAM_NAME);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Command to list different options and algorithms supported by Gostcrypt\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        return 0;
    }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0)
    {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout, end, PROGRAM_NAME);
        printf("Try 'gc_cmdline %s --help' for more information.\n", argv[0]);
        return 1;
    }

    if (category->count > 0) {
        item = std::string(category->sval[0]);
    }

    if (item == "algorithms") {
        DiskEncryptionAlgorithmList dealist = Core::GetEncryptionAlgorithms();
        for (auto dea : dealist) {
            std::string name = dea->GetID();
            std::string desc = "[" + dea->GetName() + "]\n" + dea->GetDescription();
            print_info(name, desc);
            delete dea;
        }
        return 0;
    }

    if (item == "kdfs") {
        KDFList kdflist = Core::GetDerivationFunctions();
        for (auto kdf : kdflist) {
            std::string name = kdf->GetID();
            std::string desc = "[" + kdf->GetName() + "]\n" + kdf->GetDescription();
            print_info(name, desc);
            delete kdf;
        }
        return 0;
    }

    if (item == "types") {
        VolumeList vlist = Core::GetVolumeTypes();
        for (auto v : vlist) { // TODO display error!
            std::string name = v->GetID();
            std::string desc = "[" + v->GetName() + "]\n" + v->GetDescription();
            print_info(name, desc);
            delete v;
        }
        return 0;
    }

    if (item == "filesystems") {
        FuseFileSystemList fslist = Core::GetFileSystems();
        for (auto fs : fslist) {
            std::string name = fs->getID();
            std::string desc = "[" + fs->getName() + "]\n" + fs->getDescription();
            print_info(name, desc);
            delete fs;
        }
        return 0;
    }

    if (item == "mounted") {
        Core::VolumeInfoList vlist = Core::list();

        for (auto v : vlist) {
            std::cout << v.mountPoint << ": " << v.file << " " << v.algorithmID << ":" << v.keyDerivationFunctionID << " (" << formatSize(v.dataSize) << ")" << std::endl;
        }
        return 0;
    }

    printf("Usage: %s", PROGRAM_NAME);
    arg_print_syntax(stdout, argtable, "\n");
    printf("Command to list different options and algorithms supported by Gostcrypt\n\n");
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    return 1;
}
