/**
 * @file Container.cpp
 * @author badbouille
 * @date 10/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include <Container.h>
#include <ContainerFile.h>

GostCrypt::ContainerList GostCrypt::Container::GetAvailableContainerTypes()
{
    GostCrypt::ContainerList l;

    // Default container type
    l.push_back(new ContainerFile(""));

    return l;
}
