/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_setup_call.c
 * Description     : Handler function for proactive command setup call.
 *
 * Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>

#include "pc_utilities.h"
#include "sim_internal.h"
#include "catd.h"
#include "catd_msg.h"
#include "catd_cn_client.h"
#include "apdu.h"
#include "cn_client.h"
#include "pc_handler.h"

#include "simd_timer.h"

typedef struct {
    uint8_t  call_id;
    uint8_t  hung_up;
    uint8_t  sc_count;
    uint32_t timer;
} timed_hangup_t;

#define TIMED_HANGUP_T_INITIALIZER {0xFF, 0xFF, 0, 0}

static timed_hangup_t* timed_hangup_get_globaldata();
static int  timed_hangup_response_handler(ste_msg_t* ste_msg);
static void timed_hangup_timeout_handler(uintptr_t timeout_event);
static void timed_hangup_call_state_change_handler(ste_msg_t* ste_msg);
static void timed_hangup_initiate(uint8_t call_id, uint32_t millisecs);
static void timed_hangup_stop();

//globals for setup call
static ste_apdu_pc_setup_call_t       * setup_call_p = NULL;
static ste_apdu_t                     * setup_call_apdu_p = NULL;
static ste_parsed_apdu_t              * setup_call_parsed_apdu_p = NULL;

static int cn_dial_response_handler(ste_msg_t *ste_msg);
static int cn_initial_call_list_response_handler(ste_msg_t* ste_msg);
static int cn_final_call_list_response_handler(ste_msg_t* ste_msg);
static int cn_hangup_call_response_handler(ste_msg_t* ste_msg);
static int cn_hold_call_response_handler(ste_msg_t* ste_msg);
static int dispatch_client_confirmation();

cn_dial_t pc_create_dial_details();

// Call id for setup call, only valid if dial request succeeded.
static uint8_t call_id = 0xFF;

/*************************************************************************
 * @brief:    request call/net module to setup the call based on the call setup details
 * @params:
 *
 * @return:   the status of the call setup
 *
 * Notes:
 *************************************************************************/
static int pc_request_setup_call()
{
    ste_sim_ctrl_block_t* ctrl_p = NULL;
    cn_dial_t             dial_details;

    dial_details = pc_create_dial_details();

    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                       CATD_FD,
                                       CATD_MSG_NONE,
                                       cn_dial_response_handler,
                                       NULL);

    if (ctrl_p) {
        catd_log_f(SIM_LOGGING_D, "%s : Now dialing %s", __func__, dial_details.phone_number);
        if (CN_SUCCESS != cn_request_dial(cn_get_client(), &dial_details, (uintptr_t)ctrl_p)) {
            catd_log_f(SIM_LOGGING_E, "%s : cn_request_dial failed", __func__);
            catd_send_answer_call_response(-1);
            return -1;
        }
    } else {
        catd_log_f(SIM_LOGGING_E, "%s : failed to create ctrl_p", __func__);
        catd_send_answer_call_response(-1);
        return -1;
    }

    // notify client about the answer call result
    catd_send_answer_call_response(0);
    return 0;
}

/**
 * Check if other calls needs to be dealt with
 */
static int pc_request_check_other_calls()
{
    ste_sim_ctrl_block_t* ctrl_p = NULL;
    catd_log_f(SIM_LOGGING_E, "%s : ENTER", __func__);
    if (STE_APDU_DISCONNECT_ALL_OTHER_CALLS == setup_call_p->command_details.option ||
            STE_APDU_PUT_ALL_OTHER_CALLS_ON_HOLD ==  setup_call_p->command_details.option) {
        ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                           CATD_FD,
                                           CATD_MSG_NONE,
                                           cn_final_call_list_response_handler,
                                           NULL);

        if (CN_SUCCESS != cn_request_current_call_list(cn_get_client(), (uintptr_t) ctrl_p)) {
            catd_log_f(SIM_LOGGING_E, "%s : cn_request_current_call_list failed", __func__);
            return -1;
        }
        return 0;
    }

    // No need to handle other calls, setup the call
    if (pc_request_setup_call() != 0) {
        return -1;
    }

    return 0;
}

