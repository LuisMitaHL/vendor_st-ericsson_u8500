/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdint.h>
#include <assert.h>
#include "catd_modem_types.h"
#include "sim_internal.h"
#include "uicc_internal.h"
#include "simd.h"
#include "uiccd_msg.h"
#include "catd_modem.h"
#include "uicc_state_machine.h"
#include "uicc_file_paths.h"

typedef enum {
    UICCD_TRANS_PIN_DISABLE_READ_BINARY,
    UICCD_TRANS_PIN_DISABLE
} uiccd_trans_pin_disable_state_t;

typedef struct {
    uiccd_trans_pin_disable_state_t trans_state;
    char*                           data_p;
    size_t                          len;
} efsst_sim_info_t;

static int uiccd_encode_and_send_pin_disable_response(int fd,
                                                      uintptr_t client_tag,
                                                      sim_uicc_status_code_t uicc_status_code,
                                                      sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                      sim_uicc_status_word_t status_word)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_pin_disable_response, memory allocation failed");
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
                     STE_UICC_RSP_PIN_DISABLE,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_pin_disable_response(ste_msg_t * ste_msg)
{
    uiccd_msg_pin_disable_response_t* msg;
    ste_sim_ctrl_block_t   *ctrl_p;

    msg = (uiccd_msg_pin_disable_response_t*)ste_msg;

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    catd_log_f(SIM_LOGGING_I, "uicc : PIN disable response message received");
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
    if (ctrl_p->transaction_data_p) {
        efsst_sim_info_t *pin_info_p = (efsst_sim_info_t *) ctrl_p->transaction_data_p;

        //check the status code and pin_id
        if (SIM_UICC_STATUS_CODE_OK == msg->uicc_status_code) {
            // reset the local PIN cache.
            uiccd_msr_reset_cached_pin();
        }
    }
#endif

    uiccd_encode_and_send_pin_disable_response(ctrl_p->fd,
                                               ctrl_p->client_tag,
                                               msg->uicc_status_code,
                                               msg->uicc_status_code_fail_details,
                                               msg->status_word);
    return 0;
}

static int uiccd_main_sim_read_sst_file_transaction_handler(ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    ste_sim_ctrl_block_t     * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    efsst_sim_info_t *data_p = (efsst_sim_info_t *) ctrl_p->transaction_data_p;
    sim_uicc_status_word_t status_word = {0,0};
    int i = 0;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_pin_disable_response(ctrl_p->fd,
                                                   ctrl_p->client_tag,
                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                   status_word);
        free(data_p->data_p);
        data_p->data_p = NULL;
        free(data_p);
        data_p = NULL;
        return -1;
    }

    switch (data_p->trans_state) {
        case UICCD_TRANS_PIN_DISABLE_READ_BINARY: {

            catd_log_f(SIM_LOGGING_D,"uicc : transaction state UICCD_TRANS_PIN_DISABLE_READ_BINARY.");

            uiccd_msg_read_sim_file_binary_response_t* msg = (uiccd_msg_read_sim_file_binary_response_t*) ste_msg;

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E,
                           "uicc : wrong msg type for transaction state UICCD_TRANS_PIN_DISABLE_READ_BINARY");
                free(data_p->data_p);
                data_p->data_p = NULL;
                free(data_p);
                data_p = NULL;
                free(ctrl_p);
                return -1;
            }

            // Check if CHV1 is allocated and activated, if so first two bits should be set to 1
            // and we will call ste_modem_pin_disable.
            if ((msg->uicc_status_code != SIM_UICC_STATUS_CODE_OK) ||
                    (msg->uicc_status_code == SIM_UICC_STATUS_CODE_OK && ((*(msg->data) & 0x03)) == 0x03)) {

                catd_log_f(SIM_LOGGING_D, "uicc : CHV1 allocated and activated!");
                data_p->trans_state = UICCD_TRANS_PIN_DISABLE;

                i = ste_modem_pin_disable(m,
                                          (uintptr_t)ctrl_p,
                                          uicc_get_app_id(),
                                          SIM_UICC_PIN_ID_PIN1,
                                          data_p->data_p,
                                          data_p->len);
                if (i != 0) {
                    uiccd_encode_and_send_pin_disable_response(ctrl_p->fd,
                                                               ctrl_p->client_tag,
                                                               SIM_UICC_STATUS_CODE_FAIL,
                                                               SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                               status_word);
                    catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_pin_disable failed");
                    free(data_p->data_p);
                    data_p->data_p = NULL;
                    free(data_p);
                    data_p = NULL;
                    free(ctrl_p);
                    return -1;
                }
            } else {
                uiccd_encode_and_send_pin_disable_response(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           SIM_UICC_STATUS_CODE_FAIL,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                           status_word);
                catd_log_f(SIM_LOGGING_E, "uicc : CHV1 not allocated or activated");
                free(data_p->data_p);
                data_p->data_p = NULL;
                free(data_p);
                data_p = NULL;
                free(ctrl_p);
                return -1;
            }
            return 1;
        }
        break;
        case UICCD_TRANS_PIN_DISABLE: {
            catd_log_f(SIM_LOGGING_D,"uicc : transaction state UICCD_TRANS_PIN_DISABLE.");
            uiccd_main_pin_disable_response(ste_msg);

            //Clean up
            free(data_p->data_p);
            data_p->data_p = NULL;
            free(data_p);
            data_p = NULL;
            free(ctrl_p);
        }
        break;
        default: {
            catd_log_f(SIM_LOGGING_E,
                       "uicc : wrong transaction state for read service table file = %d",
                       data_p->trans_state);
            free(data_p);
            free(ctrl_p);
            return -1;
        }
    }

    return 0;
}

