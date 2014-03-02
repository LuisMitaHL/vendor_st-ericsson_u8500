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
#include <assert.h>
#include "uicc_file_paths.h"

static int uiccd_encode_and_send_smsc_save_to_record_result(int fd,
                                                            uintptr_t client_tag,
                                                            sim_uicc_status_code_t uicc_status_code,
                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                            sim_uicc_status_word_t status_word)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

    buf_p = malloc(buf_len);

    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

    if (buf_len != (size_t)(p-buf_p)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_encode_and_send_smsc_save_to_record_result, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_UICC_RSP_SMSC_SAVE_TO_RECORD,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

/**
 * @brief                  Handler function for smsc save to record responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 * All transaction states check the status code. If the status code indicate a failure, the transaction is ended
 * and the error is sent to the client together with the status words received from the SIM I/O operation that failed.
 *
 * Transaction states:
 *
 * 1. UICCD_TRANS_SMSC_READ_RECORD. Record 1 is read and the raw record data is used to update the record identified
 *                                  by the record ID provided by the client.
 * 2. UICCD_TRANS_SMSC_UPDATE_RECORD. The result of the update operation is sent to the client.
 */
static int uiccd_smsc_save_to_record_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t     * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_smsc_t  * data_p;
    sim_uicc_status_word_t     status_word = { 0, 0 };
    int                        result = 0; // Default is to end the transaction
    int                        rv = 0;

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
        case UICCD_TRANS_SMSC_READ_RECORD:
        {
            uiccd_msg_read_sim_file_record_response_t * rsp_p;

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_RECORD_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SMSC_READ_RECORD");
                result = -1; // Continue transaction
                break;
            }

            rsp_p = (uiccd_msg_read_sim_file_record_response_t*)ste_msg;

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
                if (uiccd_encode_and_send_smsc_save_to_record_result(ctrl_p->fd,
                                                                     ctrl_p->client_tag,
                                                                     rsp_p->uicc_status_code,
                                                                     rsp_p->uicc_status_code_fail_details,
                                                                     status_word) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
                }
                free(data_p);
                free(ctrl_p);
                break;
            }

            rv = uiccd_update_smsp_record(ctrl_p, data_p->client_record_id, rsp_p->data, rsp_p->len);
            data_p->trans_state = UICCD_TRANS_SMSC_UPDATE_RECORD;
            if (rv < 0) {
                if (uiccd_encode_and_send_smsc_save_to_record_result(ctrl_p->fd,
                                                                     ctrl_p->client_tag,
                                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                     status_word) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
                }
                free(data_p);
                free(ctrl_p);
                break;
            }
            result = -1; // Continue transaction
       }
       break;
       case UICCD_TRANS_SMSC_UPDATE_RECORD:
       {
            uiccd_msg_update_sim_file_record_response_t   * rsp_p;

            if (ste_msg->type != UICCD_MSG_UPDATE_SIM_FILE_RECORD_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_UPDATE_RECORD");
                result = -1;
                break;
            }

            rsp_p = (uiccd_msg_update_sim_file_record_response_t*)ste_msg;

            if (uiccd_encode_and_send_smsc_save_to_record_result(ctrl_p->fd,
                                                                 ctrl_p->client_tag,
                                                                 rsp_p->uicc_status_code,
                                                                 rsp_p->uicc_status_code_fail_details,
                                                                 rsp_p->status_word) != 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
            }

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SMSC_INIT;

            free(data_p);
            free(ctrl_p);
       }
       break;
       default:
       {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for save to record = %d", data_p->trans_state);
            free(data_p);
            free(ctrl_p);
       }
       break;
    }

    return result;
}

/**
 * @brief                  Main function for smsc save to record
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * This function starts the transaction handler that is used for saving the active SMSC parameters in the SMSP file to
 * a specified record in the same file.
 *
 * The transaction implementation includes the handler function for the responses from modem, the user data(named transaction data)
 * needed for the transaction including transaction state, etc., and the transaction id, which is exactly the same as the
 * message type from client. In this way, when a response is received from modem, the response function
 * will check if the transaction id is the same as the response message type, if it is the same, then it goes through the old
 * response function. Otherwise it will go through the transaction handler function.
 *
 */
int uiccd_main_smsc_save_to_record(ste_msg_t * ste_msg)
{
    ste_modem_t                     * m;
    uiccd_msg_smsc_save_to_record_t * msg = (uiccd_msg_smsc_save_to_record_t *) ste_msg;
    ste_sim_ctrl_block_t            * ctrl_p;
    uiccd_trans_data_smsc_t         * data_p;
    const char                      * p = msg->data;
    const char                      * p_max = msg->data + msg->len;
    sim_uicc_status_word_t            status_word = { 0, 0 };
    int                               result = 0;
    int                               record_id;

    catd_log_f(SIM_LOGGING_I, "uicc : SMSC SAVE TO RECORD message received from: %d ",msg->fd);

    p = sim_dec(p, &record_id, sizeof(record_id), p_max);

    if ( !p ) {
        if (uiccd_encode_and_send_smsc_save_to_record_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
        }

        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_smsc_save_to_record failed");
        return -1;
    }

    m = catd_get_modem();

    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        if (uiccd_encode_and_send_smsc_save_to_record_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
        }
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_smsc_t));

    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        if (uiccd_encode_and_send_smsc_save_to_record_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
        }
        return -1;
    }

    memset(data_p, 0, sizeof(uiccd_trans_data_smsc_t));

    data_p->trans_state = UICCD_TRANS_SMSC_READ_RECORD;
    data_p->client_record_id = record_id;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_smsc_save_to_record_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for read smsc transaction failed");
        if (uiccd_encode_and_send_smsc_save_to_record_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
        }
        free(data_p);
        return -1;
    }

    // Read out the default record (Record_id 1) so it can be stored in the record_id provided by the client
    result = uiccd_read_one_smsp_record(ctrl_p, 1, 0, SMSP_FILE_ID);

    if (result != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_smsc_get_record_max failed.");

        if (uiccd_encode_and_send_smsc_save_to_record_result(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_smsc_save_to_record_result failed");
        }
        //transaction must be ended
        free(data_p);
        free(ctrl_p);
        return -1;
    }

    return 0;
}
