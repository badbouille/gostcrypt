/**
 * @file BlockCipherXOR.cpp
 * @author badbouille
 * @date 30/04/2020
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "../ince/BlockCipherXOR.h"

void GostCrypt::BlockCipherXOR::Encrypt(GostCrypt::SecureBufferPtr &buffer) const
{
    uint8_t *dataptr = buffer.get();
    const uint8_t *ctxptr = ctx.get();
    size_t i = 0;

    // TODO : speed test. All these conditions are probably very slow!!
    assertInit();

    if (buffer.size() != keySize) {
        throw INVALIDPARAMETEREXCEPTION("Input data size different from block size.");
    }

    // TODO : Obvious 32/64 bit optimization needed
    for (;i<keySize;i++) {
        dataptr[i] ^= ctxptr[i];
    }

}
