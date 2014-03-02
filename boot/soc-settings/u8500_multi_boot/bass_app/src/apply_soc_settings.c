/*-----------------------------------------------------------------------------
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 *---------------------------------------------------------------------------*/


#include <bass_app.h>
#include <bass_app_intern.h>
#include <tee_client_api.h>

/* The static UUID */
static TEEC_UUID ta_static_uuid = {
    0xBC765EDE,
    0x6724,
    0x11DF,
    {0x8E, 0x12, 0xEC, 0xDB, 0xDF, 0xD7, 0x20, 0x85} };

static void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    size_t m;

    for (m = 0; m < n; m++)
	    p[m] = c;
    return s;
}

static bass_return_code tee_invoke_command(uint32_t cmd, TEEC_Operation *op)
{
    bass_return_code rc = BASS_RC_ERROR_INTERNAL;
    TEEC_Session session;
    bool session_opened = false;
    TEEC_Result result;
    TEEC_Context context;
    bool context_inited = false;
    TEEC_ErrorOrigin errorOrigin;

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
        goto cleanup_return;
    }
    context_inited = true;

    result = TEEC_OpenSession(&context, &session, &ta_static_uuid,
			      TEEC_LOGIN_PUBLIC, NULL, NULL, &errorOrigin);

    if (result != TEEC_SUCCESS) {
        goto cleanup_return;
    }
    session_opened = true;

    result = TEEC_InvokeCommand(&session, cmd, op, &errorOrigin);

cleanup_return:

    if (session_opened) {
        (void)TEEC_CloseSession(&session);
    }

    if (context_inited) {
        (void)TEEC_FinalizeContext(&context);
    }

    if (result == TEEC_SUCCESS) {
        rc = BASS_RC_SUCCESS;
    }

    return rc;
}

bass_return_code bass_app_get_avs_parameters(uint8_t *avs_params)
{
    bass_return_code rc = BASS_RC_ERROR_INTERNAL;
    TEEC_Session session;
    bool session_opened = false;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_Context context;
    bool context_inited = false;
    TEEC_ErrorOrigin errorOrigin;
    uint8_t ret_val = 1; /* if it isn't updated, we'll still get an error */

    memset((void*)(&operation), 0, sizeof(TEEC_Operation));

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
	    goto cleanup_return;
    }
    context_inited = true;

    result = TEEC_OpenSession(&context, &session, &ta_static_uuid,
			      TEEC_LOGIN_PUBLIC, NULL, NULL, &errorOrigin);

    if (result != TEEC_SUCCESS) {
	    goto cleanup_return;
    }
    session_opened = true;

    /* ret_val */
    operation.flags = TEEC_MEMREF_0_USED;
    operation.memRefs[0].buffer = (void *)(&ret_val);
    operation.memRefs[0].size = sizeof(ret_val);
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

    operation.flags |= TEEC_MEMREF_1_USED;
    operation.memRefs[1].buffer = avs_params;
    operation.memRefs[1].size = BASS_APP_AVS_PARAMS_LEN;
    operation.memRefs[1].flags = TEEC_MEM_OUTPUT;

    result = TEEC_InvokeCommand(&session, BASS_APP_GET_AVS_PARAMETERS,
				&operation, &errorOrigin);

cleanup_return:

    if (session_opened) {
	    (void)TEEC_CloseSession(&session);
    }

    if (context_inited) {
	    (void)TEEC_FinalizeContext(&context);
    }

    if (result == TEEC_SUCCESS) {
	    if (ret_val != 0) {
		    rc = BASS_RC_FAILURE;
	    } else {
		    rc = BASS_RC_SUCCESS;
	    }
    }

    return rc;
}

