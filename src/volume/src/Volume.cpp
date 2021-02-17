/**
 * @file Volume.cpp
 * @author badbouille
 * @date 05/05/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Volume.h>
#include <VolumeStandard.h>

GostCrypt::VolumeList GostCrypt::Volume::GetAvailableVolumeTypes()
{
    GostCrypt::VolumeList l;

    // Default volume type
    l.push_back(new VolumeStandard());

    return l;
}
