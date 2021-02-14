/**
 * @file VolumeCreator.h
 * @author badbouille
 * @date 09/11/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _VOLUMECREATOR_H
#define _VOLUMECREATOR_H

#include "Buffer.h"

void stdtests_createvolume(const std::string& file, size_t length, const char *content);
void stdtests_createvolumes();
void stdtests_deletevolumes();

#endif //_VOLUMECREATOR_H
