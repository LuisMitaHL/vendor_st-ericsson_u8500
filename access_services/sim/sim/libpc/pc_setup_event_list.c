/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_setup_event_list.c
 * Description     : Handler function for proactive command setup event list.
 *
 * Author          : Mikael Johansson <mikael.a.johansson@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

#include "pc_utilities.h"
#include "sim_internal.h"
#include "catd.h"
#include "catd_msg.h"
#include "apdu.h"
#include "sim_unused.h"
#include "func_trace.h"
#include "catd_cn_client.h"

//to trace the current_reg_info event list we are listening to
static unsigned int  event_list = 0;

//Added cn_message_type_t to prevent multiple MT_call events.
static struct {
    cn_rat_type_t rat;
    cn_network_search_mode_t search_mode;
} current_reg_info = {-1, -1};

//to trace the call state which would be used by other functionalities
static cn_call_state_t gcall_state = CN_CALL_STATE_IDLE;

// Added to prevent multiple Location Status Events. Now this will get sent
// only if there is a change in the status
static cn_nw_service_status_t previous_loc_status_sent = CN_NW_LIMITED_SERVICE_WAITING;
static ste_apdu_t* loc_status_apdu_p = NULL;
static uint8_t is_location_status_apdu_sending_in_progress = 0;

extern uint8_t ENABLE_VIVO_ROAMING_BROKER;

