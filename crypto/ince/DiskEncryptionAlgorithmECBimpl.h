
#ifndef _ENCRYPTIONALGORITHMECBIMPL_H_
#define _ENCRYPTIONALGORITHMECBIMPL_H_

#include "BlockCipherXOR.h"

namespace GostCrypt {

    /* explicit instanciation of all existing DiskEncryptionAlgorithm classes */

    template class DiskEncryptionAlgorithmECB<BlockCipherXOR>;

}

#endif //_ENCRYPTIONALGORITHMECB_H_
