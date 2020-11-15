//
// Created by badbouille on 05/05/2020.
//

#include <Volume.h>
#include <VolumeStandard.h>
#include "../ince/FuseFileSystem.h"
#include "../ince/FuseFileSystemNone.h"

GostCrypt::FuseFileSystemList GostCrypt::FuseFileSystem::GetFileSystems()
{
    GostCrypt::FuseFileSystemList l;

    // Default volume type
    l.push_back(new FuseFileSystemNone());

    return l;
}