/*************************************************************************
 * @brief:    handle the particular proactive command of setup event list internally
 * @params:
 *            parsed_apdu: the parsed apdu structure.
 *            msg:         the original message with APDU data, and client_tag, etc. for possible future usage.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static int pc_main_handle_setup_event_list(ste_parsed_apdu_t * parsed_apdu_p,
                                            catd_msg_apdu_t   * msg)
{
    ste_sat_apdu_error_t rv;
    ste_apdu_pc_set_up_event_list_t * sel_p;
    ste_apdu_t * apdu = msg->apdu;

    rv = ste_parsed_apdu_get(parsed_apdu_p, (void**) &sel_p, STE_APDU_CMD_TYPE_SET_UP_EVENT_LIST);

    if (sel_p && rv == STE_SAT_APDU_ERROR_NONE) {

        if (sel_p->bit_mask & PC_SET_UP_EVENT_LIST_Event_present) {

            // Most events are from CN, make sure we have a CN client
            if (NULL == cn_get_client()) {
                catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected no CN client.",__PRETTY_FUNCTION__);
                pc_send_terminal_response_error(STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION, apdu);

                ste_parsed_apdu_delete(parsed_apdu_p);
                return -1;
            }

            event_list = sel_p->event_list;

            catd_pc_send_pc_notification_ind(apdu);

            pc_send_terminal_response_general(parsed_apdu_p, SAT_RES_CMD_PERFORMED_SUCCESSFULLY);
            ste_parsed_apdu_delete(parsed_apdu_p);

            return 1; //Tell framework that we already done the notification.
        }
        rv = SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }

    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected.",__PRETTY_FUNCTION__);
    pc_send_terminal_response_error(rv, apdu);

    ste_parsed_apdu_delete(parsed_apdu_p);
    return -1;
}

/*************************************************************************
 * @brief:    handle the particular proactive command of setup event list
 * @params:
 *            msg:         the original message with APDU data, and client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
int catd_handle_pc_setup_event_list(catd_msg_apdu_t  * msg)
{
    ste_sat_apdu_error_t                 rv;
    ste_apdu_t                         * apdu = msg->apdu;
    ste_parsed_apdu_t                  * parsed_apdu_p = NULL;

    rv = ste_apdu_parse(apdu, &parsed_apdu_p);

    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : parse apdu failed");
        pc_send_terminal_response_error(rv, apdu);
        ste_parsed_apdu_delete(parsed_apdu_p);
        return -1;
    }

    return pc_main_handle_setup_event_list(parsed_apdu_p, msg);
}

/*************************************************************************
 * @brief:    handle the event from call network
 * @params:
 *            cn_message      the message from cn that includes the event data
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_handle_event(ste_msg_t* ste_msg)
{
    ste_apdu_t* apdu_p = NULL;

    catd_msg_cn_t *cn_message = (catd_msg_cn_t*) ste_msg;
    cn_message_type_t type = cn_message->cn_msg->type;

    catd_log_f(SIM_LOGGING_D, "catd %s: msg_type=0x%X event_list=0x%x", __func__, type, event_list);
    switch(type)
    {
        case CN_EVENT_MODEM_REGISTRATION_STATUS:
        {
            cn_registration_info_t * reg_info_p = (cn_registration_info_t*) cn_message->cn_msg->payload;

            if (reg_info_p->rat != current_reg_info.rat && (event_list & (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED
                    << STE_SAT_SET_UP_EVENT_LIST_ACCESS_TECHNOLOGY_CHANGE)))
            {
                //Rat mode changed
                current_reg_info.rat = reg_info_p->rat;
                apdu_p = ste_apdu_event_download(type, cn_message->cn_msg->payload, STE_SAT_KIND_OF_EVENT_REQUESTED_RAT_MODE);
                catd_sig_ec(CATD_FD, apdu_p, CATD_CLIENT_TAG);

                //clean up
                ste_apdu_delete(apdu_p);
            }

            if (reg_info_p->search_mode != current_reg_info.search_mode && (event_list & (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED
                    << STE_SAT_SET_UP_EVENT_LIST_NW_SEARCH_MODE_CHANGE)))
            {
                //Search mode changed
                current_reg_info.search_mode = reg_info_p->search_mode;
                apdu_p = ste_apdu_event_download(type, cn_message->cn_msg->payload, STE_SAT_KIND_OF_EVENT_REQUESTED_SEARCH_MODE);
                catd_sig_ec(CATD_FD, apdu_p, CATD_CLIENT_TAG);

                //clean up
                ste_apdu_delete(apdu_p);
            }
        }
        break;

        case CN_EVENT_CALL_STATE_CHANGED:
        {
            cn_call_context_t * call_context_p = (cn_call_context_t*) cn_message->cn_msg->payload;

            catd_log_f(SIM_LOGGING_D, "catd %s: call state %d", __func__, call_context_p->call_state);
            if (cn_message->cn_msg->payload_size == 0) {
                //Note the we still want to consume the event so true will be returned
                gcall_state = CN_CALL_STATE_IDLE;
            }
            else if (call_context_p->call_state == CN_CALL_STATE_ACTIVE)
            {
                //save the call state to be used by other functionalities
                gcall_state = call_context_p->call_state;

                if (event_list & (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED << STE_SAT_SET_UP_EVENT_LIST_CALL_CONNECTED)) {
                    // event call connected
                    apdu_p = ste_apdu_event_download(type, cn_message->cn_msg->payload, STE_SAT_KIND_OF_EVENT_REQUESTED_CALL_CONNECTED);
                    catd_sig_ec(CATD_FD, apdu_p, CATD_CLIENT_TAG);

                    //clean up
                    ste_apdu_delete(apdu_p);
                }
            }
            else if (call_context_p->call_state == CN_CALL_STATE_IDLE)
            {
                //save the call state to be used by other functionalities
                gcall_state = call_context_p->call_state;

                if (event_list & (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED << STE_SAT_SET_UP_EVENT_LIST_CALL_DISCONNECTED)) {
                    //event call disconnected
                    apdu_p = ste_apdu_event_download(type, cn_message->cn_msg->payload, STE_SAT_KIND_OF_EVENT_REQUESTED_CALL_DISCONNECTED);
                    catd_sig_ec(CATD_FD, apdu_p, CATD_CLIENT_TAG);

                    //clean up
                    ste_apdu_delete(apdu_p);
                }
            }
            else if (call_context_p->call_state == CN_CALL_STATE_MT_SETUP) {
                //save the call state to be used by other functionalities
                gcall_state = call_context_p->call_state;
                if(event_list & (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED << STE_SAT_SET_UP_EVENT_LIST_MT_CALL))
                {
                    //MT call
                    apdu_p = ste_apdu_event_download(type, cn_message->cn_msg->payload, STE_SAT_KIND_OF_EVENT_REQUESTED_INCOMING);
                    catd_sig_ec(CATD_FD, apdu_p, CATD_CLIENT_TAG);

                    //clean up
                    ste_apdu_delete(apdu_p);
                }
            }
            else
            {
                //save the call state to be used by other functionalities
                gcall_state = call_context_p->call_state;
            }
        }
        break;
        case CN_EVENT_CELL_INFO:
        {
            if (event_list & (STE_SAT_SET_UP_EVENT_LIST_BIT_SEED << STE_SAT_SET_UP_EVENT_LIST_LOCATION_STATUS))
            {
                //Location status and location information
                cn_cell_info_t *cell_info_p = (cn_cell_info_t*)(cn_message->cn_msg->payload);
                if (cell_info_p && cell_info_p->service_status != CN_NW_LIMITED_SERVICE_WAITING) {
                    if ( ENABLE_VIVO_ROAMING_BROKER == 0 ) {
                        if ( previous_loc_status_sent == cell_info_p->service_status &&
                             cell_info_p->service_status != CN_NW_SERVICE ) {
                            catd_log_f(SIM_LOGGING_D, "catd %s: Loc ST not sent", __func__);
                            break;
                        }
                    } else {
                        catd_log_f(SIM_LOGGING_D, "catd %s: Cell Info Ind: Prev=%d Curr=%d",
                                   __func__, previous_loc_status_sent, cell_info_p->service_status);
                        switch( cell_info_p->service_status ) {
                            case CN_NW_SERVICE: // Send the envelope in Full Service anyway.
                                break;
                            case CN_NW_LIMITED_SERVICE: // Do not send envelope if prev is Limited
                                if ( previous_loc_status_sent == CN_NW_LIMITED_SERVICE ) {
                                    goto exit_cell_info;
                                }
                                break;
                            case CN_NW_NO_COVERAGE: // Do not send if prev is limited/No coverage
                                if ( previous_loc_status_sent == CN_NW_LIMITED_SERVICE ||
                                     previous_loc_status_sent == CN_NW_NO_COVERAGE ) {
                                    goto exit_cell_info;
                                }
                                break;
                            default:
                                goto exit_cell_info;
                        }
                    }
                    previous_loc_status_sent = cell_info_p->service_status;
                    is_location_status_apdu_sending_in_progress = 1;
                    /* If Pending, then send only the latest */
                    if ( loc_status_apdu_p ) {
                        ste_apdu_delete (loc_status_apdu_p);
                        loc_status_apdu_p = NULL;
                    }
                    apdu_p = ste_apdu_event_download(type, cn_message->cn_msg->payload, STE_SAT_KIND_OF_EVENT_REQUESTED_LOCATION_STATUS);

                    loc_status_apdu_p = ste_apdu_copy(apdu_p);

                    catd_sig_ec(CATD_FD, apdu_p, CATD_CLIENT_TAG);

                    //clean up
                    ste_apdu_delete(apdu_p);
                }
            }
        }
        exit_cell_info:
        break;
        default:
        {
        }
        break;
    }
}

