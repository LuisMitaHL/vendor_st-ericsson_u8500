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

bass_return_code bass_imad_store(const uint8_t *data, size_t data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)data;
    op.memRefs[0].size = data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_IMAD_STORE);
}


bass_return_code bass_imad_release(void)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    return teec_invoke_secworld(&op, BASS_APP_IMAD_RELEASE);
}

bass_return_code bass_imad_exec(const struct bass_imad_entry *entries,
            size_t num_entries, size_t *failed_entry)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)entries;
    op.memRefs[0].size = sizeof(struct bass_imad_entry) * num_entries;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    op.flags |= TEEC_MEMREF_1_USED;
    op.memRefs[1].buffer = (void *)failed_entry;
    op.memRefs[1].size = sizeof(size_t);
    op.memRefs[1].flags = TEEC_MEM_OUTPUT;

    return teec_invoke_secworld(&op, BASS_APP_IMAD_EXEC);
}


