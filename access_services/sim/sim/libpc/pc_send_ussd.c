/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_send_ussd.c
 * Description     : Handler function for proactive command send ussd.
 *
 * Author          : Leif Simmons <Leif.Simmons@stericsson.com>
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
#include "pc_handler.h"

#include "catd_cn_client.h"
#include "cn_client.h"

#ifndef MIN
#define MIN(x,y) ((x<y) ? x : y)
#endif

#ifndef MAX
#define MAX(x,y) ((x>y) ? x : y)
#endif

typedef enum
{
    GOT_SESSION_ID = 0x0001,
    GOT_USSD_END   = 0x0002,
    GOT_STATUS_IND = 0x0004,
    GOT_CC_IND     = 0x0008,
} flag_values_t;

/*
 * This the struct to hold the stuff we need to stash away while performing asynchronous requests...
 */
typedef struct {
    ste_apdu_t*        apdu_raw;     // Points to the original "rawer" APDU
    ste_parsed_apdu_t* apdu_parsed;  // Points to the parsed APDU
    boolean            icon_handled; // False if there is a icon and the client doesn't have a GUI

    unsigned int        flags;
    uint32_t            session_id;
    cn_ussd_info_t      ussd;
    cn_ss_status_info_t status;
} pc_send_ussd_stash_t;

/*
 * Deletes a stash
 */
static void delete_stash(pc_send_ussd_stash_t* stash)
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

static pc_send_ussd_stash_t* global_ussd_stash = NULL;

static int     pc_main_handle_send_ussd(catd_msg_apdu_t* msg, boolean gui_capable_client);
static int     pc_handle_send_ussd_response_handler(ste_msg_t* ste_msg);
static void    pc_handle_send_ussd_event_ussd(cn_message_t* msg);
static void    pc_handle_send_ussd_event_ss_status_ind(cn_message_t* msg);
static void    pc_handle_send_ussd_evaluate_if_complete();
static uint8_t convert_cb_dcs2sms_dcs4ussd(uint8_t cb_dcs);
static void    map_cn_ss_error2result(cn_ss_error_t* err, ste_command_result_t* res, uint8_t* byte);

int catd_handle_pc_send_ussd(catd_msg_apdu_t* msg, boolean gui_assumed)
{
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    return pc_main_handle_send_ussd(msg, gui_assumed);
}

