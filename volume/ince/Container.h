/**
 * @file Container.h
 * @author badbouille
 * @date 10/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _CONTAINER_H
#define _CONTAINER_H

#include <Buffer.h>
#include <list>
#include <string>

namespace GostCrypt
{

    class Container;

    typedef std::list<Container*> ContainerList;

    /**
     * @brief Abstract structure used to define a Container, the entity representing a support where a Volume can be written.
     *
     * The Containers are just file-like structures. They can be created, opened, written, resized.
     * Having a generic structure lets us write our volumes anywhere.
     *
     * The basic implementation of the container is #GostCrypt::ContainerFile, a simple random access file where the encrypted data is written.
     *
     * This class may lead to few other implementations in the future:
     * - ContainerOnline, a container stored online to be accessed from anywhere using credentials.
     * - ContainerDrive, a usb-key or drive. Pretty much the same as file but can't be resized, created or deleted.
     * - ContainerSteganographic, entirely written inside a video, audio, or image file using steganography techniques.
     * - ..., This class is abstract, implement your own!
     *
     */
    class Container
    {
    public:

        // ----- INIT -----
        Container() : opened(false) {};
        virtual ~Container() = default;

        // Creation

        /**
         * @brief Generic open function to open the Container.
         * @note takes no parameters because they are given by the implementation.
         */
        virtual void open() = 0;

        /**
         * @brief Generic create function to create the Container.
         * @note takes no parameters because they are given by the implementation.
         * @param requested size of the empty container to create
         */
        virtual void create(size_t size) = 0;

        // ----- RUNTIME -----
        // Filesystem interface
        /**
         * @brief Function to write plain data to the Container.
         *
         * @param buffer data to write to the container
         * @param offset offset to write to.
         */
        virtual void write(SecureBufferPtr buffer, size_t offset) = 0;

        /**
         * @brief Function to read plain data to the Container.
         *
         * @param buffer the buffer to fill with data from the Container
         * @param offset offset to read from the Container
         */
        virtual void read(SecureBufferPtr buffer, size_t offset) = 0;

        // Core interface (control)
        /**
         * Erases the container (meaning it is destroyed forever on the disk)
         */
        virtual void erase() = 0;

        /**
         * Closes the container and deletes the current credentials etc.
         * @warning The generic interface will not allow to open it again.
         */
        virtual void close() = 0;

        /**
         * Resizes the container to the given size.
         * Will throw an exception if th container can't be resized.
         * @param size the size in byte we want to container to be.
         */
        virtual void resize(size_t size) = 0;

        /**
         * Static function to get the list of available container types
         * They are uninitialized and need a call to open or create.
         * @note The generic interface does not define a generic open or create.
         * @return A list of all available container types
         */
        static ContainerList GetAvailableContainerTypes();

        // Core interface (static display)
        /**
         * Function to get the name of this implementation of Container, in a fancy and displayable way.
         * @return name of the container type
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the container type. Usually just the name of the class without 'Container'
         * @return The id of this container type
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small description of this container type for the user.
         * @return description of this container type.
         */
        virtual std::string GetDescription() const = 0;

        // Core interface (dynamic display)
        /**
         * Returns the size in bytes of this Volume
         * @return Size in bytes
         */
        virtual size_t getSize() = 0;

        /**
         * Returns the maximum size this volume can take
         * @return Max size in bytes
         */
        virtual size_t getMaxSize() = 0;

        /**
         * Returns the name of the managed container
         * @return the container's path
         */
        virtual std::string getSource() = 0;

        /**
         * @brief to know if the current container is opened and ready to use
         * @return True if the container is ready for use
         */
        bool isOpened() { return opened; }

    protected:

        /**
         * Boolean to know if this Container is opened and ready to use or not.
         */
        bool opened;

    };

}


#endif //_CONTAINER_H
