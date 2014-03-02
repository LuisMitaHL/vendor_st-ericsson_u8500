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
#include "uicc_state_machine.h"

typedef enum
{
    UICCD_TRANS_READ_PREFERRED_RAT_SETTING_INIT,
    UICCD_TRANS_READ_PREFERRED_RAT_SETTING_READ_TRANSPARENT
} uiccd_trans_read_preferred_rat_state_t;

typedef enum sim_RAT_setting_tag_t
{
  SIM_RAT_RAT_SETTING_NO_PREFERENCES = 0, /**< The SIM has no preferred RAT setting */
  SIM_RAT_SETTING_DUAL_MODE,          /**< SIM mandates that the RAT mode shall be set to dual mode */
  SIM_RAT_SETTING_2G_ONLY,            /**< SIM mandates that the RAT mode shall be set to 2G only */
  SIM_RAT_SETTING_3G_ONLY,            /**< SIM mandates that the RAT mode shall be set to 3G only */
  SIM_RAT_SETTING_ENABLE_ALL_SUPPORTED_RATS /**< SIM mandates that all supported RAT shall be enabled */
} sim_RAT_setting_t;

typedef struct {
    uiccd_trans_smsc_state_t trans_state;
    sim_RAT_setting_t        RAT;
} uiccd_trans_data_read_preferred_rat_t;

static int uiccd_encode_and_send_read_preferred_rat_setting_result(int fd,
                                                                   uintptr_t client_tag,
                                                                   sim_uicc_status_code_t uicc_status_code,
                                                                   sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                                   sim_RAT_setting_t RAT)
{
    char                    *buf_p = NULL;
    char                    *smsc_buf_p = NULL;
    char                    *p = NULL;
    size_t                   smsc_buf_len;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(RAT);

    buf_p = malloc(buf_len);

    if (buf_p == NULL) {
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_preferred_rat_setting_result, memory allocation failed");
        free(smsc_buf_p);
        return -1;
    }

    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &RAT, sizeof(RAT));

    if (buf_len != (size_t)(p-buf_p)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_encode_and_send_read_preferred_rat_setting_result, assert failure");
    }

    assert( buf_len == (size_t)(p - buf_p) );

    sim_send_generic(fd,
                     STE_UICC_RSP_SIM_READ_PREFERRED_RAT_SETTING,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);
    free(smsc_buf_p);

    return 0;
}

int uiccd_read_ef_rat(ste_sim_ctrl_block_t * ctrl_p)
{
    int                 i;
    ste_modem_t       * m;

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        return -1;
    }

    if (uicc_get_card_type() == STE_UICC_CARD_TYPE_ICC) {
        return -2; // Bail since the file is only present on USIM
    }

    i = ste_modem_file_read_binary(m,
                                   (uintptr_t)ctrl_p,
                                   uicc_get_app_id(),
                                   EF_RAT_FILE_ID,
                                   0,
                                   1,
                                   EF_RAT_USIM_PATH);

    if (i != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : "
                   "ste_modem_file_read_binary failed.");
        return -1;
    }

    return i;
}

/**
 * @brief                  Handler function for read preferred rat responses.
 * @param ste_msg          The original message from client.
 * @return                 Status of the transaction. Transaction is DONE: 0; Transaction to be continued: -1
 *
 * All transaction states check the status code. If the status code indicate a failure, the transaction is ended
 * and the error is sent to the client together with the status words received from the SIM I/O operation that failed.
 *
 * Transaction state:
 *
 * 1. UICCD_TRANS_READ_PREFERRED_RAT_SETTING_READ_TRANSPARENT. The EFrat file is decoded and the RAT setting returned to the client.
 *
 */
