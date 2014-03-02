/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>

#include <bass_app.h>
#include <debug.h>
#include <teec_handler.h>
#include <tee_client_api.h>
#include <uuid.h>

bass_return_code get_product_config(struct tee_product_config *product_config)
{
    bass_return_code ret_code = BASS_RC_FAILURE;
    TEEC_Operation operation;

    if (NULL == product_config) {
        dprintf(ERROR, "NULL pointer error\n");
        goto function_exit;
    }

    memset((void *)(&operation), 0, sizeof(TEEC_Operation));

    operation.memRefs[0].buffer = product_config;
    operation.memRefs[0].size = sizeof(struct tee_product_config);
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

    operation.flags = TEEC_MEMREF_0_USED;

    ret_code = teec_invoke_secworld(&operation, BASS_APP_GET_PRODUCT_CONFIG);

function_exit:
    return ret_code;
}
