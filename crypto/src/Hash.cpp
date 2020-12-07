//
// Created by badbouille on 07/12/2020.
//


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