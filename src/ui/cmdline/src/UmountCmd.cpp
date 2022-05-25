/**
 * @file UmountCmd.cpp
 * @author badbouille
 * @date 15/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <argtable3.h>
#include <iostream>
#include "Commands.h"
#include "Buffer.h"
#include "Core.h"

using namespace GostCrypt;

/* ---------------------------- Umount command ---------------------------- */

struct UmountOptions_s {
    std::string mountpoint;
    bool all;
};

/* Real command */
int cmd_umount(int argc, char **argv) {
    struct UmountOptions_s opts;
    SecureBuffer pass(64);
    int nerrors;

    struct arg_lit *help, *all;
    struct arg_file *folder;
    struct arg_end *end;
    void *argtable[] = {
            help     = arg_litn("h", "help", 0, 1, "display this help and exit"),
            all      = arg_litn("a", "all", 0, 1, "Unmount all volumes. Do not give a folder name when using this option."),
            folder   = arg_filen(NULL, NULL, "<mountpoint>", 0, 1, "Folder to umount"),
            end      = arg_end(20),
    };

    opts.mountpoint = "";
    opts.all = false;

    nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0)
    {
        printf("Usage: %s", PROGRAM_NAME);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Command to unmount a previously mounted volume to close it.\n\n");
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

    if (all->count > 0) {
        opts.all = true;
    } else if (folder->count > 0) {
        opts.mountpoint = std::string(folder->filename[0]);
    } else {
        printf("GostCrypt: missing option <mountpoint> or '--all'");
        printf("Try 'gc_cmdline %s --help' for more information.\n", argv[0]);
    }

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
