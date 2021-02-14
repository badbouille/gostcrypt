/**
 * @file Hash.cpp
 * @author badbouille
 * @date 07/12/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */


#include "Hash.h"
#include "HashXOR0.h"

namespace GostCrypt {

    HashList Hash::GetAvailableHashes()
    {
        GostCrypt::HashList l;

        // XOR0 - 16 bytes (most simple algorithm for this early version)
        l.push_back(new HashXOR0<16>());

        return l;
    }

}