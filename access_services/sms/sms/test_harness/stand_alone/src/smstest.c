/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
* DESCRIPTION:
* Simple SMS send/receive test harness
*
*
*************************************************************************/

#include <stdint.h>
#include <stdio.h>

#include <pthread.h>

#include "r_sms.h"
#include "r_smslinuxporting.h"
#include "g_sms.h"
#include "test_cases.h"


// Construct array of test cases based on shared data in test_cases.con
#ifdef SMS_TEST_CASE_EXECUTE
#undef SMS_TEST_CASE_EXECUTE
#endif
#define SMS_TEST_CASE_EXECUTE(FncName) {FncName, #FncName, SMS_TEST_CASE_FRAMEWORK_ID_STAND_ALONE},
static const SMS_TestCaseFncs_t TestCasesArray[] = {
#include "test_cases.con"
};

#undef SMS_TEST_CASE_EXECUTE


/*************************************************************************
**************************************************************************
*
* SMSTest_RunTestCases
*
* Iterate through array of test cases and call them.
*
**************************************************************************
**************************************************************************/
static uint8_t SMSTest_RunTestCases(
    uint32_t * const NumTestPasses_p,
    uint32_t * const NumTestFailures_p)
{
    unsigned int Index;
    uint8_t OverallResult = TRUE;

    for (Index = 0; Index < sizeof TestCasesArray / sizeof TestCasesArray[0]; Index++) {
        uint8_t TempResult;

        SMS_A_(printf("\nSMS TEST: INDEX=%d %s: START\n", Index, TestCasesArray[Index].TestCaseName_p));
        TempResult = TestCasesArray[Index].TestCaseFunction(TestCasesArray[Index].TestCaseName_p, TestCasesArray[Index].TestCaseFrameworkId);
        SMS_A_(printf("\nSMS TEST: INDEX=%d %s: %s\n", Index, TestCasesArray[Index].TestCaseName_p, TempResult ? "PASS" : "FAIL"));

        if (!TempResult) {
            OverallResult = FALSE;
            (*NumTestFailures_p)++;
        } else {
            (*NumTestPasses_p)++;
        }
    }

    return OverallResult;
}


/*************************************************************************
**************************************************************************
*
* Process:  SMSTestClient1 - Client process
*
* This thread operates in a CLIENT context, subscribing to the SMS Server
* and sending/receiving SMS.
*
**************************************************************************
**************************************************************************/
static void *SMSTestClient1(
    void *threadid)
{
    uint8_t Result;
    uint32_t NumTestPasses = 0;
    uint32_t NumTestFailures = 0;

    // Run Test Cases, but only if Initialise was successful
    Result = SMSTest_RunTestCases(&NumTestPasses, &NumTestFailures);

    if (Result) {
        SMS_A_(printf("\nSMS TEST: SMSTestClient1: All %d Test Cases Passed.\n", NumTestPasses + NumTestFailures));
    } else {
        SMS_A_(printf("\nSMS TEST: SMSTestClient1: Test Cases Failed.\n"));
    }
    SMS_A_(printf("\nSMS TEST: SMSTestClient1: %d Test Case(s) Passed. %d Test Case(s) Failed.\n", NumTestPasses, NumTestFailures));

    SMS_IDENTIFIER_NOT_USED(threadid);
    pthread_exit(NULL);
    return NULL;
}



/*************************************************************************
**************************************************************************
*
* main
*
* Test harness process main function.
*
**************************************************************************
**************************************************************************/
int main(
    void)
{
    int rc;
    long t = 111;
    pthread_t TestThread1;

    SMS_A_(printf("\n *** SMS SERVER TEST HARNESS started! ***\n"));

    //Start the Test harness thread
    rc = pthread_create(&TestThread1, NULL, SMSTestClient1, (void *) t);

    if (rc) {
        SMS_A_(printf("\nERROR: SMS SERVER TEST HARNESS return code from pthread_create() is %d\n", rc));
        exit(1);
    }

    // Block until TestThread1 terminates.
    pthread_join(TestThread1, NULL);
    SMS_A_(printf("\nSMS: smstest.c: SMS SERVER TEST HARNESS Thread Terminated\n"));

    return 0;
}