/*************************************************************************
* @brief:    create dial details from PC
* @params:
*
* @return:   cn_dial_t
*
* Notes:
*************************************************************************/
cn_dial_t pc_create_dial_details()
{
    cn_dial_t dial_details;
    memset(&dial_details, 0, sizeof(cn_dial_t));

    char* phone_nbr_dst = dial_details.phone_number;
    size_t phone_nbr_remaining = CN_MAX_STRING_SIZE;

    dial_details.sat_initiated = TRUE; //no CC will be performed by CN
    dial_details.clir = 0; //use subscription default value

    if (setup_call_p->address.str_length == 2 &&
            setup_call_p->address.dial_text_p[0] == 0x11 &&
            setup_call_p->address.dial_text_p[1] == 0xF2) {
        catd_log_f(SIM_LOGGING_D, "%s : Emergency call detected", __func__);
        dial_details.call_type = CN_CALL_TYPE_EMERGENCY_CALL;
    } else {
        dial_details.call_type = CN_CALL_TYPE_VOICE_CALL;
    }

    /*
     * Add a '+' as first char in dial string if dialing an international number.
     * Bit 5-7 of ton_npi is the TON
     */
    if (((setup_call_p->address.ton_npi >> 4) & 0x07) == STE_SIM_TON_INTERNATIONAL) {
        *phone_nbr_dst++ = '+';
        phone_nbr_remaining--;
    }

    if (convert_bcd2ascii(setup_call_p->address.dial_text_p,
                          setup_call_p->address.str_length,
                          phone_nbr_dst,
                          phone_nbr_remaining) <= 0)
    {
        catd_log_f(SIM_LOGGING_E, "%s : Failed to convert dial string", __func__);
    }

    //ccp
    if (setup_call_p->bit_mask & PC_SETUP_CALL_CCP_present)
    {
        dial_details.bc_length = MIN(setup_call_p->ccp.ccp_length, CN_BEARER_CAPABILITY_DATA_MAX_SIZE);
        memcpy(dial_details.bc_data, setup_call_p->ccp.ccp_p, dial_details.bc_length);
    }

    return dial_details;
}

