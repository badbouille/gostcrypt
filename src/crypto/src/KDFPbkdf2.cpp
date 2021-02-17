/**
 * @file KDFPbkdf2.cpp
 * @author badbouille
 * @date 14/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */


#include "KDFPbkdf2.h"
#include "GostCryptException.h"

namespace GostCrypt {

    template<class MAC, uint32_t ITERATIONS>
    void KDFPbkdf2<MAC, ITERATIONS>::Derivate(const SecureBufferPtr &password,
                                              const SecureBufferPtr &salt,
                                              SecureBufferPtr &key)
    {
        uint32_t l = key.size() / prf->GetBlockSize();
        if (key.size() % prf->GetBlockSize()) l++;

        SecureBufferPtr KeyPtr(key.get(), key.size());

        SecureBuffer Tn(prf->GetBlockSize());
        SecureBufferPtr TnPtr(Tn.get(), Tn.size());

        SecureBuffer Un(prf->GetDigestSize());
        SecureBufferPtr UnPtr(Un.get(), Un.size());

        SecureBuffer INT(sizeof(uint32_t));
        SecureBufferPtr INTPtr(INT.get(), INT.size());

        // Setting HMAC password
        prf->SetKey(password);

        for (uint32_t i = 0; i < l; i++) {

            // computing INT(i)
            *((uint32_t*)INTPtr.get()) = htobe32(i+1);

            // processing Salt || INT(i)
            prf->Process(salt);
            prf->Process(INTPtr);

            // First Un
            prf->GetDigest(UnPtr);
            TnPtr.copyFrom(UnPtr);

            // Other Uns
            for (uint32_t j = 1; j < ITERATIONS; j++) {
                prf->Reset();
                prf->Process(UnPtr);
                prf->GetDigest(UnPtr);
                TnPtr.Xor(UnPtr);
            }

            // reset for next computation
            prf->Reset();

            // Copy of Tn into real key space
            if ((i+1)*prf->GetBlockSize() > key.size()) {
                key.getRange(KeyPtr, i*prf->GetBlockSize(), key.size()-i*prf->GetBlockSize());
                TnPtr.getRange(TnPtr, 0, KeyPtr.size());
            } else {
                key.getRange(KeyPtr, i*prf->GetBlockSize(), prf->GetBlockSize());
            }

            KeyPtr.copyFrom(TnPtr);
        }

    }

}