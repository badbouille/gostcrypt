/**
 * @file HMAC.cpp
 * @author badbouille
 * @date 12/01/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "HMAC.h"
#include "HashXOR0.h"

namespace GostCrypt {

    template<class H>
    void HMAC<H>::InitFromCurrentKey()
    {
        SecureBufferPtr initvaluePtr(initvalue->get(), initvalue->size());
        SecureBuffer pad(initvaluePtr.size());
        SecureBufferPtr padPtr(pad.get(), pad.size());

        // i_key_pad as defined by HMAC standard
        for (uint32_t i = 0; i < pad.size(); i++) {
            pad.get()[i] = 0x36 ^ initvaluePtr.get()[i];
        }
        IHash->Process(padPtr);

        // o_key_pad as defined by HMAC standard
        for (uint32_t i = 0; i < pad.size(); i++) {
            pad.get()[i] = 0x5c ^ initvaluePtr.get()[i];
        }
        OHash->Process(padPtr);

        // end
        currentState = READY;
        pad.erase();
    }

    template<class H>
    void HMAC<H>::SetKey(const SecureBufferPtr &key)
    {
        SecureBufferPtr initvaluePtr(initvalue->get(), initvalue->size());

        // restarting if called after other functions
        IHash->Reset();
        OHash->Reset();

        // key copy to a standard buffer
        initvaluePtr.erase();
        if (key.size() > initvaluePtr.size()) {
            IHash->Process(key);
            IHash->GetDigest(initvaluePtr);
            IHash->Reset();
        } else {
            initvaluePtr.copyFrom(key);
        }

        InitFromCurrentKey();
    }

    template<class H>
    void HMAC<H>::Process(const SecureBufferPtr &data)
    {
        assertInit();
        IHash->Process(data);
    }

    template<class H>
    void HMAC<H>::GetDigest(SecureBufferPtr &digest)
    {
        assertInit();
        // as defined by the standard
        IHash->GetDigest(digest);
        OHash->Process(digest);
        OHash->GetDigest(digest);

        // any processing AFTER GetDigest will not work
        currentState = NOT_INIT;
    }

    template<class H>
    void HMAC<H>::Reset()
    {
        IHash->Reset();
        OHash->Reset();
        currentState = NOT_INIT;
        InitFromCurrentKey();
    }

}