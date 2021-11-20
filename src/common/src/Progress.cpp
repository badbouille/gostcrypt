/**
 * @file Progress.cpp
 * @author badbouille
 * @date 18/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "Progress.h"
#include <cstring>

void GostCrypt::Progress::report(const char *m, float p)
{
    if(p < 0.0f || p > 1.0f)
        throw INVALIDPARAMETEREXCEPTION("percentage not between 0 and 1 ("+std::to_string(p)+")");
    if(!callback_enable)
        return;
    if(master)
        master->reportFromChild(m, p);
    if(f_report)
        f_report(f_report_ctx, m, p);
    if(shm) {
        /* Building message */
        ProgressInfo_t i;
        i.type = ProgressInfo::PROGRESS;
        strncpy(i.name, "progressreport", ProgressInfo::NAME_MAX_SIZE);
        i.progress = p;
        strncpy(i.msg, m, ProgressInfo::MSG_MAX_SIZE);
        /* Sending it */
        shm->put(i);
    }
}

void GostCrypt::Progress::reportException(GostCrypt::GostCryptException &e)
{
    // Exceptions are reported regardless of callback_enable
    if(shm) {
        /* Building message */
        ProgressInfo_t i;
        i.type = ProgressInfo::EXCEPTION;
        strncpy(i.name, e.name(), ProgressInfo::NAME_MAX_SIZE);
        i.progress = 0;
        strncpy(i.msg, e.what(), ProgressInfo::MSG_MAX_SIZE);
        /* Sending it */
        shm->put(i);
    }
}

void GostCrypt::Progress::reportFromChild(const char *m, float p)
{
    if(p < 0.0f || p > 1.0f)
        throw INVALIDPARAMETEREXCEPTION("percentage not between 0 and 1 ("+std::to_string(p)+")");
    report(m, p*(child_bound_high-child_bound_low) + child_bound_low);
}

void GostCrypt::Progress::listenSharedMemory(SharedWindow<ProgressInfo_t> *shm_listen)
{
    ProgressInfo_t i;
    uint32_t n; // used to not loop indefinetely if the child process is insanely fast
    if(!shm_listen)
        return; // return if no shm
    for(n=0; shm_listen->get(&i) == 0 && n < shm_listen->ELEMENT_NUMBER; n++) // reading all entries
    {
        switch (i.type)
        {
            case ProgressInfo::PROGRESS:
                report(i.msg, i.progress);
                break;
            case ProgressInfo::EXCEPTION:
                /* Rebuilding exception from deserialized infos */
                throw GostCryptException(i.msg, i.name);
            default:
                throw INVALIDPARAMETEREXCEPTION("Wrong deserialized value when reading info from shared window.");
        }
    }
}
