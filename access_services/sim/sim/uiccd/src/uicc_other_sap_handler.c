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
#include "uicc_other_sap_common.h"
#include "catd_cn_client.h"
#include "cn_client.h"

static int uiccd_state_busy_resp_fd;
extern int uiccd_sap_requestor_fd;
extern uiccd_sap_substate_t uiccd_sap_substate;
static cn_context_t *cn_context_p = NULL;

//control data for read SMSC transaction
typedef enum {
    UICCD_TRANS_GET_CURRENT_CALL_LIST
} uiccd_trans_disconnect_state_t;

typedef struct {
    uiccd_trans_disconnect_state_t trans_state;
} uiccd_trans_data_disconnect_t;

int uiccd_encode_and_send_sim_disconnect_response(int fd,
                                                  uintptr_t client_tag,
                                                  sim_uicc_status_code_t uicc_status_code)
{
    sim_send_generic(fd,
                     STE_UICC_RSP_SAP_SESSION_START,
                     (char*)&uicc_status_code,
                     sizeof(uicc_status_code),
                     client_tag);

    return 0;
}

void uiccd_encode_and_send_sap_event(int fd, int event)
{
    if (fd == -1) {
        return;
    }

    sim_send_generic(fd,
                     event,
                     NULL,
                     0,
                     0);
}

// -----------------------------------------------------------------------------
// Disconnect

void uiccd_main_disconnect(ste_msg_t * ste_msg)
{
    uiccd_msg_disconnect_t *msg = (uiccd_msg_disconnect_t *) ste_msg;
    catd_log_f(SIM_LOGGING_I, "uicc : DISCONNECT message received fd=%d", msg->fd);

    // always make sure to exit SAP session when SAP requestor disconnects
    if (msg->fd == uiccd_sap_requestor_fd &&
        ((uiccd_sap_substate == UICCD_SAP_SUBSTATE_CONNECTING) ||
        (uiccd_sap_substate == UICCD_SAP_SUBSTATE_CONNECTED))) {
            catd_log_f(SIM_LOGGING_I, "uicc : SAP requestor disconnected, exiting SAP session");
            cn_uiccd_subscribe(0); // CN events are not needed after this
            uiccd_sap_requestor_fd = -1;
            uiccd_sig_sap_session_exit(uiccd_sap_requestor_fd, 0);
    }
}

static int uiccd_cn_event_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t             * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_disconnect_t    * data_p;
    int                                result = 0; // Default is to the end the transaction
    ste_modem_t                       *m;
    uiccd_msg_cn_t                    *msg = (uiccd_msg_cn_t *) ste_msg;

    if (ctrl_p == NULL) {
        return 0;
    }

    if (! (msg->cn_msg->error_code == CN_SUCCESS &&
        msg->cn_msg->type == CN_RESPONSE_CURRENT_CALL_LIST)) {
        catd_log_f(SIM_LOGGING_D, "uicc : CN FAILURE. err-code: %d type: %d!", msg->cn_msg->error_code, msg->cn_msg->type);
    } else {
        cn_call_list_t *call_list = (cn_call_list_t *) msg->cn_msg->payload;

        catd_log_f(SIM_LOGGING_D, "uicc : Received current call list!");
        if (call_list->nr_of_calls == 0) {
            catd_log_f(SIM_LOGGING_D, "uicc : Dispatching STE_UICC_SAP_SESSION_SAP_READY_IND!");
            uiccd_encode_and_send_sap_event(uiccd_sap_requestor_fd, STE_UICC_SAP_SESSION_SAP_READY_IND);
        }
    }

    free(ctrl_p);
    return 0;
}