/*************************************************************************
 * @brief:    clean everything after the PC is done
 * @params:
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void pc_setup_call_data_reset()
{
    catd_log_f(SIM_LOGGING_I, "ENTER %s", __func__);

    setup_call_p = NULL;
    if (setup_call_parsed_apdu_p)
    {
        catd_log_f(SIM_LOGGING_I, "%s delete sc_parsed_apdu_p", __func__);
        ste_parsed_apdu_delete(setup_call_parsed_apdu_p);
        setup_call_parsed_apdu_p = NULL;
    }
    if (setup_call_apdu_p)
    {
        ste_apdu_delete(setup_call_apdu_p);
        setup_call_apdu_p = NULL;
    }
    call_id = 0xFF;
}

/*************************************************************************
 * @brief:    send a TR to card
 * @params:
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void pc_send_terminal_response_setup_call(ste_apdu_general_result_t         general_result,
                                            ste_apdu_additional_info_t      * add_info_p,
                                            ste_cat_call_control_response_t * UNUSED(cc_rsp_p))
{
    ste_command_result_t             result;
    ste_apdu_t                     * tr_apdu_p = NULL;
    ste_sat_apdu_error_t             rv;

    result.general_result = general_result;
    result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    if (add_info_p)
    {
        result.additional_info_size = 1;
        result.additional_info_p = (uint8_t*)add_info_p;
    }
    else
    {
        result.additional_info_size = 0;
        result.additional_info_p = NULL;
    }

    result.other_data_type = STE_CMD_RESULT_NOTHING;
    result.other_data.cc_result_p = NULL;

    rv = ste_apdu_terminal_response(setup_call_parsed_apdu_p, &result, &tr_apdu_p);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
        catd_log_f(SIM_LOGGING_E, "catd : failed to create terminal response.");
        pc_send_terminal_response_error(rv, setup_call_apdu_p);

        if (tr_apdu_p != NULL) {
            ste_apdu_delete(tr_apdu_p);
        }

        return;
    }
    catd_sig_tr(CATD_FD, tr_apdu_p, CATD_CLIENT_TAG);
    ste_apdu_delete(tr_apdu_p);
}

static int cn_dial_response_handler(ste_msg_t *ste_msg)
{
    catd_msg_cn_t*                    msg_p;
    cn_message_t*                     cn_msg_p;
    ste_apdu_additional_info_t        add_info;

    catd_log_f(SIM_LOGGING_I, "ENTER %s", __func__);

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*)ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*)msg_p->cn_msg;

    if (CN_RESPONSE_DIAL == cn_msg_p->type) {
        if (cn_msg_p->error_code == CN_SUCCESS) {
            cn_response_dial_t* dial_response = (cn_response_dial_t*)cn_msg_p->payload;

            if (dial_response->service_type == CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL) {

                catd_log_f(SIM_LOGGING_D, "catd/libpc : %s got real dial response", __PRETTY_FUNCTION__);

                // Save the call id to differentiate our call from others
                call_id = dial_response->data.call_id;

                if (setup_call_p->bit_mask & PC_SETUP_CALL_Duration_present) {
                    uint32_t millisecs = setup_call_p->duration * 100;
                    if (call_id && millisecs) {
                        timed_hangup_initiate(call_id, millisecs);
                    }
                }

                 /* This state means that the modem has started to initiate the call, we don't know
                 * yet if it will be successfully set up or not, that will be given in the event
                 * CN_EVENT_CALL_STATE_CHANGED. We can trust the cn module to send this so there
                 * isn't need for internal timer for error handling. Setup_call_p will tell us
                 * to look for this event so no need for a internal FSM. */

            } else {
                catd_log_f(SIM_LOGGING_D, "%s : set up call modified by call control to unsupported service type %d", __func__, dial_response->service_type);
                pc_send_terminal_response_error(STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, setup_call_apdu_p);
                pc_setup_call_data_reset();
                return -1;
            }
        } else if (cn_msg_p->error_code == CN_REQUEST_CC_REJECTED) {
            catd_log_f(SIM_LOGGING_D, "%s : set up call rejected by call control",    __func__);
            add_info = SAT_RES_CC_ACTION_NOT_ALLOWED;
            pc_send_terminal_response_setup_call(SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_PERM,
                                                 &add_info,
                                                 NULL);
            pc_setup_call_data_reset();
            return -1;
        } else {
            //something wrong, we end the PC
            catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
            pc_send_terminal_response_error(STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION, setup_call_apdu_p);
            pc_setup_call_data_reset();
            return -1;
        }
    } else if (CN_RESPONSE_USSD == cn_msg_p->type) {
        /* This is for the case CC -> USSD (CC with modification of service USSD).
         * A response to SAT will first be sent when CN is done with the USSD request.
         */
        if (cn_msg_p->error_code == CN_SUCCESS) {

            /* If CN returned successfully then we expect an event and the TR will be sent from pc_send_ussd,
             * therefore we need to stash some data for pc_send_ussd.
             */

            cn_response_ussd_t* cn_ussd_p = (cn_response_ussd_t*)cn_msg_p->payload;

            set_stash_for_cc_ussd(cn_ussd_p->session_id, setup_call_apdu_p);
            pc_setup_call_data_reset();
        } else {
            //USSD returned unsuccessfully
            ste_apdu_additional_info_t add_info = SAT_RES_CC_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

            //Build the first command result for this error case
            ste_command_result_t command_result_first = {SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_TEMP,
                    STE_SAT_PC_CMD_STATUS_NONE,
                    0,
                    NULL,
                    STE_CMD_RESULT_NOTHING,
                    {NULL}};

            //Build the second command result for this error case
            ste_command_result_t command_result_second = {SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
                    STE_SAT_PC_CMD_STATUS_NONE,
                    1,
                    (uint8_t*)(&add_info),
                    STE_CMD_RESULT_NOTHING,
                    {NULL}};

            ste_apdu_t* tr = NULL;

            catd_log_f(SIM_LOGGING_D, "%s : error detected, CN error code = 0x%x, line = %d", __func__, cn_msg_p->error_code, __LINE__);

            (void)ste_apdu_terminal_response_with_second_result(setup_call_parsed_apdu_p, &command_result_first, &command_result_second,&tr);

            catd_sig_tr(CATD_FD, tr, CATD_CLIENT_TAG);

            ste_apdu_delete(tr);
            pc_setup_call_data_reset();
            return -1;
        }

    } else {
        //something wrong, we end the PC
        catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION, setup_call_apdu_p);
        pc_setup_call_data_reset();
        return -1;
    }

    catd_log_f(SIM_LOGGING_I, "EXIT %s", __func__);
    return 0;
}

