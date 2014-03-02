/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_send_ss.c
 * Description     : Handler function for proactive command send ss.
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

static void map_cn_ss_error2result(cn_ss_error_t* error, ste_command_result_t* res, uint8_t* byte);

typedef struct {
    ste_apdu_t*        apdu_raw;     // Points to the original "rawer" APDU
    ste_parsed_apdu_t* apdu_parsed;  // Points to the parsed APDU
    boolean            icon_handled; // False if there is a icon and the client doesn't have a GUI
} pc_send_ss_stash_t;

/*
 * Deletes the stash
 */
static void delete_stash(pc_send_ss_stash_t* stash)
{
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    if (stash) {
        if (stash->apdu_raw) {
            ste_apdu_delete(stash->apdu_raw);
        }
        if (stash->apdu_parsed) {
            ste_parsed_apdu_delete(stash->apdu_parsed);
        }

        /*
         * Lets reset the memory to have a good chance of early fail if
         * someone tries to access the memory after it has been free'd.
         */
        memset(stash, 0, sizeof(*stash));
        free(stash);
    }
}

static int convert_ss_bcd2ascii(ste_apdu_address_t* in, uint8_t* out, int maxlen)
{
    char translate[] = "0123456789*#CDEF"; // Digits C, D & E are not valid, F is end marker.
    int  len         = -1;

    assert(in);
    assert(in->dial_text_p);
    assert(out);

    // Calculate space needed
    len = (in->str_length * 2) + 1; // +1 do make room for a NULL terminator, just in case...

    if (out && len <= maxlen) {
        uint8_t i;
        for (len = 0, i = 0; i < in->str_length; i++) {
            char byte = in->dial_text_p[i];

            // First digit in low nibble
            uint8_t bcd = byte & 0x0F;
            if (bcd == 0x0F) {
                break;
            } else if (bcd < 0x0C) {
                (out)[len++] = translate[bcd];
            }

            // Second digit in high nibble
            bcd = (byte >> 4) & 0x0F;
            if (bcd == 0x0F) {
                break;
            } else if (bcd < 0x0C) {
                (out)[len++] = translate[bcd];
            }
        }
        (out)[len] = 0; // Null terminate, just in case...
    }
    return len;
}

/**
 * @brief Asynchronous response handler for pc_handle_send_ss().
 */
static int pc_handle_send_ss_cn_response_handler(ste_msg_t* ste_msg);

