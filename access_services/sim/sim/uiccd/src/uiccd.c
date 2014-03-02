/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * File name       : uiccd.c
 * Description     : Universal Integrated Circuit Card (UICC) main.
 *
 * Author          : Steve Critchlow <steve.critchlow@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <assert.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "catd_reader.h"
#include "cat_internal.h"
#include "sim_internal.h"

#include "cat_barrier.h"
#include "msgq.h"
#include "state_machine.h"
#include "client_data.h"
#include "func_trace.h"

#include "catd_modem.h"
#include "simd.h"
#include "uicc_internal.h"
#include "uiccd.h"
#include "uiccd_msg.h"
#include "uicc_interface_handlers.h"
#include "sim_unused.h"
#include "uicc_state_machine.h"

extern int ste_modem_server_status(ste_modem_t * m);

// Parameters used by the main loop
#define NO_STATE_USERDATA   (0)

// -----------------------------------------------------------------------------
// Section: UICCD message queue.
// Description: This section contains the uiccd message queue and the functions
//              used by the uiccd state machine to work with it.
// -----------------------------------------------------------------------------

// The UICCD message queue
static ste_msgq_t      *uiccd_mq = 0;

// Function used for adding a message to the uiccd message queue.
void                    uiccd_sig_msg( ste_msg_t* msg )
{
    if ( msg ) {
        ste_msgq_add(uiccd_mq, msg);
    }
}

// Function used for retrieving a message from the message queue.
ste_msg_t * uicc_get_any_msg(const ste_sim_state_t* UNUSED(s), void *UNUSED(ud))
{
    ste_msg_t* msg;
    msg = ste_msgq_pop(uiccd_mq);
    return msg;
}

// Function used to initiate the uiccd message queue at thread startup.
static void init_uiccd()
{
    uiccd_mq = ste_msgq_new();  // Main input message queue.
}

// Function used to delete and deallocate the message queue at thread shutdown.
static void fini_uiccd()
{
    ste_msgq_delete(uiccd_mq);
    uiccd_mq = 0;
#ifdef SIMD_SUPPORT_PIN_CACHE_FOR_MSR
    uiccd_msr_reset_pin_caching();
#endif
}

// -----------------------------------------------------------------------------
// Section: UICCD thread and command handling
// Description: This section contains functions for handling commands sent from
//              the client for handling by the SIMD. It also have the starter
//              function for the uicc thread.
// -----------------------------------------------------------------------------