/*************************************************************************
 * @brief:    handle the particular proactive command of setup call internally
 * @params:
 *            parsed_apdu_p: the parsed apdu structure.
 *            msg:           the original message with APDU data, and client_tag, etc. for possible future usage.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void pc_main_handle_setup_call( ste_parsed_apdu_t    * parsed_apdu_p,
                                       catd_msg_apdu_t      * msg)
{
    ste_sat_apdu_error_t                   rv;
    ste_apdu_t                           * apdu = msg->apdu;
    ste_sim_ctrl_block_t                 * ctrl_p = NULL;

    setup_call_apdu_p = ste_apdu_copy(apdu);

    rv = ste_parsed_apdu_get(parsed_apdu_p, (void**)&setup_call_p, STE_APDU_CMD_TYPE_SET_UP_CALL);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu);
        pc_setup_call_data_reset();
        return;
    }

    if (!setup_call_p)
    {
        catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu);
        pc_setup_call_data_reset();
        return;
    }

    if (setup_call_p->bit_mask & PC_SETUP_CALL_SubAddress_present) {
        // No support for called party subaddress
        catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
        catd_log_f(SIM_LOGGING_E, "%s : No support for called party subaddress", __func__);
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_COMMAND_BEYOND_ME_CAPABILITY, apdu);
        pc_setup_call_data_reset();
        return;
    }

    if (setup_call_p->bit_mask & PC_SETUP_CALL_Address_present)
    {
        if (STE_APDU_ONLY_IF_NOT_BUSY == setup_call_p->command_details.option) {
            // if busy on another call, dont setup new call
            ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                               CATD_FD,
                                               CATD_MSG_NONE,
                                               cn_initial_call_list_response_handler,
                                               NULL);

            if (CN_SUCCESS != cn_request_current_call_list(cn_get_client(), (uintptr_t) ctrl_p)) {
                catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
                catd_log_f(SIM_LOGGING_E, "%s : cn_request_current_call_list failed", __func__);
                pc_send_terminal_response_error(rv, apdu);
                pc_setup_call_data_reset();
                return;
            }
        } else if (dispatch_client_confirmation() != 0) {
            catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
            pc_send_terminal_response_error(rv, apdu);
            pc_setup_call_data_reset();
        }

    }
    else
    {
        catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
        catd_log_f(SIM_LOGGING_E, "catd : Error in APDU data.");
        pc_send_terminal_response_setup_call(SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        NULL,
                                        NULL);
        pc_setup_call_data_reset();
        return;
    }
}

/*
 * Handles the initial check if there are calls ongoing
 */
static int cn_initial_call_list_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t* msg_p;
    cn_message_t* cn_msg_p;
    cn_call_list_t *call_list_p;
    uint8_t number_of_calls;
    int i;
    ste_apdu_additional_info_t add_info = SAT_RES_ADD_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*) ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*) msg_p->cn_msg;

    if (CN_RESPONSE_CURRENT_CALL_LIST != cn_msg_p->type) {
        //something wrong, we end the PC
        goto error;
    }

    call_list_p = (cn_call_list_t *)cn_msg_p->payload;
    if (call_list_p) {
        number_of_calls = call_list_p->nr_of_calls;
        for (i = 0; i < number_of_calls; i++) {
            if (CN_CALL_STATE_IDLE != call_list_p->call_context[i].call_state) {
                if (STE_APDU_ONLY_IF_NOT_BUSY == setup_call_p->command_details.option) {
                    // Other calls are ongoing, dont setup this call
                    catd_log_f( SIM_LOGGING_I, "%s : Other calls in progress, aborting", __func__);
                    add_info = SAT_RES_MEP_CURRENTLY_BUSY_ON_CALL;
                    catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
                    pc_send_terminal_response_setup_call(SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD,
                                                    &add_info, NULL);
                    pc_setup_call_data_reset();
                    return 0;
                }
            }
        }
        // no calls are ongoing, continue
        if (dispatch_client_confirmation() != 0) {
            goto error;
        }
    }

    return 0;

error:
    //something wrong, we end the PC
    catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
    pc_send_terminal_response_error(STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION, setup_call_apdu_p);
    pc_setup_call_data_reset();
    return -1;
}

static int dispatch_client_confirmation()
{
    //dispatch this command to client if the confirmation from client is needed
    ste_cat_pc_setup_call_ind_t call_ind;

    call_ind.simd_tag = 0;
    call_ind.duration = setup_call_p->duration;
    call_ind.redial = setup_call_p->command_details.redial;
    call_ind.option = setup_call_p->command_details.option;

    if (catd_send_pc_setup_call_ind(&call_ind) < 1)
    {
        //no client is interested we continue with the call setup
        if (pc_request_check_other_calls() != 0) {
            catd_log_f(SIM_LOGGING_E, "%s : error during call setup", __func__);
            return -1;
        }
    }
    //otherwise, we wait for the confirmation from client
    return 0;
}

/**
 * Handles the final check if there are calls ongoing
 */
