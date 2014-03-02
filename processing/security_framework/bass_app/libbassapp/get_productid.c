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

bass_return_code get_productid(uint32_t *const product_id)
{
    bass_return_code ret_code = BASS_RC_FAILURE;
    TEEC_Operation operation;

    if (NULL == product_id) {
        dprintf(ERROR, "NULL == product_id_buffer\n");
        goto function_exit;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));

    *product_id = 0;

    operation.memRefs[0].buffer = product_id;
    operation.memRefs[0].size = sizeof(product_id);
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

    operation.flags = TEEC_MEMREF_0_USED;

    ret_code = teec_invoke_secworld(&operation, BASS_APP_GET_PRODUCT_ID);

function_exit:
    return ret_code;
}
