/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : catd_msg.c
 * Description     : message handling
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *
 */


#include "catd_msg.h"

#include <stdlib.h>
#include <string.h>

#include "r_sms.h"


// -----------------------------------------------------------------------------
// General empty messages



catd_msg_modem_ready_t *catd_msg_modem_ready_create(uintptr_t client_tag)
{
    return ste_msg_void_create(CATD_MSG_MODEM_READY, client_tag);
}


catd_msg_modem_not_ready_t *catd_msg_modem_not_ready_create(uintptr_t client_tag)
{
    return ste_msg_void_create(CATD_MSG_MODEM_NOT_READY, client_tag);
}

catd_msg_modem_reset_t *catd_msg_modem_reset_create(uintptr_t client_tag)
{
    return ste_msg_void_create(CATD_MSG_MODEM_RESET, client_tag);
}

catd_msg_modem_ind_reg_ok_t *catd_msg_modem_ind_reg_ok_create(uintptr_t client_tag)
{
    return ste_msg_void_create(CATD_MSG_MODEM_IND_REG_OK, client_tag);
}

catd_msg_modem_cat_not_supported_t *catd_msg_modem_cat_not_supported_create(uintptr_t client_tag)
{
    return ste_msg_void_create(CATD_MSG_MODEM_CAT_NOT_SUPPORTED, client_tag);
}

catd_msg_timeout_event_t *catd_msg_timeout_event_create(uintptr_t timeout_event)
{
    return ste_msg_void_create(CATD_MSG_TIMEOUT_EVENT, timeout_event);
}

catd_msg_cat_server_ready_t *catd_msg_cat_server_ready_create(uintptr_t client_tag)
{
    return ste_msg_void_create(CATD_MSG_CAT_SERVER_READY, client_tag);
}

catd_msg_reset_t *catd_msg_reset_create(uintptr_t client_tag)
{
    return ste_msg_void_create(CATD_MSG_RESET, client_tag);
}

// -----------------------------------------------------------------------------
// file descriptor messages

static catd_msg_fd_t   *catd_msg_fd_create(int type, int fd, uintptr_t client_tag)
{
    catd_msg_fd_t          *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = type;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->fd = fd;
    }
    return p;
}


// -----------------------------------------------------------------------------
// Generic data Message


static void catd_msg_data_delete(ste_msg_t * ste_msg)
{
    catd_msg_data_t       *msg = (catd_msg_data_t *) ste_msg;
    if (msg) {
        if (msg->buf_p) {
            free(msg->buf_p);
        }
        msg->buf_p = 0;
        free(msg);
    }
}

catd_msg_data_t *catd_msg_data_create(int type,
                                      int fd,
                                      uintptr_t client_tag,
                                      const uint8_t *data,
                                      uint16_t len)
{
    catd_msg_data_t *p;
    uint8_t *d = NULL;

    p = malloc(sizeof(*p));
    if (!p) {
        return 0;
    }

    if (len > 0) {
        d = malloc(len);
        if (!d) {
            free(p);
            return 0;
        }
        memcpy(d, data, len);
    }
    p->type = type;
    p->delete_func = catd_msg_data_delete;
    p->client_tag = client_tag;
    p->fd = fd;
    p->buf_p = d;
    p->len = len;
    return p;
}

catd_msg_connect_t     *catd_msg_connect_create(int fd, uintptr_t client_tag)
{
    catd_msg_connect_t     *p = catd_msg_fd_create(CATD_MSG_CONNECT, fd, client_tag);
    return p;
}



catd_msg_disconnect_t  *catd_msg_disconnect_create(int fd, uintptr_t client_tag)
{
    catd_msg_disconnect_t  *p =
        catd_msg_fd_create(CATD_MSG_DISCONNECT, fd, client_tag);
    return p;
}



catd_msg_startup_t    *catd_msg_startup_create(int fd, uintptr_t client_tag)
{
    catd_msg_startup_t    *p = catd_msg_fd_create(CATD_MSG_STARTUP, fd, client_tag);
    return p;
}



