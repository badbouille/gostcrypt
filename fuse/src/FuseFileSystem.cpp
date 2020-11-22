//
// Created by badbouille on 05/05/2020.
//

#include <Volume.h>
#include <VolumeStandard.h>
#include "FuseFileSystem.h"
#include "FuseFileSystemNone.h"
#include "FuseFileSystemExt2.h"

GostCrypt::FuseFileSystemList GostCrypt::FuseFileSystem::GetFileSystems()
{
    GostCrypt::FuseFileSystemList l;

    // Default volume type
    l.push_back(new FuseFileSystemNone());
    l.push_back(new FuseFileSystemExt2());

    return l;
}
