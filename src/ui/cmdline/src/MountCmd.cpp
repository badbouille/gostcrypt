/**
 * @file MountCmd.cpp
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

/* ---------------------------- Mount command ---------------------------- */

/* Real command */
int cmd_mount(int argc, char **argv) {
    Core::MountParams_t arguments;
    SecureBuffer password(64);
    int nerrors;

    struct arg_lit *help;
    struct arg_str *pass, *fs;
    struct arg_file *folder, *file;
    struct arg_end *end;
    void *argtable[] = {
            help     = arg_litn("h", "help", 0, 1, "display this help and exit"),
            pass     = arg_strn("p", "password", "<pass>", 0, 1, "Manually enter password (deprecated)"),
            fs       = arg_strn("f", "filesystem", "<fs>", 0, 1, "Filesystem to use when mounting this volume. Type 'gostcrypt list filesystems' for list of supported filesystems"),
            file     = arg_filen(NULL, NULL, "<volume>", 1, 1, "Volume to mount"),
            folder   = arg_filen(NULL, NULL, "<mountpoint>", 1, 1, "Folder where to mount it"),
            end      = arg_end(20),
    };

    arguments.mountPoint = "";
    arguments.password = SecureBufferPtr(password.get(), 0);
    arguments.fileSystemID = DEFAULT_FILESYSTEMID;
    arguments.volumePath = "";

    nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0)
    {
        printf("Usage: %s", PROGRAM_NAME);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Command to mount an encrypted container on a virtual folder.\n\n");
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

    if (fs->count > 0) {
        arguments.fileSystemID = std::string(fs->sval[0]);
    }

    if (pass->count > 0) {
        size_t tmp = strlen(pass->sval[0]);
        if (tmp > 64) { // Checking against buffer size
            printf("Password too big for current buffer."); // TODO 64 chars is WEAK
            printf("Try 'gc_cmdline %s --help' for more information.\n", argv[0]);
            return 1;
        }
        strcpy((char *)arguments.password.get(), pass->sval[0]);
        // Usually very illegal but here we know the buffer is big enough
        arguments.password.set(arguments.password.get(), tmp);
        // TODO delete password from argv
    } else {
        // TODO ask password here
    }

    arguments.volumePath = file->filename[0];
    arguments.mountPoint = folder->filename[0];

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