catd_msg_shutdown_t    *catd_msg_shutdown_create(int fd, uintptr_t client_tag)
{
    catd_msg_shutdown_t    *p = catd_msg_fd_create(CATD_MSG_SHUTDOWN, fd, client_tag);
    return p;
}

// -----------------------------------------------------------------------------
// Register message

catd_msg_register_t    *catd_msg_register_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_data_t       *msg;
    msg = catd_msg_data_create(CATD_MSG_REGISTER, fd, client_tag, buf_p, len);

    return (catd_msg_register_t *) msg;
}

// -----------------------------------------------------------------------------
// DeRegister message

catd_msg_deregister_t    *catd_msg_deregister_create(int fd, uintptr_t client_tag)
{
    catd_msg_deregister_t    *p = catd_msg_fd_create(CATD_MSG_DEREGISTER, fd, client_tag);
    return p;
}

// -----------------------------------------------------------------------------
// STOP Message

catd_msg_stop_t        *catd_msg_stop_create(cat_barrier_t * bar, uintptr_t client_tag)
{
    catd_msg_stop_t        *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_STOP;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->bar = bar;
    }
    return p;

}



// -----------------------------------------------------------------------------
// INT Messages

static catd_msg_int_t  *catd_msg_int_create(int type, uintptr_t client_tag, int i)
{
    catd_msg_int_t         *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = type;
        p->delete_func = 0;
        p->client_tag = client_tag;
        p->i = i;
    }
    return p;
}

catd_msg_tpdl_status_t *catd_msg_tpdl_status_create(uintptr_t client_tag, int i)
{
    catd_msg_tpdl_status_t *p;
    p = catd_msg_int_create(CATD_MSG_TPDL_STATUS, client_tag, i);
    return p;
}

catd_msg_adapt_terminal_support_table_update_status_t *catd_msg_adapt_terminal_support_table_update_create(uintptr_t client_tag, int status, sim_uicc_status_word_t status_word)
{
    catd_msg_adapt_terminal_support_table_update_status_t *p;

    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_ADAPT_TERMINAL_SUPPORT_TABLE_UPDATE_STATUS;
        p->delete_func = 0;
        p->client_tag = client_tag;
        if (status && (status_word.sw1 == 0x91 || status_word.sw1 == 0x90)) {
            p->adapt_file_updated = TRUE;
        } else {
            p->adapt_file_updated = FALSE;
        }
    }
    return p;
}

catd_msg_enable_status_t *catd_msg_enable_status_create(uintptr_t client_tag, int i)
{
    catd_msg_enable_status_t *p;
    p = catd_msg_int_create(CATD_MSG_MODEM_ENABLE_STATUS, client_tag, i);
    return p;
}

catd_msg_disable_status_t *catd_msg_disable_status_create(uintptr_t client_tag, int i)
{
    catd_msg_disable_status_t *p;
    p = catd_msg_int_create(CATD_MSG_MODEM_DISABLE_STATUS, client_tag, i);
    return p;
}

catd_msg_app_ready_t *catd_msg_app_ready_create(uintptr_t client_tag, int i)
{
    catd_msg_app_ready_t *p;
    p = catd_msg_int_create(CATD_MSG_APP_READY, client_tag, i);
    return p;
}

// -----------------------------------------------------------------------------
// REFRESH STATUS message
catd_msg_modem_refresh_status_t *catd_msg_modem_refresh_status_create(uintptr_t client_tag, int status)
{
    catd_msg_modem_refresh_status_t *p;
    p = catd_msg_int_create(CATD_MSG_MODEM_REFRESH_STATUS, client_tag, status);
    return p;
}



// -----------------------------------------------------------------------------
// APDU message


