/**
 * @file QThreads.h
 * @author badbouille
 * @date 21/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef GOSTCRYPT_QTHREADS_H
#define GOSTCRYPT_QTHREADS_H

#include "Core.h"

#include <QThread>

namespace GostCrypt
{
    class MountThread : public QThread {

    public:
        // constructor
        explicit MountThread(GostCrypt::Core::MountParams_t *a, GostCrypt::SecureBuffer *p) { params = a; pass = p; };

        // overriding the QThread's run() method
        void run();

    private:
        GostCrypt::Core::MountParams_t *params;
        GostCrypt::SecureBuffer *pass;
    };

    class CreateThread : public QThread {

    public:
        // constructor
        explicit CreateThread(Core::CreateParams_t *a, GostCrypt::SecureBuffer *p) { params = a; pass = p; };

        // overriding the QThread's run() method
        void run();

    private:
        Core::CreateParams_t *params;
        GostCrypt::SecureBuffer *pass;
    };

}

#endif //GOSTCRYPT_QTHREADS_H
