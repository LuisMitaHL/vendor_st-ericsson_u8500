/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "sim_internal.h"
#include "catd_modem_types.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "uicc_file_service_table_common.h"
#include "uicc_state_machine.h"

static int uiccd_encode_and_send_get_service_table_response(int fd,
                                                            uintptr_t client_tag,
                                                            sim_uicc_status_code_t uicc_status_code,
                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                            sim_uicc_status_word_t status_word,
                                                            sim_uicc_service_status_t service_status )
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(service_status);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_get_service_table_response, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
    p = sim_enc(p, &service_status, sizeof(service_status));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_GET_SERVICE_TABLE,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

/**
 * @brief                  Handler function for get service table responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE (failure and success): 0;
 *                                                    Transaction to be continued: -1
 *
 * Please read the description for this transaction from the comment for function 'uiccd_main_get_service_table'.
 * This transaction handler handles the following scenario:
 * USIM: GET READ BINARY RESPONSE  -> retrieve the data requested according to the information from client
 * -> return read and service status to client -> transaction is done
 * SIM: GET READ BINARY RESPONSE  -> check the service status using the retrieved data. If needed -> SEND APDU to further check status of file
 * -> return service status to client -> transaction is done
 */
static int uiccd_get_service_table_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t              * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_service_table_t  * data_p;
    int                               rv;
    sim_uicc_status_word_t            status_word = { 0, 0 };
    sim_uicc_service_status_t         service_status = SIM_UICC_SERVICE_STATUS_UNKNOWN;


    if (ctrl_p == NULL) {
        return 0;
    }
    data_p = (uiccd_trans_data_service_table_t*)(ctrl_p->transaction_data_p);
    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    catd_log_f(SIM_LOGGING_I, "uicc : Enter uiccd_get_service_table_transaction_handler");

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_SERVICE_TABLE_READ_BINARY:
        {
            uiccd_msg_read_sim_file_binary_response_t   * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_SERVICE_TABLE_READ_BINARY");

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SERVICE_TABLE_READ_BINARY");
                return -1;
            }

            rsp_p = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;

            if (rsp_p->len != 1) {
                catd_log_f(SIM_LOGGING_E, "uicc : one byte only expected from service table");
                rv = uiccd_encode_and_send_get_service_table_response(ctrl_p->fd,
                                                                      ctrl_p->client_tag,
                                                                      SIM_UICC_STATUS_CODE_FAIL,
                                                                      SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                      status_word,
                                                                      SIM_UICC_SERVICE_STATUS_UNKNOWN);
                free(data_p);
                free(ctrl_p);
                return 0;
            }

            if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
              switch (data_p->service_type)
              {
                 case SIM_UICC_SERVICE_TYPE_FDN:
                 {
                    if ((*rsp_p->data & SIM_SERVICE_TYPE_FDN_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_FDN_ACTIVATED_ALLOCATED){

                       if ((*rsp_p->data & SIM_SERVICE_TYPE_ADN_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_ADN_ACTIVATED_ALLOCATED){
                         // If FDN and ADN are allocated and activated in the service table, status of EF_ADN must checked.
                         data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_GET_FILE_INFORMATION;

                         rv = uiccd_get_ef_adn_file_information(ctrl_p);

                         if (rv < 0) {
                           rv = uiccd_encode_and_send_get_service_table_response(ctrl_p->fd,
                                                                                 ctrl_p->client_tag,
                                                                                 SIM_UICC_STATUS_CODE_FAIL,
                                                                                 SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                                 status_word,
                                                                                 SIM_UICC_SERVICE_STATUS_UNKNOWN);
                           free(data_p);
                           free(ctrl_p);
                           return 0;
                         }

                         // Continue the transaction
                         return -1;

                       }
                       else{
                         // FDN is allocated and activated but ADN is not => FDN is  enabled.
                         catd_log_f(SIM_LOGGING_I, "uicc : FDN service is enabled in GSM");

                         service_status = SIM_UICC_SERVICE_STATUS_ENABLED;

                       }
                    }
                    else{
                      // FDN isn't allocated and activated i.e. it is disabled
                      catd_log_f(SIM_LOGGING_I, "uicc : FDN service is disabled in GSM");

                      service_status = SIM_UICC_SERVICE_STATUS_DISABLED;
                    }
                 }
                 break;

                 default:
                 {
                    catd_log_f(SIM_LOGGING_E, "uicc : unexpected service type for get service table");
                    rv = uiccd_encode_and_send_get_service_table_response(ctrl_p->fd,
                                                                          ctrl_p->client_tag,
                                                                          SIM_UICC_STATUS_CODE_FAIL,
                                                                          SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                          status_word,
                                                                          SIM_UICC_SERVICE_STATUS_UNKNOWN);
                    free(data_p);
                    free(ctrl_p);
                    return 0;
                 }

               } // switch
             }
             else if (uicc_get_app_type() == STE_SIM_APP_TYPE_USIM) {
               switch (data_p->service_type)
               {
                 case SIM_UICC_SERVICE_TYPE_FDN:
                 {
                   if ((*rsp_p->data & USIM_SERVICE_TYPE_FDN_ENABLE) == USIM_SERVICE_TYPE_FDN_ENABLE) {
                     service_status = SIM_UICC_SERVICE_STATUS_ENABLED;
                   }
                   else {
                     service_status = SIM_UICC_SERVICE_STATUS_DISABLED;
                   }
                 }
                 break;

                 case SIM_UICC_SERVICE_TYPE_BDN:
                 {
                   if ((*rsp_p->data & USIM_SERVICE_TYPE_BDN_ENABLE) == USIM_SERVICE_TYPE_BDN_ENABLE) {
                     service_status = SIM_UICC_SERVICE_STATUS_ENABLED;
                   }
                   else {
                     service_status = SIM_UICC_SERVICE_STATUS_DISABLED;
                   }
                 }
                 break;

                 case SIM_UICC_SERVICE_TYPE_ACL:
                 {
                   if ((*rsp_p->data & USIM_SERVICE_TYPE_ACL_ENABLE) == USIM_SERVICE_TYPE_ACL_ENABLE) {
                     service_status = SIM_UICC_SERVICE_STATUS_ENABLED;
                   }
                   else {
                     service_status = SIM_UICC_SERVICE_STATUS_DISABLED;
                   }
                 }
                 break;

                 default:
                 {
                    catd_log_f(SIM_LOGGING_E, "uicc : unexpected service type for get service table");
                    rv = uiccd_encode_and_send_get_service_table_response(ctrl_p->fd,
                                                                          ctrl_p->client_tag,
                                                                          SIM_UICC_STATUS_CODE_FAIL,
                                                                          SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                          status_word,
                                                                          SIM_UICC_SERVICE_STATUS_UNKNOWN);
                    free(data_p);
                    free(ctrl_p);
                    return 0;
                 }

               } // switch

            }
            else {
              catd_log_f(SIM_LOGGING_E, "uicc : uiccd_get_service_table: Unexpected app_type");
              rv = uiccd_encode_and_send_get_service_table_response(ctrl_p->fd,
                                                                    ctrl_p->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    status_word,
                                                                    SIM_UICC_SERVICE_STATUS_UNKNOWN);
              free(data_p);
              free(ctrl_p);
              return 0;
            }

            //send the result back to client
            rv = uiccd_encode_and_send_get_service_table_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  rsp_p->uicc_status_code,
                                                                  rsp_p->uicc_status_code_fail_details,
                                                                  rsp_p->status_word,
                                                                  service_status);

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_INIT;

            free(data_p);
            free(ctrl_p);
            return 0;
        }
        break;

        case UICCD_TRANS_SERVICE_TABLE_GET_FILE_INFORMATION:
        {
            uiccd_msg_get_file_information_response_t   * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_SERVICE_TABLE_GET_FILE_INFORMATION");

            if (ste_msg->type != UICCD_MSG_GET_FILE_INFORMATION_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SERVICE_TABLE_GET_FILE_INFORMATION");
                return -1;
            }

            rsp_p = (uiccd_msg_get_file_information_response_t*)ste_msg;

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_SERVICE_TABLE_GET_FILE_INFORMATION,  status = %d, sw1 = %d, sw2 = %d",
                                                            rsp_p->uicc_status_code, rsp_p->status_word.sw1, rsp_p->status_word.sw2 );

            //check the EF_ADN status result and send the result back to client

            if (rsp_p->uicc_status_code == SIM_UICC_STATUS_CODE_OK)
            {
               catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_SERVICE_TABLE_GET_FILE_INFORMATION, file id  %#04x %#04x, file record len =  %#04x, file status =  %#04x",
                                         rsp_p->data[4] , rsp_p->data[5] ,rsp_p->data[14], rsp_p->data[11] );

               if ((rsp_p->data[SIM_FILE_INFO_FILE_STATUS] & SIM_SERVICE_TYPE_ADN_NOT_INVALIDATED)  == SIM_SERVICE_TYPE_ADN_NOT_INVALIDATED)
               {
                 // EF_ADN is NOT invalidated == FDN is disabled
                  catd_log_f(SIM_LOGGING_I, "uicc : SIM_UICC_SERVICE_STATUS_DISABLED");
                  service_status = SIM_UICC_SERVICE_STATUS_DISABLED;
               }
               else
               {
                 // EF_ADN is invalidated == FDN is enabled
                  catd_log_f(SIM_LOGGING_I, "uicc : SIM_UICC_SERVICE_STATUS_ENABLED");
                  service_status = SIM_UICC_SERVICE_STATUS_ENABLED;
               }
            }
            else {
               catd_log_f(SIM_LOGGING_I, "uicc : SIM_UICC_SERVICE_STATUS_UNKNOWN, status = %d, sw1 = %d, sw2 = %d",
                               rsp_p->uicc_status_code, rsp_p->status_word.sw1, rsp_p->status_word.sw2 );
               service_status = SIM_UICC_SERVICE_STATUS_UNKNOWN;
            }

            rv = uiccd_encode_and_send_get_service_table_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  SIM_UICC_STATUS_CODE_OK,
                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                                                  status_word,
                                                                  service_status);


            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_INIT;

            free(data_p);
            free(ctrl_p);
            return 0;
        }
        break;

        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for get service table");
            free(data_p);
            free(ctrl_p);
            return 0;
        }
    }
    return -1;
}