static int uiccd_disconnect_transaction_handler(ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t             * ctrl_p = (ste_sim_ctrl_block_t*)(ste_msg->client_tag);
    uiccd_trans_data_disconnect_t    * data_p;
    int                                result = 0; // Default is to the end the transaction
    ste_modem_t                       *m;

    if (ctrl_p == NULL) {
        return 0;
    }

    data_p = (uiccd_trans_data_disconnect_t*)(ctrl_p->transaction_data_p);

    if (data_p == NULL) {
        free(ctrl_p);
        return 0;
    }

    m = catd_get_modem();

    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sim_disconnect_response(ctrl_p->fd,
                                                      ctrl_p->client_tag,
                                                      SIM_UICC_SAP_STATUS_CODE_FAIL);
        return 0;
    }

    switch (data_p->trans_state)
    {
        case UICCD_TRANS_GET_CURRENT_CALL_LIST:
        {
            int status;
            cn_call_list_t *call_list;
            uiccd_msg_cn_t *msg = (uiccd_msg_cn_t *) ste_msg;

            catd_log_f(SIM_LOGGING_D, "uicc : Received UICCD_TRANS_GET_CURRENT_CALL_LIST");

            free(data_p);

            ctrl_p->transaction_data_p = NULL;
            ctrl_p->transaction_handler_func = NULL;

            if (! (msg->cn_msg->error_code == CN_SUCCESS &&
                msg->cn_msg->type == CN_RESPONSE_CURRENT_CALL_LIST)) {
                uiccd_encode_and_send_sim_disconnect_response(ctrl_p->fd,
                                                              ctrl_p->client_tag,
                                                              SIM_UICC_SAP_STATUS_CODE_FAIL);
                free(ctrl_p);
                return 0;
            }

            call_list = (cn_call_list_t *) msg->cn_msg->payload;

            if (call_list->nr_of_calls == 0) {
                cn_uiccd_subscribe(0); // CN events are not needed after this

                catd_log_f(SIM_LOGGING_D, "uicc : No calls ongoing, disconnecting SIM!");
                status = ste_modem_sim_disconnect(m, (uintptr_t)ctrl_p);

                if (status != 0) {
                    uiccd_encode_and_send_sim_disconnect_response(ctrl_p->fd,
                                                                  ctrl_p->client_tag,
                                                                  SIM_UICC_SAP_STATUS_CODE_FAIL);
                    catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_disconnect failed");
                    free(ctrl_p);
                    return -1;
                }
            } else {
                catd_log_f(SIM_LOGGING_D, "uicc : Call ongoing, SAP session not allowed!");
                uiccd_encode_and_send_sim_disconnect_response(ctrl_p->fd,
                                                              ctrl_p->client_tag,
                                                              SIM_UICC_SAP_STATUS_CODE_BUSY_ON_CALL);
            }
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

void uiccd_main_sap_handle_cn_event(const ste_msg_t * ste_msg)
{
    ste_sim_ctrl_block_t   * ctrl_p;
    cn_call_context_t *call_context_p = NULL;
    uiccd_msg_cn_t *msg = (uiccd_msg_cn_t *)ste_msg;

    if (uiccd_sap_requestor_fd == -1 ||
        msg->cn_msg->type != CN_EVENT_CALL_STATE_CHANGED) {
        return;
    }

    call_context_p = (cn_call_context_t *) msg->cn_msg->payload;

    if (call_context_p->call_state == CN_CALL_STATE_IDLE) {
        ctrl_p = ste_sim_create_ctrl_block(0, -1, UICCD_MSG_CN_EVENT, uiccd_cn_event_transaction_handler, NULL);

        if (!ctrl_p) {
            catd_log_f(SIM_LOGGING_E,"uicc : %s: Could not allocate memory for control block", __func__);
            return;
        }

        if (!cn_context_p) {
            cn_context_p = cn_get_client();
        }

        if (CN_SUCCESS != cn_request_current_call_list(cn_context_p, (uintptr_t)ctrl_p)) {
            catd_log_f(SIM_LOGGING_E,"uicc : %s: Failure when getting current call list!", __func__);
            free(ctrl_p);
            return;
        }
    }
}

int uiccd_main_sim_disconnect(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_t *msg = (uiccd_msg_sap_session_t *) ste_msg;
    ste_sim_ctrl_block_t   * ctrl_p;

    catd_log_f(SIM_LOGGING_I, "uicc : SIM DISCONNECT info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sim_disconnect_response(msg->fd,
                                                      msg->client_tag,
                                                      SIM_UICC_SAP_STATUS_CODE_FAIL);
        return -1;
    } else {
        int i;
        uiccd_trans_data_disconnect_t *data_p;

        if (!cn_context_p) {
            cn_context_p = cn_get_client();
        }

        if(cn_context_p == NULL) {
            catd_log_f(SIM_LOGGING_E, "%s : Failed to get cn client context", __func__);
            uiccd_encode_and_send_sim_disconnect_response(msg->fd,
                                                          msg->client_tag,
                                                          SIM_UICC_SAP_STATUS_CODE_FAIL);
            return -1;
        }

        data_p = malloc(sizeof(*data_p));

        if (!data_p) {
            catd_log_f(SIM_LOGGING_E, "%s : Failed to allocate transaction data", __func__);
            uiccd_encode_and_send_sim_disconnect_response(msg->fd,
                                                          msg->client_tag,
                                                          SIM_UICC_SAP_STATUS_CODE_FAIL);
            return -1;
        }

        data_p->trans_state = UICCD_TRANS_GET_CURRENT_CALL_LIST;
        ctrl_p = ste_sim_create_ctrl_block(msg->client_tag, msg->fd, msg->type, uiccd_disconnect_transaction_handler, data_p);

        if (!ctrl_p) {
            uiccd_encode_and_send_sim_disconnect_response(msg->fd,
                                                          msg->client_tag,
                                                          SIM_UICC_SAP_STATUS_CODE_FAIL);
            free(data_p);
            return -1;
        }

        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;

        // Subscribe to CN events
        cn_uiccd_subscribe(1);

        // Wait for CN_RESPONSE_CURRENT_CALL_LIST
        if (CN_SUCCESS != cn_request_current_call_list(cn_context_p, (uintptr_t)ctrl_p)) {
            uiccd_encode_and_send_sim_disconnect_response(msg->fd,
                                                          msg->client_tag,
                                                          SIM_UICC_SAP_STATUS_CODE_FAIL);
            free(data_p);
            free(ctrl_p);
            return -1;
        }
    }

    return 0;
}

int uiccd_main_sim_connect(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_t *msg = (uiccd_msg_sap_session_t *) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SIM CONNECT info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_sig_sim_connect_response(msg->client_tag, SIM_UICC_STATUS_CODE_FAIL, SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR);
        return -1;
    } else {
        int i;

        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;
        i = ste_modem_sim_connect(m, msg->client_tag);
        if (i != 0) {
            uiccd_sig_sim_connect_response(msg->client_tag, SIM_UICC_STATUS_CODE_FAIL, SIM_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sim_connect failed");
            return -1;
        }
    }

    return 0;
}

int uiccd_main_sap_session_start_response(int status, int client_tag)
{
    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION START response message received");

    if ( uiccd_sap_requestor_fd< 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION START response message but no receiver");
        return -1;
    }

    sim_send_generic(uiccd_sap_requestor_fd,
                     STE_UICC_RSP_SAP_SESSION_START, (char*)&status, sizeof(status), client_tag);

    return 0;
}

int uiccd_main_sap_session_exit_response(int status, int client_tag)
{
    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION EXIT response message received");

    if ( uiccd_sap_requestor_fd < 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION EXIT response message but no receiver");
        return -1;
    }

    sim_send_generic(uiccd_sap_requestor_fd,
                     STE_UICC_RSP_SAP_SESSION_EXIT, (char*)&status, sizeof(status), client_tag);

    cn_uiccd_subscribe(0); // CN events are not needed after this
    return 0;
}

int uiccd_encode_and_send_sap_session_poweron_card_response(int fd,
                                                            uintptr_t client_tag,
                                                            sim_uicc_sap_status_code_t status)
{
  // check if card powered on due to SAP being connected
  if (uiccd_sap_substate == UICCD_SAP_SUBSTATE_CONNECTING) {
     // card power on failed, need to respond to client and exit SAP session
     sim_send_generic(fd,
                      STE_UICC_RSP_SAP_SESSION_START,
                      (char*)&status,
                      sizeof(status),
                      client_tag);

     uiccd_sap_requestor_fd = -1;
     uiccd_sig_sap_session_exit(-1, 0);
  }
  else {
    sim_send_generic(fd,
                     STE_UICC_RSP_SAP_SESSION_PWRON,
                     (char*)&status,
                     sizeof(status),
                     client_tag);
  }

  return 0;

}

int uiccd_main_sap_session_poweron_card(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_t *msg =  (uiccd_msg_sap_session_t *)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION POWERON info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sap_session_poweron_card_response(msg->fd,
                                                                msg->client_tag,
                                                                SIM_UICC_SAP_STATUS_CODE_FAIL);
        return -1;
    } else {
        int i;
        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;
        i = ste_modem_sap_session_ctrl_activate_card(m, msg->client_tag);
        if (i != 0) {
            uiccd_encode_and_send_sap_session_poweron_card_response(msg->fd,
                                                                    msg->client_tag,
                                                                    SIM_UICC_SAP_STATUS_CODE_FAIL);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sap_session_poweron_card failed");
            return -1;
        }
    }

    return 0;
}