/*************************************************************************
 * @brief:    the interface for outsider to get the current call state
 * @params:
 *            void
 *
 * @return:   current call state
 *
 * Notes:
 *************************************************************************/
cn_call_state_t catd_get_current_call_state(void)
{
    return gcall_state;
}

/*************************************************************************
 * @brief:   Reset the Loc status flags
 * @params:
 *           is_complete_reset - Specifies whether both flag and APDU is to
 *                               be reset.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_reset_loc_status( uint8_t is_complete_reset )
{
    if ( is_complete_reset && loc_status_apdu_p ) {
        ste_apdu_delete(loc_status_apdu_p);
        loc_status_apdu_p = NULL;
    }
    is_location_status_apdu_sending_in_progress = 0;
}

/*************************************************************************
 * @brief:    reset setup eventlist subscriptions and its states
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_reset_pc_setup_event_list(void) {
    // Invalidate all subscriptions done by setup event list.
    event_list = 0;
    // Reset tracker of current call state
    gcall_state = CN_CALL_STATE_IDLE;
     previous_loc_status_sent = CN_NW_LIMITED_SERVICE_WAITING;
    catd_reset_loc_status(1);
}

/*************************************************************************
 * @brief:    Check and re-send Location Status
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_pc_check_for_location_status_retransmissions(void)
{
    if ( is_location_status_apdu_sending_in_progress == 0 &&
         loc_status_apdu_p != NULL ) {
        is_location_status_apdu_sending_in_progress = 1;
        catd_sig_ec(CATD_FD, loc_status_apdu_p, CATD_CLIENT_TAG);
    }
    return;
}

