/**
 * @file progressTests.cpp
 * @author badbouille
 * @date 20/11/2021
 *
 * This project is released under the GNU General Public License v3.0.
 */

#include "Progress.h"
#include "unity.h"
#include <string>
#include <cstring>


using namespace GostCrypt;

float g_TP_expected_value;
std::string g_TP_expected_msg;
enum {TP_NOTCALLED, TP_WRONGCALLED, TP_OK} g_TP_testResult;

void test_progress_testfunction(const char *m, float p) {
    g_TP_testResult = TP_OK;

    if(std::string(m) != g_TP_expected_msg) {
        g_TP_testResult = TP_WRONGCALLED;
        g_TP_expected_msg = std::string(m);
    }

    if(p != g_TP_expected_value) {
        g_TP_testResult = TP_WRONGCALLED;
        g_TP_expected_value = p;
    }

}

void test_progress_callback() {
    Progress p;

    g_TP_expected_value = 0.1f;
    g_TP_expected_msg = "no message";

    p.setCallBack(test_progress_testfunction);

    // default test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.2f;
    p.report("no message", 0.2f);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_OK, g_TP_testResult, "Callback was not called in default mode");

    // disabled test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.3f;
    p.disableCallback();
    p.report("no message", 0.3f);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_NOTCALLED, g_TP_testResult, "Callback was called when disabled");

    // reenabled test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.4f;
    p.enableCallback();
    p.report("no message", 0.4f);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_OK, g_TP_testResult, "Callback was not called when enabled again");

    // remove test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.5f;
    p.removeCallBack();
    p.report("no message", 0.5f);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_NOTCALLED, g_TP_testResult, "Callback was called after being removed");

}

void test_progress_master() {
    Progress p;
    Progress pchild;

    g_TP_expected_value = 0.1f;
    g_TP_expected_msg = "no message";

    p.setCallBack(test_progress_testfunction);

    // default test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.2f;
    pchild.report("no message", 0.2f);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_NOTCALLED, g_TP_testResult, "Child was not supposed to know the callback");

    // master default test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.3f;
    pchild.setMaster(&p);
    pchild.report("no message", 0.3f);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_OK, g_TP_testResult, "Callback was not called by the child");

    // bounded test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.44f;
    p.setChildBounds(0.4f, 0.5f);
    pchild.report("no message", 0.4f); // child advanced 40% in his 40-50% range, meaning master is at 44%
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_OK, g_TP_testResult, "Callback failed to report bounded value from child");

    // disabled test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.45f;
    p.disableCallback();
    pchild.report("no message", 0.5f);
    p.enableCallback();
    pchild.disableCallback();
    pchild.report("no message", 0.5f);
    pchild.enableCallback();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_NOTCALLED, g_TP_testResult, "Callback was called after being disabled");
}

void test_progress_shm () {
    Progress p_send;
    Progress p_listen;
    SharedWindow<Progress::ProgressInfo_t> shm_send(328);
    SharedWindow<Progress::ProgressInfo_t> shm_listen(328);

    g_TP_expected_value = 0.1f;
    g_TP_expected_msg = "no message";

    p_listen.setCallBack(test_progress_testfunction);
    p_send.setSharedMemory(&shm_listen);

    // failure test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.1f;
    p_send.report("no message", 0.1f);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_NOTCALLED, g_TP_testResult, "Listener was not supposed to call the callback");

    // default test
    g_TP_testResult = TP_NOTCALLED;
    g_TP_expected_value = 0.2f;
    p_send.report("no message", 0.2f);
    p_listen.listenSharedMemory(&shm_listen); // tricky. Will call twice the callback (0.1 and 0.2)
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_OK, g_TP_testResult, "progress over shared memory did not return the right value");

    // exception test
    g_TP_testResult = TP_NOTCALLED;
    try {
        throw GOSTCRYPTEXCEPTION("no message");
    } catch(GostCryptException &e) {
        p_send.reportException(e); // sending the exception in process 1
    }
    try {
        p_listen.listenSharedMemory(&shm_listen); // catching the exception in process 2
    } catch (InvalidParameterException &e) {
        g_TP_testResult = TP_WRONGCALLED;
    } catch(GostCryptException &e) {
        g_TP_testResult = TP_OK;
    }
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TP_OK, g_TP_testResult, "Exception was not received in second process");
}
