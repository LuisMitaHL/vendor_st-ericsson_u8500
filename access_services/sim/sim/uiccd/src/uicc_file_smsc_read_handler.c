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

static int uiccd_encode_and_send_read_smsc_result(int fd,
                                                  uintptr_t client_tag,
                                                  sim_uicc_status_code_t uicc_status_code,
                                                  sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                  sim_uicc_status_word_t status_word,
                                                  ste_sim_call_number_t * smsc_p)
{
    char                    *buf_p = NULL;
    char                    *smsc_buf_p = NULL;
    char                    *p = NULL;
    size_t                   smsc_buf_len;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

    if (smsc_p) {
        smsc_buf_p = sim_encode_smsc_string(smsc_p, &smsc_buf_len);

        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_encode_and_send_read_smsc_result, sending response");

        if (smsc_buf_p == NULL) {
            catd_log_f(SIM_LOGGING_E, "uicc : "
                       "uiccd_encode_and_send_read_smsc_result: encode smsc string failed, abort.");
            return -1;
        }

        buf_len += smsc_buf_len;
    }

    buf_p = malloc(buf_len);

    if (buf_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result, memory allocation failed");
        free(smsc_buf_p);
        return -1;
    }

    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

    if (smsc_p && smsc_buf_len > 0) {
        p = sim_enc(p, smsc_buf_p, smsc_buf_len);
    }

    if (buf_len != (size_t)(p-buf_p)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_encode_and_send_read_smsc_result, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_UICC_RSP_READ_SMSC,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);
    free(smsc_buf_p);

    return 0;
}

/**
 * @brief                  Handler function for read smsc responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 * All transaction states check the status code. If the status code indicate a failure, the transaction is ended
 * and the error is sent to the client together with the status words received from the SIM I/O operation that failed.
 *
 * Transaction states:
 *
 * 1. UICCD_TRANS_SMSC_GET_FORMAT. If there are no records, the transaction ends. Otherwise, the active SMSC parameters
 *                                 are requested through a read sim file record call.
 * 2. UICCD_TRANS_SMSC_READ_RECORD. The record is decoded into the SMSC parameter struct and returned to the client.
 *
 */
static int uiccd_read_smsc_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t     * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_smsc_t  * data_p;
    int                        result = 0; // Default is to the end the transaction
    int                        rv;
    sim_uicc_status_word_t     status_word = { 0, 0 };

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
                return -1; // Continue transaction
            }

            rsp_p = (uiccd_msg_sim_file_get_format_response_t*)ste_msg;

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
                if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           rsp_p->uicc_status_code,
                                                           rsp_p->uicc_status_code_fail_details,
                                                           rsp_p->status_word,
                                                           NULL) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                }
                free(data_p);
                free(ctrl_p);
                break;
            }
            data_p->record_len = rsp_p->record_len;
            data_p->num_records = rsp_p->num_records;

            if (data_p->num_records == 0) {
                if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           SIM_UICC_STATUS_CODE_FAIL,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND,
                                                           status_word,
                                                           NULL) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                }

                catd_log_f(SIM_LOGGING_E, "uicc : smsp file not found");

                free(data_p);
                free(ctrl_p);
                break;
            }

            //read the first record
            data_p->record_id = 1;
            data_p->trans_state = UICCD_TRANS_SMSC_READ_RECORD;
            rv = uiccd_read_one_smsp_record(ctrl_p, data_p->record_id, 0 , SMSP_FILE_ID);

            if (rv < 0) {
                //the transaction must be ended since an error has occured
                if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           SIM_UICC_STATUS_CODE_FAIL,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                           status_word,
                                                           NULL) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                }
                free(data_p);
                free(ctrl_p);
                break;
            }

            result = -1; // Continue transaction
        }
        break;
        case UICCD_TRANS_SMSC_READ_RECORD:
        {
            uiccd_msg_read_sim_file_record_response_t * rsp_p;
            ste_sim_smsp_t                              smsp;
            ste_sim_call_number_t                       smsc;

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_RECORD_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SMSC_READ_RECORD");
                result = -1; // Continue transaction
                break;
            }

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_SMSC_READ_RECORD");

            memset(&smsc, 0, sizeof(smsc));

            rsp_p = (uiccd_msg_read_sim_file_record_response_t*)ste_msg;

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
                if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           rsp_p->uicc_status_code,
                                                           rsp_p->uicc_status_code_fail_details,
                                                           rsp_p->status_word,
                                                           NULL) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                }
                free(data_p);
                free(ctrl_p);
                break;
            }

            // Decode the SMSC that was retrieved from the first (default) record
            rv = sim_decode_smsp_record(&smsp, rsp_p->data, rsp_p->len);
            if (rv != 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : decode smsp record error");
                if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           SIM_UICC_STATUS_CODE_FAIL,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                           status_word,
                                                           NULL) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                }

                //the transaction failed, deallocate transaction memory
                data_p->trans_state = UICCD_TRANS_SMSC_INIT;
                free(data_p);
                free(ctrl_p);
                break;
            }

            // Find the smsc, convert SC address based on the coding scheme, then end the transaction
            if (smsp.service_center_address.length > 0) {
                rv = sim_create_ril_smsc_str(&smsc, &(smsp.service_center_address));
                if (rv != 0) {
                    if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                               ctrl_p->client_tag,
                                                               SIM_UICC_STATUS_CODE_FAIL,
                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                               status_word,
                                                               NULL) != 0) {
                         catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                     }
                } else {
                    if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                               ctrl_p->client_tag,
                                                               SIM_UICC_STATUS_CODE_OK,
                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                                               status_word,
                                                               &smsc) != 0) {
                        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                    }
                }
                free(smsc.num_text.text_p);
                smsc.num_text.text_p = NULL;
            }
            else {
                // No SMSC
                if (uiccd_encode_and_send_read_smsc_result(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           SIM_UICC_STATUS_CODE_OK,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                                           status_word,
                                                           NULL) != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
                }
            }
            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SMSC_INIT;
            free(data_p);
            free(ctrl_p);
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for read smsc=%d",data_p->trans_state);
            free(data_p);
            free(ctrl_p);
        }
        break;
    }
    return result;
}

