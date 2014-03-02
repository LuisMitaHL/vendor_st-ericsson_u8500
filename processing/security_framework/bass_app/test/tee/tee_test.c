/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>

#include <debug.h>
#include <tee_test.h>
#include <tee_test_common.h>
#include <tee_test_data.h>

TEEC_Result TEE_Test_Run_All(void)
{
    TEEC_Result         tee_result;
    TEEC_Context        tee_context;
    TEEC_Session        tee_session;
    TEEC_ErrorOrigin    tee_errorOrigin;

    int                 retVal;
    SessionCommandInfo  sci;
    enum TestTypes      testTypeCntr;
    int                 i;

    memset(&sci, 0, sizeof(SessionCommandInfo));
    memset(&tee_session, 0, sizeof(TEEC_Session));

    tee_result = TEEC_InitializeContext(NULL, &tee_context);

    if (tee_result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InitializeContext failed! result: %lu\n",
                (unsigned long)tee_result);
        goto out;
    }

    /*
     * Add verification of context here What should context contain? Is it
     * correctly initialized?
     */
    for (testTypeCntr = 0;
            testTypeCntr < NUMBER_OF_TEST_TYPES;
            testTypeCntr++) {

        /* Retrieve data. */
        retVal = getSessionCommandInfo(testTypeCntr, &sci);

        tee_result  = TEEC_OpenSession(&tee_context,
                                       &tee_session,
                                       &sci.uuid,
                                       0,
                                       NULL,
                                       NULL,
                                       &tee_errorOrigin);

        if (tee_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_OpenSession() failed! result: 0x%lx, origin: "
                    "%0xlx\n", (unsigned long)tee_result,
                    (unsigned long)tee_errorOrigin);

            /* Clean up. */
            (void)cleanUpSessionCommandInfo(testTypeCntr, &sci);

            return NW_TO_TEE_TEST_FAILURE;
        }

        /*
         * Add verification of open session here.
         * Is the session correctly opened?
         * What should be checked?
         * Context?
         * Session?
         * ErrorOrigin?
         */

        /* Run tests for the test type. */
        for (i = 0; i < sci.numOperationInfos; i++) {

            tee_result  = TEEC_InvokeCommand(&tee_session,
                                             sci.commandID,
                                             &sci.opInfo[i].operation,
                                             &tee_errorOrigin);

            if (tee_result != TEEC_SUCCESS) {
                dprintf(ERROR, "TEEC_InvokeCommand() failed! result: %lu\n",
                        (unsigned long)tee_result);

                /* Clean up. */
                (void)cleanUpSessionCommandInfo(testTypeCntr, &sci);

                return NW_TO_TEE_TEST_FAILURE;
            }

            /* Compare calculated result to the expected result. */
            if (compareResult(testTypeCntr, &sci.opInfo[i]) != 0) {
                dprintf(ERROR, "Test failed, or error in comparison!\n");
            } else {
                dprintf(INFO, "Test success!\n");
            }
        }

        /* Clean up. */
        retVal = cleanUpSessionCommandInfo(testTypeCntr, &sci);

        if (retVal != 0) {
            dprintf(ERROR, "cleanUpSessionCommandInfo() failed!\n");
            return retVal;
        }

        /* Close Session */
        tee_result = TEEC_CloseSession(&tee_session);

        if (tee_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_CloseSession() failed! Error: %lu\n",
                    (unsigned long)tee_result);

            return NW_TO_TEE_TEST_FAILURE;
        }

        /*
         * Add verification that Close Session worked What should session
         * contain here?! Null?
         */
    }

    tee_result = TEEC_FinalizeContext(&tee_context);

    if (tee_result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext() failed! Error: %lu\n",
                (unsigned long)tee_result);
    }

    /* Add verification of Finalize here What shold context contain?! NULL? */
out:
    return tee_result;
}
