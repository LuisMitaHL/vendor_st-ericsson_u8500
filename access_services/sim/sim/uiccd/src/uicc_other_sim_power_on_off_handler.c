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
#include "catd.h"
#include "uicc_interface_handlers.h"


static int uiccd_sim_power_on_off_send_response( int fd,
                                                 uintptr_t client_tag,
                                                 sim_uicc_status_code_t uicc_status_code,
                                                 sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                 sim_uicc_status_word_t status_word,
                                                 ste_uicc_rsp_sim_t rsp_id)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);

    buf_p = malloc(buf_len);

    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_sim_power_on_off_send_response, memory allocation failed");
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
                     rsp_id,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_sim_power_on(const ste_msg_t *ste_msg)
{
    ste_modem_t *m;
    sim_uicc_status_word_t status_word = {0,0};
    sim_uicc_status_code_t status_code = SIM_UICC_STATUS_CODE_OK;
    sim_uicc_status_code_fail_details_t fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    uiccd_msg_sim_power_on_t *msg = (uiccd_msg_sim_power_on_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;

    catd_log_f(SIM_LOGGING_I, "uiccd_main_sim_power_on: Enter");

    // Call mal method to connect sim card
    m = catd_get_modem();

    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        status_code = SIM_UICC_STATUS_CODE_FAIL;
        fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
        goto error;
    } else {
        int i;

        ste_modem_reset_backend_state(m, 0); // Reset the backend state

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, NULL, NULL);

        if (!ctrl_p) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_power_on: failed to create ctrl block.");
            status_code = SIM_UICC_STATUS_CODE_FAIL;
            fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            goto error;
        }

        i = ste_modem_sim_connect(m, (uintptr_t)ctrl_p);
        if (i != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_power_on: failed to connect sim.");
            status_code = SIM_UICC_STATUS_CODE_FAIL;
            fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            goto error;
        }
    }

    return 0;
error:
    // Set status_code right before getting here.
    (void)uiccd_sim_power_on_off_send_response(((uiccd_msg_fd_t *)msg)->fd,
                                               msg->client_tag,
                                               status_code,
                                               fail_details,
                                               status_word,
                                               STE_UICC_RSP_SIM_POWER_ON);
     return -1;
}

int uiccd_main_sim_power_off(const ste_msg_t *ste_msg)
{
    ste_modem_t *m;
    sim_uicc_status_word_t status_word = {0,0};
    sim_uicc_status_code_t status_code = SIM_UICC_STATUS_CODE_OK;
    sim_uicc_status_code_fail_details_t fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
    uiccd_msg_sim_power_off_t *msg = (uiccd_msg_sim_power_off_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;

    catd_log_f(SIM_LOGGING_I, "uiccd_main_sim_power_off: Enter");

    // Call mal method to connect sim card
    m = catd_get_modem();

    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        status_code = SIM_UICC_STATUS_CODE_FAIL;
        fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
        goto error;
    } else {
        int i;

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, NULL, NULL);

        if (!ctrl_p) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_power_off: failed to create ctrl block.");
            status_code = SIM_UICC_STATUS_CODE_FAIL;
            fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            goto error;
        }

        i = ste_modem_sim_disconnect(m, (uintptr_t)ctrl_p);
        if (i != 0) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_power_off: failed to disconnect sim.");
            status_code = SIM_UICC_STATUS_CODE_FAIL;
            fail_details = SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR;
            goto error;
        }
    }

    return 0;
error:
    // Set status_code right before getting here.
    (void)uiccd_sim_power_on_off_send_response(((uiccd_msg_fd_t *)msg)->fd,
                                               msg->client_tag,
                                               status_code,
                                               fail_details,
                                               status_word,
                                               STE_UICC_RSP_SIM_POWER_OFF);
     return -1;
}

int uiccd_main_sim_power_on_response(ste_msg_t * ste_msg)
{
    uiccd_msg_sim_connect_response_t* msg;
    ste_sim_ctrl_block_t   *ctrl_p;

    msg = (uiccd_msg_sim_connect_response_t*)ste_msg;

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    catd_log_f(SIM_LOGGING_I, "uicc : sim connect response message received");
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
    uiccd_msr_cache_pin();
#endif

    (void)uiccd_sim_power_on_off_send_response(ctrl_p->fd,
                                               ctrl_p->client_tag,
                                               msg->uicc_status_code,
                                               msg->uicc_status_code_fail_details,
                                               msg->status_word,
                                               STE_UICC_RSP_SIM_POWER_ON);

    free(ctrl_p);

    return 0;
}

int uiccd_main_sim_power_off_response(ste_msg_t * ste_msg)
{
    uiccd_msg_sim_disconnect_response_t* msg;
    ste_sim_ctrl_block_t   *ctrl_p;

    msg = (uiccd_msg_sim_disconnect_response_t*)ste_msg;

    if (SIM_UICC_STATUS_CODE_OK == msg->uicc_status_code)
    {
        //inform CATD about the power off
        catd_sig_sim_power_on_off(0, 0);
    }

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    catd_log_f(SIM_LOGGING_E, "uicc : sim disconnect response message received");
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

    (void)uiccd_sim_power_on_off_send_response(ctrl_p->fd,
                                               ctrl_p->client_tag,
                                               msg->uicc_status_code,
                                               msg->uicc_status_code_fail_details,
                                               msg->status_word,
                                               STE_UICC_RSP_SIM_POWER_OFF);

    free(ctrl_p);

    return 0;
}

int uiccd_main_disconnect_uicc_status_ind(uiccd_msg_uicc_status_ind_t *msg)
{
    int rv = 0;

    catd_log_f(SIM_LOGGING_I, "uicc : sim disconnect state: uicc status ind received: %d", msg->uicc_status);
    switch (msg->uicc_status) {
    case STE_UICC_STATUS_DISCONNECTED_CARD:
        catd_log_f(SIM_LOGGING_I, "uicc : SIM is disconnected.");
        break;

    default:
        catd_log_f(SIM_LOGGING_E, "uicc : invalid uicc status ind in disconnect state");
        rv = -1;
        break;
    }

    return rv;
}
