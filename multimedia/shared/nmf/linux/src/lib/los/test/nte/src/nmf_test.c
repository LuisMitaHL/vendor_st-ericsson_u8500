/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/inc/cm.h>
#include <cm/inc/cm_macros.h>
#include <share/inc/macros.h>

#include <los/api/los_api.h>

// string related
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <test/api/test.h>
#include <test/nte/inc/th_output.h>

/****************************** CONFIG CONSTANTS ***************************/
#define STRING_MAX_LENGTH 255

static const char TAG_TestStart         =  'S'; /*!< tag for test start */
static const char TAG_TestDescription   =  'D'; /*!< tag for test description */
static const char TAG_TestProperty      =  'P'; /*!< tag fo test property */
static const char TAG_StepDescription   =  'N'; /*!< tag for test step description */
static const char TAG_StepResult        =  'R'; /*!< tag for test step status */
static const char TAG_TestStepComment   =  'C'; /*!< tag for test step information */
static const char TAG_TestEnd           =  'E'; /*!< tag for end of test */

static const char TAG_SuccessStep[]     =  "PASSED";  /*!< string for test step success */
static const char TAG_FailedStep[]      =  "FAILED";  /*!< string for test step failure */
static const char TAG_SkippedStep[]     =  "SKIPPED"; /*!< string for test step skipped */

static const char SUFFIX_OutputFile[]   =  "_tst.out" ; /*!< suffix for output report file */

/***************** INTERNAL DECLARATIONS AND VARIABLES *********************/
static int testFailures = 0;
static int testSuccesses = 0;
static int testSkipped = 0;
static int testFailuresGlobal = 0;
static int testSuccessesGlobal = 0;
static int testSkippedGlobal = 0;
static unsigned long long stepStartTime = 0;
static int stepId = 0;

/**************** MMTE TEST API IMPLEMENTATION FUNCTIONS *******************/
EXPORT_SHARED void mmte_testStart(const char* testName, const char* testDesc, const char* logDir)
{
    char tmpMsg[STRING_MAX_LENGTH];
    if ((logDir != 0) && (strlen(logDir) != 0)) {
	    snprintf(tmpMsg, STRING_MAX_LENGTH, "%s/%s%s", logDir, testName, SUFFIX_OutputFile);
    } else {
	    snprintf(tmpMsg, STRING_MAX_LENGTH, "%s%s", testName, SUFFIX_OutputFile);
    }
    th_fopen(tmpMsg);

#ifndef LINUX
    {
        /* Yes we really know what we are doing .... */
        /* Since arm libc cache file handle and so don't free allocated memory on a fclose */
        /* we do this dummy open so there is one filehandle in the cache. it allow us not to break */
        /* memory leak check done latter on */
        (void)fopen("_fsdgsd_sdfg_sdg_","r");
    }
 #endif

    snprintf(tmpMsg, STRING_MAX_LENGTH, "%c:%x:%s", TAG_TestStart, LOS_GetCurrentId(), testName);
    th_fprintf(tmpMsg);

    snprintf(tmpMsg, STRING_MAX_LENGTH, "%c:%x:%s", TAG_TestDescription, LOS_GetCurrentId(), testDesc);
    th_fprintf(tmpMsg);

    testFailures = testSuccesses = testSkipped = 0;
    stepId = 0;
    stepStartTime = 0;
}

EXPORT_SHARED void mmte_testNext(const char* stepDescription)
{
    char stringMessage[STRING_MAX_LENGTH];
    stepId++;
    snprintf(stringMessage, STRING_MAX_LENGTH, "%c:%x:%d:%s", TAG_StepDescription, LOS_GetCurrentId(), stepId, stepDescription);
    th_fprintf(stringMessage);
    stepStartTime = LOS_getSystemTime();
}

EXPORT_SHARED void mmte_testResult(t_test_status status)
{
    char stringMessage[STRING_MAX_LENGTH];
    unsigned long long stepStopTime = LOS_getSystemTime();
    const char* statusMsg;
    char extra;

    if (status == TEST_PASSED) {
        statusMsg = TAG_SuccessStep;
        testSuccesses++;
        extra = ' ';
    } else if (status == TEST_SKIPPED) {
        statusMsg = TAG_SkippedStep;
        testSkipped++;
        extra = ' ';
    } else {
        statusMsg = TAG_FailedStep;
        testFailures++;
        extra = '\n';
    }

    snprintf(stringMessage, STRING_MAX_LENGTH, "%c:%x:%d:%s:%lld%c", TAG_StepResult, LOS_GetCurrentId(), stepId, statusMsg, stepStopTime - stepStartTime, extra);
    th_fprintf(stringMessage);
}

EXPORT_SHARED void mmte_testEnd(void)
{
    char stringMessage[STRING_MAX_LENGTH];
    const char* status;
    if (testFailures == 0)
        status = TAG_SuccessStep;
    else
        status = TAG_FailedStep;
    snprintf(stringMessage, STRING_MAX_LENGTH, "%c:%x:%s:%d/%d", TAG_TestEnd, LOS_GetCurrentId(),
            status, testSuccesses, testSuccesses + testFailures + testSkipped);
    testSuccessesGlobal += testSuccesses;
    testFailuresGlobal += testFailures;
    testSkippedGlobal += testSkipped;
    th_fprintf(stringMessage);
    th_fclose();
}

EXPORT_SHARED void mmte_testGlobalEnd(int* _testSuccessesGlobal, int *_testFailuresGlobal, int *_testSkippedGlobal)
{
    *_testSuccessesGlobal = testSuccessesGlobal;
    *_testFailuresGlobal = testFailuresGlobal;
    *_testSkippedGlobal = testSkippedGlobal;
}

EXPORT_SHARED void mmte_testComment(const char* myMessage) {
    char stringMessage[STRING_MAX_LENGTH];
    snprintf(stringMessage, STRING_MAX_LENGTH, "%c:%x:%s", TAG_TestStepComment, LOS_GetCurrentId(), myMessage);
    th_fprintf(stringMessage);
}

EXPORT_SHARED void mmte_testProperty(const char* key, const char* value)
{
    char stringMessage[STRING_MAX_LENGTH];
    snprintf(stringMessage, STRING_MAX_LENGTH, "%c:%x:%s=%s", TAG_TestProperty, LOS_GetCurrentId(), key, value);
    th_fprintf(stringMessage);
}

EXPORT_SHARED void mmte_testConfig(t_test_config cmd, t_sint8 value)
{
    switch(cmd) {
    case TEST_CONFIG_TRACE_LEVEL:
        th_config(value);
        break;
    default:
        LOS_Log("Unknown config command\n"); //FIXME, assert in this case?
        break;
    }
}

