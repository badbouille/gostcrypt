/**
 * @file VolumeCreator.cpp
 * @author badbouille
 * @date 08/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "VolumeCreator.h"
#include "VolumeCreatorContents.h"
#include <fstream>

// TODO : cross platform needed for mkdir and rmdir
#include <unistd.h>
#include <sys/stat.h>

void stdtests_createvolume(const std::string& file, size_t length, const char *content) {
    std::fstream v;

    v.open(file, std::ios::binary | std::ios::out);

    v.write(content, length);

    v.close();
}

void stdtests_createvolumes() {

    // creating folder for volume files
#ifdef PLATFORM_WINDOWS
    mkdir(CREATOR_FILE_FOLDER);
#else
    mkdir(CREATOR_FILE_FOLDER, S_IRWXU);
#endif
    // creating volume files inside
    for (const auto & creator_file : creator_files) {
        stdtests_createvolume(creator_file.filename, creator_file.size, (const char *)creator_file.content);
    }
}

void stdtests_deletevolumes() {

    // deleting volume files inside
    for (const auto & creator_file : creator_files) {
        remove(creator_file.filename.c_str());
    }

    // removing folder on cleanup
    rmdir(CREATOR_FILE_FOLDER);

}
