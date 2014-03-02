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

bass_return_code bass_handle_bootp_magic(bass_handle_bootp_magic_t *bootp_data)
{
    TEEC_Operation operation;
    bass_return_code bass_result;

    memset(&operation, 0, sizeof(TEEC_Operation));

    /* We have only input, all three */
    operation.flags = TEEC_MEMREF_0_USED |
                      TEEC_MEMREF_1_USED |
                      TEEC_MEMREF_2_USED;
    /* Settings for input */
    operation.memRefs[0].buffer = &bootp_data->bootp_locked;
    operation.memRefs[0].size = sizeof(uint32_t);
    operation.memRefs[0].flags = TEEC_MEM_INPUT;

    operation.memRefs[1].buffer = bootp_data->eng_inbuffer_p;
    operation.memRefs[1].size = bootp_data->eng_inbuffer_size;
    operation.memRefs[1].flags = TEEC_MEM_INPUT;

    operation.memRefs[2].buffer = bootp_data->sec_inbuffer_p;
    operation.memRefs[2].size = bootp_data->sec_inbuffer_size;
    operation.memRefs[2].flags = TEEC_MEM_INPUT;

    bass_result = teec_invoke_secworld(&operation,
                                       BASS_APP_HANDLE_BOOT_AREA_DATA);

    /* If the result was a failure we need to report it */
    if (BASS_RC_SUCCESS != bass_result) {
        dprintf(ERROR, "Handling boot-partition magic words failed\n");
    }

    return bass_result;
}
