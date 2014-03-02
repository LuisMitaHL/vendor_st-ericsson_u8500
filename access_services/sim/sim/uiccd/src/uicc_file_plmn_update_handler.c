/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_plmn_common.h"
#include "uicc_file_paths.h"
#include "uicc_state_machine.h"

static int uiccd_encode_and_send_update_plmn_result(ste_sim_ctrl_block_t *ctrl_p,
                                                    sim_uicc_status_code_t uicc_status_code,
                                                    sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                    sim_uicc_status_word_t status_word) {
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len;

    buf_len = sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(uicc_status_code) + sizeof(uicc_status_code_fail_details);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : EF Record update response, memory alloc failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(sim_uicc_status_code_t));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(sim_uicc_status_code_fail_details_t));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f(SIM_LOGGING_E,
                   "uicc: uiccd_main_update_sim_file_record_response, assert failure");
    }

    assert(buf_len == (size_t)(p - buf_p));
    sim_send_generic(ctrl_p->fd, STE_UICC_RSP_UPDATE_PLMN, buf_p, buf_len, ctrl_p->client_tag);

    free(buf_p);

    return 0;
}

static int uiccd_update_plmn_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t    *ctrl_p = (ste_sim_ctrl_block_t*) (ste_msg->client_tag);
    uiccd_trans_data_plmn_t *data_p;
    ste_modem_t             *m;
    const char              *file_path_p;
    uint8_t                 *plmn_data_p;
    int                      plmn_data_len, plmn_data_offset, rv, file_id_offset, file_id;

    if (ctrl_p == NULL) {
        return 0;
    }
    data_p = (uiccd_trans_data_plmn_t*) (ctrl_p->transaction_data_p);
    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    file_id_offset = (data_p->file_id == SIM_FILE_ID_PLMN_WACT) ? SIM_PLMN_WITH_ACT_SIZE : SIM_PLMN_SIZE;
    file_id = (data_p->file_id == SIM_FILE_ID_PLMN_WACT) ? PLMN_WACT_FILE_ID : PLMN_SEL_FILE_ID;
    switch (data_p->trans_state) {
    case UICCD_TRANS_PLMN_GET_FORMAT: {
        uiccd_msg_sim_file_get_format_response_t* rsp_p;

        if (ste_msg->type != UICCD_MSG_SIM_FILE_GET_FORMAT_RSP) {
            catd_log_f(SIM_LOGGING_E,
                       "uicc : wrong msg type for transaction state UICCD_TRANS_PLMN_GET_FORMAT");
            return -1;
        }

        rsp_p = (uiccd_msg_sim_file_get_format_response_t*) ste_msg;

        if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
            //the transaction must be ended since there is error.
            catd_log_f(SIM_LOGGING_E,"uicc %s: get format failed",__func__);
            uiccd_encode_and_send_update_plmn_result(ctrl_p, rsp_p->uicc_status_code,
                    rsp_p->uicc_status_code_fail_details, rsp_p->status_word);
            free(data_p);
            free(ctrl_p);
            return 0;
        }
        data_p->file_size = rsp_p->file_size;

        if ( (data_p->index >= 0) && ((size_t)(data_p->index) > (data_p->file_size / file_id_offset)) ) {
            /* Index is out of range */
            catd_log_f(SIM_LOGGING_E, "uicc : index out-of-range");
            //the transaction must be ended since there is an error.
            sim_send_generic(ctrl_p->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, ctrl_p->client_tag);
            free(data_p->plmn_p);
            free(data_p);
            free(ctrl_p);
            return 0;
        }

        if ((NULL != data_p->plmn_p) && (data_p->index < 0)) {
            //update the file and put the data at first free location
            data_p->trans_state = UICCD_TRANS_PLMN_UPDATE_ELEMENT;

            rv = uiccd_send_read_binary_plmn_request(ctrl_p, data_p->file_size, file_id);
            if (rv < 0) {
                //the transaction must be ended since there is an error.
                sim_send_generic(ctrl_p->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, ctrl_p->client_tag);
                free(data_p->plmn_p);
                free(data_p);
                free(ctrl_p);
                return 0;
            }
            return -1;
        }
    }
        // Intentional fallthrough
    case UICCD_TRANS_PLMN_UPDATE_ELEMENT: {
        // update element at index
        uint8_t high_nibble;
        uint8_t low_nibble;
        uint8_t *plmn_tail_p;
        ste_sim_plmn_with_AcT_t *plmn_p = data_p->plmn_p;

        plmn_data_p = malloc(file_id_offset);
        if (!plmn_data_p) {
            catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
            sim_send_generic(ctrl_p->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, ctrl_p->client_tag);
            return -1;
        }

        if (NULL != plmn_p) {
            // find first free location
            if (ste_msg->type == UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                uiccd_msg_read_sim_file_binary_response_t * rsp_p;
                rsp_p = (uiccd_msg_read_sim_file_binary_response_t*) ste_msg;
                rv = uiccd_get_first_free_plmn_index(rsp_p, &(data_p->index), data_p->file_id);
                if (rv < 0) {
                    //the transaction must be ended since there is an error.
                    sim_send_generic(ctrl_p->fd,
                                     STE_UICC_RSP_UPDATE_PLMN,
                                     NULL,
                                     0,
                                     ctrl_p->client_tag);
                    free(plmn_data_p);
                    free(data_p->plmn_p);
                    free(data_p);
                    free(ctrl_p);
                    return 0;
                }

            }

            plmn_tail_p = plmn_data_p;
            plmn_data_offset = data_p->index * file_id_offset;
            plmn_data_len = file_id_offset;

            low_nibble = plmn_p->PLMN.MCC1;
            high_nibble = (plmn_p->PLMN.MCC2) << 4;
            *plmn_tail_p = high_nibble | low_nibble;
            plmn_tail_p++;

            low_nibble = plmn_p->PLMN.MCC3;
            high_nibble = (plmn_p->PLMN.MNC3) << 4;
            *plmn_tail_p = high_nibble | low_nibble;
            plmn_tail_p++;

            low_nibble = plmn_p->PLMN.MNC1;
            high_nibble = (plmn_p->PLMN.MNC2) << 4;
            *plmn_tail_p = high_nibble | low_nibble;
            plmn_tail_p++;
            if (data_p->file_id == SIM_FILE_ID_PLMN_WACT) {
                uiccd_set_access_technology(plmn_p->AcT, plmn_tail_p);
            }
            data_p->trans_state = UICCD_TRANS_PLMN_SEND_RESPONSE;
        } else {
            // remove element at index
            // set all bit to 1
            catd_log_f(SIM_LOGGING_V, "uicc %s: removing element at index %d", __func__, data_p->index);
            memset(plmn_data_p, ~0, file_id_offset);
            plmn_data_offset = data_p->index * file_id_offset;
            plmn_data_len = file_id_offset;
            data_p->trans_state = UICCD_TRANS_PLMN_SEND_RESPONSE;
        }

        m = catd_get_modem();
        if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC) {
            file_path_p = PLMN_GSM_PATH;
        } else {
            file_path_p = PLMN_USIM_PATH;
        }
        rv = ste_modem_file_update_binary(m,
                                          (uintptr_t) ctrl_p,
                                          uicc_get_app_id(),
                                          file_id,
                                          plmn_data_offset,
                                          plmn_data_len,
                                          file_path_p,
                                          plmn_data_p);
        if (rv != 0) {
            //the transaction must be ended since there is an error.
            sim_send_generic(ctrl_p->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, ctrl_p->client_tag);
            free(data_p->plmn_p);
            free(plmn_data_p);
            free(ctrl_p);
            return 0;
        }
        free(plmn_data_p);
        break;
    }

    case UICCD_TRANS_PLMN_SEND_RESPONSE: {
        uiccd_msg_update_sim_file_binary_response_t * rsp_p;
        catd_log_f(SIM_LOGGING_D, "uicc : Transaction state UICCD_TRANS_PLMN_SEND_RESPONSE");

        if (ste_msg->type != UICCD_MSG_UPDATE_SIM_FILE_BINARY_RSP) {
            catd_log_f(SIM_LOGGING_D,
                       "uicc : wrong msg type for transaction state UICCD_TRANS_PLMN_SEND_RESPONSE");
            return -1;
        }
        rsp_p = (uiccd_msg_update_sim_file_binary_response_t*) ste_msg;

        //check the update result and send the result back to client
        rv = uiccd_encode_and_send_update_plmn_result(ctrl_p, rsp_p->uicc_status_code,
                rsp_p->uicc_status_code_fail_details, rsp_p->status_word);
        if (rv < 0) {
            //the transaction must be ended since there is error.
            sim_send_generic(ctrl_p->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, ctrl_p->client_tag);
            free(data_p->plmn_p);
            free(data_p);
            free(ctrl_p);
            return 0;
        }
        //the transaction is done
        data_p->trans_state = UICCD_TRANS_PLMN_INIT;
        free(data_p->plmn_p);
        free(data_p);
        free(ctrl_p);
        return 0;
    }
    default: {
        catd_log_f(SIM_LOGGING_E,
                   "uicc : wrong transaction state for update plmn=%d",
                   data_p->trans_state);
        free(data_p->plmn_p);
        free(data_p);
        free(ctrl_p);
        return 0;
    }
    }
    return -1;
}