static int pc_main_handle_send_ussd(catd_msg_apdu_t* msg, boolean gui_capable_client)
{
    ste_sim_ctrl_block_t* ctrl_p  = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    pc_send_ussd_stash_t* stash   = NULL;
    ste_apdu_t*           apdu    = NULL;
    ste_sat_apdu_error_t  rc      = STE_SAT_APDU_ERROR_UNKNOWN;
    boolean               notify  = TRUE;
    cn_context_t*         context;
    cn_ussd_info_t        ussd;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    apdu = msg->apdu;
    msg->apdu = NULL; // Make sure the framework does not delete the APDU when we exit...
    assert(apdu);

    // Check that the global stash is free.
    // If it is set, then we're busy (already serving one USSD). However, that should never happen since the
    // SAT App wont issue a new PC until the previous one is completed.
    if (!global_ussd_stash)
    {
        stash = malloc(sizeof(*stash));
        if (stash) {
            stash->flags        = 0;
            stash->icon_handled = TRUE;
            stash->apdu_raw     = apdu;
            stash->apdu_parsed  = NULL;
            rc = ste_apdu_parse(stash->apdu_raw, &stash->apdu_parsed); // Allocates memory
        }

        ctrl_p  = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                            CATD_FD,
                                            CATD_MSG_NONE,
                                            pc_handle_send_ussd_response_handler,
                                            NULL); // No stash, using global variable instead

        if (rc == STE_SAT_APDU_ERROR_NONE) {
            ste_apdu_pc_send_ussd_t* sat_ussd;
            rc = ste_parsed_apdu_get(stash->apdu_parsed, (void**)&sat_ussd, STE_APDU_CMD_TYPE_SEND_USSD); // Doesn't allocate memory
            if (rc == STE_SAT_APDU_ERROR_NONE) {
                if ((sat_ussd->bit_mask & PC_SEND_USSD_AlphaIdentifier_present) &&
                    (sat_ussd->alpha_identifier.alpha_id_length == 0)           ) {
                    notify = FALSE;
                }

                if (notify & sat_ussd->bit_mask & PC_SEND_USSD_IconIdentifier_present) {
                    stash->icon_handled = gui_capable_client;
                }

                if ((sat_ussd->bit_mask & PC_SEND_USSD_USSD_String_present)  &&
                    (sat_ussd->ussd_string.str_p)                            ) {

                    memset(&ussd, 0, sizeof(ussd));
                    ussd.type          = CN_USSD_TYPE_USSD_REQUEST; //No docs, not used?
                    ussd.dcs           = sat_ussd->ussd_string.dcs;
                    ussd.length        = MIN(sat_ussd->ussd_string.str_len, (size_t)CN_MAX_STRING_BUFF);
                    ussd.sat_initiated = TRUE;
                    memcpy(ussd.ussd_string, sat_ussd->ussd_string.str_p, ussd.length);

                } else {
                    rc = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
                }
            }
        }

        if (rc == STE_SAT_APDU_ERROR_NONE) {
            context = cn_get_client();
            if (context && ctrl_p && rc == STE_SAT_APDU_ERROR_NONE) {
                catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);
                cn_error_code_t err = cn_request_ussd(context, &ussd, (uintptr_t)ctrl_p);
                if (err == CN_SUCCESS) {
                    global_ussd_stash = stash; // Need to set the global stash in order to be able to handle the events!
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

    pc_send_terminal_response_error(rc, apdu);
    if (stash) {
        delete_stash(stash); // de-allocate raw & parsed APDUs
    } else {
        ste_apdu_delete(apdu);
    }

    free(ctrl_p);
    return -1;
}

static int pc_handle_send_ussd_response_handler(ste_msg_t* ste_msg)
{
    pc_send_ussd_stash_t*   stash    = global_ussd_stash;
    catd_msg_cn_t*          msg      = (catd_msg_cn_t*)ste_msg;
    cn_response_ussd_t*     response = (cn_response_ussd_t*) msg->cn_msg->payload;
    cn_ss_error_code_type_t err_type;

    assert(stash); // The stash has to be set!

    assert(msg);                                   // There has to be a message, and
    assert(msg->cn_msg);                           // the message must contain a C&N message, and
    assert(msg->cn_msg->type == CN_RESPONSE_USSD); // the C&N message type must be a USSD response.

    assert(msg->cn_msg->payload_size == sizeof(cn_response_ussd_t)); // The payload must be the correct size

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s session_id: %d error_code_type: %d", __PRETTY_FUNCTION__, response->session_id, response->ss_error.cn_ss_error_code_type);

    if (response->ss_error.cn_ss_error_code_type == CN_SS_ERROR_CODE_TYPE_CN) {
        catd_log_f(SIM_LOGGING_D, "catd/libpc : %s cn_error_code: %d", __PRETTY_FUNCTION__, response->ss_error.cn_ss_error_value.cn_ss_error_code);
    }
    else if (response->ss_error.cn_ss_error_code_type != CN_SS_ERROR_CODE_TYPE_NONE) {
        catd_log_f(SIM_LOGGING_D, "catd/libpc : %s 3gpp_error_code_value: %d", __PRETTY_FUNCTION__, response->ss_error.cn_ss_error_value.cause_value);
    }

    err_type = response->ss_error.cn_ss_error_code_type;
    if ((msg->cn_msg->error_code == CN_SUCCESS) &&
        (err_type == CN_SS_ERROR_CODE_TYPE_NONE)) {

        assert(!(stash->flags & GOT_SESSION_ID)); // We should not get the response twice...

        stash->flags |= GOT_SESSION_ID;
        stash->session_id = response->session_id;

        if ((stash->flags & GOT_USSD_END)                &&
            (stash->session_id != stash->ussd.session_id)) {

            /*
             * This means we speculatively stored an USSD_END before we got the response.
             * However, it seems it doesn't belong to our session, so we just clear the
             * "got ussd end" flag
             */
            stash->flags &= ~GOT_USSD_END;
        }

        if ((stash->flags & GOT_STATUS_IND)                &&
            (stash->session_id != stash->status.session_id)) {

            /*
             * This means we speculatively stored an USSD_END before we got the response.
             * However, it seems it doesn't belong to our session, so we just clear the
             * "got status ind" flag
             */
            stash->flags &= ~GOT_STATUS_IND;
        }

        pc_handle_send_ussd_evaluate_if_complete();

    } else {

        uint8_t               byte;
        ste_command_result_t  result;
        ste_apdu_t*           tr     = NULL;

        // If we got CN_REQUEST_CC_REJECTED then we know CC is not allowed.
        if (msg->cn_msg->error_code == CN_REQUEST_CC_REJECTED) {
            byte = SAT_RES_CC_ACTION_NOT_ALLOWED;
            result.general_result = SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_PERM;
            result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
            result.additional_info_size = sizeof(byte);
            result.additional_info_p = &byte;
            result.other_data_type = STE_CMD_RESULT_NOTHING;

        } else if ( msg->cn_msg->error_code == CN_REQUEST_MODEM_NOT_READY ) {
            byte = SAT_RES_CC_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
            result.general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
            result.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;
            result.additional_info_size = sizeof(byte);
            result.additional_info_p = &byte;
        } else {
            map_cn_ss_error2result(&response->ss_error, &result, &byte);
        }

        // No need to check return value, if tr != NULL, then all is well
        (void)ste_apdu_terminal_response(stash->apdu_parsed, &result, &tr);

        if (tr) { // If we managed to create a tr, then all is well!
            catd_sig_tr(CATD_FD, tr, CATD_CLIENT_TAG);
        } else {
            /*
             *  Most likely, this will fail too, but hey' let's try, at least we'll get
             *  a nice log print before we get stuck...
             */
            pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, stash->apdu_raw);
        }

        global_ussd_stash = NULL;
        ste_apdu_delete(tr);      // Can handle null pointer...
        delete_stash(stash);
    }

    return 0;
}

void catd_handle_pc_send_ussd_event_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t* msg = (catd_msg_cn_t*) ste_msg;
    assert(msg);
    assert(msg->cn_msg);

    // Check that the global stash is in use, since this means that we are currently handling a send USSD PC.
    if (global_ussd_stash)
    {
        /*
         *  This printout works because the payload for both CN_EVENT_USSD & CN_EVENT_NET_SS_STATUS_IND
         *  starts with an enum (int) describing the "subtype"...
         */
        catd_log_f(SIM_LOGGING_D, "catd/libpc : %s type = %d, subtype = %d", __PRETTY_FUNCTION__,
                                                                             msg->cn_msg->type,
                                                                             *((int*)msg->cn_msg->payload));
        switch (msg->cn_msg->type) {
        case CN_EVENT_USSD:
            pc_handle_send_ussd_event_ussd(msg->cn_msg);
            break;
        case CN_EVENT_SS_STATUS_INFO:
            pc_handle_send_ussd_event_ss_status_ind(msg->cn_msg);
            break;
        default:
            break;
        }
    }
}

