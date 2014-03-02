/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include "sim_internal.h"
#include "sim.h"
#include "uiccd_msg.h"

// control data for PLMN transaction
typedef enum {
    UICCD_TRANS_PLMN_INIT,
    UICCD_TRANS_PLMN_GET_FORMAT,
    UICCD_TRANS_PLMN_READ_BINARY,
    UICCD_TRANS_PLMN_UPDATE_ELEMENT,
    UICCD_TRANS_PLMN_SEND_RESPONSE,
    UICCD_TRANS_PLMN_INVALID,
} uiccd_trans_plmn_state_t;

typedef struct {
    uiccd_trans_plmn_state_t    trans_state;
    size_t                      file_size;
    int                         index;
    ste_sim_plmn_with_AcT_t    *plmn_p;
    ste_uicc_sim_plmn_file_id_t file_id;
} uiccd_trans_data_plmn_t;

int uiccd_get_plmn_format(ste_sim_ctrl_block_t * ctrl_p, const int file_id);
int uiccd_send_read_binary_plmn_request(ste_sim_ctrl_block_t *ctrl_p, size_t file_size, const int file_id);
boolean uiccd_valid_plmn(const ste_uicc_sim_plmn_t * const i_p);
int uiccd_get_first_free_plmn_index(uiccd_msg_read_sim_file_binary_response_t *read_response_p,
                                    int *index,
                                    ste_uicc_sim_plmn_file_id_t file_id);
void uiccd_set_access_technology(const ste_uicc_sim_plmn_AccessTechnology_t access_tech,
                                 uint8_t * const access_tech_bytes_p);
