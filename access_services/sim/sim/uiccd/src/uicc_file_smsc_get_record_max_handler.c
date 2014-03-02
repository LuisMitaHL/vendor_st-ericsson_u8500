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
#include "uicc_file_smsc_common.h"
#include "uicc_file_paths.h"
#include <assert.h>
#include "uicc_file_paths.h"

static int uiccd_encode_and_send_smsc_get_record_max_result(int fd,
                                                            uintptr_t client_tag,
                                                            sim_uicc_status_code_t uicc_status_code,
                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                            sim_uicc_status_word_t status_word,
                                                            int record_max)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(record_max);

    buf_p = malloc(buf_len);

    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_get_record_max_result, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
    p = sim_enc(p, &record_max, sizeof(record_max));

    if (buf_len != (size_t)(p-buf_p)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_encode_and_send_smsc_get_record_max_result, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_UICC_RSP_SMSC_GET_RECORD_MAX,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

/**
 * @brief                  Handler function for smsc get record max responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 * Transaction states:
 *
 * 1. UICCD_TRANS_SMSC_GET_FORMAT. The record number is read and returned to the client as the record max.
 *
 */
static int uiccd_smsc_get_record_max_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t     * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_smsc_t  * data_p;
    int                        result = 0; // Default is to end the transaction

    if (ctrl_p == NULL) {
        return 0;
    }

    data_p = (uiccd_trans_data_smsc_t*)(ctrl_p->transaction_data_p);

    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_SMSC_GET_FORMAT:
        {
            uiccd_msg_sim_file_get_format_response_t* rsp_p;

            if (ste_msg->type != UICCD_MSG_SIM_FILE_GET_FORMAT_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SMSC_GET_FORMAT");
                result = -1; // Continue transaction
                break;
            }

            rsp_p = (uiccd_msg_sim_file_get_format_response_t*)ste_msg;

            if (uiccd_encode_and_send_smsc_get_record_max_result(ctrl_p->fd,
                                                                 ctrl_p->client_tag,
                                                                 rsp_p->uicc_status_code,
                                                                 rsp_p->uicc_status_code_fail_details,
                                                                 rsp_p->status_word,
                                                                 rsp_p->num_records) != 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_get_record_max_result failed");
            }

            free(data_p);
            free(ctrl_p);
       }
       break;
       default:
       {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for get record max result = %d",data_p->trans_state);
            free(data_p);
            free(ctrl_p);
       }
       break;
    }

    return result;
}

/**
 * @brief                  Main function for smsc get record max
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * For retriving the max record that can be used for storing SMSC parameter, one needs to get the smsp format information for the SMSP file.
 *
 * To implement this feature, a conception of transaction is imported.
 *
 * The transaction implementation includes the handler function for the responses from modem, the user data(named transaction data)
 * needed for the transaction including transaction state, etc., and the transaction id, which is exactly the same as the
 * message type from client. In this way, when a response is received from modem, the response function
 * will check if the transaction id is the same as the response message type, if it is the same, then it goes through the old
 * response function. Otherwise it will go through the transaction handler function.
 *
 */
int uiccd_main_smsc_get_record_max(ste_msg_t * ste_msg)
{
    ste_modem_t              * m;
    uiccd_msg_smsc_get_record_max_t    * msg = (uiccd_msg_smsc_get_record_max_t *) ste_msg;
    sim_uicc_status_word_t     status_word = { 0, 0 };
    ste_sim_ctrl_block_t     * ctrl_p;
    int result = 0;
    int record_max = 0;
    uiccd_trans_data_smsc_t  * data_p;

    catd_log_f(SIM_LOGGING_I, "uicc : SMSC GET RECORD MAX message received from: %d ",msg->fd);

    m = catd_get_modem();

    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        if (uiccd_encode_and_send_smsc_get_record_max_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             record_max) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_get_record_max_result failed");
        }
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_smsc_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        if (uiccd_encode_and_send_smsc_get_record_max_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             record_max) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_get_record_max_result failed");
        }
        return -1;
    }
    memset(data_p, 0, sizeof(uiccd_trans_data_smsc_t));

    data_p->trans_state = UICCD_TRANS_SMSC_GET_FORMAT;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_smsc_get_record_max_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for read smsc transaction failed");
        if (uiccd_encode_and_send_smsc_get_record_max_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             record_max) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_get_record_max_result failed");
        }
        free(data_p);
        return -1;
    }

    result = uiccd_get_smsp_format(ctrl_p, SMSP_FILE_ID);

    if (result != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_smsc_get_record_max failed.");

        if (uiccd_encode_and_send_smsc_get_record_max_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             record_max) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_get_record_max_result failed");
        }
        //transaction must be ended
        free(data_p);
        free(ctrl_p);
        return -1;
    }

    return 0;
}
