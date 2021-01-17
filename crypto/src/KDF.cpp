//
// Created by badbouille on 13/01/2021.
//


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