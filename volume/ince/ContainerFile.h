//
// Created by badbouille on 10/01/2021.
//

#ifndef _CONTAINERFILE_H
#define _CONTAINERFILE_H

#include <Container.h>
#include <list>
#include <string>
#include <fstream>
#include <utility>

namespace GostCrypt
{

    /**
     * @brief Implementation of Container class using a simple file.
     * This is the most simple and most used way of storing Volumes.
     */
    class ContainerFile : public Container
    {
    public:

        // ----- INIT -----
        ContainerFile(std::string path) : volumefilepath(std::move(path)), volumesize(0) {};
        ~ContainerFile() { ContainerFile::close(); };

        // Creation
        /**
         * @brief Opens a file from the given path
         * If the file is not present, the method will throw an exception
         */
        void open() override;

        /**
         * @brief Method function to create a file from a given path and size
         * @param size the size of the file to create
         */
        void create(size_t size) override;

        // ----- RUNTIME -----
        // Filesystem interface
        void write(SecureBufferPtr buffer, size_t offset) override;
        void read(SecureBufferPtr buffer, size_t offset) override;

        // Core interface (control)
        /**
         * Deletes the file
         */
        void erase() override;

        /**
         * Closes the file
         */
        void close() override;

        void resize(size_t size) override;

        // Core interface (static display)

        std::string GetName() const override { return "File Container"; };
        std::string GetID() const override { return "file"; };
        std::string GetDescription() const override { return "File Container is a simple file that contains the volume."; };

        // Core interface (dynamic display)
        /**
         * Returns the size in bytes of this Volume
         * @return Size in bytes
         */
        size_t getSize() { return volumesize; };

        /**
         * Returns the maximum size this volume can take
         * @return Max size in bytes
         */
        size_t getMaxSize() { if (sizeof(size_t) == 8) return 0xFFFFFFFFFFFFFFFF; else return 0xFFFFFFFF; };

        /**
         * Returns the name of the managed container
         * @return the container's path
         */
        std::string getSource() override;

    private:

        /**
         * Path of the target file.
         */
        std::string volumefilepath;

        /**
         * Stream pointing to the current encrypted volume file. Must be handled with care.
         */
        std::fstream volumefile;

        /**
         * Current size of the container
         */
        size_t volumesize;

    };

}


#endif //_CONTAINERFILE_H
