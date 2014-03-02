/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : pc_more_time.c
 * Description     : Handler function for proactive command more time.
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

#include "pc_utilities.h"
#include "sim_internal.h"
#include "catd.h"
#include "catd_msg.h"
#include "apdu.h"
#include "sim_unused.h"
#include "func_trace.h"

/*************************************************************************
 * @brief:    handle the particular proactive command of more time internally
 * @params:
 *            parsed_apdu_p: the parsed apdu structure.
 *            msg:           the original message with APDU data, and client_tag, etc. for possible future usage.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
static void pc_main_handle_more_time( ste_parsed_apdu_t    * parsed_apdu_p,
                                      catd_msg_apdu_t      * msg)
{
    ste_sat_apdu_error_t                   rv;
    ste_apdu_pc_more_time_t              * more_time_p;
    ste_apdu_t                           * apdu = msg->apdu;

    rv = ste_parsed_apdu_get(parsed_apdu_p, (void**)&more_time_p, STE_APDU_CMD_TYPE_MORE_TIME);
    if (rv != STE_SAT_APDU_ERROR_NONE)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu);
        return;
    }
    if (!more_time_p)
    {
        catd_log_f(SIM_LOGGING_E, "catd : failed to fetch APDU data.");
        pc_send_terminal_response_error(rv, apdu);
        return;
    }
    if (more_time_p->bit_mask & PC_MORE_TIME_DeviceID_present)
    {
        //dispatch this command to somewhere

        pc_send_terminal_response_general(parsed_apdu_p, SAT_RES_CMD_PERFORMED_SUCCESSFULLY);
    }
    else
    {
        catd_log_f(SIM_LOGGING_E, "catd : Error in APDU data.");
        pc_send_terminal_response_general(parsed_apdu_p, SAT_RES_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME);
        return;
    }

}

/*************************************************************************
 * @brief:    handle the particular proactive command of more time
 * @params:
 *            msg:         the original message with APDU data, and client_tag, etc.
 *
 * @return:   void
 *
 * Notes:
 *************************************************************************/
void catd_handle_pc_more_time(catd_msg_apdu_t  * msg)
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
    pc_main_handle_more_time(parsed_apdu_p, msg);
    ste_parsed_apdu_delete(parsed_apdu_p);
}