int uiccd_main_update_plmn(ste_msg_t * ste_msg)
{
    ste_modem_t *m;
    uiccd_msg_read_plmn_t *msg = (uiccd_msg_read_plmn_t *) ste_msg;
    ste_sim_ctrl_block_t *ctrl_p;
    uiccd_trans_data_plmn_t *data_p;
    int i;
    int file_id;
    int remove = 0;

    catd_log_f(SIM_LOGGING_I, "uicc : UPDATE PLMN message received from: %d ", msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        sim_send_generic(msg->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, msg->client_tag);
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_plmn_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        sim_send_generic(msg->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, msg->client_tag);
        return -1;
    }
    memset(data_p, 0, sizeof(uiccd_trans_data_plmn_t));

    // save the plmn from client
    data_p->plmn_p = malloc(sizeof(ste_sim_plmn_with_AcT_t));
    if (!data_p->plmn_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        sim_send_generic(msg->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, msg->client_tag);
        free(data_p);
        return -1;
    }
    memset(data_p->plmn_p, 0, sizeof(ste_sim_plmn_with_AcT_t));
    sim_decode_plmn_string(data_p->plmn_p, &data_p->index, &data_p->file_id, &remove, msg->data, msg->len);

    if (remove) {
        // element at index should be removed
        free(data_p->plmn_p);
        data_p->plmn_p = NULL;
    }
    data_p->trans_state = UICCD_TRANS_PLMN_GET_FORMAT;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag,
                                       msg->fd,
                                       msg->type,
                                       uiccd_update_plmn_transaction_handler,
                                       data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for update plmn transaction failed");
        sim_send_generic(msg->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, msg->client_tag);
        free(data_p->plmn_p);
        free(data_p);
        return -1;
    }

    file_id = (data_p->file_id == SIM_FILE_ID_PLMN_WACT) ? PLMN_WACT_FILE_ID : PLMN_SEL_FILE_ID;
    i = uiccd_get_plmn_format(ctrl_p, file_id);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_update_plmn failed.");
        sim_send_generic(msg->fd, STE_UICC_RSP_UPDATE_PLMN, NULL, 0, msg->client_tag);

        //transaction must be ended
        free(data_p->plmn_p);
        free(data_p);
        free(ctrl_p);
        return -1;
    }
    return 0;
}
