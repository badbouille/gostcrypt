//
// Created by badbouille on 05/05/2020.
//

#include <Volume.h>
#include <VolumeStandard.h>

GostCrypt::VolumeList GostCrypt::Volume::GetAvailableVolumeTypes()
{
    GostCrypt::VolumeList l;

    // Default volume type
    l.push_back(new VolumeStandard());

    return l;
}
