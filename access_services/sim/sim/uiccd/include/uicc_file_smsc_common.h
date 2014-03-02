/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//control data for read SMSC transaction
typedef enum {
    UICCD_TRANS_SMSC_INIT,
    UICCD_TRANS_SMSC_GET_FORMAT,
    UICCD_TRANS_SMSC_READ_RECORD,
    UICCD_TRANS_SMSC_UPDATE_RECORD,
    UICCD_TRANS_SMSC_INVALID,
} uiccd_trans_smsc_state_t;

typedef struct {
    uiccd_trans_smsc_state_t trans_state;
    uint8_t                  client_record_id;
    uint8_t                  record_id;
    uint8_t                  num_records;
    uint8_t                  record_len;
    uint8_t                  man_backup;
    ste_sim_call_number_t  * smsc_p;
} uiccd_trans_data_smsc_t;

int uiccd_read_one_smsp_record(ste_sim_ctrl_block_t * ctrl_p, uint8_t record_id, uint8_t record_len, int file_id);

int uiccd_update_smsp_record(ste_sim_ctrl_block_t * ctrl_p, uint8_t record_id, uint8_t *data_p, int length);

int uiccd_get_smsp_format(ste_sim_ctrl_block_t * ctrl_p, int file_id);