static int cn_final_call_list_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t* msg_p;
    cn_message_t* cn_msg_p;
    cn_call_list_t *call_list_p;
    uint8_t number_of_calls = 0;
    ste_sim_ctrl_block_t* ctrl_p = NULL;
    int i;

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*) ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*) msg_p->cn_msg;

    if (CN_RESPONSE_CURRENT_CALL_LIST != cn_msg_p->type) {
        //something wrong, we end the PC
        goto error;
    }
    catd_log_f(SIM_LOGGING_D, "%s : enter", __func__);
    call_list_p = (cn_call_list_t *) cn_msg_p->payload;
    if (call_list_p) {
        number_of_calls = call_list_p->nr_of_calls;
        for (i = 0; i < number_of_calls; i++) {
            if (CN_CALL_STATE_IDLE != call_list_p->call_context[i].call_state) {
                if (STE_APDU_DISCONNECT_ALL_OTHER_CALLS == setup_call_p->command_details.option) {
                    // Call is ongoing, disconnect all calls
                    catd_log_f(SIM_LOGGING_I, "%s : Disconnecting all calls", __func__);
                    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                                       CATD_FD,
                                                       CATD_MSG_NONE,
                                                       cn_hangup_call_response_handler,
                                                       NULL);

                    if (CN_SUCCESS != cn_request_hangup(cn_get_client(),
                                      CN_CALL_STATE_FILTER_ALL,
                                      0, // call-id not used when filters are used
                                      (uintptr_t) ctrl_p)) {
                        catd_log_f(SIM_LOGGING_E, "%s : cn_request_hangup failed", __func__);
                        goto error;
                    }
                    return 0;

                } else if (STE_APDU_PUT_ALL_OTHER_CALLS_ON_HOLD == setup_call_p->command_details.option) {
                    if (CN_CALL_STATE_HOLDING == call_list_p->call_context[i].call_state) {
                        // continue if call is already holding
                        continue;
                    }
                    //Call is ongoing, put it on hold
                    catd_log_f(SIM_LOGGING_D, "%s : Putting call on hold: %d", __func__, call_list_p->call_context[i].call_id);
                    ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                                       CATD_FD,
                                                       CATD_MSG_NONE,
                                                       cn_hold_call_response_handler,
                                                       NULL);
                    if (CN_SUCCESS != cn_request_hold_call(cn_get_client(),
                            call_list_p->call_context[i].call_id, (uintptr_t) ctrl_p)) {
                        catd_log_f(SIM_LOGGING_E, "%s : cn_request_hold_call failed", __func__);
                        goto error;
                    }
                    return 0;
                }
            }
        }

        // Nothing to handle, setup the call
        if(pc_request_setup_call() != 0) {
            goto error;
        }
    }

    return 0;

error:
    //something wrong, we end the PC
    catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
    if ( ctrl_p ) free(ctrl_p);
    catd_send_answer_call_response(-1);/*sending response to user/client*/
    pc_send_terminal_response_error(STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION, setup_call_apdu_p);
    pc_setup_call_data_reset();
    return -1;
}

static int cn_hangup_call_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t* msg_p;
    cn_message_t* cn_msg_p;
    cn_exit_cause_t *exit_cause_p;

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*) ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*) msg_p->cn_msg;

    if (CN_RESPONSE_HANGUP != cn_msg_p->type) {
        //something wrong, we end the PC
        goto error;
    }
    if (CN_SUCCESS != cn_msg_p->error_code) {
        exit_cause_p = (cn_exit_cause_t*) cn_msg_p->payload;
        if (exit_cause_p) {
            catd_log_f(SIM_LOGGING_E, "%s failure: cause: %d, sender cause: %d", __func__,
                    exit_cause_p->cause, exit_cause_p->cause_type_sender);
        }
        goto error;
    }
    // Other calls hanged up, setup the call
    if (pc_request_setup_call() != 0) {
        goto error;
    }

    return 0;

error:
    catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
    catd_send_answer_call_response(-1);/*sending response to user/client*/
    pc_send_terminal_response_error(STE_SAT_APDU_ERROR_INTERNAL_IMPLEMENTATION, setup_call_apdu_p);
    return -1;
}

static int cn_hold_call_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t* msg_p;
    cn_message_t* cn_msg_p;
    cn_exit_cause_t *exit_cause_p;
    ste_apdu_additional_info_t add_info = SAT_RES_ADD_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

    catd_log_f(SIM_LOGGING_E, "%s : ENTER", __func__);

    assert(ste_msg);
    msg_p = (catd_msg_cn_t*) ste_msg;

    assert(msg_p->cn_msg);
    cn_msg_p = (cn_message_t*) msg_p->cn_msg;

    if (CN_RESPONSE_HOLD_CALL != cn_msg_p->type) {
        //something wrong, we end the PC
        goto error;
    }
    if (CN_SUCCESS != cn_msg_p->error_code) {
        exit_cause_p = (cn_exit_cause_t*) cn_msg_p->payload;
        if (exit_cause_p) {
            catd_log_f(SIM_LOGGING_E, "%s failure: cause: 0x%X, sender cause: %d", __func__,
                    exit_cause_p->cause, exit_cause_p->cause_type_sender);

            if (exit_cause_p->cause_type_sender == CN_CALL_CAUSE_TYPE_SENDER_NETWORK) {
                /* cause values based on values in 3GPP spec TS24.008 annex H */
                add_info = exit_cause_p->cause | 0x80;
            }
        }
        goto error;
    }

    // Iterate through the list by calling check_other_calls() again
    if(pc_request_check_other_calls() != 0) {
        goto error;
    }
    return 0;