static void catd_msg_apdu_delete(ste_msg_t * ste_msg)
{
    catd_msg_apdu_t        *msg = (catd_msg_apdu_t *) ste_msg;
    if (msg) {
        if (msg->type != CATD_MSG_APDU) {
            abort();
        } else {
            if (msg->apdu)
                ste_apdu_delete(msg->apdu);
        }
        free(msg);
    }
}


catd_msg_apdu_t        *catd_msg_apdu_create(ste_apdu_t * apdu, uintptr_t client_tag)
{
    catd_msg_apdu_t        *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_APDU;
        p->delete_func = catd_msg_apdu_delete;
        p->client_tag = client_tag;
        if (apdu) {
            p->apdu = ste_apdu_copy(apdu);
        }
        else {
            p->apdu = NULL;
        }
    }
    return p;
}



// -----------------------------------------------------------------------------
// TERMINAL RESPONSE message


static void catd_msg_tr_delete(ste_msg_t * ste_msg)
{
    catd_msg_tr_t          *msg = (catd_msg_tr_t *) ste_msg;
    if (msg) {
        if (msg->type != CATD_MSG_TR) {
            abort();
        } else {
            if (msg->apdu)
                ste_apdu_delete(msg->apdu);
        }
        free(msg);
    }
}


catd_msg_tr_t          *catd_msg_tr_create(int fd, ste_apdu_t * apdu, uintptr_t client_tag)
{
    catd_msg_tr_t          *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_TR;
        p->fd = fd;
        p->delete_func = catd_msg_tr_delete;
        p->client_tag = client_tag;
        if (apdu) {
            p->apdu = ste_apdu_copy(apdu);
        }
        else {
            p->apdu = NULL;
        }
    }
    return p;
}

// -----------------------------------------------------------------------------
// ENVELOPE COMMAND message

static void catd_msg_ec_delete(ste_msg_t * ste_msg)
{
    catd_msg_ec_t          *msg = (catd_msg_ec_t *) ste_msg;
    if (msg) {
        if (msg->type != CATD_MSG_EC) {
            abort();
        } else {
            if (msg->apdu)
                ste_apdu_delete(msg->apdu);
        }
        free(msg);
    }
}

catd_msg_ec_t          *catd_msg_ec_create(int fd, ste_apdu_t * apdu, uintptr_t client_tag)
{
    catd_msg_ec_t          *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_EC;
        p->fd = fd;
        p->delete_func = catd_msg_ec_delete;
        p->client_tag = client_tag;
        if (apdu) {
            p->apdu = ste_apdu_copy(apdu);
        }
        else {
            p->apdu = NULL;
        }
    }
    return p;
}

// -----------------------------------------------------------------------------
// CALL NETWORKING message

static void catd_msg_cn_delete(ste_msg_t * ste_msg)
{
    catd_msg_cn_t *msg = (catd_msg_cn_t *) ste_msg;
    if (msg) {
        if (msg->type != CATD_MSG_CN_RESPONSE && msg->type != CATD_MSG_CN_EVENT) {
            abort();
        }
        if (msg->cn_msg) {
            free(msg->cn_msg);
        }
        free(msg);
    }
}

catd_msg_cn_t *catd_msg_cn_create(cn_message_t * cn_msg)
{
    catd_msg_cn_t *p;
    p = malloc(sizeof(*p));
    if (p) {
        if ((cn_msg->type & CN_RESPONSE_UNKNOWN) != 0) {
            p->type = CATD_MSG_CN_RESPONSE;
        } else {
            p->type = CATD_MSG_CN_EVENT;
        }
        p->delete_func = catd_msg_cn_delete;
        p->client_tag = cn_msg->client_tag;
        p->cn_msg = cn_msg;
    }
    return p;
}

// -----------------------------------------------------------------------------
// CAT response message

static void catd_msg_modem_cat_response_delete(ste_msg_t * ste_msg)
{
    catd_msg_modem_cat_response_t    *msg = (catd_msg_modem_cat_response_t *) ste_msg;
    if (msg) {
        if (msg->type != CATD_MSG_MODEM_CAT_RESPONSE) {
            abort();
        } else {
            if (msg->apdu)
                ste_apdu_delete(msg->apdu);
        }
        free(msg);
    }
}

