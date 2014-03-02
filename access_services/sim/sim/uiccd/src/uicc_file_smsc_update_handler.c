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
#include "uicc_file_smsc_common.h"
#include "uicc_file_paths.h"

static int uiccd_encode_and_send_update_smsc_result(int fd,
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
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result, memory allocatin failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

    if (buf_len != (size_t)(p-buf_p)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_encode_and_send_update_smsc_result, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_UICC_RSP_UPDATE_SMSC,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

/**
 * @brief                  Handler function for update smsc responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 * All transaction states check the status code. If the status code indicate a failure, the transaction is ended
 * and the error is sent to the client together with the status words received from the SIM I/O operation that failed.
 *
 * Transaction states:
 *
 * 1. UICCD_TRANS_SMSC_GET_FORMAT.    If there are no records, the transaction ends. If there are more than 1 record in
 *                                    the file, record 2 will be read out first. This position is the one reserved
 *                                    for backing up the manufacturer default SMSC parameter settings. If there is only
 *                                    one record, this one will be read and used for storing the new SMSC parameters.
 * 2. UICCD_TRANS_SMSC_READ_RECORD.   This state will decode the record that was read out in state 1. If the record ID is
 *                                    2, it will check if the service center address is already stored in the position. If
 *                                    it has not been set, an indication stating that a manufacturer backup shall be done is
 *                                    set. Record 1 will be read out after record 2 has been read. When state 2 (this state)
 *                                    is reached for record 1, the manufacturer backup indicator is checked and if it is true, the
 *                                    record will be updated with the raw data from record 1 (Manufacturer default) and the state will
 *                                    be exited. If the manufacturer backup indicator is not set, the SMSC parameters are decoded
 *                                    from record 1 and the client provided SMSC parameters are added to the record data which
 *                                    then is sent to the record update function.
 * 3. UICCD_TRANS_SMSC_UPDATE_RECORD. This state will check if the record updated was record 2. If it was, the update that
 *                                    resulted in this state was the manufacturer backup update. If this was the case,
 *                                    the client provided SMSC parameters have yet to be updated for record 1. Therefore,
 *                                    record 1 will be read which will transition the state machine to state 2. On the
 *                                    other hand, if state 3 (this state) was caused by the record 1 update in state 2, the
 *                                    transaction is done.
 */
static int uiccd_update_smsc_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t     * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_smsc_t  * data_p;
    ste_sim_call_number_t    * smsc_p;
    int                        result = 0; // Default is to end the transaction
    int                        rv = 0;
    uint8_t                  * record_binary_p;
    int                        record_len;
    sim_uicc_status_word_t     status_word = { 0, 0 };

    if (ctrl_p == NULL) {
        return 0;
    }
    data_p = (uiccd_trans_data_smsc_t*)(ctrl_p->transaction_data_p);
    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }
    smsc_p = data_p->smsc_p;
    if (smsc_p == NULL) {
        free(ctrl_p->transaction_data_p);
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
                result = -1; // Continue the transaction
                break;
            }

            rsp_p = (uiccd_msg_sim_file_get_format_response_t*)ste_msg;

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
                if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                             ctrl_p->client_tag,
                                                             rsp_p->uicc_status_code,
                                                             rsp_p->uicc_status_code_fail_details,
                                                             rsp_p->status_word) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                }
                free(smsc_p->num_text.text_p);
                free(smsc_p);
                free(data_p);
                free(ctrl_p);
                break;
            }

            data_p->record_len = rsp_p->record_len;
            data_p->num_records = rsp_p->num_records;

            if (data_p->num_records == 0) {
                if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                             ctrl_p->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND,
                                                             status_word) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                }

                catd_log_f(SIM_LOGGING_I, "uicc : smsp file not found");
                free(smsc_p->num_text.text_p);
                free(smsc_p);
                free(data_p);
                free(ctrl_p);
                break;
            } else if (data_p->num_records > 1) {
                data_p->record_id = 2; // Read the record reserved for storing the manufacturer default
            } else {
                data_p->record_id = 1;
            }

            data_p->trans_state = UICCD_TRANS_SMSC_READ_RECORD;
            rv = uiccd_read_one_smsp_record(ctrl_p, data_p->record_id, 0, SMSP_FILE_ID);
            if (rv < 0) {
                if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                             ctrl_p->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                }
                free(smsc_p->num_text.text_p);
                free(smsc_p);
                free(data_p);
                free(ctrl_p);
                break;
            }
            result = -1; // Continue transaction
        }
        break;
        case UICCD_TRANS_SMSC_READ_RECORD:
        {
            uiccd_msg_read_sim_file_record_response_t   * rsp_p;
            ste_sim_smsp_t                                smsp;

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_RECORD_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SMSC_READ_RECORD");
                result = -1;
                break;
            }

            rsp_p = (uiccd_msg_read_sim_file_record_response_t*)ste_msg;

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
                if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                             ctrl_p->client_tag,
                                                             rsp_p->uicc_status_code,
                                                             rsp_p->uicc_status_code_fail_details,
                                                             rsp_p->status_word) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                }
                free(smsc_p->num_text.text_p);
                free(smsc_p);
                free(data_p);
                free(ctrl_p);
                break;
            }

            // Decode the record
            rv = sim_decode_smsp_record(&smsp, rsp_p->data, rsp_p->len);
            if (rv != 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : decode smsp record error");
                if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                             ctrl_p->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                }
                free(smsc_p->num_text.text_p);
                free(smsc_p);
                free(data_p);
                free(ctrl_p);
                break;
            }

            if (data_p->record_id == 2) {

                if ((smsp.indicators & STE_SIM_SMSP_SC_ADDRESS_ABSENT) ||
                    (smsp.service_center_address.length == 0)          ) {
                    data_p->man_backup = 1;
                }

                data_p->record_id = 1;

                rv = uiccd_read_one_smsp_record(ctrl_p, data_p->record_id, 0, SMSP_FILE_ID);
                if (rv < 0) {
                    if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                                 ctrl_p->client_tag,
                                                                 SIM_UICC_STATUS_CODE_FAIL,
                                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                 status_word) != 0) {
                        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                    }
                    free(smsc_p->num_text.text_p);
                    free(smsc_p);
                    free(data_p);
                    free(ctrl_p);
                    break;
                }
                result = -1; // Continue transaction
                break;
            }

            data_p->trans_state = UICCD_TRANS_SMSC_UPDATE_RECORD;

            if (data_p->man_backup == 1) {
                data_p->record_id = 2; // Update the reserved position (2) with the manufacturer default found in record 1
                rv = uiccd_update_smsp_record(ctrl_p, data_p->record_id, rsp_p->data, rsp_p->len);
                if (rv < 0) {
                    if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                                 ctrl_p->client_tag,
                                                                 SIM_UICC_STATUS_CODE_FAIL,
                                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                 status_word) != 0) {
                        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                    }
                    free(smsc_p->num_text.text_p);
                    free(smsc_p);
                    free(data_p);
                    free(ctrl_p);
                    break;
                }
            } else { // Record_id is 1
                //update the current record with the data from client
                record_binary_p = sim_encode_smsp_record(data_p->smsc_p, &record_len, &smsp, data_p->record_len);
                if (record_binary_p == NULL) { //something wrong with the encode, end the transaction
                    if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                                 ctrl_p->client_tag,
                                                                 SIM_UICC_STATUS_CODE_FAIL,
                                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                 status_word) != 0) {
                        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                    }

                    catd_log_f(SIM_LOGGING_E, "uicc : failed to encode smsp record");
                    free(smsc_p->num_text.text_p);
                    free(smsc_p);
                    free(data_p);
                    free(ctrl_p);
                    break;
                }

                rv = uiccd_update_smsp_record(ctrl_p, data_p->record_id, record_binary_p, record_len);
                free(record_binary_p);
                if (rv < 0) {
                    if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                                 ctrl_p->client_tag,
                                                                 SIM_UICC_STATUS_CODE_FAIL,
                                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                 status_word) != 0) {
                        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                    }
                    free(smsc_p->num_text.text_p);
                    free(smsc_p);
                    free(data_p);
                    free(ctrl_p);
                    break;
                }
            }

            result = -1; // Continue transaction
        }
        break;

        case UICCD_TRANS_SMSC_UPDATE_RECORD:
        {
            uiccd_msg_update_sim_file_record_response_t   * rsp_p;

            if (ste_msg->type != UICCD_MSG_UPDATE_SIM_FILE_RECORD_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_UDPATE_RECORD");
                return -1;
            }

            rsp_p = (uiccd_msg_update_sim_file_record_response_t*)ste_msg;

            if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                         ctrl_p->client_tag,
                                                         rsp_p->uicc_status_code,
                                                         rsp_p->uicc_status_code_fail_details,
                                                         rsp_p->status_word) != 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
            }

            if (data_p->record_id == 2) {
                data_p->record_id = 1;
                data_p->man_backup = 0; // The manufacturer backup is done, now save the client provided smsc

                data_p->trans_state = UICCD_TRANS_SMSC_READ_RECORD;
                rv = uiccd_read_one_smsp_record(ctrl_p, data_p->record_id, 0, SMSP_FILE_ID);
                if (rv < 0) {
                    if (uiccd_encode_and_send_update_smsc_result(ctrl_p->fd,
                                                                 ctrl_p->client_tag,
                                                                 SIM_UICC_STATUS_CODE_FAIL,
                                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                 status_word) != 0) {
                        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
                    }
                    free(smsc_p->num_text.text_p);
                    free(smsc_p);
                    free(data_p);
                    free(ctrl_p);
                    break;
                }
                result = -1; // Continue transaction
                break;
            }

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SMSC_INIT;

            free(smsc_p->num_text.text_p);
            free(smsc_p);
            free(data_p);
            free(ctrl_p);
        }
        break;

        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for update smsc=%d",data_p->trans_state);
            free(smsc_p->num_text.text_p);
            free(smsc_p);
            free(data_p);
            free(ctrl_p);
        }
        break;
    }
    return result;
}

