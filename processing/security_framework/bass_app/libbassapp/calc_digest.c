/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdint.h>

#include <bass_app.h>
#include <teec_handler.h>
#include <tee_client_api.h>
#include <debug.h>
#include <uuid.h>

bass_return_code bass_calcdigest(const t_bass_hash_type ht,
                                 const uint8_t *data,
                                 const size_t data_length,
                                 bass_hash_t *const hash)
{
    bass_return_code ret_code = BASS_RC_FAILURE;
    TEEC_Operation operation;

    if (NULL == data || NULL == hash) {
        dprintf(ERROR, "NULL pointer error");
        goto function_exit;
    }

    operation.memRefs[0].buffer = (void *)(&ht);
    operation.memRefs[0].size = sizeof(t_bass_hash_type);
    operation.memRefs[0].flags = TEEC_MEM_INPUT;
    operation.flags = TEEC_MEMREF_0_USED;

    operation.memRefs[1].buffer = (void *)data;
    operation.memRefs[1].size = data_length;
    operation.memRefs[1].flags = TEEC_MEM_INPUT;
    operation.flags |= TEEC_MEMREF_1_USED;

    operation.memRefs[2].buffer = hash;
    operation.memRefs[2].size = BASS_HASH_LENGTH;
    operation.memRefs[2].flags = TEEC_MEM_OUTPUT;
    operation.flags |= TEEC_MEMREF_2_USED;

    ret_code = teec_invoke_secworld(&operation, BASS_APP_CALC_DIGEST);

function_exit:
    return ret_code;
}
