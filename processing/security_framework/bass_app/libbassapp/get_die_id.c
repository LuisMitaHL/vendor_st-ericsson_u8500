/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include <stdint.h>

#include <bass_app.h>
#include <tee_client_api.h>
#include <teec_handler.h>
#include <uuid.h>

bass_return_code bass_get_die_id(uint8_t *data, const size_t data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)data;
    op.memRefs[0].size = data_len;
    op.memRefs[0].flags = TEEC_MEM_OUTPUT;

    return teec_invoke_secworld(&op, BASS_APP_GET_DIE_ID);
}

