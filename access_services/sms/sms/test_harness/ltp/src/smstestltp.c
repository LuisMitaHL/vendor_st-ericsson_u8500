/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/******************************************************************************
* Description: sms test
*
* Test Assertion and Strategy: -
*
******************************************************************************/

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

//SMS header
#include "r_sms.h"
#include "g_sms.h"
#include "r_smslinuxporting.h"
#include "test_cases.h"


#include <ltp_macro.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



//
// Extern Global Variables
//
extern char *TESTDIR;           /* temporary dir created by tst_tmpdir() */

//
// Global Variables
//
const char *TCID = "sms_test";  /* test program identifier.             */
#define TEST_MODULE_NAME "sms_test"


/***************************************************************
 * cleanup() - performs all ONE TIME cleanup for this test at
 *             completion or premature exit.
 ***************************************************************/
static void cleanup(
    void)
{
    TEST_CLEANUP;
    tst_exit();
}

/***************************************************************
 * setup() - performs all ONE TIME setup for this test.
 ***************************************************************/
static void setup(
    void)
{
    int ret = 0;

    /*
       ret = system("modprobe " TEST_MODULE_NAME);
       sleep(1);
     */

    if (ret < 0) {
        tst_resm(TBROK, "Failed to load module %s", TEST_MODULE_NAME);
        TEST_CLEANUP;
        tst_exit();
    }
}




// Construct array of test cases based on shared data in test_cases.con
#ifdef SMS_TEST_CASE_EXECUTE
#undef SMS_TEST_CASE_EXECUTE
#endif
#define SMS_TEST_CASE_EXECUTE(FncName) {FncName, #FncName, SMS_TEST_CASE_FRAMEWORK_ID_LTP},
static const SMS_TestCaseFncs_t TestCasesArray[] = {
#include "test_cases.con"
};

#undef SMS_TEST_CASE_EXECUTE


// Setup LTP global variable for number of test cases.
int TST_TOTAL = sizeof TestCasesArray / sizeof TestCasesArray[0];



int main(
    int argc,
    const char **argv)
{
    //int lc;                       /* loop counter */
    //char *msg;            /* message returned from parse_opts */
    //int fd1, fd2;
    //SMS_RequestStatus_t RequestStatus = SMS_REQUEST_PENDING;
    //SMS_Error_t     ErrorCode     = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
    //uint8_t         result        = FALSE;
    //int     Index;
    //uint8_t OverallResult = TRUE;
    uint8_t TempResult;

    tst_resm(TINFO, "\nLTP_SMS_TEST Start\n");

    /***************************************************************
     * parse standard options
    ***************************************************************/
    /*
       if ((msg = parse_opts(argc, argv, (option_t *) NULL, NULL)) != (char *) NULL) {
       tst_brkm(TBROK, NULL, "OPTION PARSING ERROR - %s", msg);
       tst_exit();
       }
     */

    setup();

    /***************************************************************
     * only perform functional verification if flag set (-f not given)
     ***************************************************************/
    if (STD_FUNCTIONAL_TEST) {
        for (Tst_count = 0; Tst_count < TST_TOTAL;) {
            SMS_A_(printf("\nLTP_SMS_TEST: Start Test %d (%s)\n", Tst_count, TestCasesArray[Tst_count].TestCaseName_p));
            TempResult = TestCasesArray[Tst_count].TestCaseFunction(TestCasesArray[Tst_count].TestCaseName_p, TestCasesArray[Tst_count].TestCaseFrameworkId);
            SMS_A_(printf("\nLTP_SMS_TEST: End Test %d (%s). Result=%s\n", Tst_count, TestCasesArray[Tst_count].TestCaseName_p, TempResult ? "PASS" : "FAIL"));
            if (TempResult == 1) {
                tst_resm(TPASS, "Functional test %i, %s OK", Tst_count, TestCasesArray[Tst_count].TestCaseName_p);
            } else {
                tst_resm(TFAIL, "Return value: %i. TCID: %s File: %s Line: %i. errno=%d : %s ", (int) TEST_RETURN, TCID, __FILE__, __LINE__, TEST_ERRNO, strerror(TEST_ERRNO));
            }

        }
    }

    cleanup();
    tst_exit();
    return 0;
}
