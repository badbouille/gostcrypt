//
// Created by badbouille on 14/01/2020.
//

#ifndef _KDFPBKDF2_H
#define _KDFPBKDF2_H

#include <KDF.h>
#include "MAC.h"

namespace GostCrypt
{
    // TODO comment this
    template<class PRF, uint32_t ITERATIONS>
    class KDFPbkdf2 : public KDF
    {
    public:
        KDFPbkdf2() : prf(new PRF) {};
        ~KDFPbkdf2() override { delete prf; };

        void Derivate(const SecureBufferPtr &password, const SecureBufferPtr &salt, SecureBufferPtr &key) override;

        std::string GetName() const override { return "Pbkdf2 "+prf->GetName(); };
        std::string GetID() const override { return "Pbkdf2-"+prf->GetID(); };
        std::string GetDescription() const override { return "Pbkdf2 is a key derivation algorithm widly used in "
                                                             "a lot of applications today."
                                                             "\n"
                                                             + prf->GetDescription(); };

    protected:
        /**
         * MAC used as a hash function (pseudo random function from standard)
         */
        MAC *prf;
    };

}


#endif //_KDFPBKDF2_H