int uiccd_main_pin_disable(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_pin_disable_t *msg = (uiccd_msg_pin_disable_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;
    sim_uicc_status_word_t status_word = {0,0};

    catd_log_f(SIM_LOGGING_I, "uicc : PIN disable message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_pin_disable_response(msg->fd,
                                                   msg->client_tag,
                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                   status_word);
        return -1;
    } else {
        int i = -1;
        efsst_sim_info_t* sst_info;

        //Store data to be transfered in ctrl block for transaction_handler.
        sst_info = malloc(sizeof(efsst_sim_info_t));
        sst_info->len = msg->len;
        sst_info->data_p = malloc(sst_info->len);
        memcpy(sst_info->data_p, msg->data, sst_info->len);

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, UICCD_MSG_PIN_DISABLE_TRANSACTION, uiccd_main_sim_read_sst_file_transaction_handler, sst_info);

        if (!ctrl_p) {
            uiccd_encode_and_send_pin_disable_response(msg->fd,
                                                       msg->client_tag,
                                                       SIM_UICC_STATUS_CODE_FAIL,
                                                       SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                       status_word);
            free(sst_info->data_p);
            sst_info->data_p = NULL;
            free(sst_info);
            sst_info = NULL;
            return -1;
        }

        //Check if SIM application, if so then send efSST info in the transaction data
        //otherwise dont use transaction data.
        if (uicc_get_app_type() == STE_SIM_APP_TYPE_SIM) {

            sst_info->trans_state = UICCD_TRANS_PIN_DISABLE_READ_BINARY;

            catd_log_f(SIM_LOGGING_D, "uicc : SIM application, read sst file");
            //The sst file only valid on SIM application not USIM. For USIM just continue
            //and disable the PIN.
            i = ste_modem_file_read_binary(m,
                                           (uintptr_t)ctrl_p,
                                           uicc_get_app_id(),
                                           EF_SST_FILE_ID,
                                           0,
                                           1,
                                           EF_SST_GSM_PATH);

            if (i != 0) {
                uiccd_encode_and_send_pin_disable_response(msg->fd,
                                                           msg->client_tag,
                                                           SIM_UICC_STATUS_CODE_FAIL,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                           status_word);
                catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_file_read_binary failed");
                free(sst_info->data_p);
                sst_info->data_p = NULL;
                free(sst_info);
                sst_info = NULL;
                free(ctrl_p);
                return -1;
            }
        } else { //USIM application
            catd_log_f(SIM_LOGGING_D, "uicc : USIM application, call ste_modem_pin_disable");

            sst_info->trans_state = UICCD_TRANS_PIN_DISABLE;

            i = ste_modem_pin_disable(m,
                                      (uintptr_t)ctrl_p,
                                      uicc_get_app_id(),
                                      SIM_UICC_PIN_ID_PIN1,
                                      sst_info->data_p,
                                      sst_info->len);
            if (i != 0) {
                uiccd_encode_and_send_pin_disable_response(ctrl_p->fd,
                                                           ctrl_p->client_tag,
                                                           SIM_UICC_STATUS_CODE_FAIL,
                                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                           status_word);
                catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_pin_disable failed");
                free(ctrl_p);
                return -1;
            }
        }
    }

    return 0;
}

