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
#include "uicc_file_service_table_common.h"
#include "uicc_file_paths.h"
#include "uicc_state_machine.h"

typedef enum {
    EF_ADN_STATUS_CHECK,
    EF_ADN_INVALIDATE,
    EF_ADN_REHABILITATE
} uiccd_trans_service_table_ef_adn_operation_t;

static int uiccd_encode_and_send_update_service_table_response(int fd, uintptr_t client_tag, sim_uicc_status_code_t uicc_status_code,
                                                               sim_uicc_status_code_fail_details_t uicc_status_code_fail_details, sim_uicc_status_word_t status_word )
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_update_service_table_response, memory allocatin failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_UPDATE_SERVICE_TABLE,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

static int uiccd_appl_apdu_send(ste_sim_ctrl_block_t * ctrl_p, uiccd_trans_service_table_ef_adn_operation_t operation)
{
    int                             i;
    ste_modem_t                   * m;
    int                             file_id;
    const char                    * file_path;
    ste_uicc_apdu_command_force_t   cmd_force;
    ste_apdu_t                    * apdu;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }

    cmd_force = STE_UICC_APDU_CMD_FORCE_NOT_USED;

    switch (operation)
    {
        case EF_ADN_INVALIDATE:
        {
          catd_log_f(SIM_LOGGING_I, "uicc : uiccd_appl_apdu_send: EF_ADN_INVALIDATE");
          file_path = ADN_FILE_PATH;
          file_id = GSM_EF_ADN_FILE_ID;

          apdu = ste_apdu_uicc_deactivate_file();
        }
        break;
        case EF_ADN_REHABILITATE:
        {
          catd_log_f(SIM_LOGGING_I, "uicc : uiccd_appl_apdu_send: EF_ADN_REHABILITATE");
          file_path = ADN_FILE_PATH;
          file_id = GSM_EF_ADN_FILE_ID;

          apdu = ste_apdu_uicc_activate_file();
        }
        break;
        default:
        {
          catd_log_f(SIM_LOGGING_E, "uicc : uiccd_appl_apdu_send: Unexpected operation");
          return -1;
        }
    } // switch

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_appl_apdu_send: call ste_modem_appl_apdu_send");
    i = ste_modem_appl_apdu_send(m,
                                 (uintptr_t)ctrl_p,
                                 uicc_get_app_id(),
                                 cmd_force,
                                 (int)ste_apdu_get_raw_length(apdu),
                                 (uint8_t *)ste_apdu_get_raw(apdu),
                                 file_id,
                                 file_path);

    ste_apdu_delete(apdu);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_appl_apdu_send: "
                   "ste_modem_appl_apdu_send failed.");
        return -1;
    }
    return i;


}

static int uiccd_update_service_table_service_status(ste_sim_ctrl_block_t * ctrl_p, uint8_t *data_p)
{
    int                 i;
    ste_modem_t       * m;
    int                 file_id, offset, length;
    const char        * file_path;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }

    if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
        file_path = SERVICE_TABLE_GSM_FILE_PATH;
        file_id = GSM_SERVICE_TABLE_FILE_ID;
    }
    else if (uicc_get_app_type() == STE_SIM_APP_TYPE_USIM) {
        file_path = SERVICE_TABLE_EST_USIM_PATH;
        file_id = USIM_SERVICE_TABLE_EST_FILE_ID;
    }
    else {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_update_service_table_service_status: Unexpected app_type");
        return -1;
    }

    offset = 0;
    length = 1;

    i = ste_modem_file_update_binary(m,
                                     (uintptr_t)ctrl_p,
                                     uicc_get_app_id(),
                                     file_id,
                                     offset,
                                     length,
                                     file_path,
                                     data_p);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_update_service_table_service_status: "
                   "ste_modem_file_update_binary failed.");
        return -1;
    }
    return i;
}

/**
 * @brief                  Handler function for update service table responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 * Please read the description for this transaction from the comment for function 'uiccd_main_update_service_table'.
 * This transaction handler handles the following scenario:
 * USIM case:
 * GET READ BINARY RESPONSE  -> change the read data according to the information from client
 * -> state changed to UPDATE DATA, to update the byte with the new data
 * -> GET the UPDATE BINARY RSP -> return update status to client -> transaction is done
 * SIM case:
 * GET READ BINARY RESPONSE  -> check the current service status for ADM and FDN according to the information from client
 * -> state changed to SEND_APDU, to change status of the EF-ADN file, invalidate or rehabilitate
 * -> GET the SEND APDU RSP -> return status to client -> transaction is done
 */
