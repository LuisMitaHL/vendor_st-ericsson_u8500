/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
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

/*************************************************************************
 * @brief:    handle the particular proactive command of language notification internally
 * @params:
 *            parsed_apdu_p: the parsed apdu structure.
 *            msg:           the original message with APDU data, and client_tag, etc. for possible future usage.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void pc_main_handle_language_notification( ste_parsed_apdu_t    * parsed_apdu_p,
                                                  catd_msg_apdu_t      * msg)
{
    ste_sat_apdu_error_t                   rv;
    ste_apdu_pc_language_notification_t  * language_notification_p;
    ste_apdu_t                           * apdu = msg->apdu;
    int                                    i;

    rv = ste_parsed_apdu_get(parsed_apdu_p, (void**)&language_notification_p, STE_APDU_CMD_TYPE_LANGUAGE_NOTIFICATION);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu);
        return;
    }
    if (!language_notification_p)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu);
        return;
    }

    for (i = 0; i < language_notification_p->language.length; i++) {
        catd_log_f(SIM_LOGGING_D, "catd : language (byte %d): 0x%x", i, language_notification_p->language.language_p[i]);
    }

    // Success
    pc_send_terminal_response_general(parsed_apdu_p, SAT_RES_CMD_PERFORMED_SUCCESSFULLY);
}

/*************************************************************************
 * @brief:    handle the particular proactive command of language notification
 * @params:
 *            msg:         the original message with APDU data, and client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_handle_pc_language_notification(catd_msg_apdu_t  * msg)
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
        return;
    }
    pc_main_handle_language_notification(parsed_apdu_p, msg);
    ste_parsed_apdu_delete(parsed_apdu_p);
}
