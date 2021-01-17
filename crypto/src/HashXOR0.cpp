//
// Created by badbouille on 07/12/2020.
//

#include "HashXOR0.h"

namespace GostCrypt {

    template<uint32_t SIZE>
    void HashXOR0<SIZE>::Process(const SecureBufferPtr &data)
    {
        for(int i=0; i< data.size(); i++) {
            digest.get()[(offset + i)%SIZE] ^= data.get()[i];
        }
        offset = (offset + data.size()) % digest.size();
    }

}