error:
    catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
    catd_send_answer_call_response(-1);/*sending response to user/client*/
    pc_send_terminal_response_setup_call(SAT_RES_NETWORK_CURRENTLY_UNABLE,
                            &add_info, NULL);
    pc_setup_call_data_reset();
    return -1;
}

/**
 * @brief Map cn call setup cause to cat cause
 *
 * @param cn_exit_cause_t       cn disconnect cause
 *
 * Return ste_cat_setup_call_result_t
 */
static ste_cat_setup_call_result_t catd_util_map_cn_call_setup_case(cn_exit_cause_t *exit_cause_p)
{
    ste_cat_setup_call_result_t call_result = STE_CAT_SETUP_CALL_ME_UNABLE_TO_PROCESS;

    switch (exit_cause_p->cause_type_sender) {
    case CN_CALL_CAUSE_TYPE_SENDER_CLIENT:
        if (exit_cause_p->cause == CN_CALL_CLIENT_CAUSE_RELEASE_BY_USER) {
            call_result = STE_CAT_SETUP_CALL_REMOTE_REJECTED;
        }
        else { //CN_CALL_CLIENT_CAUSE_BUSY_USER_REQUEST
            call_result = STE_CAT_SETUP_CALL_REMOTE_UNAVAILABLE;
        }
        break;
    case CN_CALL_CAUSE_TYPE_SENDER_SERVER:
        switch(exit_cause_p->cause) {
        case CN_CALL_SERVER_CAUSE_CALL_ACTIVE:
            call_result = STE_CAT_SETUP_CALL_ME_BUSY_ON_CALL;
            break;
        case CN_CALL_SERVER_CAUSE_DTMF_SEND_ONGOING:
            call_result = STE_CAT_SETUP_CALL_ME_BUSY_ON_DTMF;
        default:
            break;
        }
        break;
    case CN_CALL_CAUSE_TYPE_SENDER_NETWORK:
        call_result = STE_CAT_SETUP_CALL_NW_UNABLE_TO_PROCESS;
        break;
    case CN_CALL_CAUSE_TYPE_SENDER_NONE:
    default:
        break;
    }
    return call_result;
}

/*************************************************************************
 * @brief:    handle the particular proactive command of setup call
 * @params:
 *            msg:         the original message with APDU data, and client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_handle_pc_setup_call(catd_msg_apdu_t  * msg)
{
    ste_sat_apdu_error_t                 rv;
    ste_apdu_t                         * apdu = msg->apdu;

    pc_setup_call_data_reset();
    rv = ste_apdu_parse(apdu, &setup_call_parsed_apdu_p);

    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
        catd_log_f(SIM_LOGGING_E, "catd : parse apdu failed");
        pc_send_terminal_response_error(rv, apdu);
        pc_setup_call_data_reset();
        return;
    }
    pc_main_handle_setup_call(setup_call_parsed_apdu_p, msg);
}

/*************************************************************************
 * @brief:    Interface for CATD to notify libpc about the client's answer on call setup
 * @params:
 *            answer:       the answer from client, answer or not (1 == answer).
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_handle_pc_setup_call_answer(int answer)
{
    ste_apdu_additional_info_t add_info;

    //First check that we have an ongoing setup call
    if (setup_call_p == NULL) {
        catd_send_answer_call_response(-1);
        return;
    }

    if (answer == 1) {
        if (pc_request_check_other_calls() >= 0) {
            return;
        }
    }

    //terminal response to reject the call setup
    add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

    catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
    pc_send_terminal_response_setup_call(SAT_RES_USER_REJECTED_PROACTIVE_CMD, &add_info, NULL);
    //notify client about the answer call result
    catd_send_answer_call_response(0); //OK, as the command to not setup the call worked
    //the pc is done
    pc_setup_call_data_reset();
}

/**************************************************************************************
 * @brief:    Send specific TR depending on call setup result from call/net module
 * @params:
 *
 * @return:   the status of the call setup
 *
 * Notes:
 **************************************************************************************/