// This function handles the commands sent from the SIMD clients. This function
// is executed in the reader thread.
void ste_uiccd_handle_command(uint16_t cmd, uintptr_t client_tag,
                              const char *buf, uint16_t len,
                              ste_sim_client_context_t * cc)
{
    switch (cmd) {

    case STE_UICC_REQ_REGISTER:
        uiccd_sig_register(cc->fd, client_tag);
        break;

    case STE_UICC_REQ_PIN_VERIFY:
        uiccd_sig_pin_verify(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_PIN_DISABLE:
        uiccd_sig_pin_disable(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_PIN_ENABLE:
        uiccd_sig_pin_enable(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_PIN_CHANGE:
        uiccd_sig_pin_change(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_PIN_INFO:
        uiccd_sig_pin_info(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_PIN_UNBLOCK:
        uiccd_sig_pin_unblock(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_READ_SIM_FILE_RECORD:
        uiccd_sig_read_sim_file_record(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_READ_SIM_FILE_BINARY:
        uiccd_sig_read_sim_file_binary(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_UPDATE_SIM_FILE_RECORD:
        uiccd_sig_update_sim_file_record(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_UPDATE_SIM_FILE_BINARY:
        uiccd_sig_update_sim_file_binary(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SIM_FILE_GET_FORMAT:
        uiccd_sig_sim_file_get_format(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_GET_FILE_INFORMATION:
        uiccd_sig_get_file_information(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_GET_SIM_STATE:
        uiccd_sig_get_sim_state(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_APP_INFO:
        uiccd_sig_get_app_info(cc->fd, client_tag);
        break;

    case STE_UICC_REQ_READ_SMSC:
        uiccd_sig_read_smsc(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_UPDATE_SMSC:
        uiccd_sig_update_smsc(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SMSC_GET_RECORD_MAX:
        uiccd_sig_smsc_get_record_max(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SMSC_SAVE_TO_RECORD:
        uiccd_sig_smsc_save_to_record(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SMSC_RESTORE_FROM_RECORD:
        uiccd_sig_smsc_restore_from_record(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SIM_FILE_READ_GENERIC:
        uiccd_sig_sim_file_read_generic(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_UPDATE_SERVICE_TABLE:
        uiccd_sig_update_service_table(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_GET_SERVICE_TABLE:
        uiccd_sig_get_service_table(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_GET_SERVICE_AVAILABILITY:
        uiccd_sig_get_service_availability(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SIM_ICON_READ:
        uiccd_sig_sim_icon_read(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_READ_SUBSCRIBER_NUMBER:
        uiccd_sig_read_subscriber_number(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_READ_PLMN:
        uiccd_sig_read_plmn(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_UPDATE_PLMN:
        uiccd_sig_update_plmn(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SIM_CHANNEL_SEND:
        uiccd_sig_sim_channel_send(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SIM_CHANNEL_OPEN:
        uiccd_sig_sim_channel_open(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SIM_CHANNEL_CLOSE:
        uiccd_sig_sim_channel_close(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_READ_FDN:
        uiccd_sig_read_fdn(cc->fd, client_tag);
        break;

    case STE_UICC_REQ_READ_ECC:
      uiccd_sig_read_ecc(cc->fd, client_tag);
      break;

    case STE_UICC_REQ_SAP_SESSION_START:
        uiccd_sig_sap_session_start(cc->fd, client_tag);
        break;

    case STE_UICC_REQ_SAP_SESSION_EXIT:
        uiccd_sig_sap_session_exit(cc->fd, client_tag);
        break;

    case STE_UICC_REQ_SAP_SESSION_PWRON:
    case STE_UICC_REQ_SAP_SESSION_PWROFF:
    case STE_UICC_REQ_SAP_SESSION_RESET:
    case STE_UICC_REQ_SAP_SESSION_ATR_GET:
        uiccd_sig_sap_session_ctrlcard(cc->fd, client_tag, cmd);
        break;

    case STE_UICC_REQ_SAP_SESSION_APDU_DATA:
        uiccd_sig_sap_session_apdu_data(cc->fd, client_tag, buf, len);
        break;

    case STE_UICC_REQ_SAP_SESSION_STATUS_GET:
        uiccd_sig_sap_session_status_get(cc->fd, client_tag);
        break;

    case STE_UICC_REQ_CARD_STATUS:
      uiccd_sig_card_status(cc->fd, client_tag);
      break;
    case STE_UICC_REQ_APP_STATUS:
      uiccd_sig_app_status(cc->fd, client_tag, buf, len);
      break;
    case STE_UICC_REQ_RESET:
      uiccd_sig_reset(cc->fd, client_tag);
      break;
    case STE_UICC_REQ_SIM_POWER_ON:
      uiccd_sig_sim_power_on(cc->fd, client_tag);
      break;
    case STE_UICC_REQ_SIM_POWER_OFF:
      uiccd_sig_sim_power_off(cc->fd, client_tag);
      break;
    case STE_UICC_REQ_SIM_READ_PREFERRED_RAT_SETTING:
      uiccd_sig_sim_read_preferred_rat_setting(cc->fd, client_tag);
      break;
#ifndef HAVE_ANDROID_OS
    case STE_UICC_REQ_SIM_SET_APP_TYPE_TO_SIM:
      ste_modem_set_app_type_to_sim();
      break;
#endif

    default:
        catd_log_f(SIM_LOGGING_E, "uicc : read BAD COMMAND: %04x len=%d", cmd, len);
    }
}

static void            *uiccd_main_loop_th(void *UNUSED(arg))
{
    ste_msg_t              *msg;
    cat_barrier_t          *bar = 0;
    setThreadName("uiccd_t");
    catd_log_f(SIM_LOGGING_I,"uicc : New Thread 0x%08lx",(unsigned long)pthread_self());

    const ste_sim_state_t  *current_state;
    const ste_sim_state_t  *next_state = STE_STATE_NOT_HANDLED;

    current_state = uicc_initiate_state_machine();

    catd_log_s(SIM_LOGGING_I, "uicc : Entering main loop", 0);

    // Enter the initial state.
    // .. If that returns a different state (except 0) we enter that instead
    // .. This allows "chaining".
    catd_log_f(SIM_LOGGING_I, "uicc : Enter state: %s", current_state->ss_name);
    next_state = current_state->ss_action_func(current_state, 0,
                                               NO_STATE_USERDATA);
    while ((next_state != current_state) && (next_state != 0)) {
        current_state = next_state;
        catd_log_f(SIM_LOGGING_I, "uicc : Enter state: %s", current_state->ss_name);
        next_state = current_state->ss_action_func(current_state, 0,
                                                   NO_STATE_USERDATA);
    }

    while (!bar) {
        catd_log_f(SIM_LOGGING_D, "uicc : in state %s", current_state->ss_name );

        msg = current_state->ss_getmsg_func(current_state, NO_STATE_USERDATA);
        if (!msg) {
            catd_log_s(SIM_LOGGING_E, "uicc : NULL message received", 0);
            continue;
        }

        catd_log_f(SIM_LOGGING_V, "uicc : in state %s msg->type %08x ",
                   current_state->ss_name, msg->type );

        // Messages that are handled independently of state are placed here.
        // Note that they are however picked out by the state get message
        // function.
        switch(msg->type) {
            case UICCD_MSG_SIM_STATUS:
                uiccd_main_sim_status(msg);
                ste_msg_delete(msg);
                continue;
            case UICCD_MSG_CARD_STATUS:
                uiccd_main_card_status(msg);
                ste_msg_delete(msg);
                continue;
            case UICCD_MSG_APP_STATUS:
                uiccd_main_app_status(msg);
                ste_msg_delete(msg);
                continue;
            case UICCD_MSG_GET_SIM_STATE:
                uiccd_main_get_sim_state(msg, current_state);
                ste_msg_delete(msg);
                continue;
            case UICCD_MSG_MODEM_SILENT_RESET:
                uiccd_main_modem_silent_reset(msg);
                ste_msg_delete(msg);
                continue;
            case UICCD_MSG_CONNECT:
            case UICCD_MSG_SHUTDOWN:
            case UICCD_MSG_STOP:
            case UICCD_MSG_REGISTER:
            case UICCD_MSG_DISCONNECT:
                catd_log_f(SIM_LOGGING_I, "uicc : invoke default handler for msg %x", msg->type);
                next_state = STE_STATE_NOT_HANDLED;
                break;
            default:
                next_state = current_state->ss_action_func(current_state, msg, NO_STATE_USERDATA);
              break;
        }

        // If state function returned STE_STATE_NOT_HANDLED, then
        // handle the message here instead.
        if ( next_state == STE_STATE_NOT_HANDLED) {

            catd_log_f(SIM_LOGGING_D, "uicc : ANY state");

            switch (msg->type) {

            case UICCD_MSG_REGISTER:
                uiccd_main_register(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_DISCONNECT:
                uiccd_main_disconnect(msg);
                ste_msg_delete(msg);
                break;

            case UICCD_MSG_STOP:
                uiccd_main_stop(msg);
                bar = ((uiccd_msg_stop_t *) msg)->bar;
                ste_msg_delete(msg);
                break;

            default:
                catd_log_f(SIM_LOGGING_E, "uicc : unknown message %d received", msg->type);
                ste_msg_delete(msg);
                break;
            }
        }
        else {

            while ( (next_state != current_state) &&
                    (next_state != STE_STATE_NOT_HANDLED)) {
                current_state = next_state;
                catd_log_f(SIM_LOGGING_I, "uicc : Enter state: %s", current_state->ss_name);
                next_state = current_state->ss_action_func(current_state, 0,
                                                           NO_STATE_USERDATA);
            }
        }

    }

    catd_log_s(SIM_LOGGING_I, "uicc : exit main loop, thread dying.", 0);

    if (bar) {
        cat_barrier_release(bar, 0);
    }

    return 0;
}

int uiccd_start(uintptr_t client_tag)
{
    int                     rv;
    pthread_t               tid;

    catd_log_s(SIM_LOGGING_I, "uicc : start", client_tag);
    init_uiccd();

    rv = sim_launch_thread(&tid, PTHREAD_CREATE_DETACHED, uiccd_main_loop_th, 0);
    if (rv < 0) {
        catd_log_s(SIM_LOGGING_E, "uicc : Failed to launch thread", client_tag);
        return rv;
    }

    return 0;
}

int uiccd_stop(uintptr_t client_tag)
{
    uiccd_msg_stop_t       *msg;
    cat_barrier_t          *bar;        /* Barrier to sync shutdown */

    bar = cat_barrier_new();
    if ( !bar ) {
        return -1;
    }

    cat_barrier_set(bar);

    msg = uiccd_msg_stop_create(bar, client_tag);
    if ( !msg ) {
        return -1;
    }

    ste_msgq_add(uiccd_mq, (ste_msg_t *) msg);

    if ( cat_barrier_wait(bar, 0) != 0 ) {   /* Wait for thread to die */
        catd_log_f(SIM_LOGGING_E, "%s: cat_barrier_wait failed", __func__);
        return -1;
    }

    cat_barrier_delete(bar);

    fini_uiccd();

    return 0;
}
