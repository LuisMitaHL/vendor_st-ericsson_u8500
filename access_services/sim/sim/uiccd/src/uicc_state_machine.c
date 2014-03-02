/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <assert.h>
#include "uiccd_msg.h"
#include "state_machine.h"
#include "msgq.h"
#include "sim_unused.h"
#include "uicc_interface_handlers.h"
#include "sim_internal.h"
#include "uicc_internal.h"
#include "uicc_other_sap_common.h"
#include "uicc_state_machine.h"
#include "catd.h"

// State function prototypes
// -------------------------
static const ste_sim_state_t *uicc_state_init(const ste_sim_state_t *current_state, ste_msg_t *msg, void *ud);
static const ste_sim_state_t *uicc_state_idle(const ste_sim_state_t * current_state, ste_msg_t* msg, void *ud);
static const ste_sim_state_t *uicc_state_closed(const ste_sim_state_t * current_state, ste_msg_t* msg, void *ud);
static const ste_sim_state_t *uicc_state_sap(const ste_sim_state_t * current_state, ste_msg_t* msg, void *ud);
static const ste_sim_state_t *uicc_state_disconnect(const ste_sim_state_t * current_state, ste_msg_t* msg, void *ud);

// -----------------------------------------------------------------------------
// State table
static const ste_sim_state_t uicc_state_table[] = {
    { "init", uicc_state_init, uicc_get_any_msg  },
    { "idle", uicc_state_idle, uicc_get_any_msg  },
    { "closed", uicc_state_closed, uicc_get_any_msg },
    { "sap", uicc_state_sap, uicc_get_any_msg  },
    { "disconnect", uicc_state_disconnect, uicc_get_any_msg  },
    { 0, 0, 0 } // TERMINATOR
};

#define DUMMY_CT 1

// Data that uiccd state machine manages from modem.
// Structure also contains information of the current state
static struct {
  ste_uicc_card_type_t card_type;
  ste_sim_app_type_t app_type;
  int app_id;  // TODO: Remove, handled in sim_mal
} current = { STE_UICC_CARD_TYPE_UNKNOWN, STE_SIM_APP_TYPE_UNKNOWN, -1};

ste_uicc_card_type_t    uicc_get_card_type() { return current.card_type; }
ste_sim_app_type_t      uicc_get_app_type() { return current.app_type; }
int                     uicc_get_app_id() { return current.app_id; }
const ste_sim_state_t * uicc_initiate_state_machine()
{
    return ste_sim_state_get_by_name(uicc_state_table,"init");
}

void uiccd_main_stop(ste_msg_t * UNUSED(ste_msg))
{
    int result;

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_main_stop");

    result = ste_modem_app_initiate_shutdown(catd_get_modem(), DUMMY_CT);

    if (result != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : deactivate app on card failed");
    }
}
// -----------------------------------------------------------------------------
// SAP-specific state attributes

uiccd_sap_substate_t uiccd_sap_substate = UICCD_SAP_SUBSTATE_NONE;

/**
 * @brief When in the sap/sap_busy state this holds the fd of client which requested SAP session to start
 *
 * Valid values are:
 * -1                           Not set.  The current state must be other than sap/sap_busy.
 * >-1:                         File descriptor of SAP session requestor.
 */
int uiccd_sap_requestor_fd = -1;

/**
 * @brief When connecting SAP session, this holds original client_tag of request
 */
static int uiccd_sap_requestor_client_tag = 0;

