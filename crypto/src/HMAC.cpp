//
// Created by badbouille on 12/01/2020.
//

#include "HMAC.h"
#include "HashXOR0.h"

namespace GostCrypt {

    template<class H>
    void HMAC<H>::SetKey(const SecureBufferPtr &key)
    {
        SecureBuffer blockKey(IHash->GetDigestSize());
        SecureBufferPtr blockKeyPtr(blockKey.get(), blockKey.size());

        SecureBuffer pad(blockKey.size());
        SecureBufferPtr padPtr(pad.get(), pad.size());

        // restarting if called after other functions
        Reset();

        // key copy to a standard buffer
        blockKey.erase();
        if (key.size() > blockKey.size()) {
            IHash->Process(key);
            IHash->GetDigest(blockKeyPtr);
            IHash->Reset();
        } else {
            blockKey.copyFrom(key);
        }

        // i_key_pad as defined by HMAC standard
        for (uint32_t i = 0; i < pad.size(); i++) {
            pad.get()[i] = 0x36 ^ blockKey.get()[i];
        }
        IHash->Process(padPtr);

        // o_key_pad as defined by HMAC standard
        for (uint32_t i = 0; i < pad.size(); i++) {
            pad.get()[i] = 0x5c ^ blockKey.get()[i];
        }
        OHash->Process(padPtr);

        // end
        currentState = READY;
        pad.erase();
        blockKey.erase();
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
    }

    template<class H>
    void HMAC<H>::Reset()
    {
        // TODO Reset doesn't really Resets for now since we forgot the key...
        IHash->Reset();
        OHash->Reset();
        currentState = NOT_INIT;
    }

}