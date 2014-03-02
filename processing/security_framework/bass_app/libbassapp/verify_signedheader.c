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

#define PRODUCTION_MODE 0
#define ENGINEERING_MODE 1

bass_return_code verify_signedheader(const uint8_t *const hdr,
                                     const uint32_t hdr_length,
                                     enum payload_type_t pl_type,
                                     bass_signed_header_info_t *const hinfo)
{
    bass_return_code ret_code = BASS_RC_FAILURE;
    TEEC_Operation operation;
    uint32_t hashSize = 0;
    uint32_t eng_mode = PRODUCTION_MODE;

    if (NULL == hdr || NULL == hinfo) {
        printf("[%s]: NULL pointer error\n", __func__);
        goto function_exit;
    }

    memset(&operation, 0, sizeof(TEEC_Operation));

    operation.memRefs[0].buffer = (void *)hdr;
    operation.memRefs[0].size = hdr_length;
    operation.memRefs[0].flags = TEEC_MEM_INPUT;
    operation.flags |= TEEC_MEMREF_0_USED;

    operation.memRefs[1].buffer = &pl_type;
    operation.memRefs[1].size = (size_t)(sizeof(enum payload_type_t));
    operation.memRefs[1].flags |= TEEC_MEM_INPUT;
    operation.flags |= TEEC_MEMREF_1_USED;

    operation.memRefs[2].buffer = &eng_mode;
    operation.memRefs[2].size = sizeof(uint32_t);
    operation.memRefs[2].flags = TEEC_MEM_OUTPUT;
    operation.flags |= TEEC_MEMREF_2_USED;

    ret_code = teec_invoke_secworld(&operation, BASS_APP_VERIFY_SIGNED_HEADER2);

    if (eng_mode == ENGINEERING_MODE) {
        dprintf(INFO, "Running in ENGINEERING_MODE!\n");
        memset(hinfo, 0, sizeof(bass_signed_header_info_t));
        goto function_exit;
    }

    if (BASS_RC_SUCCESS != ret_code) {
        dprintf(ERROR, "verify signedheader error\n");
        goto function_exit;
    }

    /* Set the output parameters. */
    hinfo->pl_size = ((bass_app_signed_header_t *)hdr)->size_of_payload;
    hinfo->ht = ((bass_app_signed_header_t *)hdr)->hash_type;
    hinfo->plt = ((bass_app_signed_header_t *)hdr)->payload_type;
    hinfo->size_of_signed_header =
        ((bass_app_signed_header_t *)hdr)->size_of_signed_header;

    switch (hinfo->ht) {
    case BASS_APP_SHA1_HASH:
        hashSize = 20;
        break;
    case BASS_APP_SHA256_HASH:
        hashSize = 32;
        break;
    case BASS_APP_SHA384_HASH:
        hashSize = 48;
        break;
    case BASS_APP_SHA512_HASH:
        hashSize = 64;
        break;
    default:
        printf("[%s]: hinfo->ht unknown hash type\n", __func__);
        return BASS_RC_FAILURE;
    }

    memcpy(hinfo->ehash.value, hdr + sizeof(bass_app_signed_header_t),
           hashSize);

function_exit:
    return ret_code;
}


