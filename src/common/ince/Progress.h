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
    /**
     * @brief Class to manage callbacks to report progress during treatment of long requests.
     *
     * The class can be instantiated in many ways (direct callback, reporting to another Progress object, shared memory reporting, etc.)
     */
    class Progress {
    public:

        /**
         * @brief Structure used to send info between processes over a shared memory.
         */
        typedef struct ProgressInfo {
            static const uint32_t NAME_MAX_SIZE = 256;
            static const uint32_t MSG_MAX_SIZE = 512;
            enum { EXCEPTION=0x1, PROGRESS=0x2 } type; /**< Type of data received. Exceptions and Progress updates can be sent across processes. */
            char name[NAME_MAX_SIZE]; /**< Name of the Exception if available */
            float progress; /**< Current progress in percent if available */
            char msg[MSG_MAX_SIZE]; /**< Message of the exception or message associated with the progress update */
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

        /* Callback mode */

        /**
         * Method used to set the 'report function', which is called by this class to report the current progress.
         * @param f the function to call everytime progress is reported
         */
        void setCallBack(ReportFunction_t f) { f_report = f; };

        /**
         * Method to get the current report function
         * @return the function called everytime progress is reported
         */
        ReportFunction_t getCallBack() { return f_report; };

        /**
         * Method used to completely remove the callback function
         */
        void removeCallBack() { f_report = nullptr; };

        /* Master mode */

        /**
         * @brief Method used to set the 'master', which is called by this class to report the current progress.
         *
         * The master may have a callback itself, but will decide how important is our task to report it accordingly.
         * For example:
         *  - Function 1 uses a Progress 1 object to report current progress to the user
         *  - Function 1 calls Function 2 during its progress. Function 2 is long and is 80% of Function 1's treatment
         *  - Function 2 will set Progress 1 as the master of its own Progress 2 object to report progress to Function 1
         *  - Function 1 will use ::setChildBounds to report Function 2's progress as it's own.
         * @param p the function to call everytime progress is reported
         */
        void setMaster(Progress *p) { master = p; };

        /**
         * Method used to completely remove the master link
         */
        void removeMaster() { master = nullptr; };

        /**
         * @brief Function used by a master progress function to set bounds to the received informations from a child.
         *
         * For example:
         *  - Progress1 is the master of Progress2
         *  - Progress1.setChildBounds(0.1f, 0.9f) will set the amout Progress2's work represents in Progress1's work
         *  - Progress2 reports 25% (0.25f) progress to Progress1
         *  - Progress1 estimates its own progress is 0.25f*(0.9-0.1)+0.1=0.3f (30%) and reports it using its callback
         *
         * @param low
         * @param high
         */
        void setChildBounds(float low, float high) { child_bound_low = low; child_bound_high = high; };

        /* Shared memory mode */

        /**
         * @brief Method used to set the 'shared memory', which is used to send progress, instead of a callback or a master.
         *
         * This shared memory is only used to send progress, reception is done by ::listenSharedMemory(),
         * used by another class in another process usually.
         *
         * @param s A pointer on the memory to fill with progress updates
         * @warning User should NEVER listen on the same shared memory setted here. In that case, the class would report itself indefinetely.
         */
        void setSharedMemory(SharedWindow<ProgressInfo_t> *s) { shm = s; };

        /**
         * Method used to completely remove the shared memory link
         */
        void removeSharedMemory() { shm = nullptr; };

        /**
         * @brief Method used to report an exception using the same shared memory as the progress reports.
         *
         * This is an alternative feature usually needed when reporting things between processes.
         *
         * @param e the exception to send across the shared memory
         */
        void reportException(GostCryptException &e);

        /**
         * @brief Method used to listen to a shared memory segment and report anything found in it.
         *
         * Method will read all progress reports currently stored in the shared memory, and report
         * them itself, using ::report. In case an exception is deserialized, it will be thrown again
         * as a GostCryptException. e.name() may be used to get the exception type. see GostCryptException.h.
         *
         * @param shm_listen the shm to listen to.
         * @warning User should NEVER listen on the same shared memory used to report. In that case, the class would report itself indefinetely.
         */
        void listenSharedMemory(SharedWindow<ProgressInfo_t> *shm_listen);

        /* Main functions */

        /**
         * @brief Main function of this module, used to report any progress made.
         *
         * Every mode initialized before (callback, master and shm) will be used to
         * report the progress made (more than one may be used).
         *
         * @param m message to send
         * @param p a number between 0 and 1 representing the current progress in percent
         */
        void report(const char *m, float p);

        /**
         * Method used to disable all reports altogether.
         * @note This will not disable the exception reports over shared memory
         */
        void disableCallback() { callback_enable = false; }

        /**
         * Method used to enable all currently configured report modes.
         */
        void enableCallback() { callback_enable = true; }

    protected:

        /**
         * Main function called from the child, to report progress and format it (see ::setChildBounds)
         * before calling the real report function.
         * @param m message to send
         * @param p a number between 0 and 1 representing the current progress in percent
         */
        void reportFromChild(const char *m, float p);

    private:

        /**
         * Attribute containing the report function called in ::report
         */
        ReportFunction_t f_report;

        /**
         * Attribute containing the master class to report to in ::report
         */
        Progress *master;

        /**
         * Attribute containing the shm filled when ::report is called
         */
        SharedWindow<ProgressInfo_t> *shm;

        /**
         * Current high bound of the child's progress if any
         */
        float child_bound_high;

        /**
         * Current low bound of the child's progress if any
         */
        float child_bound_low;

        /**
         * Boolean to know if the ::report function has been disabled or not
         */
        bool callback_enable;

    };
}

#endif //GOSTCRYPT_PROGRESS_H