static int uiccd_update_service_table_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t              * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_service_table_t  * data_p;
    int                               rv;
    sim_uicc_status_word_t            status_word = { 0, 0 };

    if (ctrl_p == NULL) {
        return 0;
    }
    data_p = (uiccd_trans_data_service_table_t*)(ctrl_p->transaction_data_p);
    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    catd_log_f(SIM_LOGGING_I, "uicc : Enter uiccd_update_service_table_transaction_handler");

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_SERVICE_TABLE_PIN_VERIFY:
        {
            uiccd_msg_pin_verify_response_t  * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_SERVICE_TABLE_PIN_VERIFY");

            if (ste_msg->type != UICCD_MSG_PIN_VERIFY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SERVICE_TABLE_PIN_VERIFY");
                return -1;
            }

            rsp_p = (uiccd_msg_pin_verify_response_t*)ste_msg;

            if (rsp_p->uicc_status_code != SIM_UICC_STATUS_CODE_OK) {
                catd_log_f(SIM_LOGGING_E, "uicc : PIN verification failed");
                rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                         ctrl_p->client_tag,
                                                                         rsp_p->uicc_status_code,
                                                                         rsp_p->uicc_status_code_fail_details,
                                                                         rsp_p->status_word);
                free(data_p);
                free(ctrl_p);
                return 0;
            }

            data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_READ_BINARY;

            rv = uiccd_read_service_table_service_status(ctrl_p, data_p->service_type);

            if (rv != 0) {
              //since this mechanism of calling uiccd_sig_update_service_table_response to trigg the send back the result to the client is not working,
              //the result is directly sent back here
               rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                        ctrl_p->client_tag,
                                                                        SIM_UICC_STATUS_CODE_FAIL,
                                                                        SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                        status_word);
               catd_log_f(SIM_LOGGING_E, "uicc :uiccd_update_service_table_transaction_handler: read service table failed.");
               //transaction must be ended
               free(data_p);
               free(ctrl_p);
               return -1;
            }
        }
        break;

        case UICCD_TRANS_SERVICE_TABLE_READ_BINARY:
        {
            uiccd_msg_read_sim_file_binary_response_t   * rsp_p;
            uint8_t                                     * data_binary_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_SERVICE_TABLE_READ_BINARY");

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SERVICE_TABLE_READ_BINARY");
                return -1;
            }

            rsp_p = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;

            if (rsp_p->len != 1) {
                catd_log_f(SIM_LOGGING_E, "uicc : one byte only expected from service table");
                rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                         ctrl_p->client_tag,
                                                                         SIM_UICC_STATUS_CODE_FAIL,
                                                                         SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                         status_word);
                free(data_p);
                free(ctrl_p);
                return 0;
            }

            if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {
              switch (data_p->service_type)
              {
                 case SIM_UICC_SERVICE_TYPE_FDN:
                 {
                   if (data_p->enable_service == 1) {
                     // To enable FDN, FDN must be allocated and activated in the service table.
                     if ((*rsp_p->data & SIM_SERVICE_TYPE_FDN_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_FDN_ACTIVATED_ALLOCATED){

                       if ((*rsp_p->data & SIM_SERVICE_TYPE_ADN_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_ADN_ACTIVATED_ALLOCATED){
                         // If ADN is allocated and activated in the service table EF_ADN must be invalidated to enable FDN.

                         data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_APPL_APDU_SEND;

                         rv = uiccd_appl_apdu_send(ctrl_p, EF_ADN_INVALIDATE);

                         if (rv < 0) {
                           rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                                    ctrl_p->client_tag,
                                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                                    status_word);
                           free(data_p);
                           free(ctrl_p);
                           return 0;
                         }
                       }
                       else{
                         // FDN is allocated and activated but ADN is not => FDN is already enabled.
                         catd_log_f(SIM_LOGGING_I, "uicc : FDN service is already enabled in GSM");

                         //Send the result back to client
                         rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                                  ctrl_p->client_tag,
                                                                                  SIM_UICC_STATUS_CODE_OK,
                                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                                                                  status_word);

                         //the transaction is done
                         data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_INIT;

                         free(data_p);
                         free(ctrl_p);
                         return 0;
                       }
                     }
                     else{
                       // The service cannot be enabled since it isn't allocated and activated
                       catd_log_f(SIM_LOGGING_E, "uicc : FDN service can not be enabled in GSM");
                       rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                                ctrl_p->client_tag,
                                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                                STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,
                                                                                status_word);
                       free(data_p);
                       free(ctrl_p);
                       return 0;

                     }
                   } // enable_service == 1

                   else {
                     // Disable FDN.
                     if ((*rsp_p->data & SIM_SERVICE_TYPE_FDN_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_FDN_ACTIVATED_ALLOCATED){

                       if ((*rsp_p->data & SIM_SERVICE_TYPE_ADN_ACTIVATED_ALLOCATED) == SIM_SERVICE_TYPE_ADN_ACTIVATED_ALLOCATED){
                         // If ADN is allocated and activated in the service table EF_ADN must be rehabilitated to disable FDN.

                         data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_APPL_APDU_SEND;

                         rv = uiccd_appl_apdu_send(ctrl_p, EF_ADN_REHABILITATE);

                         if (rv < 0) {
                           rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                                    ctrl_p->client_tag,
                                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                                    status_word);
                           free(data_p);
                           free(ctrl_p);
                           return 0;
                         }
                       }
                       else{
                         // FDN is allocated and activated but ADN is not => FDN cannot be disabled.
                         catd_log_f(SIM_LOGGING_I, "uicc : FDN service cannot be disabled in GSM");

                         //Send the result back to client
                         rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                                  ctrl_p->client_tag,
                                                                                  SIM_UICC_STATUS_CODE_FAIL,
                                                                                  SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,
                                                                                  status_word);

                         //the transaction is done
                         data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_INIT;

                         free(data_p);
                         free(ctrl_p);
                         return 0;
                       }
                     }
                     else{
                       // The service is already disabled since it isn't allocated and activated
                       catd_log_f(SIM_LOGGING_I, "uicc : FDN service is already disabled in GSM");
                       rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                                ctrl_p->client_tag,
                                                                                SIM_UICC_STATUS_CODE_OK,
                                                                                STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
                                                                                status_word);
                       free(data_p);
                       free(ctrl_p);
                       return 0;

                     }
                   }
                 }
                 break;

                 default:
                 {
                    catd_log_f(SIM_LOGGING_E, "uicc : unexpected service type for update service table");
                    rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                             ctrl_p->client_tag,
                                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                             status_word);
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
                   if (data_p->enable_service == 1) {
                     *rsp_p->data = (*rsp_p->data | USIM_SERVICE_TYPE_FDN_ENABLE);
                   }
                   else {
                     *rsp_p->data = (*rsp_p->data & USIM_SERVICE_TYPE_FDN_DISABLE);
                   }
                 }
                 break;

                 case SIM_UICC_SERVICE_TYPE_BDN:
                 {
                   if (data_p->enable_service == 1) {
                     *rsp_p->data = (*rsp_p->data | USIM_SERVICE_TYPE_BDN_ENABLE);
                   }
                   else {
                     *rsp_p->data = (*rsp_p->data & USIM_SERVICE_TYPE_BDN_DISABLE);
                   }
                 }
                 break;

                 case SIM_UICC_SERVICE_TYPE_ACL:
                 {
                   if (data_p->enable_service == 1) {
                     *rsp_p->data = (*rsp_p->data | USIM_SERVICE_TYPE_ACL_ENABLE);
                   }
                   else {
                     *rsp_p->data = (*rsp_p->data & USIM_SERVICE_TYPE_ACL_DISABLE);
                   }
                 }
                 break;

                 default:
                 {
                    catd_log_f(SIM_LOGGING_E, "uicc : unexpected service type for update service table");
                    rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                             ctrl_p->client_tag,
                                                                             SIM_UICC_STATUS_CODE_FAIL,
                                                                             SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                             status_word);
                    free(data_p);
                    free(ctrl_p);
                    return 0;
                 }

               } // switch

               data_binary_p = rsp_p->data;

               data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_UPDATE_BINARY;

               rv = uiccd_update_service_table_service_status(ctrl_p, data_binary_p);

               if (rv < 0) {
                  rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                         ctrl_p->client_tag,
                                                                         SIM_UICC_STATUS_CODE_FAIL,
                                                                         SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                         status_word);
                  free(data_p);
                  free(ctrl_p);
                  return 0;
               }
            }
            else {
              catd_log_f(SIM_LOGGING_E, "uicc : uiccd_update_service_table: Unexpected app_type");
              rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                       ctrl_p->client_tag,
                                                                       SIM_UICC_STATUS_CODE_FAIL,
                                                                       SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                       status_word);
              free(data_p);
              free(ctrl_p);
              return 0;
            }

        }
        break;

        case UICCD_TRANS_SERVICE_TABLE_UPDATE_BINARY:
        {
            uiccd_msg_update_sim_file_binary_response_t   * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_SERVICE_TABLE_UPDATE_BINARY");

            if (ste_msg->type != UICCD_MSG_UPDATE_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_update_BINARY");
                return -1;
            }

            rsp_p = (uiccd_msg_update_sim_file_binary_response_t*)ste_msg;

            //check the update result and send the result back to client
            rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                     ctrl_p->client_tag,
                                                                     rsp_p->uicc_status_code,
                                                                     rsp_p->uicc_status_code_fail_details,
                                                                     rsp_p->status_word);

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_INIT;

            free(data_p);
            free(ctrl_p);
            return 0;
        }
        break;

        case UICCD_TRANS_SERVICE_TABLE_APPL_APDU_SEND:
        {
            uiccd_msg_appl_apdu_send_response_t   * rsp_p;

            catd_log_f(SIM_LOGGING_I, "uicc : Transaction state UICCD_TRANS_SERVICE_TABLE_APPL_APDU_SEND");

            if (ste_msg->type != UICCD_MSG_APPL_APDU_SEND_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_SERVICE_TABLE_APPL_APDU_SEND");
                return -1;
            }

            rsp_p = (uiccd_msg_appl_apdu_send_response_t*)ste_msg;

            //check the update result and send the result back to client
            rv = uiccd_encode_and_send_update_service_table_response(ctrl_p->fd,
                                                                     ctrl_p->client_tag,
                                                                     rsp_p->uicc_status_code,
                                                                     rsp_p->uicc_status_code_fail_details,
                                                                     rsp_p->status_word);

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_INIT;

            free(data_p);
            free(ctrl_p);
            return 0;
        }
        break;



        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for update service table");
            free(data_p);
            free(ctrl_p);
            return 0;
        }
    }
    return -1;
}



