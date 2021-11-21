/**
 * @file QThreads.cpp
 * @author badbouille
 * @date 21/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "QThreads.h"
#include <cstring>
#include "GraphicInterface.h"

extern GraphicInterface *current_instance;

void GostCrypt::MountThread::run()
{
    try
    {
        GostCrypt::Core::mount(params);
        params->password.erase();
        delete params;
        delete pass;
    } catch (GostCrypt::GostCryptException &e) {
        if(std::string(e.name()) == "VolumePasswordException") {
            emit current_instance->volumePasswordIncorrect();
        } else {
            QVariantList r;
            r << e.name();
            r << e.what();
            emit current_instance->sprintSendError(r);
        }
        return;
    }

    emit current_instance->sprintMountVolume(QVariantList());
}

void GostCrypt::CreateThread::run()
{
    try
    {
        GostCrypt::Core::create(params);
        params->password.erase();
        delete params;
        delete pass;
    } catch (GostCrypt::GostCryptException &e) {
        QVariantList r;
        r << e.name();
        r << e.what();
        emit current_instance->sprintSendError(r);
        return;
    }

    emit current_instance->sprintCreateVolume(QVariantList());
}
