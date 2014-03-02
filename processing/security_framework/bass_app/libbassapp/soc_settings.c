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

bass_return_code bass_get_avs_fuses(uint8_t *fuses,
                                    const size_t fuses_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)fuses;
    op.memRefs[0].size = fuses_len;
    op.memRefs[0].flags = TEEC_MEM_OUTPUT;

    return teec_invoke_secworld(&op, BASS_APP_GET_AVS_FUSES);
}

bass_return_code bass_prcmu_apply_avs_settings(const uint8_t *avs_data,
                                               const size_t avs_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)avs_data;
    op.memRefs[0].size = avs_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_APPLY_AVS_SETTINGS);
}

bass_return_code bass_prcmu_set_ap_exec(void)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_SET_AP_EXEC);
}

bass_return_code bass_prcmu_ddr_pre_init(
                    const uint8_t *ddr_data, const size_t ddr_data_len,
                    const uint8_t *ddr_read_regs, size_t ddr_read_regs_len,
                    uint8_t *ddr_regs, const size_t ddr_regs_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

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

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_DDR_PRE_INIT);
}

bass_return_code bass_prcmu_ddr_init(const uint8_t *ddr_data,
                                     const size_t ddr_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)ddr_data;
    op.memRefs[0].size = ddr_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_DDR_INIT);
}

bass_return_code bass_prcmu_set_ddr_speed(const uint8_t *speed_data,
                                          const size_t speed_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)speed_data;
    op.memRefs[0].size = speed_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_SET_DDR_SPEED);
}

bass_return_code bass_prcmu_set_memory_speed(const uint8_t *speed_data,
                                             const size_t speed_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)speed_data;
    op.memRefs[0].size = speed_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_SET_MEMORY_SPEED);
}

bass_return_code bass_prcmu_abb_init(const uint8_t *abb_data,
                                     const size_t abb_data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)abb_data;
    op.memRefs[0].size = abb_data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_ABB_INIT);
}

bass_return_code bass_set_l2cc_filter_registers(const uint32_t start_reg,
                                                const uint32_t end_reg)
{
    uint32_t regs[2] = { start_reg, end_reg };
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)regs;
    op.memRefs[0].size = sizeof(regs);
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_SET_L2CC_FILTER_REGISTERS);
}

bass_return_code bass_prcmu_set_thermal_sensors(const uint8_t *data,
                    const size_t data_len)
{
    TEEC_Operation op;

    memset(&op, 0, sizeof(op));

    op.flags = TEEC_MEMREF_0_USED;
    op.memRefs[0].buffer = (void *)data;
    op.memRefs[0].size = data_len;
    op.memRefs[0].flags = TEEC_MEM_INPUT;

    return teec_invoke_secworld(&op, BASS_APP_PRCMU_SET_THERMAL_SENSORS);
}