int   uiccd_encode_and_send_sap_session_poweroff_card_response( int fd,
                                                                uintptr_t client_tag,
                                                                sim_uicc_sap_status_code_t status)
{
    sim_send_generic(fd,
                     STE_UICC_RSP_SAP_SESSION_PWROFF,
                     (char*)&status,
                     sizeof(status),
                     client_tag);

    return 0;

}

int uiccd_main_sap_session_poweroff_card(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_t *msg = (uiccd_msg_sap_session_t *)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION POWEROFF info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sap_session_poweroff_card_response(msg->fd,
                                                                 msg->client_tag,
                                                                 SIM_UICC_SAP_STATUS_CODE_FAIL);
        return -1;
    } else {
        int i;

        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;
        i = ste_modem_sap_session_ctrl_deactivate_card(m, msg->client_tag);
        if (i != 0) {
            uiccd_encode_and_send_sap_session_poweroff_card_response(msg->fd,
                                                                     msg->client_tag,
                                                                     SIM_UICC_SAP_STATUS_CODE_FAIL);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sap_session_poweroff_card failed");
            return -1;
        }
    }

    return 0;
}

int   uiccd_encode_and_send_sap_session_ctrlcard_reset_response( int fd,
                                                                 uintptr_t client_tag,
                                                                 sim_uicc_sap_status_code_t status)
{
    sim_send_generic(fd,
                     STE_UICC_RSP_SAP_SESSION_RESET,
                     (char*)&status,
                     sizeof(status),
                     client_tag);

    return 0;

}

