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
#include <assert.h>

static int uiccd_encode_and_send_sim_channel_open_response( int fd,
                                                            uintptr_t client_tag,
                                                            sim_uicc_status_code_t uicc_status_code,
                                                            sim_uicc_status_code_fail_details_t uicc_status_code_fail_details,
                                                            sim_uicc_status_word_t status_word,
                                                            uint16_t session_id)
{
    char                    *buf_p = NULL;
    char                    *p = NULL;
    size_t                   buf_len = 0;

    buf_len = sizeof(uicc_status_code);
    buf_len += sizeof(uicc_status_code_fail_details);
    buf_len += sizeof(status_word.sw1) + sizeof(status_word.sw2);
    buf_len += sizeof(session_id);

    buf_p = malloc(buf_len);
    if (buf_p == NULL){
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_sim_channel_open_response, memory allocation failed");
        return -1;
    }
    p = buf_p;

    p = sim_enc(p, &uicc_status_code, sizeof(uicc_status_code));
    p = sim_enc(p, &uicc_status_code_fail_details, sizeof(uicc_status_code_fail_details));
    p = sim_enc(p, &status_word.sw1, sizeof(status_word.sw1));
    p = sim_enc(p, &status_word.sw2, sizeof(status_word.sw2));
    p = sim_enc(p, &session_id, sizeof(session_id));

    if (buf_len != (size_t)(p-buf_p)) {
        free(buf_p);
        return -1;
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_SIM_CHANNEL_OPEN,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_sim_channel_open_response(ste_msg_t * ste_msg)
{
    uiccd_msg_sim_channel_open_response_t* msg;
    ste_sim_ctrl_block_t   * ctrl_p;

    catd_log_f(SIM_LOGGING_D, "uicc : sim_channel_open_response message received");

    msg = (uiccd_msg_sim_channel_open_response_t*)ste_msg;
    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

    uiccd_encode_and_send_sim_channel_open_response( ctrl_p->fd,
                                                     ctrl_p->client_tag,
                                                     msg->uicc_status_code,
                                                     msg->uicc_status_code_fail_details,
                                                     msg->status_word,
                                                     msg->session_id);

    free(ctrl_p);
    return 0;
}

int uiccd_main_sim_channel_open(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sim_channel_open_t *msg = (uiccd_msg_sim_channel_open_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;
    sim_uicc_status_word_t status_word = {0,0};

    catd_log_f(SIM_LOGGING_I, "uicc : SIM Channel Open message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sim_channel_open_response(msg->fd,
                                                        msg->client_tag,
                                                        SIM_UICC_STATUS_CODE_FAIL,
                                                        SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                        status_word,
                                                        0);
        return -1;
    } else {
        int                     i;
        const char             *p = msg->data;
        const char             *p_max = msg->data + msg->len;
        uint32_t                aid_len;
        uint8_t                *aid = NULL;

        p = sim_dec(p, &aid_len, sizeof(aid_len), p_max);
        if ( p == NULL ) {
            uiccd_encode_and_send_sim_channel_open_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            0);
            return -1;
        }

        if ( aid_len > 0 ) {
            aid = malloc(aid_len);
            if ( aid == NULL ) {
                uiccd_encode_and_send_sim_channel_open_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word,
                                                                0);
                return -1;
            }
            p = sim_dec(p, aid, aid_len, p_max);

            if ( !p ) {
                // Something went wrong in one of the sim_dec above.
                uiccd_encode_and_send_sim_channel_open_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_STATUS_CODE_FAIL,
                                                                SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                                status_word,
                                                                0);
                catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_channel_open failed");
                free(aid);
                return -1;
            }
        }

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, NULL, NULL);

        if (!ctrl_p) {
            uiccd_encode_and_send_sim_channel_open_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            0);
            free(aid);
            return -1;
        }

        i = ste_modem_channel_open(m, (uintptr_t)ctrl_p, aid_len, aid);
        free(aid);
        if (i != 0) {
            uiccd_encode_and_send_sim_channel_open_response(msg->fd,
                                                            msg->client_tag,
                                                            SIM_UICC_STATUS_CODE_FAIL,
                                                            SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                            status_word,
                                                            0);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_sig_sim_channel_open failed");
            free(ctrl_p);
            return -1;
        }
    }

    return 0;
}
