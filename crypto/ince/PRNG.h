/**
 * @file PRNG.h
 * @author badbouille
 * @date 10/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef _RNG_H
#define _RNG_H

#include <Buffer.h>

namespace GostCrypt
{
    class PRNG
    {
    public:
        PRNG() {};

        /**
         * @brief This method fills a buffer with random data
         * @param[out] data the buffer to fill with random content
         */
        virtual void Get(SecureBufferPtr &data) = 0;

        /**
         * Function to get the name of the RNG (ex: Pseudo random number generator)
         * @return name as a string
         */
        virtual std::string GetName() const = 0;

        /**
         * Function to get the ID of the RNG (ex: PRNG)
         * @return id as a string
         */
        virtual std::string GetID() const = 0;

        /**
         * Function to get a small, user-friendly description of this RNG.
         * @return description as a string
         */
        virtual std::string GetDescription() const = 0;

    };

}


#endif //_RNG_H