int uiccd_main_sap_session_coldreset_card(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_t *msg = (uiccd_msg_sap_session_t *)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION COLDRESET info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sap_session_ctrlcard_reset_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_SAP_STATUS_CODE_FAIL);
        return -1;
    } else {
        int i;

        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;
        i = ste_modem_sap_session_ctrl_coldreset_card(m, msg->client_tag);
        if (i != 0) {
            uiccd_encode_and_send_sap_session_ctrlcard_reset_response(msg->fd,
                                                                      msg->client_tag,
                                                                      SIM_UICC_SAP_STATUS_CODE_FAIL);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sap_session_coldreset_card failed");
            return -1;
        }
    }

    return 0;
}

int uiccd_main_sap_session_warmreset_card(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_t *msg = (uiccd_msg_sap_session_t *)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION WARMRESET info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sap_session_ctrlcard_reset_response(msg->fd,
                                                                  msg->client_tag,
                                                                  SIM_UICC_SAP_STATUS_CODE_FAIL);
        return -1;
    } else {
        int i;

        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;
        i = ste_modem_sap_session_ctrl_warmreset_card(m, msg->client_tag);
        if (i != 0) {
            uiccd_encode_and_send_sap_session_ctrlcard_reset_response(msg->fd,
                                                                      msg->client_tag,
                                                                      SIM_UICC_SAP_STATUS_CODE_FAIL);
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_sap_session_warmreset_card failed");
            return -1;
        }
    }

    return 0;
}

int  uiccd_encode_and_send_sap_session_atr_get_response( int fd,
                                                         uintptr_t client_tag,
                                                         sim_uicc_sap_status_code_t sap_status_code,
                                                         const uint8_t* data,
                                                         size_t data_len )

{
    uint8_t *msg_data = NULL;
    int len;

    if (SIM_UICC_SAP_STATUS_CODE_OK == sap_status_code) {
        len = sizeof(int) + data_len; // status + atr
        msg_data = malloc(len);
        *((int*)msg_data) = 0; // status=ok
        memcpy(msg_data + 4, data, data_len); // atr
    } else {
        len = sizeof(int); // status, no payload
        msg_data = malloc(len);
        *((int*)msg_data) = 1; // status=fail
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_SAP_SESSION_ATR_GET,
                     (const char*)msg_data,
                     len,
                     client_tag);

    free(msg_data);

    return 0;

}

