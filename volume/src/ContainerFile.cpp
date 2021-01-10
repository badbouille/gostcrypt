//
// Created by badbouille on 10/01/2021.
//

#include <ContainerFile.h>

#include <utility>

/* TODO windows */
#include <unistd.h>

void GostCrypt::ContainerFile::open(const std::string& path)
{
    volumefilepath = path;
    volumefile.open(path.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    if (volumefile.fail()) {
        throw CANTOPENFILEEXCEPTION(volumefilepath);
    }
    opened = true;
    volumefile.seekg(0, std::ios_base::end);
    volumesize = volumefile.tellg();
}

void GostCrypt::ContainerFile::create(const std::string& path, size_t size)
{
    volumefilepath = path;
    volumefile.open(path.c_str(), std::ios_base::out | std::ios_base::binary);
    if (volumefile.fail()) {
        throw CANTCREATEFILEEXCEPTION(volumefilepath);
    }
    opened = true;
    volumefile.close();
    open(path);
    resize(size);
}

void GostCrypt::ContainerFile::write(GostCrypt::SecureBufferPtr buffer, size_t offset)
{
    // no checks on input. User can write anywhere and try to extend the volume size.
    if (!isOpened())
        CONTAINERUNINITIALIZEDEXCEPTION();
    volumefile.seekp(offset, std::ios_base::beg);
    volumefile.write((char *)buffer.get(), buffer.size());
    volumefile.flush();
    if (offset + buffer.size() > volumesize)
        volumesize = offset + buffer.size();
}

void GostCrypt::ContainerFile::read(GostCrypt::SecureBufferPtr buffer, size_t offset)
{
    if (!isOpened())
        CONTAINERUNINITIALIZEDEXCEPTION();
    if (offset + buffer.size() > volumesize)
        throw INVALIDPARAMETEREXCEPTION("Trying to read outside of file range");
    volumefile.seekg(offset, std::ios_base::beg);
    volumefile.read((char *)buffer.get(), buffer.size());
}

void GostCrypt::ContainerFile::erase()
{
    if (!isOpened())
        throw CONTAINERUNINITIALIZEDEXCEPTION();
    close();
    remove(volumefilepath.c_str());
}

void GostCrypt::ContainerFile::close()
{
    if (!isOpened())
        return;
    volumefile.close();
    volumesize = 0;
    opened = false;
}

void GostCrypt::ContainerFile::reopen()
{
    close();
    open(volumefilepath);
}

void GostCrypt::ContainerFile::resize(size_t size)
{
    close();
    truncate(volumefilepath.c_str(), size);
    volumesize = size;
    open(volumefilepath);
}

std::string GostCrypt::ContainerFile::getSource()
{
    return volumefilepath;
}
