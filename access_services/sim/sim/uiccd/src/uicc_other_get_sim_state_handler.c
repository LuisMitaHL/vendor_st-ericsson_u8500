/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "msgq.h"
#include "state_machine.h"
#include "sim_internal.h"
#include "uiccd_msg.h"
#include "simd.h"
#include "uicc_internal.h"
#include "uicc_state_machine.h"
#include "catd_modem_types.h"

static struct {
    int          fd;
    uintptr_t    ct;
    char         *buf;
    size_t       buf_len;
} s_uiccd_deferred_sim_ready_state = {
    -1,
     0,
     0,
     0
};

static int uicc_startup_completed = 0;

//SIM State
void uiccd_main_get_sim_state(ste_msg_t * ste_msg, const ste_sim_state_t  *state)
{
    char *p = NULL;
    char *buf = NULL;
    size_t buf_len = 0;
    size_t status_len = 0;
    size_t data_len = 0;
    uint32_t status = 0;
    const char *state_name = NULL;
    ste_uicc_status_t uicc_status;
    uiccd_msg_get_sim_state_t *msg = (uiccd_msg_get_sim_state_t *) ste_msg;
    ste_modem_t * m = catd_get_modem();
    int cpin_ready = 0;

    status_len = sizeof(status);

    if (!strcmp(state->ss_name, "init")) {
        state_name = "init";
    } else if (!strcmp(state->ss_name, "disconnect")) {
        state_name = "card_disconnected";
    } else if (!strcmp(state->ss_name, "sap")) {
        state_name = "card_disconnected";
    } else if (!strcmp(state->ss_name, "closed")) {
        ste_modem_uicc_status_info(m, &uicc_status);
        switch (uicc_status) {
            case STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK:
                state_name = "card_sim_locked";
                break;
            case STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00:
                state_name = "card_sim_consecutive_6f00_lock";
                break;
            case STE_UICC_STATUS_REMOVED_CARD:
            case STE_UICC_STATUS_NO_CARD:
                state_name = "no_card";
                break;
            case STE_UICC_STATUS_DISCONNECTED_CARD:
                state_name = "card_disconnected";
                break;
            case STE_UICC_STATUS_REJECTED_CARD_INVALID:
                state_name = "card_invalid";
                break;
            case STE_UICC_STATUS_CLOSED:
            default:
                state_name = "closed";
                break;
        }
    } else if (!strcmp(state->ss_name, "idle")) {
        ste_modem_uicc_status_info(m, &uicc_status);
        switch (uicc_status) {
            case STE_UICC_STATUS_PIN_NEEDED:
                state_name = "pin_needed";
                break;
            case STE_UICC_STATUS_PUK_NEEDED:
                state_name = "puk_needed";
                break;
            case STE_UICC_STATUS_PIN2_NEEDED:
                state_name = "pin2_needed";
                break;
            case STE_UICC_STATUS_PUK2_NEEDED:
                state_name = "puk2_needed";
                break;
            case STE_UICC_STATUS_PIN_VERIFIED:
            case STE_UICC_STATUS_PIN2_VERIFIED:
            case STE_UICC_STATUS_READY:
            default:
                state_name = "ready";
                cpin_ready = 1;
                break;
        }
    } else {
        state_name = "unknown";
    }

    data_len = strlen(state_name) + 1;

    buf_len = sizeof(char) * (status_len + data_len);
    buf_len += sizeof(status_len) + sizeof(data_len);

    buf = malloc(buf_len);
    if (!buf) {
      catd_log_f(SIM_LOGGING_E, "uicc : Could not allocate memory in uiccd_main_get_sim_state");
      return;
    }
    p = buf;

    p = sim_enc(p, &status_len, sizeof(status_len));
    p = sim_enc(p, &status,      sizeof(char)*(status_len));

    p = sim_enc(p, &data_len, sizeof(data_len));
    p = sim_enc(p, state_name,      sizeof(char)*(data_len));

    if (buf_len != (size_t)(p-buf)) {
        catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_get_sim_state, assert failure");
    }

    assert( buf_len == (size_t)(p-buf) );

    if (cpin_ready && !uicc_startup_completed) {
        catd_log_f(SIM_LOGGING_D, "uicc: Deffered call to response handler");
        s_uiccd_deferred_sim_ready_state.fd = msg->fd;
        s_uiccd_deferred_sim_ready_state.buf = malloc(buf_len);
        if (!s_uiccd_deferred_sim_ready_state.buf) {
            catd_log_f(SIM_LOGGING_E, "uicc : Could not allocate memory in uiccd_main_get_sim_state");
            free( buf );
            return;
        }
        memcpy(s_uiccd_deferred_sim_ready_state.buf,buf,buf_len);
        s_uiccd_deferred_sim_ready_state.buf_len = buf_len;
        s_uiccd_deferred_sim_ready_state.ct = msg->client_tag;
    } else {
        sim_send_generic(msg->fd,
                     STE_UICC_RSP_GET_SIM_STATE,
                     buf,
                     buf_len,
                     msg->client_tag);
    }

    free( buf );
}

void uiccd_received_startup_complete()
{

    catd_log_f(SIM_LOGGING_D, "uicc: uiccd_received_startup_complete");

    uicc_startup_completed = 1;

    if (s_uiccd_deferred_sim_ready_state.fd == -1 ||
        s_uiccd_deferred_sim_ready_state.buf == NULL ||
        s_uiccd_deferred_sim_ready_state.buf_len == 0) {
        return;
    }

    catd_log_f(SIM_LOGGING_D, "uicc: uiccd_received_startup_complete- Calling GET_SIM_STATE response");

    sim_send_generic(s_uiccd_deferred_sim_ready_state.fd,
                 STE_UICC_RSP_GET_SIM_STATE,
                 s_uiccd_deferred_sim_ready_state.buf,
                 s_uiccd_deferred_sim_ready_state.buf_len,
                 s_uiccd_deferred_sim_ready_state.ct);

    free(s_uiccd_deferred_sim_ready_state.buf);
    s_uiccd_deferred_sim_ready_state.buf_len = 0;
    s_uiccd_deferred_sim_ready_state.fd = -1;

}

void uiccd_reset_startup_complete()
{
    uicc_startup_completed = 0;
}