catd_msg_modem_cat_response_t *
catd_msg_modem_cat_response_create(int status, ste_apdu_t * apdu, uintptr_t client_tag)
{
    catd_msg_modem_cat_response_t          *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_MODEM_CAT_RESPONSE;
        p->delete_func = catd_msg_modem_cat_response_delete;
        p->client_tag = client_tag;
        p->status = status;
        if (apdu) {
            p->apdu = ste_apdu_copy(apdu);
        }
        else {
            p->apdu = NULL;
        }
    }
    return p;
}

// -----------------------------------------------------------------------------
// CAT set poll interval response message

static void catd_msg_modem_set_poll_interval_response_delete(ste_msg_t* ste_msg)
{
    catd_msg_modem_set_poll_interval_response_t* msg = (catd_msg_modem_set_poll_interval_response_t*) ste_msg;
    if (msg) {
        if (msg->type == CATD_MSG_MODEM_SET_POLL_INTERVAL_RESPONSE) {
            free(msg);
        } else {
            abort();
        }
    }
}

catd_msg_modem_set_poll_interval_response_t *
catd_msg_modem_set_poll_interval_response_create(int status, int interval, uintptr_t client_tag) {
    catd_msg_modem_set_poll_interval_response_t* p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_MODEM_SET_POLL_INTERVAL_RESPONSE;
        p->delete_func = catd_msg_modem_set_poll_interval_response_delete;
        p->client_tag = client_tag;
        p->status = status;
        p->interval = interval;
    }
    return p;
}

// -----------------------------------------------------------------------------
// CAT SMS/CB signal messages

static void catd_msg_sms_cp_perform_connect_delete(ste_msg_t* ste_msg)
{
    catd_msg_sms_cb_signal_t* msg = (catd_msg_sms_cb_signal_t*) ste_msg;
    if (msg) {
        if (msg->type == CATD_MSG_SMS_CB_PERFORM_CONNECT) {
            free(msg);
        } else {
            abort();
        }
    }

}

catd_msg_sms_cb_perform_connect_t* catd_msg_sms_cp_perform_connect_create(uintptr_t client_tag)
{
    catd_msg_sms_cb_perform_connect_t* p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_SMS_CB_PERFORM_CONNECT;
        p->delete_func = catd_msg_sms_cp_perform_connect_delete;
        p->client_tag = client_tag;
    }
    return p;
}

static void catd_msg_modem_sms_cb_signal_delete(ste_msg_t* ste_msg)
{
    catd_msg_sms_cb_signal_t* msg = (catd_msg_sms_cb_signal_t*) ste_msg;
    if (msg) {
        switch (msg->type) {
        case CATD_MSG_SMS_CB_RESPONSE:
        case CATD_MSG_SMS_CB_EVENT:
            if (msg->signal) {
                Util_SMS_SignalFree(msg->signal);
            }
            /*
             * Zero everything in case someone tries to access parts of this message after deallocation
             * (e.g. make sure there's a good chance of an early fail if it happens).
             */
            memset(msg, 0, sizeof(*msg));
            free(msg);
            break;
        default:
            abort();
        }
    }
}

catd_msg_sms_cb_signal_t* catd_msg_sms_cb_response_create(void* signal, uint32_t primitive, uintptr_t client_tag)
{
    catd_msg_sms_cb_signal_t* p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type        = CATD_MSG_SMS_CB_RESPONSE;
        p->delete_func = catd_msg_modem_sms_cb_signal_delete;
        p->client_tag  = client_tag;
        p->primitive   = primitive;
        p->signal      = signal;
    }
    return p;
}

