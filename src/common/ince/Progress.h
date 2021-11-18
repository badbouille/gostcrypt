/**
 * @file Progress.h
 * @author badbouille
 * @date 18/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#ifndef GOSTCRYPT_PROGRESS_H
#define GOSTCRYPT_PROGRESS_H

#include "platform/shared.h"
#include "GostCryptException.h"

namespace GostCrypt
{
    class Progress { // TODO comment
    public:

        /**
         * @brief Structure used to send info between the standard core and the forked one
         */
        typedef struct ProgressInfo {
            enum { EXCEPTION=0x1, PROGRESS=0x2 } type;
            // exception infos
            static const uint32_t NAME_MAX_SIZE = 256;
            char name[NAME_MAX_SIZE];
            // progress infos
            float progress;
            // both
            static const uint32_t MSG_MAX_SIZE = 512;
            char msg[MSG_MAX_SIZE];
        } ProgressInfo_t;

        /**
         * @brief type for report function, to report current status of jobs to the caller during execution
         */
        typedef void (*ReportFunction_t)(const char *, float);

        // Constructors
        Progress() { master = nullptr;
            f_report = nullptr;
            shm = nullptr;
            callback_enable = true;
            child_bound_high = 1.0f;
            child_bound_low = 0.0f;
        };
        ~Progress() = default;

        /**
         * Function used to set the 'report function', which is called byt this module to report the current progress.
         * @param f the function to call everytime progress is reported
         */
        void setCallBack(ReportFunction_t f) { f_report = f; };
        ReportFunction_t getCallBack() { return f_report; };
        void removeCallBack() { f_report = nullptr; };

        void setMaster(Progress *p) { master = p; };
        void removeMaster() { master = nullptr; };

        void setChildBounds(float low, float high) { child_bound_low = low; child_bound_high = high; };

        void setSharedMemory(SharedWindow<ProgressInfo_t> *s) { shm = s; };
        void removeSharedMemory() { shm = nullptr; };

        void disableCallback() { callback_enable = false; }
        void enableCallback() { callback_enable = true; }

        /**
         * Main function of this module, used to report any progress made
         * @param m message to send
         * @param p a number between 0 and 1 representing the current progress in percent
         */
        void report(const char *m, float p);

        void reportException(GostCryptException &e);

    protected:
        void reportFromChild(const char *m, float p);

    private:
        ReportFunction_t f_report;
        Progress *master;
        SharedWindow<ProgressInfo_t> *shm;

        float child_bound_high;
        float child_bound_low;
        bool callback_enable;

    };
}

#endif //GOSTCRYPT_PROGRESS_H
