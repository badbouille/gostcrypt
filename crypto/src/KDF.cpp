//
// Created by badbouille on 13/01/2020.
//


#include "KDF.h"
#include "KDFPbkdf2.h"
#include "HMAC.h"
#include "HashXOR0.h"

namespace GostCrypt {

    KDFList KDF::GetAvailableKDFs()
    {
        GostCrypt::KDFList l;

        // Test algorithm
        l.push_back(new KDFPbkdf2<HMAC<HashXOR0<16>>, 4096>());

        return l;
    }

}