catd_msg_sms_cb_signal_t* catd_msg_sms_cb_event_create(void* signal, uint32_t primitive, uintptr_t client_tag)
{
    catd_msg_sms_cb_signal_t* p = catd_msg_sms_cb_response_create(signal, primitive, client_tag);
    if (p) {
        p->type = CATD_MSG_SMS_CB_EVENT;
    }
    return p;
}

// -----------------------------------------------------------------------------
// CAT UICCd signal messages

static void ccatd_msg_uiccd_response_delete(ste_msg_t* ste_msg)
{
    catd_msg_uiccd_response_t* msg = (catd_msg_uiccd_response_t*) ste_msg;
    if (msg) {
        if (msg->type == CATD_MSG_UICCD_RESPONSE) {
            msg->command = 0;
            if (msg->response) {
                free(msg->response);
            }
            /*
             * Zero everything in case someone tries to access parts of this message after deallocation
             * (e.g. make sure there's a good chance of an early fail if it happens).
             */
            memset(msg, 0, sizeof(*msg));
            free(msg);
        } else {
            abort();
        }
    }
}

catd_msg_uiccd_response_t* catd_msg_uiccd_response_create(char* response, ssize_t response_len,
                                                          uint16_t command, uintptr_t client_tag)
{
    catd_msg_uiccd_response_t* p = malloc(sizeof(*p));
    if (p) {
        p->type         = CATD_MSG_UICCD_RESPONSE;
        p->delete_func  = ccatd_msg_uiccd_response_delete;
        p->client_tag   = client_tag;
        p->command      = command;
        p->response     = response;
        p->response_len = response_len;
    }
    return p;
}

// -----------------------------------------------------------------------------
// CALL CONTROL message

catd_msg_call_control_t *
catd_msg_call_control_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_data_t       *msg;
    msg = catd_msg_data_create(CATD_MSG_CALL_CONTROL, fd, client_tag, buf_p, len);

    return (catd_msg_call_control_t *) msg;
}

// -----------------------------------------------------------------------------
// SET TERMINAL PROFILE message

catd_msg_set_terminal_profile_t *
catd_msg_set_terminal_profile_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_data_t       *msg;

    msg = catd_msg_data_create(CATD_MSG_SET_TERMINAL_PROFILE, fd, client_tag, buf_p, len);

    return (catd_msg_set_terminal_profile_t *) msg;
}

// -----------------------------------------------------------------------------
// GET TERMINAL PROFILE message

catd_msg_get_terminal_profile_t  *catd_msg_get_terminal_profile_create(int fd, uintptr_t client_tag)
{
    catd_msg_get_terminal_profile_t    *p = catd_msg_fd_create(CATD_MSG_GET_TERMINAL_PROFILE, fd, client_tag);
    return p;
}

// -----------------------------------------------------------------------------
// GET CAT STATUS message

catd_msg_get_cat_status_t  *catd_msg_get_cat_status_create(int fd, uintptr_t client_tag)
{
    catd_msg_get_cat_status_t    *p = catd_msg_fd_create(CATD_MSG_GET_CAT_STATUS, fd, client_tag);
    return p;
}

// -----------------------------------------------------------------------------
// CAT SESSION EXPIRED message

static void catd_msg_cat_session_expired_delete(ste_msg_t * ste_msg)
{
    catd_msg_cat_session_expired_t          *msg = (catd_msg_cat_session_expired_t *) ste_msg;
    if (msg) {
        if (msg->type != CATD_MSG_CAT_SESSION_EXPIRED) {
            abort();
        } else {
        }
        free(msg);
    }
}

catd_msg_cat_session_expired_t *
catd_msg_cat_session_expired_create(int session_id, uintptr_t client_tag)
{
    catd_msg_cat_session_expired_t          *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_CAT_SESSION_EXPIRED;
        p->delete_func = catd_msg_cat_session_expired_delete;
        p->session_id = session_id;
        p->client_tag = client_tag;
    }
    return p;
}

// -----------------------------------------------------------------------------
// ANSWER CALL message

