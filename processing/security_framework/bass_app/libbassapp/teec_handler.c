/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdint.h>

#include <bass_app.h>
#include <debug.h>
#include <teec_handler.h>
#include <tee_client_api.h>
#include <uuid.h>

bass_return_code teec_invoke_secworld(TEEC_Operation *const operation,
                                      const uint32_t command_id)
{
    bass_return_code ret_code = BASS_RC_FAILURE;
    TEEC_Result result = TEEC_ERROR_GENERIC;
    TEEC_Session session;
    TEEC_Context context;
    TEEC_ErrorOrigin errorOrigin;
    TEEC_UUID ta_static_uuid = STATIC_TA_UUID;

    result = TEEC_InitializeContext(NULL, &context);

    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "TEEC_InitializeContext error\n");
        goto function_exit;
    }

    result = TEEC_OpenSession(&context, &session, &ta_static_uuid,
                              TEEC_LOGIN_PUBLIC, NULL, NULL, &errorOrigin);

    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "TEEC_OpenSession error\n");
        goto finalize;
    }

    result = TEEC_InvokeCommand(&session, command_id,
                                operation, &errorOrigin);

    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "TEEC_InvokeCommand error\n");
        goto close;
    }

close:
    if (TEEC_CloseSession(&session) != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_CloseSession error\n");
    }

finalize:
    if (TEEC_FinalizeContext(&context) != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext error\n");
    }

function_exit:

    if (TEEC_SUCCESS != result) {
        dprintf(ERROR, "result = 0x%X, errorOrigin = %d\n", result,
                errorOrigin);

        switch (errorOrigin) {
        case TEEC_ORIGIN_API:
            ret_code = BASS_RC_ERROR_TEE_API;
            break;
        case TEEC_ORIGIN_COMMS:
            ret_code = BASS_RC_ERROR_TEE_COMMS;
            break;
        case TEEC_ORIGIN_TEE:
            ret_code = BASS_RC_ERROR_TEE_CORE;
            break;
        case TEEC_ORIGIN_TRUSTED_APP:
            ret_code = BASS_RC_ERROR_TEE_TRUSTED_APP;
            break;
        default:
            ret_code = BASS_RC_ERROR_UNKNOWN;
            break;

        }
    } else {
        ret_code = BASS_RC_SUCCESS;
    }

    return ret_code;
}