void pc_send_terminal_response_setup_call_result(ste_cat_setup_call_result_t result)
{
    ste_apdu_additional_info_t        add_info;
    ste_apdu_general_result_t         general_result;

    add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    catd_log_f(SIM_LOGGING_E, "%s : ENTER, result: %d", __func__, result);

    //check the return of the call setup, if the call setup is successful
    //send a terminal response with OK result
    //otherwise send a failed result with specific reason
    if (result == STE_CAT_SETUP_CALL_OK)
    {
        general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;

        catd_log_f(SIM_LOGGING_E, "%s:%d call response", __func__, __LINE__);
        pc_send_terminal_response_setup_call(general_result, NULL, NULL);
        pc_setup_call_data_reset();
        return;
    }

    catd_log_f(SIM_LOGGING_E, "%s : FAILED", __func__);
    //call setup failed cases
    switch (result)
    {
        case STE_CAT_SETUP_CALL_ME_UNABLE_TO_PROCESS:
        {
            general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
        case STE_CAT_SETUP_CALL_ME_BUSY_ON_CALL:
        {
            general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            add_info = SAT_RES_MEP_CURRENTLY_BUSY_ON_CALL;
        }
        break;
        case STE_CAT_SETUP_CALL_ME_BUSY_ON_SS:
        {
            general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            add_info = SAT_RES_MEP_CURRENTLY_BUSY_ON_SS_TRANSACTION;
        }
        break;
        case STE_CAT_SETUP_CALL_ME_BUSY_ON_USSD:
        {
            general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            add_info = SAT_RES_MEP_CURRENTLY_BUSY_ON_USSD_TRANSACTION;
        }
        break;
        case STE_CAT_SETUP_CALL_ME_BUSY_ON_DTMF:
        {
            general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            add_info = SAT_RES_MEP_CURRENTLY_BUSY_ON_SEND_DTMF;
        }
        break;
        case STE_CAT_SETUP_CALL_NW_UNABLE_TO_PROCESS:
        {
            general_result = SAT_RES_NETWORK_CURRENTLY_UNABLE;
            add_info = SAT_RES_MEP_RADIO_RESOURCE_NOT_GRANTED;
        }
        break;
        case STE_CAT_SETUP_CALL_REMOTE_REJECTED:
        case STE_CAT_SETUP_CALL_REMOTE_UNAVAILABLE:
        {
            general_result = SAT_RES_USER_TERMINATED_SESSION;
            add_info = SAT_RES_ADD_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
        case STE_CAT_SETUP_CALL_DURATION_TIMEOUT:
        {
            general_result = SAT_RES_NETWORK_CURRENTLY_UNABLE;
            add_info = 0x91; //user busy ref Table 10.5.123/3GPP TS 24.008: Cause information element values
        }
        break;
        default:
        {
            general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            add_info = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        }
        break;
    }
    catd_log_f(SIM_LOGGING_D, "%s : error detected, line = %d", __func__, __LINE__);
    pc_send_terminal_response_setup_call(general_result, &add_info, NULL);
    pc_setup_call_data_reset();
    return;
}

void catd_handle_cn_event_pc_setup_call(ste_msg_t *ste_msg)
{
    ste_cat_setup_call_result_t call_result;

    catd_msg_cn_t *cn_message = (catd_msg_cn_t*) ste_msg;
    cn_message_type_t type = cn_message->cn_msg->type;

    switch (type) {
    case CN_EVENT_CALL_STATE_CHANGED:
    {
        cn_call_context_t * call_context_p = (cn_call_context_t*) cn_message->cn_msg->payload;

        catd_log_f(SIM_LOGGING_D, "catd %s: call state %d", __func__, call_context_p->call_state);
        if (call_id != call_context_p->call_id) {
            catd_log_f(SIM_LOGGING_D, "catd %s: ignoring call %d state %d", __func__,
                    call_context_p->call_id, call_context_p->call_state);
            return;
        }

        timed_hangup_call_state_change_handler(ste_msg);

        if (cn_message->cn_msg->payload_size == 0) {
            //Something has failed badly, we end the setup call
            call_result = STE_CAT_SETUP_CALL_ME_UNABLE_TO_PROCESS;
        } else if (call_context_p->call_state == CN_CALL_STATE_ACTIVE) {
            call_result = STE_CAT_SETUP_CALL_OK;
        } else if (call_context_p->call_state == CN_CALL_STATE_IDLE) {
            timed_hangup_t* tup = timed_hangup_get_globaldata();
            if (tup->hung_up == call_context_p->call_id) {
                catd_log_f(SIM_LOGGING_D, "%s: Disconnect caused by duration timeout", __func__);
                tup->hung_up = 0xFF;
                call_result = STE_CAT_SETUP_CALL_DURATION_TIMEOUT;
            } else {
                call_result = catd_util_map_cn_call_setup_case(&(call_context_p->call_cause));
            }
        } else {
            return;
        }
    }
    break;

    default:
        return;
    }

    //Send terminal response
    pc_send_terminal_response_setup_call_result(call_result);

}

/*
 * Global data for the hangup handler
 */
static timed_hangup_t* timed_hangup_get_globaldata()
{
    static timed_hangup_t global_data = TIMED_HANGUP_T_INITIALIZER; // Will be initalized only once!
    return &global_data;
}

/*
 * Response handler for hangup request.
 *
 * We're really not interested in the response...
 */
static int timed_hangup_response_handler(ste_msg_t* UNUSED(ste_msg))
{
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);
    return 0;
}

/*
 * Handle the timeout.
 *
 * Time to hangup a call, YAY! :D
 */
static void timed_hangup_timeout_handler(uintptr_t timeout_event)
{
    int rc = simd_timer_validate_timeout_event(timeout_event);
    if (!rc) {
        /*
         * This is a valid time-out event and not a race-condition artifact
         */
        timed_hangup_t* tup = timed_hangup_get_globaldata();
        if (tup && tup->timer) {
            uint8_t       call_id = tup->call_id;
            cn_context_t* context = cn_get_client();

            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s call_id = %d", __PRETTY_FUNCTION__, call_id);

            timed_hangup_stop();

            if (context) {
                ste_sim_ctrl_block_t* ctrl_p = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                                                         CATD_FD,
                                                                         CATD_MSG_NONE,
                                                                         timed_hangup_response_handler,
                                                                         NULL);
                if (ctrl_p) {
                    cn_error_code_t err = cn_request_hangup(context,
                                                            CN_CALL_STATE_FILTER_NONE,
                                                            call_id,
                                                            (uintptr_t)ctrl_p);
                    if (err == CN_SUCCESS) {
                        tup->hung_up = call_id; // @TODO: Put in response handler when msg queue is fixed...
                        return;
                    }
                    free(ctrl_p);
                }
            }
        }
        catd_log_f(SIM_LOGGING_D, "catd/libpc : %s failure", __PRETTY_FUNCTION__);
    }
}