/**
 * @brief                  Main function for get information from the service table.
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * The active status for a service is indicated by specific bit in the service table. In USIM the service table is in file EF-EST (Enable Service Table)
 * and in GSM in file EF-SST (SIM Service Table). The value of this bit is returned to the client.
 *
 * The transaction implementation includes the handler function for the responses from modem, the user data(named transaction data)
 * needed for the transaction including transaction state, etc., and the transaction id, which is exactly the same as the
 * message type from client. In this way, when a response(for example, read record resp) is received from modem, the response function
 * will check if the transaction id is the same as the resposne message type, if it is the same, then it goes through the old
 * response function. Otherwise it will go through the transaction handler function.
 *
 * For get service table information the transaction includes a  call to read_binary and in the SIM case also send apdu. Corresponding transaction
 * states are named read_binary and send apdu respectively, and handle the responses from modem. Before sending the first request
 * to the modem, a ctrl block to save all the transaction related data is created.
 * The typical state transition for this  transation is :
 * USIM:  READ BINARY -> DONE
 * SIM: READ BINARY -> SEND APDU -> DONE
 */
int uiccd_main_get_service_table(ste_msg_t * ste_msg)
{
    ste_modem_t                       * m;
    uiccd_msg_get_service_table_t     * msg = (uiccd_msg_get_service_table_t *) ste_msg;
    ste_sim_ctrl_block_t              * ctrl_p;
    int                               i;
    uiccd_trans_data_service_table_t  * data_p;
    sim_uicc_status_word_t            status_word = { 0, 0 };

    const char *p = msg->data;
    const char *p_max = msg->data + msg->len;
    sim_uicc_service_type_t service_type;


    catd_log_f(SIM_LOGGING_I, "uicc : GET service table message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        //since this mechanism of calling uiccd_sig_get_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_table_response(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             SIM_UICC_SERVICE_STATUS_UNKNOWN);
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_service_table_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        //since this mechanism of calling uiccd_sig_get_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_table_response(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             SIM_UICC_SERVICE_STATUS_UNKNOWN);
        return -1;
    }
    memset(data_p, 0, sizeof(uiccd_trans_data_service_table_t));

    //decode the data from client
    p = sim_dec(p, &service_type, sizeof(service_type), p_max);

    if ( !p ) {
        // Something went wrong in the sim_dec above.
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for get service table transaction failed decoding msg data");
        //since this mechanism of calling uiccd_sig_get_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_table_response(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             SIM_UICC_SERVICE_STATUS_UNKNOWN);
        free(data_p);
        return -1;
    }

    //save the data from client
    data_p->service_type = service_type;
    data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_READ_BINARY;

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_get_service_table_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for update service table transaction failed");
        //since this mechanism of calling uiccd_sig_get_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_get_service_table_response(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                             status_word,
                                                             SIM_UICC_SERVICE_STATUS_UNKNOWN);
        free(data_p);
        return -1;
    }

    i = uiccd_read_service_table_service_status(ctrl_p, data_p->service_type);

    if (i != 0) {
        //since this mechanism of calling uiccd_sig_get_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here

        if (i == -2)
           i = uiccd_encode_and_send_get_service_table_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,
                                                                status_word,
                                                                SIM_UICC_SERVICE_STATUS_UNKNOWN);
        else
           i = uiccd_encode_and_send_get_service_table_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word,
                                                                SIM_UICC_SERVICE_STATUS_UNKNOWN);

        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "uiccd_main_get_service_table failed: read service table failed.");
        //transaction must be ended
        free(data_p);
        free(ctrl_p);
        return -1;
    }

    return 0;
}