/*************************************************************************
 * @brief:    handle the particular proactive command of send ss internally
 * @params:
 *            parsed_apdu: the parsed apdu structure.
 *            msg:         the original message with APDU data, and client_tag, etc. for possible future usage.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static int pc_handle_send_ss(catd_msg_apdu_t* msg, boolean gui_capable_client)
{
    ste_sim_ctrl_block_t*  ctrl_p     = NULL;
    ste_sat_apdu_error_t   rv         = STE_SAT_APDU_ERROR_UNKNOWN;
    ste_apdu_pc_send_ss_t* send_ss_p  = NULL;
    ste_apdu_t*            apdu       = NULL;
    cn_context_t*          context    = NULL;
    cn_ss_command_t        ss_command;
    ste_apdu_address_t*    ss_str_p   = NULL;
    pc_send_ss_stash_t*    stash      = NULL;
    boolean                notify     = TRUE;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    apdu = msg->apdu;
    msg->apdu = NULL; // Make sure the framework does not delete the APDU when we exit...
    assert(apdu);

    stash = malloc(sizeof(*stash));
    if (stash) {
        stash->icon_handled = TRUE;
        stash->apdu_parsed  = NULL;
        stash->apdu_raw = apdu;
        rv = ste_apdu_parse(stash->apdu_raw, &stash->apdu_parsed); // Allocates memory
        if (rv != STE_SAT_APDU_ERROR_NONE)
        {
            catd_log_f(SIM_LOGGING_E, "catd : failed to parse apdu data.");
            pc_send_terminal_response_error(rv, stash->apdu_raw);
            delete_stash(stash);
            return -1;
        }
    }

    ctrl_p  = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                        CATD_FD,
                                        CATD_MSG_NONE,
                                        pc_handle_send_ss_cn_response_handler,
                                        stash);

    if (stash) {
        rv = ste_parsed_apdu_get(stash->apdu_parsed, (void**)&send_ss_p, STE_APDU_CMD_TYPE_SEND_SS);
    }

    if (send_ss_p && rv == STE_SAT_APDU_ERROR_NONE)
    {


        if ((send_ss_p->bit_mask & PC_SEND_SS_IconIdentifier_present) &&
                (!(send_ss_p->bit_mask & PC_SEND_SS_AlphaIdentifier_present))) {
            if (send_ss_p->icon_id.icon_qualifier) {
                catd_log_f(SIM_LOGGING_E, "catd : failed to parse APDU data, alpha is missing");
                rv = STE_SAT_APDU_ERROR_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
            }
        }

        if ((send_ss_p->bit_mask & PC_SEND_SS_AlphaIdentifier_present) &&
                (send_ss_p->alpha_identifier.alpha_id_length == 0)) {
            notify = FALSE;
        }

        //Check if Icon is present, then set to gui_capable_client
        if (notify && send_ss_p->bit_mask & PC_SEND_SS_IconIdentifier_present)
        {
            stash->icon_handled = gui_capable_client;
        }

        //If SS_string set then copy the ss_string object
        if ((send_ss_p->bit_mask & PC_SEND_SS_SS_String_present) && send_ss_p->ss_string.dial_text_p)    {
            int len;
            ss_str_p = &(send_ss_p->ss_string);

            //fill in ss_command
            ss_command.sat_initiated = TRUE;
            ss_command.additional_results = TRUE;

            len = convert_ss_bcd2ascii(ss_str_p, (uint8_t*)&ss_command.mmi_string, (CN_SS_MAX_MMI_STRING_LENGTH + 1));
            assert(len >= 0); // len can only be less than zero if CN_SS_MAX_MMI_STRING_LENGTH definition has been messed up.
            ss_command.mmi_string_length = (uint8_t)len;

            ss_command.ton_npi_used = TRUE;
            ss_command.numbering_plan_id = (ss_str_p->ton_npi & 0x0F); //NPI first 4 bits of ton_npi
            ss_command.type_of_number = ((ss_str_p->ton_npi >> 4) & 0x07); //Bit 5-7 is the TON
        }
        else
        {
            rv = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
        }

        if ( rv == STE_SAT_APDU_ERROR_NONE ) {
            //init cn_client and request ss command
            context = cn_get_client();
            if (context && ctrl_p && rv == STE_SAT_APDU_ERROR_NONE) {
                catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);
                cn_error_code_t err = cn_request_ss(context, ss_command, (uintptr_t)ctrl_p);
                if (err == CN_SUCCESS) {
                    if (notify) {
                        catd_pc_send_pc_notification_ind(stash->apdu_raw); // Must call notify ourselves since we set msg->apdu to NULL
                    }
                    return 1; // Tell framework not to notify, we already did it...
                }
            }
        }
    }

    // Error handling & clean up.
    catd_log_f(SIM_LOGGING_E, "catd/libpc : %s - error detected.", __PRETTY_FUNCTION__);

    pc_send_terminal_response_error(rv, apdu);
    if (stash) {
        delete_stash(stash); //de-allocate apdu and parsed apdu
    } else {
        ste_apdu_delete(apdu);
    }

    free(ctrl_p);
    return -1;
}

static int pc_handle_send_ss_cn_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t*           msg        = (catd_msg_cn_t*)ste_msg;
    cn_ss_response_t*        response_p = (cn_ss_response_t*)msg->cn_msg->payload;
    pc_send_ss_stash_t*      stash      = NULL;
    ste_sim_ctrl_block_t*    ctrl_p     = NULL;
    ste_apdu_t*              tr         = NULL;
    ste_command_result_t     result;
    cn_ss_error_code_type_t  error_type;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    assert(msg);                                        // There has to be a message, and
    assert(msg->cn_msg);                                // the message must contain a C&N message, and
    assert(msg->cn_msg->type == CN_RESPONSE_SS); // the C&N message must be a response to our send SS request...

    ctrl_p = (ste_sim_ctrl_block_t*)msg->client_tag;
    assert(ctrl_p);

    stash  = (pc_send_ss_stash_t*)ctrl_p->transaction_data_p;
    assert(stash);

    error_type = response_p->error_code.cn_ss_error_code_type;

    if (msg->cn_msg->error_code == CN_SUCCESS && error_type == CN_SS_ERROR_CODE_TYPE_NONE) {

        //Fill in command result
        if (stash->icon_handled) {
            result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
        } else {
            result.general_result = SAT_RES_CMD_PERFORMED_SUCC_WITHOUT_REQ_ICON;
        }

        if (response_p->additional_result.length > 0) {
            //fill in the additional info fetched from CN
            result.additional_info_p =  (uint8_t*)response_p->additional_result.additional_result_data;
            result.additional_info_size = response_p->additional_result.length;

        } else { /*No additional info added*/
            result.additional_info_p = NULL;
            result.additional_info_size = 0;
        }

    } else {
        uint8_t byte;

        // If we got CN_REQUEST_CC_REJECTED then we know CC is not allowed.
        if (msg->cn_msg->error_code == CN_REQUEST_CC_REJECTED) {
            byte = SAT_RES_CC_ACTION_NOT_ALLOWED;
            result.general_result = SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_PERM;
            result.additional_info_size = sizeof(byte);
            result.additional_info_p = &byte;

        } else if ( msg->cn_msg->error_code == CN_REQUEST_MODEM_NOT_READY ) {
            byte = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            result.general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            result.additional_info_size = sizeof(byte);
            result.additional_info_p = &byte;
        } else {
            map_cn_ss_error2result(&response_p->error_code, &result, &byte);
        }
    }

    result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
    result.other_data_type = STE_CMD_RESULT_NOTHING;

    // No need to check return value, If tr != NULL then all is ok,
    (void)ste_apdu_terminal_response(stash->apdu_parsed, &result, &tr);
    if (tr) {
        catd_sig_tr(CATD_FD, tr, CATD_CLIENT_TAG);
    } else {
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, stash->apdu_raw);
    }

    ste_apdu_delete(tr);      // Can handle null pointer...
    delete_stash(stash);
    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, msg->cn_msg->error_code);
    return 0;
}