catd_msg_answer_call_t *
catd_msg_answer_call_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_data_t       *msg;
    msg = catd_msg_data_create(CATD_MSG_ANSWER_CALL, fd, client_tag, buf_p, len);

    return (catd_msg_answer_call_t *) msg;
}

// -----------------------------------------------------------------------------
// EVENT DOWNLOAD message

catd_msg_event_download_t *
catd_msg_event_download_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_data_t       *msg;
    msg = catd_msg_data_create(CATD_MSG_EVENT_DOWNLOAD, fd, client_tag, buf_p, len);

    return (catd_msg_event_download_t *) msg;
}

// -----------------------------------------------------------------------------
// SMS CONTROL message

catd_msg_sms_control_t *
catd_msg_sms_control_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_data_t       *msg;
    msg = catd_msg_data_create(CATD_MSG_SMS_CONTROL, fd, client_tag, buf_p, len);

    return (catd_msg_sms_control_t *) msg;
}

// -----------------------------------------------------------------------------
// CAT ENABLE message

catd_msg_cat_enable_t *
catd_msg_cat_enable_create(int fd, uint8_t * buf_p, uint16_t len, uintptr_t client_tag)
{
    catd_msg_data_t       *msg;
    msg = catd_msg_data_create(CATD_MSG_CAT_ENABLE, fd, client_tag, buf_p, len);

    return (catd_msg_cat_enable_t *) msg;
}


// -----------------------------------------------------------------------------
// TIMER EXPIRATION message
static void catd_msg_timer_expiration_ec_delete(ste_msg_t*  ste_msg)
{
    catd_msg_timeout_t *msg = (catd_msg_timeout_t*) ste_msg;
    if (msg) {
        if (msg->type == CATD_MSG_TIMER_EXPIRATION_SEND_EC) {
            free(msg);
        } else {
            abort();
        }
    }
}
catd_msg_timeout_t * catd_msg_timer_expiration_ec_create(const uint16_t timer_info)
{
    catd_msg_timeout_t *p = malloc(sizeof(*p));

    if (p) {
        p->type         = CATD_MSG_TIMER_EXPIRATION_SEND_EC;
        p->delete_func  = catd_msg_timer_expiration_ec_delete;
        p->client_tag   = 0;  // We have no client tag. The information in timer_info is enough for us.
        p->timer_info   = timer_info;
    }
    return p;
}

// -----------------------------------------------------------------------------
// CAT MODEM REFRESH IND message

static void catd_msg_modem_refresh_ind_delete(ste_msg_t * ste_msg)
{
    catd_msg_modem_refresh_ind_t          *msg = (catd_msg_modem_refresh_ind_t *) ste_msg;
    if (msg) {
        if (msg->type != CATD_MSG_MODEM_REFRESH_IND) {
            abort();
        } else {
        }
        free(msg);
    }
}

catd_msg_modem_refresh_ind_t *
catd_msg_modem_refresh_ind_create(uintptr_t client_tag,
                                  ste_apdu_refresh_type_t refresh_type,
                                  ste_cat_refresh_ind_type_t ind_type,
                                  uint8_t app_id)
{
    catd_msg_modem_refresh_ind_t          *p;
    p = malloc(sizeof(*p));
    if (p) {
        p->type = CATD_MSG_MODEM_REFRESH_IND;
        p->delete_func = catd_msg_modem_refresh_ind_delete;
        p->client_tag = client_tag;
        p->refresh_type = refresh_type;
        p->ind_type = ind_type;
        p->app_id = app_id;
    }
    return p;
}

// -----------------------------------------------------------------------------
// SIM POWER ON/OFF message

catd_msg_sim_power_on_off_t *catd_msg_sim_power_on_off_create(uintptr_t client_tag, int power_on)
{
    catd_msg_sim_power_on_off_t *p;
    p = catd_msg_int_create(CATD_MSG_SIM_POWER_ON_OFF, client_tag, power_on);
    return p;
}

