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
#include <tee_client_api.h>
#include <uuid.h>

#define PIN_LENGTH 16

static int smcl_common_invoke_secwld(uint8_t cmd, TEEC_Operation *op)
{
    TEEC_Context ctx = 0;
    TEEC_ErrorOrigin org = TEEC_ORIGIN_API;
    TEEC_Result result = TEEC_ERROR_GENERIC;
    TEEC_Session session;
    TEEC_UUID uuid = SMCL_TA_UUID;
    uint32_t ret = SMCL_GENERAL_FAILURE;

    memset(&session, 0, sizeof(TEEC_Session));

    result = TEEC_InitializeContext(NULL, &ctx);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InitializeContext failed\n");
        goto out;
    }

    result = TEEC_OpenSession(&ctx, &session, &uuid, TEEC_LOGIN_PUBLIC, NULL,
                              NULL, &org);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_OpenSession failed\n");
        goto out_finalize;
    }

    op->memRefs[3].buffer = (void *)(&ret);
    op->memRefs[3].size = sizeof(uint32_t);
    op->memRefs[3].flags = TEEC_MEM_OUTPUT;
    op->flags |= TEEC_MEMREF_3_USED;

    result = TEEC_InvokeCommand(&session, cmd, op, &org);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_InvokeCommand failed\n");
    }

    result = TEEC_CloseSession(&session);
    if (result != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_CloseSession failed\n");
    }

out_finalize:
    if (TEEC_FinalizeContext(&ctx) != TEEC_SUCCESS) {
        dprintf(ERROR, "TEEC_FinalizeContext failed\n");
        /*
         * Special handling since there is a risk of overwriting old failing
         * result code from previous calls to tee.
         */
        if (result == TEEC_SUCCESS) {
            result = TEEC_ERROR_GENERIC;
        }
    }
out:
    if (result != TEEC_SUCCESS) {
        return SMCL_TEE_FAILURE;
    }

    return ret;
}

int smcl_unlock_dbg_modem_mem()
{
    TEEC_Operation op;
    memset(&op, 0, sizeof(TEEC_Operation));

    return smcl_common_invoke_secwld(SMCL_MODEM_MEM_UNLOCK, &op);
}

int smcl_reset_unlock_modem_mem()
{
    TEEC_Operation op;
    memset(&op, 0, sizeof(TEEC_Operation));

    return smcl_common_invoke_secwld(SMCL_MODEM_RESET_MEM_UNLOCK, &op);
}

int smcl_restart_modem(uint32_t ab8500cutid)
{
    TEEC_Operation op;
    memset(&op, 0, sizeof(TEEC_Operation));

    op.memRefs[0].buffer = (void *)(&ab8500cutid);
    op.memRefs[0].size = sizeof(uint32_t);
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    op.flags = TEEC_MEMREF_0_USED;

    return smcl_common_invoke_secwld(SMCL_MODEM_RESTART_MEM_LOCK, &op);
}

int smcl_get_modem_memory(uint8_t mem_id, uint8_t *buffer, uint32_t size)
{
    TEEC_Operation op;

    if (buffer == NULL) {
        return SMCL_GENERAL_FAILURE;
    }

    if ((mem_id == TCM_L1) && (size > TCM_L1_MAX_SIZE)) {
        return SMCL_GENERAL_FAILURE;
    }

    if ((mem_id == TCM_L2) && (size > TCM_L2_MAX_SIZE)) {
        return SMCL_GENERAL_FAILURE;
    }

    memset(&op, 0, sizeof(TEEC_Operation));

    op.memRefs[0].buffer = &mem_id;
    op.memRefs[0].size = sizeof(uint8_t);
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    op.memRefs[1].buffer = buffer;
    op.memRefs[1].size = size;
    op.memRefs[1].flags = TEEC_MEM_OUTPUT;

    op.flags = (TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED);

    return smcl_common_invoke_secwld(SMCL_MODEM_GET_MEMORY, &op);
}

uint32_t smcl_encrypt_pin(uint8_t *pin,
                          uint32_t pin_length,
                          uint32_t encrypt_dir)
{
    TEEC_Operation op;
    uint32_t ret = SMCL_GENERAL_FAILURE;

    if (pin_length != PIN_LENGTH) {
        dprintf(ERROR, "Pin length incorrect (!=%d bytes)\n", PIN_LENGTH);
        ret = SMCL_GENERAL_FAILURE;
        goto out;
    }

    if (!pin) {
        dprintf(ERROR, "Buffer for the pin is a NULL pointer\n");
        ret = SMCL_GENERAL_FAILURE;
        goto out;
    }

    if (encrypt_dir != SMCL_DIR_ENCRYPT &&
        encrypt_dir != SMCL_DIR_DECRYPT) {
        dprintf(ERROR, "Incorrect encrypt_dir, only SMCL_DIR_ENCRYPT and "
                "SMCL_DIR_DECRYPT is allowed\n");
        ret = SMCL_GENERAL_FAILURE;
        goto out;
    }

    memset(&op, 0, sizeof(TEEC_Operation));

    op.memRefs[0].buffer = (void *)pin;
    op.memRefs[0].size = PIN_LENGTH;
    op.memRefs[0].flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

    op.memRefs[1].buffer = (void *)(&encrypt_dir);
    op.memRefs[1].size = sizeof(uint32_t);
    op.memRefs[1].flags = TEEC_MEM_INPUT;

    op.flags = TEEC_MEMREF_0_USED | TEEC_MEMREF_1_USED;

    ret = smcl_common_invoke_secwld(SMCL_ENCRYPT_PIN, &op);
out:
    return ret;
}
