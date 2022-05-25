/**
 * @file CreateCmd.cpp
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

/* ---------------------------- Create command ---------------------------- */

static int parse_size(const char * arg, size_t *size) {
    char * unit = nullptr;
    if (size == nullptr) {
        return -1;
    }

    size[0] = std::strtoll(arg, &unit, 10);

    if (unit[0] == '\0') {
        return 0;
    }

    std::string u(unit);
    if (u == "KB" || u == "K") {
        size[0]*=KB;
        return 0;
    }
    if (u == "MB" || u == "M") {
        size[0]*=MB;
        return 0;
    }
    if (u == "GB" || u == "G") {
        size[0]*=GB;
        return 0;
    }
    /*if (u == "TB" || u == "T") {
        size[0]*=TB; // TODO : why overflow here
        break;
    }*/

    return -1;
}

/* Real command */
int cmd_create(int argc, char **argv) {
    Core::CreateParams_t arguments;
    SecureBuffer password(64);
    int nerrors;

    struct arg_lit *help;
    struct arg_str *pass, *fs, *vtype, *algo, *kdf, *vsize, *ssize;
    struct arg_file *folder, *file;
    struct arg_end *end;
    void *argtable[] = {
            help     = arg_litn("h", "help", 0, 1, "display this help and exit"),
            pass     = arg_strn("p", "password", "<pass>", 0, 1, "Manually enter password (deprecated)"),
            fs       = arg_strn("f", "filesystem", "<fs>", 0, 1, "Filesystem to use when mounting this volume. Type 'gostcrypt list filesystems' for list of supported filesystems"),
            vsize    = arg_strn("s", "size", "<size>", 0, 1, "Volume size in bytes"),
            ssize    = arg_strn("b", "sector-size", "<size>", 0, 1, "Sector size in bytes"),
            vtype    = arg_strn("t", "type", "<type>", 0, 1, "Volume type. Type 'gostcrypt list types' for list of supported volume types."),
            algo     = arg_strn("a", "algorithm", "<algo>", 0, 1, "Encryption algorithm to use. Type 'gostcrypt list algorithms' for list of supported algorithms."),
            kdf      = arg_strn("k", "kdf", "<algo>", 0, 1, "Key derivation algorithm to use. Type 'gostcrypt list kdfs' for list of supported derivation functions."),
            file     = arg_filen(NULL, NULL, "<volume>", 1, 1, "Volume to mount"),
            folder   = arg_filen(NULL, NULL, "<mountpoint>", 1, 1, "Folder where to mount it"),
            end      = arg_end(20),
    };

    /* Default init */
    arguments.afterCreationMount.password = SecureBufferPtr(password.get(), 0);
    arguments.afterCreationMount.volumePath = "";
    arguments.afterCreationMount.mountPoint = "";
    arguments.afterCreationMount.fileSystemID = DEFAULT_FILESYSTEMID;

    arguments.password = SecureBufferPtr(password.get(), 0);
    arguments.volumePath = "";

    arguments.dataSize = DEFAULT_VOLUMESIZE;
    arguments.sectorSize = DEFAULT_SECTORSIZE;

    arguments.volumeTypeID = DEFAULT_VOLUMETYPE;
    arguments.algorithmID = DEFAULT_ALGORITHM;
    arguments.keyDerivationFunctionID = DEFAULT_KDF;

    nerrors = arg_parse(argc, argv, argtable);

    if (help->count > 0)
    {
        printf("Usage: %s", PROGRAM_NAME);
        arg_print_syntax(stdout, argtable, "\n");
        printf("Command to create an encrypted volume and mount it on a virtual folder.\n\n");
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
        arguments.afterCreationMount.fileSystemID = std::string(fs->sval[0]);
    }

    if (vtype->count > 0) {
        arguments.volumeTypeID = std::string(vtype->sval[0]);
    }

    if (algo->count > 0) {
        arguments.algorithmID = std::string(algo->sval[0]);
    }

    if (kdf->count > 0) {
        arguments.keyDerivationFunctionID = std::string(kdf->sval[0]);
    }

    if (vsize->count > 0) {
        if (parse_size(vsize->sval[0], &arguments.dataSize) != 0) {
            printf("GostCrypt: wrong formatting in option --size");
            printf("Try 'gc_cmdline %s --help' for more information.\n", argv[0]);
            return 1;
        }
    }

    if (ssize->count > 0) {
        if (parse_size(ssize->sval[0], &arguments.sectorSize) != 0) {
            printf("GostCrypt: wrong formatting in option --sector-size");
            printf("Try 'gc_cmdline %s --help' for more information.\n", argv[0]);
            return 1;
        }
    }

    if (pass->count > 0) {
        size_t tmp = strlen(pass->sval[0]);
        if (tmp > 64) { // Checking against buffer size
            printf("GostCrypt: Password too big for current buffer."); // TODO 64 chars is WEAK
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
    arguments.afterCreationMount.volumePath = file->filename[0];
    arguments.afterCreationMount.mountPoint = folder->filename[0];

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

    try {
        Core::create(&arguments);
    } catch (GostCryptException &e) {
        std::cout << "Cannot create volume. " << e.what() << std::endl;
    }

    return 0;
}
