/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include <stdint.h>

#include <bass_app_test.h>
#include <debug.h>
#include <nw_to_tee_test_data.h>
#include <tee_client_api.h>
#include <tee_test_common.h>

uint32_t bass_app_sha_aes_test(void)
{
    TEEC_Result tee_result;
    TEEC_Context tee_context;
    TEEC_Session tee_session;
    TEEC_ErrorOrigin tee_errorOrigin;

    int retVal;
    SessionCommandInfo sci;
    enum TestTypes testTypeCntr;
    int i;

    memset(&sci, 0, sizeof(SessionCommandInfo));
    memset(&tee_session, 0, sizeof(TEEC_Session));

    /* Initialize */
    tee_result = TEEC_InitializeContext(NULL, &tee_context);

    if (tee_result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InitializeContext failed! result: %lu\n",
                (unsigned long)tee_result);
        goto out;
    }

    for (testTypeCntr = 0;
         testTypeCntr < NUMBER_OF_TEST_TYPES;
         testTypeCntr++) {

        /* Retrieve data */
        retVal = getSessionCommandInfo(testTypeCntr, &sci);

        /* TEEC_OpenSession */
        tee_session.uuid = sci.uuid;
        tee_session.ta = NULL;
        tee_session.id = 0;

        tee_result  = TEEC_OpenSession(&tee_context,
                                       &tee_session,
                                       &sci.uuid,
                                       0,
                                       NULL,
                                       NULL,
                                       &tee_errorOrigin);

        if (tee_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_OpenSession() failed! result: 0x%lx, origin: "
                    "0x%lx\n", (unsigned long)tee_result,
                    (unsigned long)tee_errorOrigin);

            /* Clean up */
            (void)cleanUpSessionCommandInfo(testTypeCntr, &sci);

            return NW_TO_TEE_TEST_FAILURE;
        }

        /* Run tests for the test type */
        for (i = 0; i < sci.numOperationInfos; i++) {

            /* Print info about the test */
            dprintf(INFO, "%s\n", sci.opInfo[i].info);

            tee_result  = TEEC_InvokeCommand(&tee_session,
                                             sci.commandID,
                                             &sci.opInfo[i].operation,
                                             &tee_errorOrigin);

            if (tee_result != TEEC_SUCCESS) {
                dprintf(ERROR, "TEEC_InvokeCommand() failed! result: %lu\n",
                        (unsigned long)tee_result);

                /* Clean up */
                (void)cleanUpSessionCommandInfo(testTypeCntr, &sci);

                return NW_TO_TEE_TEST_FAILURE;
            }

            /* Compare calculated result to the expected result */
            if (compareResult(testTypeCntr, &sci.opInfo[i]) != 0) {
                dprintf(ERROR, "Test failed, or error in comparison!\n");
            } else {
                dprintf(INFO, "Test success!\n");
            }
        }

        /* Clean up */
        retVal = cleanUpSessionCommandInfo(testTypeCntr, &sci);

        if (retVal != 0) {
            dprintf(ERROR, "cleanUpSessionCommandInfo() failed!\n");
            return retVal;
        }

        tee_result = TEEC_CloseSession(&tee_session);

        if (tee_result != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_CloseSession() failed! result: %lu\n",
                    (unsigned long)tee_result);

            return NW_TO_TEE_TEST_FAILURE;
        }
    }

    tee_result = TEEC_FinalizeContext(&tee_context);

    if (tee_result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext() failed! result: %lu\n",
                (unsigned long)tee_result);
    }

out:
    return tee_result;
}