/**
 * @brief                  Main function for update smsc.
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * This function will start the transaction handler for updating the smsc parameters in the SMSP file.
 *
 * To implement this feature, a conception of transaction is imported. That is to say, the update smsc request from client
 * consists of several async calls, and those async calls together make a transaction.
 * The transaction implementation includes the handler function for the responses from modem, the user data(named transaction data)
 * needed for the transaction including transaction state, etc., and the transaction id, which is exactly the same as the
 * message type from client. In this way, when a response(for example, read record resp) is received from modem, the response function
 * will check if the transaction id is the same as the response message type, if it is the same, then it goes through the old
 * response function. Otherwise it will go through the transaction handler function.
 *
 */
int uiccd_main_update_smsc(ste_msg_t * ste_msg)
{
    ste_modem_t              * m;
    uiccd_msg_update_smsc_t  * msg = (uiccd_msg_update_smsc_t *) ste_msg;
    ste_sim_ctrl_block_t     * ctrl_p;
    int                        result;
    uiccd_trans_data_smsc_t  * data_p;
    sim_uicc_status_word_t     status_word = { 0, 0 };
    catd_log_f(SIM_LOGGING_I, "uicc : UPDATE SMSC message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        if (uiccd_encode_and_send_update_smsc_result(msg->fd,
                                                     msg->client_tag,
                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                     status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
        }
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_smsc_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        if (uiccd_encode_and_send_update_smsc_result(msg->fd,
                                                     msg->client_tag,
                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                     status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
        }
        return -1;
    }

    memset(data_p, 0, sizeof(uiccd_trans_data_smsc_t));

    //save the smsc from client
    data_p->smsc_p = malloc(sizeof(ste_sim_call_number_t));
    if (!data_p->smsc_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        if (uiccd_encode_and_send_update_smsc_result(msg->fd,
                                                          msg->client_tag,
                                                          SIM_UICC_STATUS_CODE_FAIL,
                                                          SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                          status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
        }
        free(data_p);
        return -1;
    }
    memset(data_p->smsc_p, 0, sizeof(ste_sim_call_number_t));

    sim_decode_smsc_string(data_p->smsc_p, msg->data, msg->len);

    data_p->trans_state = UICCD_TRANS_SMSC_GET_FORMAT;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_update_smsc_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for update smsc transaction failed");
        if (uiccd_encode_and_send_update_smsc_result(msg->fd,
                                                     msg->client_tag,
                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                     status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
        }
        free(data_p->smsc_p->num_text.text_p);
        free(data_p->smsc_p);
        free(data_p);
        return -1;
    }

    result = uiccd_get_smsp_format(ctrl_p, SMSP_FILE_ID);

    if (result != 0) {
        if (uiccd_encode_and_send_update_smsc_result(msg->fd,
                                                     msg->client_tag,
                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                     status_word) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_smsc_result failed");
        }

        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "uiccd_main_update_smsc failed.");
        //transaction must be ended
        free(data_p->smsc_p->num_text.text_p);
        free(data_p->smsc_p);
        free(data_p);
        free(ctrl_p);
        return -1;
    }

    return 0;
}
