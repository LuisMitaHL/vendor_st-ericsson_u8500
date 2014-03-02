/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include <stdint.h>

#include <bass_app.h>
#include <debug.h>
#include <teec_handler.h>
#include <tee_client_api.h>
#include <uuid.h>

bass_return_code get_ta_key_hash(bass_hash_t *ta_key_hash, uint32_t *key_size)
{
    bass_return_code ret_code = BASS_RC_FAILURE;
    TEEC_Operation operation;

    if (NULL == ta_key_hash || NULL == key_size) {
        dprintf(ERROR, "Argument error.\n");
        goto function_exit;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));
    memset(ta_key_hash->value, 0, sizeof(ta_key_hash->value));

    operation.memRefs[0].buffer = ta_key_hash->value;
    operation.memRefs[0].size = SHA256_HASH_SIZE;
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

    operation.flags = TEEC_MEMREF_0_USED;

    *key_size = (uint32_t)SHA256_HASH_SIZE;

    ret_code = teec_invoke_secworld(&operation, BASS_APP_GET_TA_KEY_HASH);

function_exit:

    return ret_code;
}
