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
#include "uicc_state_machine.h"

// 0 = no pending req, 1 = pending
static int pending_pin_req = 0;
static int pending_pin_unblock_req = 0;
static int pending_pin_perm_blocked_ind = 0;

static int              reg_fd = -1;    // Registered fd to get proactive commands, -1 is none

// -----------------------------------------------------------------------------
// Register

/*
 * Process register message. So far nothing is done excepting sending a
 * response back.
 * Deprecated: This function is not used anymore. Will be removed in future revisions.
 */
void uiccd_main_register(ste_msg_t * ste_msg)
{
    uiccd_msg_register_t   *msg = (uiccd_msg_register_t *) ste_msg;
    uintptr_t client_tag;
    catd_log_f(SIM_LOGGING_I, "uicc : REGISTER message received fd=%d", msg->fd);

    client_tag = msg->client_tag;

    if (reg_fd != -1) {
        // Some other has already registered, so there must be a mistake
        // FIXME:  Send status failed here.
        sim_send_generic(msg->fd, STE_UICC_RSP_REGISTER, 0, 0, client_tag);
    } else {
      // Registered client gets pending pin required
      reg_fd = msg->fd;
      sim_send_generic(reg_fd, STE_UICC_RSP_REGISTER, 0, 0, client_tag);
    }

    // Send out any saved stuff
    if (pending_pin_req) {
      sim_send_generic(reg_fd, STE_UICC_REQ_PIN_NEEDED, 0, 0, client_tag);
      pending_pin_req = 0;
    }

    if (pending_pin_unblock_req) {
      char *buf = "pin_unblock_needed";
      size_t buf_len = strlen(buf);

      catd_log_f(SIM_LOGGING_I, "uicc : Sending STE_UICC_SIM_STATE_CHANGED event: pin_unblock_needed, fd=%d", reg_fd);

      sim_send_generic(reg_fd,
                       STE_UICC_SIM_STATE_CHANGED,
                       buf,
                       buf_len,
                       client_tag);

      pending_pin_unblock_req = 0;
    }

    if (pending_pin_perm_blocked_ind) {
     char *buf = "pin_permanently_blocked";
     size_t buf_len = strlen(buf);

     catd_log_f(SIM_LOGGING_I, "uicc : Sending STE_UICC_SIM_STATE_CHANGED event:pin_permanently_blocked, fd=%d", reg_fd);

     sim_send_generic(reg_fd,
                      STE_UICC_SIM_STATE_CHANGED,
                      buf,
                      buf_len,
                      client_tag);

      pending_pin_perm_blocked_ind = 0;
    }

}

static int uiccd_encode_and_send_appl_apdu_send_response(int fd,
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
        catd_log_f(SIM_LOGGING_E, "uicc : uiccd_encode_and_send_appl_apdu_send_response, memory allocation failed");
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
                     STE_UICC_RSP_APPL_APDU_SEND,
                     buf_p,
                     buf_len,
                     client_tag);

    free(buf_p);

    return 0;
}


// TODO: Implement the function uiccd_main_appl_apdu_send completely
int uiccd_main_appl_apdu_send(ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_appl_apdu_send_t *msg = (uiccd_msg_appl_apdu_send_t *) ste_msg;
    sim_uicc_status_word_t status_word = {0,0};
 // Enter busy state...

    catd_log_f(SIM_LOGGING_I, "uicc : APPL APDU SEND message received from: %d ",msg->fd);

    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_appl_apdu_send_response(msg->fd,
                                                      msg->client_tag,
                                                      SIM_UICC_STATUS_CODE_FAIL,
                                                      SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                      status_word);
        return -1;
    } else {
        catd_log_f(SIM_LOGGING_E, "uicc : Function uiccd_main_appl_apdu_send to be implemented");
        uiccd_encode_and_send_appl_apdu_send_response(msg->fd,
                                                      msg->client_tag,
                                                      SIM_UICC_STATUS_CODE_FAIL,
                                                      SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR,
                                                      status_word);
        return -1;
    }
}

int uiccd_main_appl_apdu_send_response(ste_msg_t * ste_msg)
{
    uiccd_msg_appl_apdu_send_response_t* msg;
    char                    *p = NULL;
    char                    *buf = NULL;
    size_t                  buf_len;
    size_t                  data_len;
    ste_sim_ctrl_block_t *  ctrl_p;

    msg = (uiccd_msg_appl_apdu_send_response_t*)ste_msg;
    ctrl_p = (ste_sim_ctrl_block_t*)(msg->client_tag);

    if (!ctrl_p) {
        catd_log_f(SIM_LOGGING_E, "uicc : Control Block is NULL");
        return -1;
    }

    catd_log_f(SIM_LOGGING_I, "uicc : APPL APDU SEND response message received");

    data_len = msg->len;

    buf_len = sizeof(char) * data_len;
    buf_len += sizeof(data_len);
    buf_len += sizeof(msg->status_word.sw1) + sizeof(msg->status_word.sw2);
    buf_len += sizeof(msg->uicc_status_code) + sizeof(msg->uicc_status_code_fail_details);

    buf = malloc(buf_len);
    if (!buf) {
      catd_log_f(SIM_LOGGING_E, "uicc : Could not allocate memory in uiccd_main_appl_apdu_send_response");
      return -1;
    }
    p = buf;

    p = sim_enc(p, &msg->uicc_status_code, sizeof(msg->uicc_status_code));
    p = sim_enc(p, &msg->uicc_status_code_fail_details, sizeof(msg->uicc_status_code_fail_details));
    p = sim_enc(p, &msg->status_word.sw1, sizeof(msg->status_word.sw1));
    p = sim_enc(p, &msg->status_word.sw2, sizeof(msg->status_word.sw2));

    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, msg->data,      sizeof(char)*(data_len));

    if (buf_len != (size_t)(p-buf)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_appl_apdu_send_response, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    sim_send_generic(ctrl_p->fd,
                     STE_UICC_RSP_APPL_APDU_SEND,
                     buf,
                     buf_len,
                     ctrl_p->client_tag);

    free(ctrl_p);
    free( buf );
    return 0;
}
