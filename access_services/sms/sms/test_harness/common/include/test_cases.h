/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_TEST_CASES_H_
#define INCLUSION_GUARD_TEST_CASES_H_

#include <stdint.h>

#include "t_sms.h"


/*************************************************************************
**************************************************************************
*
* Test Case Handling
*
* Data types and arrays used to handle test cases.
*
**************************************************************************
**************************************************************************/

// Enum specifying the test framework being used to run the tests. This is used
// to enhance the debug information printed to the log.
typedef enum {
    SMS_TEST_CASE_FRAMEWORK_ID_STAND_ALONE,
    SMS_TEST_CASE_FRAMEWORK_ID_LTP,
    SMS_TEST_CASE_FRAMEWORK_ID_UNKNOWN
} SMS_TestCaseFrameworkId_t;

// Function prototype typedef used for all test harness test cases.  This means that each test case must be in
// the format:
// uint8_t MyTestCaseFunction(const char* TestCaseName_p, const SMS_TestCaseFrameworkId_t TestCaseFrameworkId);
typedef uint8_t(
    *SMS_TestCaseFuncPtr) (
    const char *TestCaseName_p,
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId);

// Structure used when constucting an array of test cases from data in test_cases.con.
typedef struct {
    SMS_TestCaseFuncPtr TestCaseFunction;
    const char *TestCaseName_p;
    const SMS_TestCaseFrameworkId_t TestCaseFrameworkId;
} SMS_TestCaseFncs_t;




// Function prototype declarations for test cases "auto-generated" from data in test_cases.con
#ifdef SMS_TEST_CASE_DECLARATION
#undef SMS_TEST_CASE_DECLARATION
#endif
#define SMS_TEST_CASE_DECLARATION(FncName) uint8_t FncName(const char* TestCaseName_p, const SMS_TestCaseFrameworkId_t TestCaseFrameworkId);
#include "test_cases.con"
#undef SMS_TEST_CASE_DECLARATION

#endif