static int uiccd_read_preferred_rat_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t     * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_read_preferred_rat_t  * data_p;
    int                        result = 0; // Default is to the end the transaction
    int                        rv;

    if (ctrl_p == NULL) {
        return 0;
    }

    data_p = (uiccd_trans_data_read_preferred_rat_t *)(ctrl_p->transaction_data_p);

    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_READ_PREFERRED_RAT_SETTING_READ_TRANSPARENT:
        {
            uiccd_msg_read_sim_file_binary_response_t * rsp_p;
            sim_RAT_setting_t                           RAT = SIM_RAT_RAT_SETTING_NO_PREFERENCES;
            char byte;
            sim_uicc_status_code_t uicc_status_code = SIM_UICC_STATUS_CODE_OK;
            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;

            if (ste_msg->type != UICCD_MSG_READ_SIM_FILE_BINARY_RSP) {
                catd_log_f(SIM_LOGGING_E, "uicc : wrong msg type for transaction state UICCD_TRANS_READ_PREFERRED_RAT_SETTING_READ_TRANSPARENT");
                result = -1; // Continue transaction
                break;
            }

            catd_log_f(SIM_LOGGING_I, "uicc : UICCD_TRANS_READ_PREFERRED_RAT_SETTING_READ_TRANSPARENT");

            rsp_p = (uiccd_msg_read_sim_file_binary_response_t*)ste_msg;

            if (rsp_p->uicc_status_code == SIM_UICC_STATUS_CODE_OK) {
                byte = rsp_p->data[0];

                switch (byte) {
                    case 0x00:
                        RAT = SIM_RAT_SETTING_DUAL_MODE;
                    break;
                    case 0x01:
                        RAT = SIM_RAT_SETTING_2G_ONLY;
                    break;
                    case 0x02:
                        RAT = SIM_RAT_SETTING_3G_ONLY;
                    break;
                    default: // Value between 0x03 and 0xFF shall be translated to SIM_RAT_SETTING_ENABLE_ALL_SUPPORTED_RATS
                        RAT = SIM_RAT_SETTING_ENABLE_ALL_SUPPORTED_RATS;
                    break;
                }
            } else if (! (rsp_p->status_word.sw1 == 0x6a && rsp_p->status_word.sw2 == 0x82)) {
                catd_log_f(SIM_LOGGING_E, "uicc : read preferred rat settings failed to read the EFrat file!");
                uicc_status_code = rsp_p->uicc_status_code;
                uicc_status_code_fail_details = rsp_p->uicc_status_code_fail_details;
            }

            if (uiccd_encode_and_send_read_preferred_rat_setting_result(ctrl_p->fd,
                                                                        ctrl_p->client_tag,
                                                                        uicc_status_code,
                                                                        uicc_status_code_fail_details,
                                                                        RAT) != 0) {
                catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_preferred_rat_setting_result failed");
            }

            //the transaction is done
            data_p->trans_state = UICCD_TRANS_READ_PREFERRED_RAT_SETTING_INIT;
            free(data_p);
            free(ctrl_p);
        }
        break;
        default:
        {
            catd_log_f(SIM_LOGGING_E, "uicc : wrong transaction state for read preferred rat setting = %d",data_p->trans_state);
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
 */
int uiccd_main_sim_read_preferred_rat_setting(ste_msg_t * ste_msg)
{
    ste_modem_t              * m;
    uiccd_msg_sim_read_preferred_rat_setting_t *msg = (uiccd_msg_sim_read_preferred_rat_setting_t *) ste_msg;
    ste_sim_ctrl_block_t     * ctrl_p;
    int                        result;
    uiccd_trans_data_read_preferred_rat_t * data_p;

    catd_log_f(SIM_LOGGING_I, "uicc : READ_PREFERRED_RAT_SETTING message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        if (uiccd_encode_and_send_read_preferred_rat_setting_result(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_RAT_RAT_SETTING_NO_PREFERENCES) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_preferred_rat_setting_result failed");
        }
        return -1;
    }

    data_p = malloc(sizeof(uiccd_trans_data_read_preferred_rat_t));

    if (!data_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : memory allocation failed");
        if (uiccd_encode_and_send_read_preferred_rat_setting_result(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_RAT_RAT_SETTING_NO_PREFERENCES) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_preferred_rat_setting_result failed");
        }
        return -1;
    }

    memset(data_p, 0, sizeof(uiccd_trans_data_read_preferred_rat_t));

    ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_read_preferred_rat_transaction_handler, data_p);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : create ctrl block for read referred rat setting transaction failed");
        if (uiccd_encode_and_send_read_preferred_rat_setting_result(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_STATUS_CODE_FAIL,
                                                                    SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                    SIM_RAT_RAT_SETTING_NO_PREFERENCES) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_preferred_rat_setting_result failed");
        }
        free(data_p);
        return -1;
    }

    //read the transparent EFrat file
    data_p->trans_state = UICCD_TRANS_READ_PREFERRED_RAT_SETTING_READ_TRANSPARENT;
    result = uiccd_read_ef_rat(ctrl_p);

    if (result < 0) {
        sim_uicc_status_code_t uicc_status_code = SIM_UICC_STATUS_CODE_FAIL;
        sim_uicc_status_code_fail_details_t uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
        sim_RAT_setting_t RAT = SIM_RAT_RAT_SETTING_NO_PREFERENCES;

        if (result == -2) { // File not available on ICC cards, therefore OK with no preferences.
            uicc_status_code = SIM_UICC_STATUS_CODE_OK;
            uicc_status_code_fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        }

        //the transaction must be ended since an error has occured
        if (uiccd_encode_and_send_read_preferred_rat_setting_result(ctrl_p->fd,
                                                                    ctrl_p->client_tag,
                                                                    uicc_status_code,
                                                                    uicc_status_code_fail_details,
                                                                    RAT) != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_read_preferred_rat_setting_result failed");
        }
        free(data_p);
        free(ctrl_p);
        return result;
    }
    return 0;
}
