/**
 * @file KDF.cpp
 * @author badbouille
 * @date 13/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */


#include "KDF.h"
#include "KDFPbkdf2.h"
#include "HMAC.h"
#include "HashXOR0.h"

namespace GostCrypt {

    KDFList KDF::GetAvailableKDFs()
    {
        KDFList l;

        // Test algorithm
        l.push_back(new KDFPbkdf2<HMAC<HashXOR0<16>>, 4096>());

        return l;
    }

}