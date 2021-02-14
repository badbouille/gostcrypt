/**
 * @file VolumeCreatorContents.h
 * @author badbouille
 * @date 09/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _VOLUMECREATORCONTENTS_H
#define _VOLUMECREATORCONTENTS_H

#include "Buffer.h"
#include "VolumeStandardHeader.h"

typedef struct VolumeFileCreator_s {
    const std::string filename;
    size_t size;
    const uint8_t *content;
} VolumeFileCreator;

#define CREATOR_FILE_NUMBER 2
#define CREATOR_FILE_FOLDER "volumefiles"

extern const uint8_t volumee_content[STANDARD_HEADER_SIZE*5+STANDARD_HEADER_SALT_AREASIZE*4];
extern const uint8_t volume1_content[STANDARD_HEADER_SIZE*5+STANDARD_HEADER_SALT_AREASIZE*4];

extern const VolumeFileCreator creator_files[CREATOR_FILE_NUMBER];

#endif // _DISKENCRYPTIONALGORITHMTESTS_H