static void pc_handle_send_ussd_event_ussd(cn_message_t* msg)
{
    pc_send_ussd_stash_t* stash = global_ussd_stash;
    cn_ussd_info_t*       ussd = (cn_ussd_info_t*)msg->payload;

    assert(stash);                                       // The stash has to be set
    assert(msg);                                         // The message must not be NULL
    assert(msg->type == CN_EVENT_USSD);                  // The message type bust be correct
    assert(msg->payload_size == sizeof(cn_ussd_info_t)); // The payload must be the correct size

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s received_state = %d", __PRETTY_FUNCTION__, ussd->received_type);

    if (ussd->received_type == CN_USSD_RECEIVED_TYPE_GSM_USSD_END) {
        /*
         * We store the USSD for further use.
         */
        stash->flags |= GOT_USSD_END;
        memcpy(&stash->ussd, ussd, sizeof(cn_ussd_info_t));

        if (stash->flags & GOT_SESSION_ID) {
            pc_handle_send_ussd_evaluate_if_complete();
        }
    }
}

static void pc_handle_send_ussd_event_ss_status_ind(cn_message_t* msg)
{
    pc_send_ussd_stash_t* stash  = global_ussd_stash;
    cn_ss_status_info_t*  status = (cn_ss_status_info_t*)msg->payload;

    assert(stash);                                            // The stash has to be set
    assert(msg);                                              // The message must not be NULL
    assert(msg->type == CN_EVENT_SS_STATUS_INFO);             // The message type bust be correct
    assert(msg->payload_size == sizeof(cn_ss_status_info_t)); // The payload must be the correct size

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s ENTER", __PRETTY_FUNCTION__);

    /*
     * If we have a session id, then the session id of the status ind must match if we are to store it.
     * If we don't have a session id yet, then we speculatively store the status ind.
     */
    if(((stash->flags & GOT_SESSION_ID) && (stash->session_id == status->session_id)) ||
       (!(stash->flags & GOT_SESSION_ID))                                           ) {
        stash->flags |= GOT_STATUS_IND;
        memcpy(&stash->status, status, sizeof(cn_ss_status_info_t));
    }

    if (stash->flags & GOT_SESSION_ID) {
        pc_handle_send_ussd_evaluate_if_complete();
    }

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s EXIT", __PRETTY_FUNCTION__);
}


