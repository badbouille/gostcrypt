//
// Created by badbouille on 10/01/2021.
//

#include <Container.h>
#include <ContainerFile.h>

GostCrypt::ContainerList GostCrypt::Container::GetAvailableContainerTypes()
{
    GostCrypt::ContainerList l;

    // Default container type
    l.push_back(new ContainerFile());

    return l;
}
