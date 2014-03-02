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

static int uiccd_encode_and_send_pin_unblock_response( int fd,
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
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_pin_unblock_response, memory allocation failed");
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
                     STE_UICC_RSP_PIN_UNBLOCK,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}

int uiccd_main_pin_unblock(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_pin_unblock_t *msg = (uiccd_msg_pin_unblock_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;
    sim_uicc_status_word_t status_word = {0,0};

    catd_log_f(SIM_LOGGING_I, "uicc : Pin_unblock message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_pin_unblock_response(msg->fd,
                                                   msg->client_tag,
                                                   SIM_UICC_STATUS_CODE_FAIL,
                                                   SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                   status_word);
        return -1;
    } else {
        int                     i;
        const char             *p = msg->data;
        const char             *p_max = msg->data + msg->len;
        char                    pin[PIN_MAX_LEN];
        unsigned                pin_len;
        char                    puk[PIN_MAX_LEN];
        unsigned                puk_len;
        sim_uicc_pin_id_t       pin_id;
        ste_uicc_pin_info_t    *pin_info_p = NULL;

        // split msg_data into new and old pin
        p = sim_dec(p, &pin_len, sizeof(pin_len), p_max);
        p = sim_dec(p, pin, sizeof(char)*(pin_len), p_max);

        p = sim_dec(p, &puk_len, sizeof(puk_len), p_max);
        p = sim_dec(p, puk, sizeof(char)*(puk_len), p_max);
        p = sim_dec(p, &pin_id, sizeof(pin_id), p_max);

        if ( !p ) {
            // Something went wrong in one of the sim_dec above.
            uiccd_sig_pin_unblock_response(msg->client_tag,
                                           SIM_UICC_STATUS_CODE_FAIL,
                                           SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                           status_word);
            catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_pin_unblock failed");
            return -1;
        }

        pin_info_p = (ste_uicc_pin_info_t*)calloc(1, sizeof(*pin_info_p));
        if (!pin_info_p) {
            uiccd_encode_and_send_pin_unblock_response(msg->fd,
                                                       msg->client_tag,
                                                       SIM_UICC_STATUS_CODE_FAIL,
                                                       SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                       status_word);
            return -1;
        }

        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, NULL, pin_info_p);
        if (!ctrl_p) {
            uiccd_encode_and_send_pin_unblock_response(msg->fd,
                                                       msg->client_tag,
                                                       SIM_UICC_STATUS_CODE_FAIL,
                                                       SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                       status_word);
            free(pin_info_p);
            return -1;
        }

        pin_info_p->pin_id = pin_id;
        pin_info_p->pin_len = pin_len;
        strncpy(pin_info_p->pin, pin, PIN_MAX_LEN);

        i = ste_modem_pin_unblock(m,
                                  (uintptr_t)ctrl_p,
                                  uicc_get_app_id(),
                                  pin_id,
                                  pin,
                                  pin_len,
                                  puk,
                                  puk_len);
        if (i != 0) {
            uiccd_encode_and_send_pin_unblock_response(msg->fd,
                                                       msg->client_tag,
                                                       SIM_UICC_STATUS_CODE_FAIL,
                                                       SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                       status_word);
            catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_pin_unblock failed");
            free(pin_info_p);
            free(ctrl_p);
            return -1;
        }
    }

    return 0;
}


int uiccd_main_pin_unblock_response(ste_msg_t * ste_msg)
{
    uiccd_msg_pin_unblock_response_t* msg;
    char                   *p = NULL;
    char                   *buf = NULL;
    size_t                  buf_len;
    ste_sim_ctrl_block_t   *ctrl_p;

    msg = (uiccd_msg_pin_unblock_response_t*)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : pin_unblock response message received");

    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);
    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
    if (ctrl_p->transaction_data_p) {
        ste_uicc_pin_info_t    *pin_info_p = (ste_uicc_pin_info_t*)(ctrl_p->transaction_data_p);

        //check the status code and pin_id
        if (pin_info_p->pin_id != SIM_UICC_PIN_ID_PIN2 &&
                SIM_UICC_STATUS_CODE_OK == msg->uicc_status_code) {
            uiccd_msr_set_cached_pin(pin_info_p->pin, pin_info_p->pin_len);
        }
    }
#endif

    buf_len = sizeof(msg->uicc_status_code);
    buf_len += sizeof(msg->uicc_status_code_fail_details);
    buf_len += sizeof(msg->status_word.sw1) + sizeof(msg->status_word.sw2);

    buf = malloc(buf_len);
    if (!buf) {
      catd_log_f(SIM_LOGGING_E, "uicc : Could not allocate memory in uiccd_main_pin_unblock_response");
      free(ctrl_p->transaction_data_p);
      free(ctrl_p);
      return -1;
    }
    p = buf;

    p = sim_enc(p, &msg->uicc_status_code, sizeof(msg->uicc_status_code));
    p = sim_enc(p, &msg->uicc_status_code_fail_details, sizeof(msg->uicc_status_code_fail_details));
    p = sim_enc(p, &msg->status_word.sw1, sizeof(msg->status_word.sw1));
    p = sim_enc(p, &msg->status_word.sw2, sizeof(msg->status_word.sw2));

    if (buf_len != (size_t)(p-buf)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_pin_unblock_response, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    sim_send_generic(ctrl_p->fd,
                     STE_UICC_RSP_PIN_UNBLOCK, buf, buf_len, ctrl_p->client_tag);

    free(buf);
    free(ctrl_p->transaction_data_p);
    free(ctrl_p);
    return 0;
}