bass_return_code bass_apply_soc_settings(const uint8_t *avs_data,
                                         const size_t avs_data_len,
                                         uint8_t *ddr_regs,
                                         size_t *ddr_regs_len)
{
    bass_return_code rc = BASS_RC_ERROR_INTERNAL;
    TEEC_Session session;
    bool session_opened = false;
    TEEC_Operation operation;
    TEEC_Result result;
    TEEC_Context context;
    bool context_inited = false;
    TEEC_ErrorOrigin errorOrigin;
    uint8_t ret_val = 1; /* if it isn't updated, we'll still get an error */

    memset((void*)(&operation), 0, sizeof(TEEC_Operation));

    result = TEEC_InitializeContext(NULL, &context);
    if (result != TEEC_SUCCESS) {
	    goto cleanup_return;
    }
    context_inited = true;

    result = TEEC_OpenSession(&context, &session, &ta_static_uuid,
			      TEEC_LOGIN_PUBLIC, NULL, NULL, &errorOrigin);

    if (result != TEEC_SUCCESS) {
	    goto cleanup_return;
    }
    session_opened = true;

    /* ret_val */
    operation.flags = TEEC_MEMREF_0_USED;
    operation.memRefs[0].buffer = (void *)(&ret_val);
    operation.memRefs[0].size = sizeof(ret_val);
    operation.memRefs[0].flags = TEEC_MEM_OUTPUT;

    /* skipping density */

    /* optional DDR registers output */
    if (ddr_regs != NULL) {
        if (ddr_regs_len == NULL) {
            goto cleanup_return;
        }
        operation.flags |= TEEC_MEMREF_2_USED;
        operation.memRefs[2].buffer = ddr_regs;
        operation.memRefs[2].size = *ddr_regs_len;
        operation.memRefs[2].flags = TEEC_MEM_OUTPUT;
    }

    /* optional AVS data */
    if (avs_data != NULL) {
        operation.flags |= TEEC_MEMREF_3_USED;
        operation.memRefs[3].buffer = (void *)avs_data;
        operation.memRefs[3].size = avs_data_len;
        operation.memRefs[3].flags = TEEC_MEM_INPUT;
    }

    result = TEEC_InvokeCommand(&session, BASS_APP_APPLY_SOC_SETTINGS,
				&operation, &errorOrigin);

    if (result == TEEC_SUCCESS && ret_val == 0 && ddr_regs_len != NULL) {
        *ddr_regs_len = operation.memRefs[2].size;
    }

cleanup_return:

    if (session_opened) {
	    (void)TEEC_CloseSession(&session);
    }

    if (context_inited) {
	    (void)TEEC_FinalizeContext(&context);
    }

    if (result == TEEC_SUCCESS) {
	    if (ret_val != 0) {
		    rc = BASS_RC_FAILURE;
	    } else {
		    rc = BASS_RC_SUCCESS;
	    }
    }

    return rc;
}


bass_return_code bass_get_avs_fuses(uint8_t *fuses,
                    const size_t fuses_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)fuses;
    op.memRefs[0].size = fuses_len;
    op.memRefs[0].flags = TEEC_MEM_OUTPUT;

    return tee_invoke_command(BASS_APP_GET_AVS_FUSES, &op);
}

bass_return_code bass_prcmu_apply_avs_settings(const uint8_t *avs_data,
                     const size_t avs_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)avs_data;
    op.memRefs[0].size = avs_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return tee_invoke_command(BASS_APP_PRCMU_APPLY_AVS_SETTINGS, &op);
}

bass_return_code bass_prcmu_set_ap_exec(void)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    return tee_invoke_command(BASS_APP_PRCMU_SET_AP_EXEC, &op);
}

bass_return_code bass_prcmu_ddr_pre_init(
                    const uint8_t *ddr_data, const size_t ddr_data_len,
                    const uint8_t *ddr_read_regs, size_t ddr_read_regs_len,
                    uint8_t *ddr_regs, const size_t ddr_regs_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)ddr_data;
    op.memRefs[0].size = ddr_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    op.flags |= TEEC_MEMREF_1_USED;
    op.memRefs[1].buffer = (void *)ddr_read_regs;
    op.memRefs[1].size = ddr_read_regs_len;
    op.memRefs[1].flags = TEEC_MEM_INPUT;

    op.flags |= TEEC_MEMREF_2_USED;
    op.memRefs[2].buffer = (void *)ddr_regs;
    op.memRefs[2].size = ddr_regs_len;
    op.memRefs[2].flags = TEEC_MEM_OUTPUT;

    return tee_invoke_command(BASS_APP_PRCMU_DDR_PRE_INIT, &op);
}

bass_return_code bass_prcmu_ddr_init(const uint8_t *ddr_data,
                    const size_t ddr_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)ddr_data;
    op.memRefs[0].size = ddr_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return tee_invoke_command(BASS_APP_PRCMU_DDR_INIT, &op);
}

bass_return_code bass_prcmu_set_ddr_speed(const uint8_t *speed_data,
                    const size_t speed_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)speed_data;
    op.memRefs[0].size = speed_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return tee_invoke_command(BASS_APP_PRCMU_SET_DDR_SPEED, &op);
}

bass_return_code bass_prcmu_abb_init(const uint8_t *abb_data,
                    const size_t abb_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)abb_data;
    op.memRefs[0].size = abb_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return tee_invoke_command(BASS_APP_PRCMU_ABB_INIT, &op);
}

bass_return_code bass_set_l2cc_filter_registers(const uint32_t start_reg,
                    const uint32_t end_reg)
{
    uint32_t regs[2] = { start_reg, end_reg };
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    /* ret_val */
    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)regs;
    op.memRefs[0].size = sizeof(regs);
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return tee_invoke_command(BASS_APP_SET_L2CC_FILTER_REGISTERS, &op);
}