int uiccd_main_sap_session_atr_get(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_t *msg = (uiccd_msg_sap_session_t *)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION ATR_GET info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sap_session_atr_get_response(msg->fd,
                                                           msg->client_tag,
                                                           SIM_UICC_SAP_STATUS_CODE_FAIL,
                                                           NULL, 0);
        return -1;
    } else {
        int i;

        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;
        i = ste_modem_sap_session_atr_get(m, msg->client_tag);
        if (i != 0) {
            uiccd_encode_and_send_sap_session_atr_get_response(msg->fd,
                                                               msg->client_tag,
                                                               SIM_UICC_SAP_STATUS_CODE_FAIL,
                                                               NULL, 0);
            catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_sap_session_atr_get failed");
            return -1;
        }
    }

    return 0;
}

int  uiccd_main_sap_session_atr_get_response(const ste_msg_t * ste_msg)
{
    uint8_t *data = NULL;
    int len;

    uiccd_msg_byte_data_status_t* msg;
    msg = (uiccd_msg_byte_data_status_t*) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION ATR_GET response message received");

    if ( uiccd_state_busy_resp_fd < 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION ATR_GET response message but no receiver");
        return -1;
    }

    if (SIM_UICC_STATUS_CODE_OK == msg->uicc_status_code) {
        len = sizeof(int) + msg->len; // status + atr
        data = malloc(len);
        *((int*)data) = 0; // status=ok
        memcpy(data + 4, msg->data, msg->len); // atr
    } else {
        len = sizeof(int); // status, no payload
        data = malloc(len);
        *((int*)data) = 1; // status=fail
    }

    sim_send_generic(uiccd_state_busy_resp_fd,
                     STE_UICC_RSP_SAP_SESSION_ATR_GET, (const char*)data, len, msg->client_tag);

    free(data);

    return 0;

}

int   uiccd_main_sap_session_ctrlcard_activate_response(const ste_msg_t * ste_msg)
{
    uiccd_msg_sap_session_status_t* msg;
    int buf;

    msg = (uiccd_msg_sap_session_status_t*) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION POWERON response message received");

    if ( uiccd_state_busy_resp_fd < 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION POWERON response message but no receiver");
        return -1;
    }

    buf = (msg->status == 0 ? 0 : 1);
    sim_send_generic(uiccd_state_busy_resp_fd,
                     STE_UICC_RSP_SAP_SESSION_PWRON, (char*)&buf, sizeof(buf), msg->client_tag);

    return 0;

}

int   uiccd_main_sap_session_ctrlcard_deactivate_response(const ste_msg_t * ste_msg)
{
    uiccd_msg_sap_session_status_t* msg;
    int buf;

    msg = (uiccd_msg_sap_session_status_t*) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION POWEROFF response message received");

    if ( uiccd_state_busy_resp_fd < 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION POWEROFF response message but no receiver");
        return -1;
    }

    buf = (msg->status == 0 ? 0 : 1);
    sim_send_generic(uiccd_state_busy_resp_fd,
                     STE_UICC_RSP_SAP_SESSION_PWROFF, (char*)&buf, sizeof(buf), msg->client_tag);

    return 0;

}

int   uiccd_main_sap_session_ctrlcard_coldreset_response(const ste_msg_t * ste_msg)
{
    uiccd_msg_sap_session_status_t* msg;
    int buf;

    msg = (uiccd_msg_sap_session_status_t*) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION CRESET response message received");

    if ( uiccd_state_busy_resp_fd < 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION CRESET response message but no receiver");
        return -1;
    }

    buf = (msg->status == 0 ? 0 : 1);
    sim_send_generic(uiccd_state_busy_resp_fd,
                     STE_UICC_RSP_SAP_SESSION_RESET, (char*)&buf, sizeof(buf), msg->client_tag);

    return 0;

}

int  uiccd_encode_and_send_sap_session_apdu_data_response( int fd,
                                                           uintptr_t client_tag,
                                                           sim_uicc_sap_status_code_t sap_status_code,
                                                           const uint8_t* data,
                                                           size_t data_len )

