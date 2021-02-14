/**
 * @file MAC.cpp
 * @author badbouille
 * @date 13/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "MAC.h"
#include "HMAC.h"
#include "HashXOR0.h"

namespace GostCrypt {

    MACList MAC::GetAvailableMACs()
    {
        MACList l;

        // HMAC - XOR0 - 16 bytes (most simple algorithm for this early version)
        l.push_back(new HMAC<HashXOR0<16>>());

        return l;
    }

}