/*
 * Monitor call-state changes
 */
static void timed_hangup_call_state_change_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t* msg  = (catd_msg_cn_t*) ste_msg;
    timed_hangup_t* tup = timed_hangup_get_globaldata();

    assert(msg);
    assert(msg->cn_msg);

    if (tup && msg->cn_msg->type == CN_EVENT_CALL_STATE_CHANGED) {
        cn_call_context_t* context  = (cn_call_context_t*)msg->cn_msg->payload;
        uint8_t            call_id  = (uint8_t)context->call_id;
        int                sc_count = tup->sc_count++;

        if (tup->timer && tup->call_id == call_id) {
            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s id = %d, c = %d, s = %d, ps = %d", __PRETTY_FUNCTION__,
                                                                                            call_id,
                                                                                            sc_count,
                                                                                            context->call_state,
                                                                                            context->prev_call_state);

            if (context->call_state == CN_CALL_STATE_ACTIVE) {
                timed_hangup_stop();
            } else if ( context->call_state == CN_CALL_STATE_IDLE) {
                uint8_t call_id = tup->call_id;
                timed_hangup_stop();
                tup->hung_up = call_id;
            }

        } else {
            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s ignored state change: id = %d", __PRETTY_FUNCTION__,
                                                                                       call_id);
        }
    }
}

/*
 * Initiate a timed hangup for the given call-id.
 */
static void timed_hangup_initiate(uint8_t call_id, uint32_t millisecs)
{
    timed_hangup_t* tup = timed_hangup_get_globaldata();

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s call_id = %d, ms = %d", __PRETTY_FUNCTION__, call_id, millisecs);

    timed_hangup_stop();

    if (tup) {
        int rc = simd_timer_create(&tup->timer);
        if (!rc) {
            tup->call_id = call_id;
            tup->hung_up = 0xFF;
            tup->sc_count = 0;
            rc = simd_timer_start(tup->timer, millisecs, catd_sig_timeout_event,
                                  (uintptr_t)timed_hangup_timeout_handler, 0, 0, 0);
            if (!rc) {
                return;
            }
            timed_hangup_stop();
        }
    }
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s failure", __PRETTY_FUNCTION__);
}

static void timed_hangup_stop()
{
    timed_hangup_t* tup = timed_hangup_get_globaldata();
    if (tup && tup->timer) {
        catd_log_f(SIM_LOGGING_D, "catd/libpc : %s call_id = %d", __PRETTY_FUNCTION__, tup->call_id);

        simd_timer_destroy(tup->timer);
        tup->call_id  = 0xFF;
        tup->hung_up  = 0xFF;
        tup->sc_count = 0;
        tup->timer    = 0;
        return;
    }
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s failure", __PRETTY_FUNCTION__);
}

/*************************************************************************
 * @brief:    reset setup call
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_reset_pc_setup_call(void) {
    timed_hangup_stop();
    pc_setup_call_data_reset();
}