static void pc_handle_send_ussd_evaluate_if_complete()
{
    pc_send_ussd_stash_t*  stash    = global_ussd_stash;
    ste_apdu_t*            tr       = NULL;
    boolean                complete = FALSE;
    ste_command_result_t   result;
    ste_apdu_text_string_t string;
    uint8_t                byte;

    assert(stash);

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s ENTER", __PRETTY_FUNCTION__);

    if (stash->flags & GOT_SESSION_ID)
    {
        if (stash->flags & GOT_USSD_END) {
            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s GOT_USSD_END", __PRETTY_FUNCTION__);
            if (stash->ussd.me_initiated) {
                result.general_result = SAT_RES_USSD_TRANS_TERMINATED_BY_USER;
            } else if (stash->icon_handled) {
                result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
            } else {
                result.general_result = SAT_RES_CMD_PERFORMED_SUCC_WITHOUT_REQ_ICON;
            }

            result.additional_info_p    = NULL;
            result.additional_info_size = 0;
            result.command_status       = STE_SAT_PC_CMD_STATUS_COMPLETED;
            result.other_data_type      = STE_CMD_RESULT_TEXT_STRING;
            result.other_data.text_p    = &string;
            string.coding_scheme        = convert_cb_dcs2sms_dcs4ussd(stash->ussd.dcs);
            string.text_length          = stash->ussd.length;
            string.text_string_p        = stash->ussd.ussd_string; // Currently gives a warning due to wrong data type in CNS
            complete = TRUE;
        } else if (stash->flags & GOT_STATUS_IND) {
            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s GOT_STATUS_IND", __PRETTY_FUNCTION__);
            switch (stash->status.type) {
            case CN_SS_STATUS_INFO_TYPE_USSD_FAILED: // Error, fall-through intended.
            case CN_SS_STATUS_INFO_TYPE_USSD_BUSY:   // Also an error.
                map_cn_ss_error2result(&stash->status.ss_error, &result, &byte);
                complete = TRUE;
                break;

            case CN_SS_STATUS_INFO_TYPE_USSD_STOP:
                /*
                 * Ignore because we should get receive a USSD end message first, so there is only two
                 * possible scenarios for us to get here:
                 *
                 * 1. CN_SS_STATUS_IND_TYPE_USSD_STOP raced a USSD end message and got here first. If this is
                 *    the case, then we can safely ignore this status indication.
                 * 2. We will never get a USSD end message, this is not according to the docs...
                 *
                 * For now lets assume its just a race condition thingie and just ignore the USSD stop.
                 *
                 * Fall-through intended!
                 */

            default:
                /*
                 * How to handle the others, according to the docs we shouldn't get them.
                 * Either ignore them, or treat it as an error...
                 *
                 * For now, just ignore them...
                 */

                stash->flags &= ~GOT_STATUS_IND; // Just pretend we didn't get a status indication...
                break;
            }
        }
    } else {
        catd_log_f(SIM_LOGGING_D, "catd/libpc : %s Stash not initiated, no session_id set", __PRETTY_FUNCTION__);
    }

    if (complete) {
        // No need to check return value, if tr != NULL, then all is well...

        //Check if this is a CC -> USSD indication, then 2 result TLVs needed.
        if (global_ussd_stash->flags & GOT_CC_IND) {
            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s CC -> USSD indicated",__PRETTY_FUNCTION__);
            ste_command_result_t result_first;
            result_first.command_status = STE_SAT_PC_CMD_STATUS_COMPLETED;

            /* Now we know that we need to send two Result TLVs.
             * So we need to check the Result from event USSD to see which general results
             * need to be set both for the first and the second Result TLV.
             * If between >=0x0 - <0x2 in USSD response then first Result should be SAT_RES_CMD_PERFORMED_WITH_CALL_CONTROL_MOD
             * in interval >=0x2 - <0x3 first Result should be  SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_TEMP
             * and last interval >=0x3 - <0x4 first Result should be SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_PERM
             * due to 3GPP spec.
             */
            if (result.general_result < 0x20) {
                result_first.general_result = SAT_RES_CMD_PERFORMED_WITH_CALL_CONTROL_MOD;
                result_first.additional_info_p = NULL;
                result_first.additional_info_size = 0;
            } else if ((result.general_result >= 0x20) && (result.general_result < 0x30)) {
                result_first.general_result = SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_TEMP;
                result_first.additional_info_p = NULL;
                result_first.additional_info_size = 0;
            } else {
                result_first.general_result = SAT_RES_INTERACTION_WITH_SIM_CALL_CONTROL_PERM;
                ste_apdu_additional_info_t add_info = SAT_RES_CC_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
                result_first.additional_info_size = 1;
                result_first.additional_info_p = (uint8_t*)(&add_info);
            }
            (void)ste_apdu_terminal_response_with_second_result(stash->apdu_parsed, &result_first, &result, &tr);
            catd_log_f(SIM_LOGGING_D,"catd/libpc : %s TR with two result TLVs created",__PRETTY_FUNCTION__);
        } else {
            (void)ste_apdu_terminal_response(stash->apdu_parsed, &result, &tr);
            catd_log_f(SIM_LOGGING_D,"catd/libpc : %s TR  created",__PRETTY_FUNCTION__);
        }

        if (tr) { // If we managed to create a tr, then all is well!
            catd_sig_tr(CATD_FD, tr, CATD_CLIENT_TAG);
        } else {
            /*
             *  Most likely, this will fail too, but hey' let's try, at least we'll get
             *  a nice log print before we get stuck...
             */
            pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, stash->apdu_raw);
        }

        global_ussd_stash = NULL;
        ste_apdu_delete(tr);      // Can handle null pointer...
        delete_stash(stash);
    }

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s EXIT", __PRETTY_FUNCTION__);
}