static int get_sender_from_msg ( ste_msg_t * ste_msg )
{
    int fd = CLOSED_FD;
    switch ( ste_msg->type ) {
        case UICCD_MSG_CONNECT:
        {
            uiccd_msg_connect_t *msg = (uiccd_msg_connect_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_DISCONNECT:
        {
            uiccd_msg_disconnect_t *msg = (uiccd_msg_disconnect_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_SHUTDOWN:
        {
            uiccd_msg_shutdown_t *msg = (uiccd_msg_shutdown_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_REGISTER:
        {
            uiccd_msg_register_t *msg = (uiccd_msg_register_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_PIN_VERIFY:
        {
            uiccd_msg_pin_verify_t *msg = (uiccd_msg_pin_verify_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_PIN_CHANGE:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_READ_SIM_FILE_RECORD:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_READ_SIM_FILE_BINARY:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_UPDATE_SIM_FILE_RECORD:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_UPDATE_SIM_FILE_BINARY:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_SIM_FILE_GET_FORMAT:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_GET_FILE_INFORMATION:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_GET_SIM_STATE:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_PIN_DISABLE:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_PIN_ENABLE:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_PIN_INFO:
        {
            uiccd_msg_pin_info_t *msg = (uiccd_msg_pin_info_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_PIN_UNBLOCK:
        {
            uiccd_msg_data_t *msg = (uiccd_msg_data_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_APP_INFO:
        {
            uiccd_msg_app_info_t *msg = (uiccd_msg_app_info_t*)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_READ_SMSC:
        {
            uiccd_msg_read_smsc_t *msg = (uiccd_msg_read_smsc_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_UPDATE_SMSC:
        {
            uiccd_msg_update_smsc_t *msg = (uiccd_msg_update_smsc_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_SMSC_GET_RECORD_MAX:
        {
            uiccd_msg_smsc_get_record_max_t *msg = (uiccd_msg_smsc_get_record_max_t *)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_SMSC_SAVE_TO_RECORD:
        {
            uiccd_msg_smsc_save_to_record_t *msg = (uiccd_msg_smsc_save_to_record_t *)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_SMSC_RESTORE_FROM_RECORD:
        {
            uiccd_msg_smsc_restore_from_record_t *msg = (uiccd_msg_smsc_restore_from_record_t *)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_SIM_FILE_READ_GENERIC:
        {
            uiccd_msg_sim_file_read_generic_t *msg = (uiccd_msg_sim_file_read_generic_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_UPDATE_SERVICE_TABLE:
        {
            uiccd_msg_update_service_table_t *msg = (uiccd_msg_update_service_table_t*)ste_msg;
            fd = msg->fd;
            break;
        }
        case UICCD_MSG_GET_SERVICE_TABLE:
        {
           uiccd_msg_get_service_table_t *msg = (uiccd_msg_get_service_table_t*)ste_msg;
           fd = msg->fd;
           break;
        }

        case UICCD_MSG_GET_SERVICE_AVAILABILITY:
        {
           uiccd_msg_get_service_availability_t *msg = (uiccd_msg_get_service_availability_t*)ste_msg;
           fd = msg->fd;
           break;
        }

        case UICCD_MSG_SIM_ICON_READ:
        {
            uiccd_msg_sim_icon_read_t *msg = (uiccd_msg_sim_icon_read_t *)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_READ_SUBSCRIBER_NUMBER:
        {
            printf("get_sender_from_msg (%d): UICCD_MSG_READ_SUBSCRIBER_NUMBER", __LINE__);
            uiccd_msg_subscriber_number_t *msg = (uiccd_msg_subscriber_number_t *)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_READ_PLMN:
        {
            uiccd_msg_read_plmn_t *msg = (uiccd_msg_read_plmn_t*)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_UPDATE_PLMN: {
            uiccd_msg_update_plmn_t *msg = (uiccd_msg_update_plmn_t*) ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_SIM_CHANNEL_SEND:
        {
            uiccd_msg_sim_channel_send_t *msg = (uiccd_msg_sim_channel_send_t *)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_SIM_CHANNEL_OPEN:
        {
            uiccd_msg_sim_channel_open_t *msg = (uiccd_msg_sim_channel_open_t *)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_SIM_CHANNEL_CLOSE:
        {
            uiccd_msg_sim_channel_close_t *msg = (uiccd_msg_sim_channel_close_t *)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_SIM_POWER_ON:
        {
            uiccd_msg_sim_power_on_t *msg = (uiccd_msg_sim_power_on_t *)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_SIM_POWER_OFF:
        {
            uiccd_msg_sim_power_off_t *msg = (uiccd_msg_sim_power_off_t *)ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_SIM_READ_PREFERRED_RAT_SETTING:
        {
            uiccd_msg_sim_read_preferred_rat_setting_t *msg = (uiccd_msg_sim_read_preferred_rat_setting_t *) ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_READ_ECC:
        {
            uiccd_msg_read_ecc_t *msg = (uiccd_msg_read_ecc_t *) ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_READ_FDN:
        {
            uiccd_msg_read_fdn_t *msg = (uiccd_msg_read_fdn_t *) ste_msg;
            fd = msg->fd;
            break;
        }

        case UICCD_MSG_SAP_SESSION_START:
        case UICCD_MSG_SAP_SESSION_EXIT:
        case UICCD_MSG_SAP_SESSION_WARMRESET_CARD:
        case UICCD_MSG_SAP_SESSION_COLDRESET_CARD:
        case UICCD_MSG_SAP_SESSION_POWEROFF_CARD:
        case UICCD_MSG_SAP_SESSION_POWERON_CARD:
        case UICCD_MSG_SAP_SESSION_ATR_GET:
        case UICCD_MSG_SAP_SESSION_APDU_DATA:
        {
            uiccd_msg_sap_session_t *msg = (uiccd_msg_sap_session_t*)ste_msg;
            fd = msg->fd;
            break;
        }

        default:
        {
          catd_log_f(SIM_LOGGING_E, "uicc : unhandled message type in get_sender_from_msg - msg=%x", ste_msg->type);
        }
    }
    return fd;
}

// This is sent whenever the uiccd is in a state where it cannot process messages
static int uiccd_main_not_ready_response(ste_msg_t * ste_msg)
{
    int fd = get_sender_from_msg(ste_msg);

    catd_log_f(SIM_LOGGING_I, "uicc : message received in not ready state");

    if ( fd != CLOSED_FD ) {
        char *p, *buf;
        size_t buf_len = 0;

        catd_log_f(SIM_LOGGING_I, "uicc : encoding a message response" );

        buf_len += sizeof( ste_msg->type);
        buf = malloc(buf_len);

        if (!buf) {
            catd_log_f(SIM_LOGGING_E, "uicc : uiccd_main_not_ready_response, memory allocation failed");
            return -1;
        }

        p = buf;
        p = sim_enc(p, &ste_msg->type, sizeof(ste_msg->type));

        if (buf_len != (size_t)(p - buf)) {
            catd_log_f(SIM_LOGGING_E, "uicc: uiccd_main_not_ready_response, assert failure");
        }

        assert(buf_len == (size_t)(p-buf) );

        catd_log_f(SIM_LOGGING_I, "uicc : message type=%x, tag=%x", ste_msg->type, ste_msg->client_tag );

        sim_send_generic(fd,
                       STE_UICC_RSP_NOT_READY,
                       buf,
                       buf_len,
                       ste_msg->client_tag);
        free(buf);
    }

    return 0;
}

static void uiccd_main_card_fallback(ste_msg_t* ste_msg)
{
    uiccd_msg_card_fallback_t * msg = (uiccd_msg_card_fallback_t*)ste_msg;

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_main_card_fallback: card_type = %d, app_id = %d, app_type = %d", msg->card_type, msg->app_id, msg->app_type);
    current.card_type = msg->card_type;
    current.app_id = msg->app_id;
    current.app_type = msg->app_type;
}

static void uiccd_main_app_shutdown_init(ste_msg_t* ste_msg)
{
    uiccd_msg_app_shutdown_init_t * msg = (uiccd_msg_app_shutdown_init_t*)ste_msg;
    int status;

    catd_log_f(SIM_LOGGING_I, "uicc : uiccd_main_app_shutdown_init: status = %d", msg->status);

    status = ste_modem_deactivate_selected_app(catd_get_modem(), DUMMY_CT);
    if (status != 0) {
	    catd_log_f(SIM_LOGGING_E, "uicc : deactivate app on card failed");
    }
}

static const ste_sim_state_t * uicc_generic_response_handler(const ste_sim_state_t * current_state, ste_msg_t* msg)
{
    const ste_sim_state_t * next_state = current_state;
    ste_sim_ctrl_block_t * ctrl_p;
    int status;

    if (!msg) {
      catd_log_f(SIM_LOGGING_E, "uicc : idle with app type=%d card type=%d", current.app_type, current.card_type);
    } else {
        switch (msg->type) {
            case UICCD_MSG_SERVER_CARD_STATUS_RSP:
                /* It is intentional that no action is taken on this response;
                   rather the request is used to trigger a msg telling uiccd that
                   modem is initialized. */
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_MODEM_STATE:
                /* It is intentional that no action is taken on this response */
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_CARD_FALLBACK:
                /* Card fallback, needs to reset the static variables within UICCD */
                uiccd_main_card_fallback(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_PIN_CHANGE_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_PIN_CHANGE != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_pin_change_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_PIN_VERIFY_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_PIN_VERIFY != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        int rv = 0;

                        rv = ctrl_p->transaction_handler_func(msg);
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
                        if (UICCD_MSG_PIN_VERIFY_MSR == ctrl_p->transaction_id && rv != 0) {
                            //something wrong with the pin verify for MSR, let user input pin
                            uiccd_main_send_state_change_event(current_state, STE_UICC_STATUS_PIN_NEEDED);
                        }
#endif
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_pin_verify_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_PIN_DISABLE_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_PIN_DISABLE != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_pin_disable_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_PIN_ENABLE_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_PIN_ENABLE != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_pin_enable_response(msg);
                ste_msg_delete(msg);
                break;

             case UICCD_MSG_PIN_INFO_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_PIN_INFO != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_pin_info_response(msg);
                ste_msg_delete(msg);
                break;

             case UICCD_MSG_PIN_UNBLOCK_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_PIN_UNBLOCK != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_pin_unblock_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_READ_SIM_FILE_RECORD_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_READ_SIM_FILE_RECORD != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_read_sim_file_record_response(msg);
                ste_msg_delete(msg);
                break;


            case UICCD_MSG_READ_SIM_FILE_BINARY_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_READ_SIM_FILE_BINARY != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_read_sim_file_binary_response(msg);
                ste_msg_delete(msg);
                break;

             case UICCD_MSG_UPDATE_SIM_FILE_RECORD_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_UPDATE_SIM_FILE_RECORD != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_update_sim_file_record_response(msg);
                ste_msg_delete(msg);
                break;


            case UICCD_MSG_UPDATE_SIM_FILE_BINARY_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_UPDATE_SIM_FILE_BINARY != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_update_sim_file_binary_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_FILE_GET_FORMAT_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_SIM_FILE_GET_FORMAT != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_sim_file_get_format_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_GET_FILE_INFORMATION_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_GET_FILE_INFORMATION != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_get_file_information_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_DISCONNECT_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_SIM_POWER_OFF == ctrl_p->transaction_id) {
                    status = uiccd_main_sim_power_off_response(msg);

                    if (!status) {
                        next_state = ste_sim_state_get_by_name(uicc_state_table,"disconnect");
                        uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_DISCONNECTED_CARD);
                    }
                }
                else {
                    //Then the disconnect must be initiated by SAP session
                    //since ctrl_p is not used in SAP handler, it is freed here
                    free(ctrl_p);
                    // only process if due to SAP session being connected
                    if (uiccd_sap_substate == UICCD_SAP_SUBSTATE_CONNECTING) {
                        catd_sig_reset(DUMMY_CT); //Reset CATD for TPDL
                        status = ((uiccd_msg_sap_session_status_t*)msg)->status;
                        if (!status) {
                            // SAP session will continue on UICCD_MSG_UICC_STATUS_IND
                            next_state = ste_sim_state_get_by_name(uicc_state_table,"sap");
                            uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_DISCONNECTED_CARD);
                        } else {
                            uiccd_main_sap_session_start_response(1, uiccd_sap_requestor_client_tag);
                            uiccd_sap_requestor_fd = -1;
                            uiccd_sap_substate = UICCD_SAP_SUBSTATE_NONE;
                        }
                    }
                }
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_APPL_APDU_SEND_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_APPL_APDU_SEND != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_appl_apdu_send_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_CHANNEL_SEND_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_SIM_CHANNEL_SEND != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_sim_channel_send_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_CHANNEL_OPEN_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_SIM_CHANNEL_OPEN != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_sim_channel_open_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_CHANNEL_CLOSE_RSP:
                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p && UICCD_MSG_SIM_CHANNEL_CLOSE != ctrl_p->transaction_id) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                status = uiccd_main_sim_channel_close_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_CN_RSP:
            {
                ste_sim_ctrl_block_t   * ctrl_p;

                ctrl_p = (ste_sim_ctrl_block_t *)(msg->client_tag);

                if (ctrl_p) {
                    if (ctrl_p->transaction_handler_func) {
                        (void)ctrl_p->transaction_handler_func(msg);
                        ste_msg_delete(msg);
                        break;
                    }
                    else {
                        //something wrong, print error msg
                        catd_log_f(SIM_LOGGING_E,"uicc : transaction handler function is null.");
                    }
                }

                ste_msg_delete(msg);
                break;
            }
            break;

            case UICCD_MSG_CN_EVENT:
                uiccd_main_sap_handle_cn_event(msg);
                ste_msg_delete(msg);
            break;

            case UICCD_MSG_APP_SHUTDOWN_INITIATE_RSP:
                uiccd_main_app_shutdown_init(msg);
                ste_msg_delete(msg);
            break;

            default:
                next_state = STE_STATE_NOT_HANDLED; // Not handled
                break;
        }
    }

    return next_state;
}

// Used when the indication event has been received in a state where it shouldn't have
// The indication is illegal if it does not relate to something which has happened to the
// SIM card
static const ste_sim_state_t *uicc_state_illegal_uicc_status_ind(const ste_sim_state_t * current_state, uiccd_msg_uicc_status_ind_t *msg) {

      switch (msg->uicc_status)
      {
          case STE_UICC_STATUS_NO_CARD:
              return ste_sim_state_get_by_name(uicc_state_table, "closed");
          break;
          case STE_UICC_STATUS_REMOVED_CARD:
              ste_modem_reset_backend_state(catd_get_modem(), DUMMY_CT);
              catd_sig_reset(DUMMY_CT);
              return ste_sim_state_get_by_name(uicc_state_table, "closed");
          break;
          case STE_UICC_STATUS_REJECTED_CARD_INVALID:
              return ste_sim_state_get_by_name(uicc_state_table, "closed");
          case STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK:
              return ste_sim_state_get_by_name(uicc_state_table, "closed");
          case STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00:
              return ste_sim_state_get_by_name(uicc_state_table, "closed");
          break;
          default:
              break;
      }

      return current_state;
}

// -----------------------------------------------------------------------------
// UICC STATES
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// State: init
// State description: The state is entered after the startup has been performed.
//                    It takes care of the startup indications with the intent
//                    to progress the state machine to the idle state. The section
//                    contain helper functions to the init state and the init
//                    state function itself.
// -----------------------------------------------------------------------------

// Helper for init state to handle uicc_status_ind
static const ste_sim_state_t *uicc_state_init_uicc_status_ind(const ste_sim_state_t * current_state,
                                                              uiccd_msg_uicc_status_ind_t *msg,
                                                              int startup_fd,
                                                              uintptr_t startup_ct) {

  switch (msg->uicc_status) {
      case STE_UICC_STATUS_UNKNOWN: // Before uicc_card_ind(CARD_READY)
      case STE_UICC_STATUS_INIT:
          current.card_type = msg->card_type;
          simd_sig_startup_completed(startup_fd, SIMD_DAEMON_UICCD, 0, startup_ct);
          return current_state;
      case STE_UICC_STATUS_PIN_NEEDED:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_PIN_NEEDED");
          return ste_sim_state_get_by_name(uicc_state_table, "idle");
      case STE_UICC_STATUS_PUK_NEEDED:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_PUK_NEEDED");
          return ste_sim_state_get_by_name(uicc_state_table, "idle");
      case STE_UICC_STATUS_READY:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_READY");
          current.app_id = 1; //TODO: Remove when not accessed anymore
          current.app_type = msg->app_type;
          return ste_sim_state_get_by_name(uicc_state_table, "idle");
      case STE_UICC_STATUS_DISCONNECTED_CARD:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_DISCONNECTED_CARD");
          return current_state;
      case STE_UICC_STATUS_REJECTED_CARD_INVALID:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_REJECTED_CARD_INVALID");
          return ste_sim_state_get_by_name(uicc_state_table, "closed");
      case STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK");
          return ste_sim_state_get_by_name(uicc_state_table, "closed");
      case STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00");
          return ste_sim_state_get_by_name(uicc_state_table, "closed");
      case STE_UICC_STATUS_NO_CARD:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_NO_CARD");
          return ste_sim_state_get_by_name(uicc_state_table, "closed");
      case STE_UICC_STATUS_REMOVED_CARD:
          catd_log_f(SIM_LOGGING_D, "Got STE_UICC_STATUS_REMOVED_CARD");
          ste_modem_reset_backend_state(catd_get_modem(), DUMMY_CT);
          catd_sig_reset(DUMMY_CT);
          uiccd_reset_startup_complete();
          return ste_sim_state_get_by_name(uicc_state_table, "closed");
      case STE_UICC_STATUS_CLOSED:
      default:
          catd_log_f(SIM_LOGGING_E, "Got invalid card indication");
          return ste_sim_state_get_by_name(uicc_state_table, "closed");
  }
}

static const ste_sim_state_t *uicc_state_init_app_list_rsp(const ste_sim_state_t *current_state, uiccd_msg_app_list_t *msg) {
    int result;

    if (msg->status != SIM_UICC_STATUS_CODE_OK) {
        catd_log_f(SIM_LOGGING_E, "uicc : list apps on card failed with code %d", msg->status);
        return ste_sim_state_get_by_name(uicc_state_table,"closed");
    }

    catd_log_f(SIM_LOGGING_I, "uicc : handle_appl_list found %d apps on card", msg->app_len);

    result = ste_modem_activate_selected_app(catd_get_modem(), DUMMY_CT);

    if (result != 0) {
        catd_log_f(SIM_LOGGING_E, "uicc : select app on card failed");
        return ste_sim_state_get_by_name(uicc_state_table,"closed");
    }

    return current_state;
}

static const ste_sim_state_t *uicc_state_app_activate_rsp(const ste_sim_state_t *current_state, uiccd_msg_app_activate_t *msg) {
    const ste_sim_state_t *next_state;

    if (msg->status != 0) {
        next_state = ste_sim_state_get_by_name(uicc_state_table, "closed");
    } else {
        next_state = current_state;
    }

    current.app_id = msg->app_id;
    current.app_type = msg->app_type;

    return next_state;
}

// Init state function
static const ste_sim_state_t *uicc_state_init(const ste_sim_state_t * current_state, ste_msg_t *ste_msg, void *UNUSED(ud))
{
    const ste_sim_state_t   *new_state = current_state;
    int status;
    static ste_uicc_status_t current_status = STE_UICC_STATUS_UNKNOWN;
    ste_sim_ctrl_block_t * ctrl_p;

    /* Store the client information for the startup */
    static int startup_fd = 0;
    static int startup_ct = 0;

    catd_log_f(SIM_LOGGING_I, "uicc : IN %s function", current_state->ss_name);

    if (!ste_msg) {
      return STE_STATE_NOT_HANDLED;
    }

    switch ( ste_msg->type ) {

        case UICCD_MSG_STARTUP: {
            uiccd_msg_startup_t* msg = (uiccd_msg_startup_t *)ste_msg;
            catd_log_f(SIM_LOGGING_I, "uicc : %s got UICCD_MSG_STARTUP", __func__);

            startup_fd = msg->fd;
            startup_ct = msg->client_tag;

            ste_modem_card_status(catd_get_modem(), DUMMY_CT);

            ste_msg_delete(ste_msg);
            break;
        }

        case UICCD_MSG_UICC_STATUS_IND: {
            uiccd_msg_uicc_status_ind_t *msg = (uiccd_msg_uicc_status_ind_t *)ste_msg;
            catd_log_f(SIM_LOGGING_D, "uicc : got UICCD_MSG_UICC_STATUS_IND" );

            if (msg->uicc_status == STE_UICC_STATUS_STARTUP_COMPLETED) {
                uiccd_received_startup_complete();
                ste_msg_delete(ste_msg);
                break;;
            }
            if (msg->uicc_status == STE_UICC_STATUS_CLOSED) {
                catd_log_f(SIM_LOGGING_I, "uicc : got status STE_UICC_STATUS_CLOSED" );
                status = ste_modem_app_initiate_shutdown(catd_get_modem(), DUMMY_CT);
                if (status != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : App shutdown on card failed");
                }
                ste_msg_delete(ste_msg);
                break;
            }

            if (msg->uicc_status == STE_UICC_STATUS_INTERNAL_CARD_RECOVERY) {
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
                uiccd_msr_cache_pin();
#endif
                ste_msg_delete(ste_msg);
                break;
            }

            // ONLY case of interest for normal flow
            new_state = uicc_state_init_uicc_status_ind(current_state, msg, startup_fd, startup_ct);
            current_status = msg->uicc_status;
            if (msg->uicc_status == STE_UICC_STATUS_INIT) {
                status = ste_modem_application_list_and_select(catd_get_modem(), DUMMY_CT);
                if (status != 0) {
                    new_state = ste_sim_state_get_by_name(uicc_state_table,"closed");
                }
            }

            if (current_state != new_state) {
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
                //for MSR, we need to check if there is cached pin or not
                if (msg->uicc_status == STE_UICC_STATUS_PIN_NEEDED) {
                    char     cached_pin[PIN_MAX_LEN + 1];
                    uint8_t  cached_pin_len = 0;

                    //try to get the cached pin from file
                    cached_pin_len = uiccd_msr_get_cached_pin(cached_pin, PIN_MAX_LEN);
                    if (cached_pin_len > 0) {
                        //if cached pin exists, try to verify with this pin
                        if (0 == uiccd_msr_pin_verify(cached_pin, cached_pin_len)) {
                            ste_msg_delete(ste_msg);
                            break;
                        }
                    }
                    //otherwise continue
                }
#endif
                uiccd_main_send_state_change_event(new_state, msg->uicc_status);
            }
            ste_msg_delete(ste_msg);
            break;
        }

        case UICCD_MSG_RESET:
            status = uiccd_main_sim_reset(ste_msg);
            ste_msg_delete(ste_msg);
            break;

        case UICCD_MSG_SIM_POWER_OFF:
            status = uiccd_main_sim_power_off(ste_msg);
            ste_msg_delete(ste_msg);
            break;

        case UICCD_MSG_APP_LIST_RSP:
            new_state = uicc_state_init_app_list_rsp(current_state, (uiccd_msg_app_list_t *)ste_msg);

            if (current_state != new_state) {
                uiccd_main_send_state_change_event(new_state, STE_UICC_STATUS_CLOSED);
            }

            ste_msg_delete(ste_msg);
            break;

        case UICCD_MSG_APP_ACTIVATE_RSP:
            new_state = uicc_state_app_activate_rsp(current_state, (uiccd_msg_app_activate_t *)ste_msg);

            if (current_state != new_state) {
                uiccd_main_send_state_change_event(new_state, STE_UICC_STATUS_CLOSED);
            }

            ste_msg_delete(ste_msg);
            break;

        case UICCD_MSG_SIM_DISCONNECT_RSP:
            ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);

            if (ctrl_p && UICCD_MSG_SIM_POWER_OFF == ctrl_p->transaction_id) {
                status = uiccd_main_sim_power_off_response(ste_msg);
                if (!status) {
                    new_state = ste_sim_state_get_by_name(uicc_state_table,"disconnect");
                    uiccd_main_send_state_change_event(new_state, STE_UICC_STATUS_DISCONNECTED_CARD);
                }
            }
            ste_msg_delete(ste_msg);
            break;

        default:
            new_state = uicc_generic_response_handler(current_state, ste_msg);

            if (new_state == STE_STATE_NOT_HANDLED) {
                uiccd_main_not_ready_response(ste_msg);         /* msg not handled */
                ste_msg_delete(ste_msg);

                new_state = current_state; /* Stay in current state */
            }
            break;
    }

    return new_state;
}

// -----------------------------------------------------------------------------
// State: closed
// State description: The state is entered when any of the other states has
//                    detected that the modem has gone into a state where the
//                    SIM cannot be accessed for one reason or another. An
//                    example is when the SIM card is rejected or not present on
//                    modem startup or if it has been removed during normal operations.
//                    This state can at present time not be reverted. All
//                    modem indications are silently ignored.
// -----------------------------------------------------------------------------

static const ste_sim_state_t *uicc_state_closed(const ste_sim_state_t * current_state, ste_msg_t* ste_msg, void *UNUSED(ud))
{
    const ste_sim_state_t *next_state = current_state;
    ste_sim_ctrl_block_t * ctrl_p;

    int status;

    catd_log_f(SIM_LOGGING_I, "uicc : IN %s function", current_state->ss_name);

    if ( !ste_msg ) {
        return STE_STATE_NOT_HANDLED;
    }

    switch ( ste_msg->type ) {
        case UICCD_MSG_UICC_STATUS_IND:
            catd_log_f(SIM_LOGGING_I, "uicc : got UICCD_MSG_UICC_STATUS_IND" );
            uiccd_msg_uicc_status_ind_t *msg = (uiccd_msg_uicc_status_ind_t *)ste_msg;

            if (msg->uicc_status == STE_UICC_STATUS_CLOSED) {
                catd_log_f(SIM_LOGGING_I, "uicc : got status STE_UICC_STATUS_CLOSED" );
                status = ste_modem_app_initiate_shutdown(catd_get_modem(), DUMMY_CT);
                if (status != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : App shutdown on card failed");
                }
                ste_msg_delete(ste_msg);
                break;
            }

            if (msg->uicc_status == STE_UICC_STATUS_INTERNAL_CARD_RECOVERY) {
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
                uiccd_msr_cache_pin();
#endif
                ste_msg_delete(ste_msg);
                break;
            }

            ste_uicc_status_t uicc_status = msg->uicc_status;
            //Check if card is inserted after removal (hotswap) or after power on
            if(msg->uicc_status == STE_UICC_STATUS_INIT) {
              // Reinit by getting the state action function for the init state and
              // pass ste_msg to that function where ste_msg also is freed.
              next_state = ste_sim_state_get_by_name(uicc_state_table, "init");
              next_state = next_state->ss_action_func(next_state, ste_msg, NULL);
            } else {
              ste_msg_delete(ste_msg);
            }

            if (current_state != next_state) {
                uiccd_main_send_state_change_event(next_state, uicc_status);
            }
            break;

        case UICCD_MSG_RESET:
            status = uiccd_main_sim_reset(ste_msg);

            if ( !status) {
              next_state = ste_sim_state_get_by_name(uicc_state_table, "init");
              uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_INIT);
            } else {
              next_state = current_state; // Failed to reset, keep current state
            }

            ste_msg_delete(ste_msg);
            break;

        case UICCD_MSG_SIM_POWER_OFF:
            status = uiccd_main_sim_power_off(ste_msg);
            ste_msg_delete(ste_msg);
            break;

        case UICCD_MSG_PIN_INFO:
            status = uiccd_main_pin_info(ste_msg);
            ste_msg_delete(ste_msg);
            break;

        case UICCD_MSG_SIM_DISCONNECT_RSP:
            ctrl_p = (ste_sim_ctrl_block_t *)(ste_msg->client_tag);

            if (ctrl_p && UICCD_MSG_SIM_POWER_OFF == ctrl_p->transaction_id) {
              status = uiccd_main_sim_power_off_response(ste_msg);
              if (!status) {
                  next_state = ste_sim_state_get_by_name(uicc_state_table,"disconnect");
                  uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_DISCONNECTED_CARD);
              }
            }
            ste_msg_delete(ste_msg);
            break;

        default:
            next_state = uicc_generic_response_handler(current_state, ste_msg);

            if (next_state == STE_STATE_NOT_HANDLED) {
                uiccd_main_not_ready_response(ste_msg);         /* msg not handled */
                ste_msg_delete(ste_msg);

                next_state = current_state; /* Stay in current state */
            }
            break;
    }

    return next_state;
}

// -----------------------------------------------------------------------------
// State: idle
// State description: The state is entered when the the modem and the SIMD are
//                    ready for sending messages to the SIM.
// -----------------------------------------------------------------------------

// helper fn to compute right next state when status ind got while idle
static const ste_sim_state_t *uicc_state_idle_status_ind(const ste_sim_state_t *current_state, uiccd_msg_uicc_status_ind_t *msg) {
  switch (msg->uicc_status) {
      case STE_UICC_STATUS_PIN_VERIFIED:
      case STE_UICC_STATUS_PIN2_VERIFIED:
      case STE_UICC_STATUS_PIN_NEEDED:
      case STE_UICC_STATUS_PUK_NEEDED:
      case STE_UICC_STATUS_PIN2_NEEDED:
      case STE_UICC_STATUS_PUK2_NEEDED:
        return ste_sim_state_get_by_name(uicc_state_table, "idle");
      case STE_UICC_STATUS_NO_CARD:
      case STE_UICC_STATUS_REMOVED_CARD:
      case STE_UICC_STATUS_REJECTED_CARD_INVALID:
      case STE_UICC_STATUS_REJECTED_CARD_SIM_LOCK:
      case STE_UICC_STATUS_REJECTED_CARD_CONSECUTIVE_6F00:
        ste_modem_reset_backend_state(catd_get_modem(), DUMMY_CT);
        catd_sig_reset(DUMMY_CT);
        uiccd_reset_startup_complete();
        return ste_sim_state_get_by_name(uicc_state_table, "closed");
      default:
        return current_state;
  }
}

static const ste_sim_state_t *uicc_state_idle(const ste_sim_state_t * current_state,
                                               ste_msg_t* msg,
                                               void *UNUSED(ud))
{
    const ste_sim_state_t * next_state = current_state;
    int status;

    catd_log_f(SIM_LOGGING_D, "uicc : IN %s function", current_state->ss_name);
    if (!msg) {
      catd_log_f(SIM_LOGGING_E, "uicc : idle with app type=%d card type=%d", current.app_type, current.card_type);
    } else {
        switch ( msg->type ) {
        case UICCD_MSG_APP_ACTIVATE_RSP:
            catd_log_f(SIM_LOGGING_D, "uicc : got UICCD_MSG_APP_ACTIVATE_RSP in idle" );
            next_state = uicc_state_app_activate_rsp(current_state, (uiccd_msg_app_activate_t *)msg);

            if (current_state != next_state) {
                uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_CLOSED);
            }
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_UICC_STATUS_IND:
            if (((uiccd_msg_uicc_status_ind_t *)msg)->uicc_status == STE_UICC_STATUS_STARTUP_COMPLETED) {
                uiccd_received_startup_complete();
                ste_msg_delete(msg);
                break;
            }
            if (((uiccd_msg_uicc_status_ind_t *)msg)->uicc_status == STE_UICC_STATUS_CLOSED) {
                catd_log_f(SIM_LOGGING_I, "uicc : got status STE_UICC_STATUS_CLOSED" );
                status = ste_modem_app_initiate_shutdown(catd_get_modem(), DUMMY_CT);
                if (status != 0) {
                    catd_log_f(SIM_LOGGING_E, "uicc : App shutdown initiate on card failed");
                }
                ste_msg_delete(msg);
                break;
            }

            if (((uiccd_msg_uicc_status_ind_t *)msg)->uicc_status == STE_UICC_STATUS_INTERNAL_CARD_RECOVERY) {
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
                uiccd_msr_cache_pin();
#endif
                ste_msg_delete(msg);
                break;
            }

            next_state = uicc_state_idle_status_ind(current_state, (uiccd_msg_uicc_status_ind_t *)msg);
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
            //for MSR, we need to check if there is cached pin or not
            if (((uiccd_msg_uicc_status_ind_t *)msg)->uicc_status == STE_UICC_STATUS_PIN_NEEDED) {
                char     cached_pin[PIN_MAX_LEN + 1];
                uint8_t  cached_pin_len = 0;

                //try to get the cached pin from file
                cached_pin_len = uiccd_msr_get_cached_pin(cached_pin, PIN_MAX_LEN);
                if (cached_pin_len > 0) {
                    //if cached pin exists, try to verify with this pin
                    if (0 == uiccd_msr_pin_verify(cached_pin, cached_pin_len)) {
                        ste_msg_delete(msg);
                        break;
                    }
                }
                //otherwise continue
            }
#endif

            uiccd_main_send_state_change_event(next_state, ((uiccd_msg_uicc_status_ind_t *)msg)->uicc_status);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_PIN_CHANGE:
            status = uiccd_main_pin_change(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_PIN_VERIFY:
            status = uiccd_main_pin_verify(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_READ_SIM_FILE_RECORD:
            status = uiccd_main_read_sim_file_record(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_PIN_DISABLE:
            status = uiccd_main_pin_disable(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_PIN_ENABLE:
            status = uiccd_main_pin_enable(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_PIN_INFO:
            status = uiccd_main_pin_info(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_PIN_UNBLOCK:
            status = uiccd_main_pin_unblock(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_READ_SIM_FILE_BINARY:
            status = uiccd_main_read_sim_file_binary(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_UPDATE_SIM_FILE_RECORD:
            status = uiccd_main_update_sim_file_record(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_UPDATE_SIM_FILE_BINARY:
            status = uiccd_main_update_sim_file_binary(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_FILE_GET_FORMAT:
            status = uiccd_main_sim_file_get_format(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_GET_FILE_INFORMATION:
            status = uiccd_main_get_file_information(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_APPL_APDU_SEND:
            status = uiccd_main_appl_apdu_send(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_APP_INFO:
            uiccd_main_app_info(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_READ_SMSC:
            status = uiccd_main_read_smsc(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_UPDATE_SMSC:
            status = uiccd_main_update_smsc(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SMSC_GET_RECORD_MAX:
            status = uiccd_main_smsc_get_record_max(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SMSC_SAVE_TO_RECORD:
            status = uiccd_main_smsc_save_to_record(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SMSC_RESTORE_FROM_RECORD:
            status = uiccd_main_smsc_restore_from_record(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_FILE_READ_GENERIC:
            status = uiccd_main_sim_file_read_generic(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_UPDATE_SERVICE_TABLE:
            status = uiccd_main_update_service_table(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_GET_SERVICE_TABLE:
            status = uiccd_main_get_service_table(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_GET_SERVICE_AVAILABILITY:
            status = uiccd_main_get_service_availability(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_ICON_READ:
            status = uiccd_main_sim_icon_read(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_READ_SUBSCRIBER_NUMBER:
            status = uiccd_main_read_subscriber_number(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_READ_PLMN:
            status = uiccd_main_read_plmn(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_UPDATE_PLMN:
            status = uiccd_main_update_plmn(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_CHANNEL_SEND:
            status = uiccd_main_sim_channel_send(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_CHANNEL_OPEN:
            status = uiccd_main_sim_channel_open(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_CHANNEL_CLOSE:
            status = uiccd_main_sim_channel_close(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_READ_FDN:
            status = uiccd_main_read_fdn(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_READ_ECC:
            status = uiccd_main_read_ecc(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SAP_SESSION_START:
        {
            // to start SAP session card needs to be disconnected from uicc server
            status = uiccd_main_sim_disconnect(msg);
            if ( !status ) {
                uiccd_sap_substate = UICCD_SAP_SUBSTATE_CONNECTING;
                uiccd_sap_requestor_fd = ((uiccd_msg_fd_t *)msg)->fd;
                uiccd_sap_requestor_client_tag = msg->client_tag;
            } else {
                catd_log_f(SIM_LOGGING_E, "UICCD_MSG_SAP_SESSION_START :: uiccd_main_sim_disconnect failed");
            }
            ste_msg_delete(msg);
            break;
        }
        case UICCD_MSG_RESET:
            status = uiccd_main_sim_reset(msg);

            if ( !status) {
                next_state = ste_sim_state_get_by_name(uicc_state_table, "init");
                uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_INIT);
            } else {
                next_state = current_state; // Failed to reset, keep current state
            }
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_POWER_OFF:
            status = uiccd_main_sim_power_off(msg);
            ste_msg_delete(msg);
            break;

        case UICCD_MSG_SIM_READ_PREFERRED_RAT_SETTING:
            status = uiccd_main_sim_read_preferred_rat_setting(msg);
            ste_msg_delete(msg);
            break;

        default:
            next_state = uicc_generic_response_handler(current_state, msg);
            break;
        }
    }

    return next_state;
}

// -----------------------------------------------------------------------------
// State: sap
// State description: The state is entered when the client want's to use
//                    Bluetooth SAP (SIM Access Profile). The SIM card
//                    is disconnected in SAP mode and no other requests
//                    than the ones comming in through the APDU interface
//                    are handled.
// -----------------------------------------------------------------------------
static const ste_sim_state_t *uicc_state_sap (const ste_sim_state_t * current_state,
                                               ste_msg_t* msg,
                                               void *UNUSED(ud))
{
    const ste_sim_state_t * next_state = current_state;
    int status;

    catd_log_f(SIM_LOGGING_D, "uicc : IN %s function", current_state->ss_name);

    if (!msg) {
        catd_log_f(SIM_LOGGING_E, "uicc : sap with app type=%d card type=%d", current.app_type, current.card_type);
    } else {
        switch ( msg->type ) {
            case UICCD_MSG_SAP_SESSION_EXIT:
            case UICCD_MSG_SAP_SESSION_WARMRESET_CARD:
            case UICCD_MSG_SAP_SESSION_COLDRESET_CARD:
            case UICCD_MSG_SAP_SESSION_POWERON_CARD:
            case UICCD_MSG_SAP_SESSION_POWEROFF_CARD:
            case UICCD_MSG_SAP_SESSION_ATR_GET:
            case UICCD_MSG_SAP_SESSION_APDU_DATA:
            case UICCD_MSG_SAP_SESSION_STATUS_GET:
                // SAP specific request should be only handled if sent by the same client which
                // started SAP session; other requests will be handled below
                if (((uiccd_msg_fd_t*)msg)->fd != uiccd_sap_requestor_fd) {
                    uiccd_main_not_ready_response(msg);
                    ste_msg_delete(msg);
                    return next_state;
                }
                break;
        }

        switch ( msg->type ) {

            case UICCD_MSG_UICC_STATUS_IND:
                if (STE_UICC_STATUS_DISCONNECTED_CARD == ((uiccd_msg_uicc_status_ind_t *)msg)->uicc_status) {
                    // card is disconnected, now activate it as a part of SAP session startup
                    catd_log_f(SIM_LOGGING_D, "injecting SAP_SESSION_PWRON");
                    uiccd_sig_sap_session_ctrlcard(uiccd_sap_requestor_fd, uiccd_sap_requestor_client_tag, STE_UICC_REQ_SAP_SESSION_PWRON);
                } else {
                    next_state = uicc_state_illegal_uicc_status_ind(current_state, (uiccd_msg_uicc_status_ind_t *)msg);
                }

                if (current_state != next_state) {
                    uiccd_main_send_state_change_event(next_state, ((uiccd_msg_uicc_status_ind_t *)msg)->uicc_status);
                }

                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_RESET_CARD_IND:
                status = ((uiccd_msg_server_status_t*)msg)->status == 0 ? 0 : 1;
                uiccd_main_sap_session_start_response(status, uiccd_sap_requestor_client_tag);
                if ( !status ) {
                    // keep sap state
                    next_state = current_state;

                    uiccd_main_sap_session_status_ind(STE_SIM_CARD_STATUS_READY);

                    uiccd_sap_substate = UICCD_SAP_SUBSTATE_CONNECTED;
                    catd_log_f(SIM_LOGGING_I, "SAP mode initialized");
                } else {
                    uiccd_sap_requestor_fd = -1; // make sure we won't send anything to requestor anymore!
                    uiccd_sig_sap_session_exit(uiccd_sap_requestor_fd, 0);
                }
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_EXIT:
                status = uiccd_main_sap_session_poweroff_card(msg);
                if ( !status ) {
                    uiccd_sap_substate = UICCD_SAP_SUBSTATE_DISCONNECTING;
                }
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_WARMRESET_CARD:
                status = uiccd_main_sap_session_warmreset_card(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_COLDRESET_CARD:
                status = uiccd_main_sap_session_coldreset_card(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_POWERON_CARD:
                status = uiccd_main_sap_session_poweron_card(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_POWEROFF_CARD:
                status = uiccd_main_sap_session_poweroff_card(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_ATR_GET:
                status = uiccd_main_sap_session_atr_get(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_APDU_DATA:
                status = uiccd_main_sap_session_apdu_data(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_STATUS_GET:
                // it's not possible to get card reader status so always reply with ready status
                // this should be reworked in case HW will allow this in future
                uiccd_main_sap_session_status_get_response(STE_SIM_CARD_STATUS_READY, msg->client_tag);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_CONNECT_RSP:
                status = ((uiccd_msg_server_status_t*)msg)->status == 0 ? 0 : 1;
                uiccd_main_sap_session_exit_response(status, msg->client_tag);

                next_state = ste_sim_state_get_by_name(uicc_state_table,"init");

                uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_INIT);

                uiccd_sap_requestor_fd = -1;
                uiccd_sap_substate = UICCD_SAP_SUBSTATE_NONE;
                catd_log_f(SIM_LOGGING_I, "exited SAP mode");
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_POWEROFF_CARD_RSP:
                if (uiccd_sap_substate == UICCD_SAP_SUBSTATE_DISCONNECTING) {
                    status = uiccd_main_sim_connect(msg);
                    if ( status != 0 ) {
                        // cannot connect sim to uicc server, we're still in SAP mode!
                        uiccd_main_sap_session_exit_response(1, msg->client_tag);
                        uiccd_sap_substate = UICCD_SAP_SUBSTATE_CONNECTED;
                    }
                } else {
                    status = uiccd_main_sap_session_ctrlcard_deactivate_response(msg);
                    if ( status != 0 ) {
                        catd_log_f(SIM_LOGGING_E,"uicc : uiccd_main_sap_session_ctrlcard_deactivate_response failed.");
                    }
                }
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_POWERON_CARD_RSP:
                // check if card powered on due to SAP being connected
                if (uiccd_sap_substate == UICCD_SAP_SUBSTATE_CONNECTING) {
                    if (((uiccd_msg_sap_session_status_t*)msg)->status != 0) {
                        // card power on failed, need to respond to client and exit SAP session
                        uiccd_main_sap_session_start_response(1, uiccd_sap_requestor_client_tag);
                        uiccd_sap_requestor_fd = -1;
                        uiccd_sig_sap_session_exit(-1, 0);
                    }
                } else {
                    status = uiccd_main_sap_session_ctrlcard_activate_response(msg);
                }
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_ATR_GET_RSP:
                status = uiccd_main_sap_session_atr_get_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_APDU_DATA_RSP:
                status = uiccd_main_sap_session_apdu_data_response(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SAP_SESSION_WARMRESET_CARD_RSP:
            case UICCD_MSG_SAP_SESSION_COLDRESET_CARD_RSP:
                status = uiccd_main_sap_session_ctrlcard_coldreset_response(msg);
                ste_msg_delete(msg);
                break;

            default:
                next_state = uicc_generic_response_handler(current_state, msg);

                if (next_state == STE_STATE_NOT_HANDLED) {
                    uiccd_main_not_ready_response(msg);         /* msg not handled */
                    ste_msg_delete(msg);

                    next_state = current_state; /* Stay in current state */
                }
                break;
        }
    }
    return next_state;
}

// -----------------------------------------------------------------------------
// State: disconnect
// State description: The state is entered when the client wants to power off the
//                    SIM card. The SIM card is disconnected then, and no other requests
//                    than the connect reqeust are handled.
// -----------------------------------------------------------------------------

static const ste_sim_state_t *uicc_state_disconnect (const ste_sim_state_t * current_state,
                                                     ste_msg_t* msg,
                                                     void *UNUSED(ud))
{
    const ste_sim_state_t * next_state = current_state;
    int status;

    catd_log_f(SIM_LOGGING_D, "uicc : IN %s function", current_state->ss_name);

    if (!msg) {
        catd_log_f(SIM_LOGGING_E, "uicc : disconnect with app type=%d card type=%d", current.app_type, current.card_type);
    } else {
        switch ( msg->type ) {

            case UICCD_MSG_UICC_STATUS_IND:
                status = uiccd_main_disconnect_uicc_status_ind((uiccd_msg_uicc_status_ind_t *)msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_POWER_ON:
                status = uiccd_main_sim_power_on(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_SIM_CONNECT_RSP:
                status = uiccd_main_sim_power_on_response(msg);
                next_state = ste_sim_state_get_by_name(uicc_state_table,"init");

                uiccd_main_send_state_change_event(next_state, STE_UICC_STATUS_INIT);

                ste_msg_delete(msg);
                break;

            default:
                next_state = uicc_generic_response_handler(current_state, msg);

                if (next_state == STE_STATE_NOT_HANDLED) {
                    uiccd_main_not_ready_response(msg);         /* msg not handled */
                    ste_msg_delete(msg);

                    next_state = current_state; /* Stay in current state */
                }
                break;
        }
    }
    return next_state;
}
