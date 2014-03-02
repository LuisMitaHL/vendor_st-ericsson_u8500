/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_send_dtmf.c
 * Description     : Handler function for proactive command send dtmf.
 *
 * Author          : Leif Simmons <leif.simmons@stericsson.com>
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
#include "cn_client.h"

/*
 * This the struct to hold the stuff we need to stash away while performing asynchronous requests...
 */
typedef struct {
    ste_apdu_t*        apdu_raw;     // Points to the original "rawer" APDU
    ste_parsed_apdu_t* apdu_parsed;  // Points to the parsed APDU
    boolean            icon_handled; // False if there is a icon and the client doesn't have a GUI
} pc_send_dtmf_stash_t;

/*
 * Deletes the stash
 */
static void delete_stash(pc_send_dtmf_stash_t* stash)
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

static int convert_dtmf_bcd2ascii(ste_apdu_dtmf_string_t* in, char** out)
{
    int maxlen;
    assert(in);
    assert(in->data_p);
    assert(out);

    maxlen = (in->data_len * 2) + 1;
    *out = NULL;
    *out = malloc(maxlen); // +1 do make room for a NULL terminator, just in case...
    if (*out) {
        return convert_bcd2ascii(in->data_p, in->data_len, *out, maxlen);
    }
    return -1;
}

//CN_REQUEST_DTMF_SEND

static int pc_handle_send_dtmf_cn_response_handler(ste_msg_t* ste_msg)
{
    catd_msg_cn_t*        msg      = (catd_msg_cn_t*)ste_msg;
    ste_sim_ctrl_block_t* ctrl_p   = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    pc_send_dtmf_stash_t* stash    = NULL;
    ste_apdu_t*           response = NULL;
    ste_command_result_t  result;
    ste_sat_apdu_error_t             rv;

    catd_log_f(SIM_LOGGING_D, "catd : %s : ENTER", __func__);

    assert(msg);                                        // There has to be a message, and
    assert(msg->cn_msg);                                // the message must contain a C&N message, and
    assert(msg->cn_msg->type == CN_RESPONSE_DTMF_SEND); // the C&N message must be a response to our send DTMF request...

    ctrl_p = (ste_sim_ctrl_block_t*)msg->client_tag;            assert(ctrl_p); // There has to be a control block, and
    stash  = (pc_send_dtmf_stash_t*)ctrl_p->transaction_data_p; assert(stash);  // there has to be a stash!

    if (msg->cn_msg->error_code == CN_SUCCESS) {
        if (stash->icon_handled) {
            result.general_result = SAT_RES_CMD_PERFORMED_SUCCESSFULLY;
        } else {
            result.general_result = SAT_RES_CMD_PERFORMED_SUCC_WITHOUT_REQ_ICON;
        }
        result.additional_info_p    = NULL;
        result.additional_info_size = 0;
    } else {
       uint8_t cause = SAT_RES_MEP_NOT_IN_SPEECH_CALL;

       result.general_result = SAT_RES_ME_CURRENTLY_UNABLE_TO_PROCESS_CMD;
       result.additional_info_p      = &cause;
       result.additional_info_size   = sizeof(cause);
    }

    result.command_status         = STE_SAT_PC_CMD_STATUS_COMPLETED;
    result.other_data_type        = STE_CMD_RESULT_NOTHING;

    rv = ste_apdu_terminal_response(stash->apdu_parsed, &result, &response); // response will only be set if all is ok

    if (rv == STE_SAT_APDU_ERROR_NONE && response) { // If we managed to create a response, then all is well!
        catd_sig_tr(CATD_FD, response, CATD_CLIENT_TAG);
    } else {
        /*
         *  Most likely, this will fail too, but hey' let's try, at least we'll get
         *  a nice log print before we get stuck...
         */
        pc_send_terminal_response_error(STE_SAT_APDU_ERROR_UNKNOWN, stash->apdu_raw);
    }

    ste_apdu_delete(response);      // Can handle null pointer...
    delete_stash(stash);
    catd_log_f(SIM_LOGGING_D, "catd : %s : EXIT(%d)", __func__, msg->cn_msg->error_code);
    return 0;
}

int pc_main_handle_send_dtmf(catd_msg_apdu_t* msg, boolean gui_capable_client)
{
    ste_sim_ctrl_block_t* ctrl_p  = NULL; // Use _p naming convention because someone likes to grep for "ctrl_p" :)
    pc_send_dtmf_stash_t* stash   = NULL;
    ste_apdu_t*           apdu    = NULL;
    ste_sat_apdu_error_t  rc      = STE_SAT_APDU_ERROR_UNKNOWN;
    char*                 str     = NULL;
    int                   len     = 0;
    cn_context_t*         context;

    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    apdu = msg->apdu;
    msg->apdu = NULL; // Make sure the framework does not delete the APDU when we exit...
    assert(apdu);

    stash = malloc(sizeof(*stash));
    if (stash) {
        stash->icon_handled = TRUE;
        stash->apdu_raw     = apdu;
        stash->apdu_parsed  = NULL;
        rc = ste_apdu_parse(stash->apdu_raw, &stash->apdu_parsed); // Allocates memory
    }

    ctrl_p  = ste_sim_create_ctrl_block(CATD_CLIENT_TAG,
                                        CATD_FD,
                                        CATD_MSG_NONE,
                                        pc_handle_send_dtmf_cn_response_handler,
                                        stash); // stash may be NULL here, but it's ok...

    if (rc == STE_SAT_APDU_ERROR_NONE) {
        ste_apdu_pc_send_dtmf_t* dtmf;
        rc = ste_parsed_apdu_get(stash->apdu_parsed, (void**)&dtmf, STE_APDU_CMD_TYPE_SEND_DTMF); // Doesn't allocate memory
        if (rc == STE_SAT_APDU_ERROR_NONE) {
            if (dtmf->bit_mask & PC_SEND_DTMF_IconIdentifier_present)
            {
                stash->icon_handled = gui_capable_client;
            }

            if ((dtmf->bit_mask & PC_SEND_DTMF_DTMF_String_present)  &&
                (dtmf->dtmf.data_len > 0)                            &&
                (dtmf->dtmf.data_p)                                  ) {
                len = convert_dtmf_bcd2ascii(&dtmf->dtmf, &str); // Have to convert since DTMF BCD functionality is useless in it's current state...
            } else {
                rc = STE_SAT_APDU_ERROR_REQUIRED_VALUES_MISSING;
            }
        }
    }

    if ( rc == STE_SAT_APDU_ERROR_NONE ) {
        context = cn_get_client();
        if (context && ctrl_p && len > 0) {
            catd_log_f(SIM_LOGGING_D, "catd/libpc : %s, str = \'%s\'", __PRETTY_FUNCTION__, str);
            cn_error_code_t err = cn_request_dtmf_send(context, str, len, CN_DTMF_STRING_TYPE_ASCII, 0, 0, (uintptr_t)ctrl_p);
            if (err == CN_SUCCESS) {
                catd_pc_send_pc_notification_ind(stash->apdu_raw); // Must call notify ourselves since we set msg->apdu to NULL
                free(str);
                return 1; // Tell framework not to notify, we already did it...
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
    free(str);
    return -1;
}

int catd_handle_pc_send_dtmf(catd_msg_apdu_t* msg, boolean gui_capable_client)
{
    catd_log_f(SIM_LOGGING_D, "catd/libpc : %s", __PRETTY_FUNCTION__);

    assert(msg);
    return pc_main_handle_send_dtmf(msg, gui_capable_client);
}