{
    uint8_t *msg_data = NULL;
    int len;


    if (SIM_UICC_SAP_STATUS_CODE_OK == sap_status_code) {
        len = sizeof(int) + data_len; // status + apdu
        msg_data = malloc(len);
        *((int*)msg_data) = 0; // status=ok
        memcpy(msg_data + 4, data, data_len); // apdu
    } else {
        len = sizeof(int); // status, no payload
        msg_data = malloc(len);
        *((int*)msg_data) = 1; // status=fail
    }

    sim_send_generic(fd,
                     STE_UICC_RSP_SAP_SESSION_APDU_DATA,
                     (const char*)msg_data,
                     len,
                     client_tag);

    free(msg_data);

    return 0;
}

int uiccd_main_sap_session_apdu_data(const ste_msg_t * ste_msg)
{
    ste_modem_t            *m;
    uiccd_msg_sap_session_apdu_data_t *msg = (uiccd_msg_sap_session_apdu_data_t *)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION APDU_DATA info message received from %d ", msg->fd);
    m = catd_get_modem();
    if (!m) {
        catd_log_f(SIM_LOGGING_E, "uicc : No modem");
        uiccd_encode_and_send_sap_session_apdu_data_response(msg->fd,
                                                             msg->client_tag,
                                                             SIM_UICC_SAP_STATUS_CODE_FAIL,
                                                             NULL, 0);
        return -1;
    } else {
        int i;
        const char *p = msg->data, *p_max = msg->data + msg->len;
        uint8_t *data = NULL;

        // Save fd to respond to
        uiccd_state_busy_resp_fd = msg->fd;

        data = malloc(msg->len);
        sim_dec(p, data, msg->len, p_max);

        i = ste_modem_sap_session_apdu_data(m, msg->client_tag, data, msg->len);
        if (i != 0) {
            uiccd_encode_and_send_sap_session_apdu_data_response(msg->fd,
                                                                 msg->client_tag,
                                                                 SIM_UICC_SAP_STATUS_CODE_FAIL,
                                                                 NULL, 0);
            catd_log_f(SIM_LOGGING_E, "uicc : ste_modem_sap_session_apdu_data failed");
            free(data);
            return -1;
        }

        free(data);
    }

    return 0;
}

int  uiccd_main_sap_session_apdu_data_response(const ste_msg_t * ste_msg)
{
    uint8_t *data = NULL;
    int len;

    uiccd_msg_byte_data_status_t* msg;
    msg = (uiccd_msg_byte_data_status_t*) ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION APDU_DATA response message received");

    if ( uiccd_state_busy_resp_fd < 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION APDU_DATA response message but no receiver");
        return -1;
    }

    if (SIM_UICC_STATUS_CODE_OK == msg->uicc_status_code) {
        len = sizeof(int) + msg->len; // status + apdu
        data = malloc(len);
        *((int*)data) = 0; // status=ok
        memcpy(data + 4, msg->data, msg->len); // apdu
    } else {
        len = sizeof(int); // status, no payload
        data = malloc(len);
        *((int*)data) = 1; // status=fail
    }

    sim_send_generic(uiccd_state_busy_resp_fd,
                     STE_UICC_RSP_SAP_SESSION_APDU_DATA, (const char*)data, len, msg->client_tag);

    free(data);

    return 0;
}

int uiccd_main_sap_session_status_get_response(int status, int client_tag)
{
    struct {
        int status;
        int card_status;
    } payload;

    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION STATUS GET response message received");

    if ( uiccd_state_busy_resp_fd < 0 ) {
        catd_log_f(SIM_LOGGING_E, "uicc : SAP SESSION STATUS GET response message but no receiver");
        return -1;
    }

    payload.status = 0;
    payload.card_status = status;

    sim_send_generic(uiccd_state_busy_resp_fd,
                     STE_UICC_RSP_SAP_SESSION_STATUS_GET, (char*)&payload, sizeof(payload), client_tag);

    return 0;
}

int uiccd_main_sap_session_status_ind(int status)
{
    catd_log_f(SIM_LOGGING_I, "uicc : SAP SESSION CARD_STATUS_IND message received");

    if ( uiccd_sap_requestor_fd < 0 )
        return -1;

    sim_send_generic(uiccd_sap_requestor_fd,
                     STE_UICC_SAP_SESSION_STATUS_IND, (const char*)&status, sizeof(status), 0);

    return 0;
}
