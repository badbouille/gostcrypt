
#ifndef _ENCRYPTIONALGORITHMECB_H_
#define _ENCRYPTIONALGORITHMECB_H_

#include <list>
#include <memory>
#include "Buffer.h"
#include "GostCryptException.h"
#include "DiskEncryptionAlgorithm.h"
#include "BlockCipher.h"

namespace GostCrypt
{

    /**
     * @brief Test class implementing the most basic encryption algorithm for testing purposes: ECB-CS
     *
     */
    template<class T>
    class DiskEncryptionAlgorithmECB : public DiskEncryptionAlgorithm
    {
    public:


        DiskEncryptionAlgorithmECB() : bc(new T()) {};
        ~DiskEncryptionAlgorithmECB() override {delete bc;}

        void SetKey(SecureBufferPtr &key) override { bc->SetKey(key); currentState = READY; };

        void Decrypt(SecureBufferPtr &data, size_t sectorIndex) const override;

        void Encrypt(SecureBufferPtr &data, size_t sectorIndex) const override;

        size_t GetKeySize() const override { return bc->GetKeySize(); };

        std::string GetName() const override { return "ECB Mode with " + bc->GetName() + " block cipher"; };

        std::string GetID() const override { return "ECB_{" + bc->GetID() + "}"; };

        std::string GetDescription() const override { return "Electronic Codebook (ECB) mode is a simple mode where the sectors "
                                                      "are divided in blocks and encrypted using the underlying block cipher. "
                                                      "This is a very simple mode that should only be used for test purposes."
                                                      "\n"
                                                      "This version of ECB comes with ciphertext stealing, in case the given data "
                                                      "can not be separated in a round number of blocks."
                                                      "\n"
                                                      + bc->GetDescription(); };

    protected:

        /** Pointer on the current BlockCipher.
         * Created with the algorithm: should never be nullptr.
         */
        BlockCipher *bc;

    };

}

/* Inclusion of all possible implementations of this template */
#include "DiskEncryptionAlgorithmECBimpl.h"

#endif //_ENCRYPTIONALGORITHMECB_H_