/**
 * @brief                  Main function for read smsc.
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * To implement this feature, a conception of transaction is imported. That is to say, the read smsc request from client
 * consists of several async calls, and those async calls together make a transaction.
 * The transaction implementation includes the handler function for the responses from modem, the user data(named transaction data)
 * needed for the transaction including transaction state, etc., and the transaction id, which is exactly the same as the
 * message type from client. In this way, when a response(for example, read record resp) is received from modem, the response function
 * will check if the transaction id is the same as the resposne message type, if it is the same, then it goes through the old
 * response function. Otherwise it will go through the transaction handler function.
 *
 * For read smsc particularly, the transaction includes calls to get_format and read_record. Correspondingly, we have transaction
 * states named get_format and read_record to handle the different responses from modem. Before sending down the first request(get_format)
 * to modem, we create the ctrl block to save all the transaction related data into it, then after we get response from modem, we can use
 * the data.
 */
int uiccd_main_read_smsc(ste_msg_t * ste_msg)
{
    ste_modem_t              * m;
    uiccd_msg_read_smsc_t    * msg = (uiccd_msg_read_smsc_t *) ste_msg;
    ste_sim_ctrl_block_t     * ctrl_p;
    int                        result;
    uiccd_trans_data_smsc_t  * data_p;
    sim_uicc_status_word_t     status_word = { 0, 0 };

    catd_log_f(SIM_LOGGING_I, "uicc : READ SMSC message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        if (uiccd_encode_and_send_read_smsc_result(msg->fd,
                                                   msg->client_tag,
                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                   status_word,
                                                   NULL) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
        }
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_smsc_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        if (uiccd_encode_and_send_read_smsc_result(msg->fd,
                                                   msg->client_tag,
                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                   status_word,
                                                   NULL) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
        }
        return -1;
    }

    memset(data_p, 0, sizeof(uiccd_trans_data_smsc_t));

    data_p->trans_state = UICCD_TRANS_SMSC_GET_FORMAT;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_read_smsc_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for read smsc transaction failed");
        if (uiccd_encode_and_send_read_smsc_result(msg->fd,
                                                   msg->client_tag,
                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                   status_word,
                                                   NULL) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
        }
        free(data_p);
        return -1;
    }

    result = uiccd_get_smsp_format(ctrl_p, SMSP_FILE_ID);

    if (result != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "uiccd_main_read_smsc failed.");

        if (uiccd_encode_and_send_read_smsc_result(msg->fd,
                                                   msg->client_tag,
                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                   status_word,
                                                   NULL) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_smsc_result failed");
        }
        //transaction must be ended
        free(data_p);
        free(ctrl_p);
        return -1;
    }

    return 0;
}
