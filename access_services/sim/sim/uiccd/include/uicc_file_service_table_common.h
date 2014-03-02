/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//control data for service table transactions
typedef enum {
    UICCD_TRANS_SERVICE_TABLE_INIT,
    UICCD_TRANS_SERVICE_TABLE_PIN_VERIFY,
    UICCD_TRANS_SERVICE_TABLE_READ_BINARY,
    UICCD_TRANS_SERVICE_TABLE_UPDATE_BINARY,
    UICCD_TRANS_SERVICE_TABLE_GET_FILE_INFORMATION,
    UICCD_TRANS_SERVICE_TABLE_GET_FORMAT,
    UICCD_TRANS_SERVICE_TABLE_APPL_APDU_SEND,
    UICCD_TRANS_SERVICE_TABLE_INVALID
} uiccd_trans_service_table_state_t;

typedef struct {
    uiccd_trans_service_table_state_t trans_state;
    sim_uicc_service_type_t           service_type;
    uint8_t                           enable_service;
} uiccd_trans_data_service_table_t;

int uiccd_read_service_table_service_status(ste_sim_ctrl_block_t * ctrl_p, sim_uicc_service_type_t service_type);
int uiccd_get_ef_adn_file_information(ste_sim_ctrl_block_t * ctrl_p);