/*************************************************************************
 * @brief:    handle the particular proactive command of send ss
 * @params:
 *            msg:         the original message with APDU data, and client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
int catd_handle_pc_send_ss(catd_msg_apdu_t * msg, boolean gui_capable_client)
{
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    return pc_handle_send_ss(msg, gui_capable_client);
}

/**
* Internal function to map general result in error cases.
*
* This function maps the general result based on the CN error value (internal errors).
*/
static uint8_t map_cn_error_to_general_result_and_additional_info( cn_ss_error_code_t err, ste_apdu_general_result_t* result_p )
{
    uint8_t ret_val = SAT_RES_ADD_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;

    switch( err )
    {
    case CN_SS_ERROR_CODE_NONE: /* Fall Through intended */
    case CN_SS_ERROR_CODE_INTERNAL_ERROR:
    case CN_SS_ERROR_CODE_CS_INACTIVE:
    case CN_SS_ERROR_CODE_GSM_BAD_PASSWORD:
    case CN_SS_ERROR_CODE_GSM_BAD_PASSWORD_FORMAT:
    case CN_SS_ERROR_CODE_GSM_DATA_ERROR:
    case CN_SS_ERROR_CODE_GSM_MSG_INCORRECT:
    case CN_SS_ERROR_CODE_GSM_MM_ERROR:
    case CN_SS_ERROR_CODE_GSM_REQUEST_CANCELLED:
    case CN_SS_ERROR_CODE_GSM_SERVICE_NOT_ON_FDN_LIST:
    case CN_SS_ERROR_CODE_RESOURCE_CONTROL_DENIED:
    case CN_SS_ERROR_CODE_RESOURCE_CONTROL_FAILURE:
    case CN_SS_ERROR_CODE_RESOURCE_CONTROL_CONF_FAIL:
    case CN_SS_ERROR_CODE_SERVICE_NOT_SUPPORTED:
    case CN_SS_ERROR_CODE_SERVICE_REQUEST_RELEASED:
    case CN_SS_ERROR_CODE_SERVICE_UNKNOWN_ERROR:
    default:
        *result_p = SAT_RES_SS_RETURN_ERROR;
        /* ret_val = 0x00 - No specific cause can be given */
        break;

    case CN_SS_ERROR_CODE_GSM_SS_NOT_AVAILABLE:
        *result_p = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
        ret_val = SAT_RES_MEP_PROB_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
        break;

    case CN_SS_ERROR_CODE_GSM_USSD_BUSY:
        *result_p = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
        ret_val = SAT_RES_MEP_CURRENTLY_BUSY_ON_USSD_TRANSACTION;
        break;

    case CN_SS_ERROR_CODE_SERVICE_BUSY:
        *result_p = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
        ret_val = SAT_RES_MEP_CURRENTLY_BUSY_ON_SS_TRANSACTION;
        break;
    }
    return ret_val;
}

static void map_cn_ss_error2result(cn_ss_error_t* error, ste_command_result_t* res, uint8_t* byte)
{
    cn_ss_error_code_type_t type;

    assert(error && res && byte);

    type = error->cn_ss_error_code_type;
    if ( type == CN_SS_ERROR_CODE_TYPE_CN ) {
        ste_apdu_general_result_t result;
        *byte = map_cn_error_to_general_result_and_additional_info(
                    error->cn_ss_error_value.cn_ss_error_code,
                    &result );
        res->general_result = result;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    } else if ( type == CN_SS_ERROR_CODE_TYPE_GSM ) {
        *byte = (uint8_t)error->cn_ss_error_value.cause_value; /* Cause value from TS 24.080 */
        res->general_result = SAT_RES_SS_RETURN_ERROR;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    } else if ( type == CN_SS_ERROR_CODE_TYPE_MM ) {
        *byte = (uint8_t)error->cn_ss_error_value.cause_value; /* Cause value from TS 24.080 */
        res->general_result = SAT_RES_NETWORK_CURRENTLY_UNABLE;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    }
    else
    {
        /* Error Case. Type should be one of the three handled above */
        *byte = 0; /* No additional Info can be given */
        res->general_result = SAT_RES_SS_RETURN_ERROR;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    }
}