/**
 * Converts a CB DCS to a SMS DCS.
 *
 * The DCS used when sending and receiving USSD between the Network and the ME is according to
 * the CBS Data Coding Scheme in TS 23.038. However, when a USSD result shall be returned to the
 * SAT App using a terminal response, the DCS should be according to the SMS Data Coding Scheme
 * (in the same specification...)
 */
static uint8_t convert_cb_dcs2sms_dcs4ussd(uint8_t cb_dcs)
{
    uint8_t cg  = (cb_dcs >> 4) & 0x0F; // Shift down top 4 bits and make sure no sign bit interferes
    uint8_t lsn = cb_dcs & 0x0F;
    uint8_t dcs = 0xFF;

    if ((cg & 0x0C) == 0x04) {
        // CB coding group '01xx' (cg = [0x04..0x07]), it is directly mappable to SMS coding group '00xx'
        dcs = cb_dcs & 0x3F;
    } else if (cg == 0x0F) {
        // CB coding group '1111'. Almost directly mappable to SMS coding group '00xx'.
        // Bit 2 is the relevant bit and has the same meaning.
        dcs = cb_dcs & 0x04;
    } else if (cg == 0x00 && lsn == 0x0F) {
        // CB coding group '0000' indicating "Language unspecified" is mappable to SMS coding group '00xx'
        // Other values for lsn in this coding group is not mappable and probably not applicable to USSD.
        dcs = 0x00;

/*
 * The mappings below are uncertain. They may be possible to map. However, the source DCS may not be
 * applicable for USSD or the mapping might be incorrect.
 */
    } else if ((cg == 0x02 && lsn >= 0x05) || cg == 0x03 || cg == 0x08 || cg == 0x0A || cg == 0x0B || cg == 0x0C) {
        // CB coding group '0010' indicating "reserved", or reserved coding group.
        // Should be interpreted as cg = '0000', "Language unspecified"
        dcs = 0x00;
        catd_log_f(SIM_LOGGING_V, "catd/libpc : %s uncertain DCS mapping used.", __PRETTY_FUNCTION__);
    } else {
        // This means thet the cg, or cg & lsn combination is not mappable the SMS dcs or not applicable for USSB
        dcs = 0x00;
        catd_log_f(SIM_LOGGING_V, "catd/libpc : %s DCS not applicable to USSD encountered.", __PRETTY_FUNCTION__);
    }

    catd_log_f(SIM_LOGGING_V, "catd/libpc : %s 0x%X -> 0x%X", __PRETTY_FUNCTION__, (int)cb_dcs, (int)dcs);

    return dcs;
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
        *result_p = SAT_RES_USSD_RETURN_ERROR;
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
/**
 * Internal function to map an SS error to a command result in error cases.
 *
 * The byte parameter is there to provide storage for the additional info. The caller must
 * make sure the memory referenced by the byte parameter remains valid as long as the filled in
 * ste_command_result_t struct should remain valid.
 */
static void map_cn_ss_error2result(cn_ss_error_t* err, ste_command_result_t* res, uint8_t* byte)
{
    cn_ss_error_code_type_t type;

    assert(err && res && byte);

    res->command_status  = STE_SAT_PC_CMD_STATUS_COMPLETED;
    res->other_data_type = STE_CMD_RESULT_NOTHING;

    type  =  err->cn_ss_error_code_type;
    if ( type == CN_SS_ERROR_CODE_TYPE_CN ) {
        ste_apdu_general_result_t result;
        *byte = map_cn_error_to_general_result_and_additional_info(
                    err->cn_ss_error_value.cn_ss_error_code,
                    &result );
        res->general_result = result;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    } else if ( type == CN_SS_ERROR_CODE_TYPE_GSM ) {
        *byte = (uint8_t)err->cn_ss_error_value.cause_value; /* Cause value from TS 24.080 */
        res->general_result = SAT_RES_USSD_RETURN_ERROR;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    } else if ( type == CN_SS_ERROR_CODE_TYPE_MM ) {
        *byte = (uint8_t)err->cn_ss_error_value.cause_value; /* Cause value from TS 24.080 */
        res->general_result = SAT_RES_NETWORK_CURRENTLY_UNABLE;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    }
    else
    {
        /* Error Case. Type should be one of the three handled above */
        *byte = 0; /* No additional Info can be given */
        res->general_result = SAT_RES_USSD_RETURN_ERROR;
        res->additional_info_size = sizeof(*byte);
        res->additional_info_p = byte;
    }
}

/*
 * Internal function to store locally valid data when its a call control with modification
 * of service USSD. Raw apdu and session id are data from the set up call
 * which is needed when creating the terminal response for the call control with modification
 * of service (USSD).
 */
void set_stash_for_cc_ussd(uint32_t session_id, ste_apdu_t* apdu_raw_p) {

    pc_send_ussd_stash_t* stash   = NULL;
    ste_sat_apdu_error_t rc = STE_SAT_APDU_ERROR_UNKNOWN;

    if (!global_ussd_stash)
    {
        // Init the stash
        stash = malloc(sizeof(pc_send_ussd_stash_t));
        if (!stash) {
            catd_log_f(SIM_LOGGING_D, "%s: calloc failed for stash", __FUNCTION__);
            return;
        }

        // Copy the raw apdu into the stash
        stash->apdu_raw = ste_apdu_copy(apdu_raw_p);

        // Parse the raw apdu and saved into the stash
        rc = ste_apdu_parse(stash->apdu_raw, &stash->apdu_parsed); // Allocates memory
        if (rc != STE_SAT_APDU_ERROR_NONE) {
            catd_log_f(SIM_LOGGING_D, "%s: failed to parse apdu_raw", __FUNCTION__);
            //Free memory
            ste_apdu_delete(stash->apdu_raw);
            free(stash);
            return;
        }

        stash->flags        = 0;
        stash->icon_handled = TRUE;
        stash->session_id   = session_id;

        // Set the CC -> USSD indication
        stash->flags |= GOT_CC_IND;

        // Set got session id
        stash->flags |= GOT_SESSION_ID;

        //Set the global stash to point to the current stash
        global_ussd_stash = stash;
    } else {
        catd_log_f(SIM_LOGGING_I, "%s: global_ussd_stash is NULL", __FUNCTION__);
    }
}

/*************************************************************************
 * @brief:    reset ussd
 * @params:
 *            void
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_reset_pc_send_ussd(void) {
    delete_stash(global_ussd_stash);
    global_ussd_stash = NULL;
}
