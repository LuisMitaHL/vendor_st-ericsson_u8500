/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_paths.h"
#include "uicc_state_machine.h"

int uiccd_read_one_smsp_record(ste_sim_ctrl_block_t * ctrl_p, uint8_t record_id, uint8_t record_len, int file_id)
{
    int                 i;
    ste_modem_t       * m;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }

    if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC) {
        file_path = SMSP_GSM_PATH;
    }
    else {
        file_path = SMSP_USIM_PATH;
    }

    i = ste_modem_file_read_record(m, (uintptr_t)ctrl_p, uicc_get_app_id(),
                                   file_id, record_id, record_len, file_path);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "ste_modem_file_read_record failed.");
        return -1;
    }
    return i;
}

int uiccd_update_smsp_record(ste_sim_ctrl_block_t * ctrl_p, uint8_t record_id, uint8_t *data_p, int length)
{
    int                 i;
    ste_modem_t       * m;
    int                 file_id = SMSP_FILE_ID;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }
    if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC) {
        file_path = SMSP_GSM_PATH;
    }
    else {
        file_path = SMSP_USIM_PATH;
    }

    i = ste_modem_file_update_record(m, (uintptr_t)ctrl_p, uicc_get_app_id(),
                                     file_id, record_id, length,
                                     file_path, data_p);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "ste_modem_file_update_record failed.");
        return -1;
    }
    return i;
}

int uiccd_get_smsp_format(ste_sim_ctrl_block_t * ctrl_p, int file_id)
{
    int                 i;
    ste_modem_t       * m;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }
    if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC) {
        file_path = SMSP_GSM_PATH;
    }
    else {
        file_path = SMSP_USIM_PATH;
    }
    i = ste_modem_sim_file_get_format(m, (uintptr_t)ctrl_p,
                                      uicc_get_app_id(),
                                      file_id,
                                      file_path);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "ste_modem_sim_file_get_format failed.");
        return -1;
    }
    return i;
}
