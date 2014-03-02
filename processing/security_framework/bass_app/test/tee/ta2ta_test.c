/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <debug.h>
#include <tee_client_api.h>
#include <uuid.h>

static const char sha256_in[] = "abc";
static const char sha256_out[] = "\xba\x78\x16\xbf\x8f\x01\xcf\xea"
                                 "\x41\x41\x40\xde\x5d\xae\x22\x23"
                                 "\xb0\x03\x61\xa3\x96\x17\x7a\x9c"
                                 "\xb4\x10\xff\x61\xf2\x00\x15\xad";


int main(int argc, char *argv[])
{
    uint8_t in[32];
    uint8_t out[32];
    uint8_t out2[32];
    uint32_t size1;
    uint32_t size2;
    TEEC_Session sess;
    bool sess_open = false;
    TEEC_Operation op;
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_ErrorOrigin error;
    TEEC_UUID ta2ta_uuid = TA2TA_UUID;
    int i;

    res = TEEC_InitializeContext(NULL, &ctx);

    if (res != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InitializeContext result (0x%x) != TEEC_SUCCESS\n",
                res);
        return 0;
    }

    dprintf(INFO, "Initialized context\n");

    res = TEEC_OpenSession(&ctx, &sess, &ta2ta_uuid, TEEC_LOGIN_PUBLIC,
                           NULL, NULL, &error);

    if (res != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_OpenSession result (0x%x) != TEEC_SUCCESS, "
                "ErrorOrigin 0x%x\n", res, error);
        goto cleanup_return;
    }

    sess_open = true;
    dprintf(INFO, "Session opened\n");

    /* SHA 256 */
    op.memRefs[0].buffer = (void *)sha256_in;;
    op.memRefs[0].size = 3;
    op.memRefs[0].flags = TEEC_MEM_INPUT;
    op.memRefs[1].buffer = out;
    memset(out2, 0, sizeof(out));
    op.memRefs[1].size = 32;
    op.memRefs[1].flags = TEEC_MEM_OUTPUT;
    op.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;

    res = TEEC_InvokeCommand(&sess, TA_TA2TA_CMD_SHA256, &op, &error);

    if (res != TEEC_SUCCESS) {
        dprintf(ERROR, "TEE Test - Invoke SHA256 TA2TA fail res 0x%x, error "
                "0x%x\n", res, error);
        goto cleanup_return;
    }

    dprintf(INFO, "TEE Test - Invoke SHA256 TA2TA ok\n");

    for (i = 0; i < 32; i++) {
        if (out[i] != (uint8_t)sha256_out[i]) {
            dprintf(ERROR, "Fail at index %d\n", i);
            goto cleanup_return;
        }
    }

    /* AES encrypt */
    op.memRefs[0].buffer = in;
    op.memRefs[0].size = 16;
    op.memRefs[0].flags = TEEC_MEM_INPUT;
    memset(in, 0, sizeof(in));
    in[0] = 22;
    in[1] = 17;
    in[15] = 60;
    op.memRefs[1].buffer = out;
    op.memRefs[1].size = 16;
    op.memRefs[1].flags = TEEC_MEM_OUTPUT;
    op.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;
    res = TEEC_InvokeCommand(&sess, TA_TA2TA_CMD_AES256ECB_ENC, &op, &error);

    if (res != TEEC_SUCCESS) {
        dprintf(ERROR, "TEE Test - Invoke 1 TA2TA fail\n");
        goto cleanup_return;
    }

    dprintf(INFO, "TEE Test - Invoke 1 TA2TA ok\n");

    /* AES decrypt */
    op.memRefs[0].buffer = out;
    op.memRefs[1].buffer = out2;
    res = TEEC_InvokeCommand(&sess, TA_TA2TA_CMD_AES256ECB_DEC, &op, &error);

    if (res != TEEC_SUCCESS) {
        dprintf(ERROR, "TEE Test - Invoke 2 TA2TA fail\n");
        goto cleanup_return;
    }

    dprintf(INFO, "TEE Test - Invoke 2 TA2TA ok\n");

    if (memcmp(in, out2, 16) != 0) {
        dprintf(ERROR, "TEE Test - TA2TA encrypt test failed\n");
        goto cleanup_return;
    }

    dprintf(INFO, "TEE Test - TA2TA encrypt test ok\n");

cleanup_return:

    if (sess_open) {
        res = TEEC_CloseSession(&sess);

        if (res != TEEC_SUCCESS) {
            dprintf(ERROR, "TEEC_CloseSession result (0x%x) != TEEC_SUCCESS\n",
                    res);
        }
    }

    res = TEEC_FinalizeContext(&ctx);

    if (res != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext result (0x%x) != TEEC_SUCCESS\n",
                res);
    }

    return 0;
}