/**
 * @brief                  Main function for update service table.
 * @param ste_msg          The original message from client.
 * @return                 Status of the operation. Success: 0; Failed: -1
 *
 * The active status for a service is indicated by specific bit in the service table. In USIM the service table is in file EF-EST (Enable Service Table)
 * and in GSM in file EF-SST (SIM Service Table). To enable/disable a specified service, the corresponding byte in the service table
 * first must be read, the specific bit altered and the service table finally updated with the new value.
 *
 * The transaction implementation includes the handler function for the responses from modem, the user data(named transaction data)
 * needed for the transaction including transaction state, etc., and the transaction id, which is exactly the same as the
 * message type from client. In this way, when a response(for example, read record resp) is received from modem, the response function
 * will check if the transaction id is the same as the resposne message type, if it is the same, then it goes through the old
 * response function. Otherwise it will go through the transaction handler function.
 *
 * For update service table the transaction includes calls to pin verify, read_binary and update_binary or send apdu. Corresponding transaction
 * states are named pin verify, read_binary, update_binary and send apdu and handle the diffrent responses from modem. Before sending the first request
 * to the modem, a ctrl block to save all the transaction related data is created.
 * The typical state transition for this  transation is :
 * USIM: VERIFY PIN -> READ BINARY -> UPDATE BINARY -> DONE
 * SIM: VERIFY PIN -> READ BINARY -> SEND APDU -> DONE
 */
