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

static int uiccd_encode_and_send_read_plmn_result(ste_sim_ctrl_block_t * ctrl_p,
                                                  sim_uicc_status_code_t uicc_status_code,
                                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                  sim_uicc_status_word_t status_word,
                                                  uiccd_msg_read_sim_file_binary_response_t * rsp_p,
                                                  ste_uicc_sim_plmn_file_id_t file_id) {
    char *buf_p = NULL;
    char *p = NULL;
    size_t buf_len;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(file_id);
    if (rsp_p) {
        buf_len += sizeof(rsp_p->len);
        buf_len += sizeof(char) * (rsp_p->len);
    }
    buf_p = malloc(buf_len);
    if (NULL == buf_p) {
        return -1;
    }
    p = buf_p;
    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
    p = sim_enc(p, &file_id, sizeof(file_id));
    if (rsp_p) {
        p = sim_enc(p, &rsp_p->len, sizeof(rsp_p->len));
        p = sim_enc(p, rsp_p->data, sizeof(*(rsp_p->data)) * (rsp_p->len));
    }
    if (buf_len != (size_t)(p - buf_p)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_read_sim_file_binary_response, assert failure");
    }

    assert(buf_len == (size_t)(p - buf_p));
    sim_send_generic(ctrl_p->fd, STE_UICC_RSP_READ_PLMN, buf_p, buf_len, ctrl_p->client_tag);

    free(buf_p);

    return 0;
}

static int uiccd_read_plmn_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t    *ctrl_p = (ste_sim_ctrl_block_t*) (ste_msg->client_tag);
    uiccd_trans_data_plmn_t *data_p;
    int                      rv;
    int                      file_id;

    if (ctrl_p == NULL) {
        return 0;
    }
    data_p = (uiccd_trans_data_plmn_t*) (ctrl_p->transaction_data_p);
    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }
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
            uiccd_encode_and_send_read_plmn_result(ctrl_p, rsp_p->uicc_status_code,
                    rsp_p->uicc_status_code_fail_details, rsp_p->status_word, NULL, data_p->file_id);
            free(data_p);
            free(ctrl_p);
            return 0;
        }
        data_p->file_size = rsp_p->file_size;
        data_p->trans_state = UICCD_TRANS_PLMN_READ_BINARY;

        file_id = (data_p->file_id == SIM_FILE_ID_PLMN_WACT) ? PLMN_WACT_FILE_ID : PLMN_SEL_FILE_ID;
        rv = uiccd_send_read_binary_plmn_request(ctrl_p, data_p->file_size, file_id);

        if (rv < 0) {
            //the transaction must be ended since there is error.
            sim_send_generic(ctrl_p->fd, STE_UICC_RSP_READ_PLMN, NULL, 0, ctrl_p->client_tag);

            free(data_p);
            free(ctrl_p);
            return 0;
        }

    }
        break;
    case UICCD_TRANS_PLMN_READ_BINARY: {
        uiccd_msg_read_sim_file_binary_response_t * rsp_p;

        rsp_p = (uiccd_msg_read_sim_file_binary_response_t*) ste_msg;

        if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_MSG_READ_PLMN");
            free(data_p);
            free(ctrl_p);
            return -1;
        }

        rv = uiccd_encode_and_send_read_plmn_result(ctrl_p, rsp_p->uicc_status_code,
                            rsp_p->uicc_status_code_fail_details, rsp_p->status_word, rsp_p, data_p->file_id);
        if (rv < 0) {
            //the transaction must be ended since there is error.
            sim_send_generic(ctrl_p->fd, STE_UICC_RSP_READ_PLMN, NULL, 0, ctrl_p->client_tag);
        }
        //the transaction is done
        free(data_p);
        free(ctrl_p);
        return 0;
    }
        break;
    default: {
        catd_log_f(SIM_LOGGING_E,
                   "uicc : wrong transaction state for read plmn=%d",
                   data_p->trans_state);
        free(data_p);
        free(ctrl_p);
        return 0;
    }
    }
    return -1;
}

int uiccd_main_read_plmn(ste_msg_t * ste_msg)
{
    ste_modem_t               * m;
    uiccd_msg_read_plmn_t     * msg = (uiccd_msg_read_plmn_t *) ste_msg;
    ste_sim_ctrl_block_t      * ctrl_p;
    int                         i;
    int                         file_id;
    uiccd_trans_data_plmn_t   * data_p;

    catd_log_f(SIM_LOGGING_I, "uicc : READ PLMN message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        // Send back result
        sim_send_generic(msg->fd, STE_UICC_RSP_READ_PLMN, NULL, 0, msg->client_tag);
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_plmn_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        // Send back result
        sim_send_generic(msg->fd, STE_UICC_RSP_READ_PLMN, NULL, 0, msg->client_tag);
        return -1;
    }
    memset(data_p, 0, sizeof(uiccd_trans_data_plmn_t));

    sim_decode_read_plmn(&data_p->file_id, msg->data, msg->len);
    data_p->trans_state = UICCD_TRANS_PLMN_GET_FORMAT;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_read_plmn_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for read plmn transaction failed");
        // Send back result
        sim_send_generic(msg->fd, STE_UICC_RSP_READ_PLMN, NULL, 0, msg->client_tag);
        free(data_p);
        return -1;
    }

    file_id = (data_p->file_id == SIM_FILE_ID_PLMN_WACT) ? PLMN_WACT_FILE_ID : PLMN_SEL_FILE_ID;
    i = uiccd_get_plmn_format(ctrl_p, file_id);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_read_plmn failed.");
        // Send back result
        sim_send_generic(msg->fd, STE_UICC_RSP_READ_PLMN, NULL, 0, msg->client_tag);
        //transaction must be ended
        free(data_p);
        free(ctrl_p);
        return -1;
    }

    return 0;
}