int uiccd_main_update_service_table(ste_msg_t * ste_msg)
{
    ste_modem_t                       * m;
    uiccd_msg_update_service_table_t  * msg = (uiccd_msg_update_service_table_t *) ste_msg;
    ste_sim_ctrl_block_t              * ctrl_p;
    int                               i;
    uiccd_trans_data_service_table_t  * data_p;
    sim_uicc_status_word_t            status_word = { 0, 0 };

    const char *p = msg->data;
    const char *p_max = msg->data + msg->len;
    char       pin[PIN_MAX_LEN];
    unsigned   pin_len = 0;
    sim_uicc_service_type_t service_type;
    uint8_t enable_service;

    catd_log_f(SIM_LOGGING_I, "uicc : UPDATE service table message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        //since this mechanism of calling uiccd_sig_update_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_update_service_table_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word);
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_service_table_t));
    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        //since this mechanism of calling uiccd_sig_update_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_update_service_table_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word);
        return -1;
    }
    memset(data_p, 0, sizeof(uiccd_trans_data_service_table_t));

    //decode the data from client
    p = sim_dec(p, &pin_len, sizeof(pin_len), p_max);

    if ((pin_len > 0) && (pin_len <= PIN_MAX_LEN)) {
       p = sim_dec(p, pin, sizeof(char)*(pin_len), p_max);
    }
    else {
       pin_len = 0;
    }

    p = sim_dec(p, &service_type, sizeof(service_type), p_max);
    p = sim_dec(p, &enable_service, sizeof(enable_service), p_max);

    if ( !p ) {
        // Something went wrong in one of the sim_dec above.
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for update service table transaction failed decoding msg data");
        //since this mechanism of calling uiccd_sig_update_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_update_service_table_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word);
        free(data_p);
        return -1;
    }

    //save the data from client that will be needed later during the transaction
    data_p->service_type = service_type;
    data_p->enable_service = enable_service;

    if (pin_len > 0) {
       data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_PIN_VERIFY;
    }
    else {
       // No PIN provided by client - read data directly.
       data_p->trans_state = UICCD_TRANS_SERVICE_TABLE_READ_BINARY;
    }

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_update_service_table_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for update service table transaction failed");
        //since this mechanism of calling uiccd_sig_update_service_table_response to trigg the send back the result to the client is not working,
        //the result is directly sent back here
        i = uiccd_encode_and_send_update_service_table_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word);
        free(data_p);
        return -1;
    }

    if (pin_len > 0) {
       // PIN2 provided by client

       i = ste_modem_pin_verify(m,
                                (uintptr_t)ctrl_p,
                                uicc_get_app_id(),
                                SIM_UICC_PIN_ID_PIN2,
                                pin,
                                pin_len);

       if (i != 0) {
           //since this mechanism of calling uiccd_sig_update_service_table_response to trigg the send back the result to the client is not working,
           //the result is directly sent back here
           i = uiccd_encode_and_send_update_service_table_response(msg->fd,
                                                                   msg->client_tag,
                                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                   status_word);
           catd_log_f(SIM_LOGGING_E, "uicc : "
                   "uiccd_main_update_service_table failed: pin verifiy failed.");

           //transaction must be ended
           free(data_p);
           free(ctrl_p);
           return -1;
        }
    }
    else {
       // No PIN provided by client - read data directly.

        i = uiccd_read_service_table_service_status(ctrl_p, data_p->service_type);

       if (i != 0) {
           //since this mechanism of calling uiccd_sig_update_service_table_response to trigg the send back the result to the client is not working,
           //the result is directly sent back here
           if (i == -2)
             i = uiccd_encode_and_send_update_service_table_response(msg->fd,
                                                                     msg->client_tag,
                                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,
                                                                     status_word);
           else
             i = uiccd_encode_and_send_update_service_table_response(msg->fd,
                                                                     msg->client_tag,
                                                                     SIM_UICC_STATUS_CODE_FAIL,
                                                                     SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                     status_word);

           catd_log_f(SIM_LOGGING_E, "uicc : "
                   "uiccd_main_update_service_table failed: read service table failed.");

           //transaction must be ended
           free(data_p);
           free(ctrl_p);
           return -1;
        }
    }

    return 0;
}
