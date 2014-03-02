/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include <atc.h>
#include <atc_config.h>
#include <atc_exe_glue.h>
#include <atc_handlers.h>
#include <atc_log.h>
#include <atc_parser.h>
#include <atc_parser_util.h>

#include "exe.h"


#define SIZE_OF_ETZV_RESPONSE 70
#define SIZE_OF_MONOTONIC_TIME_STRING 19+1
#define SIZE_OF_REAL_TIME_STRING 19+1
/*
 * Handles ETZV related event and sends an unsolicited.
 */
void handle_incoming_ETZV_event(exe_etzv_response_t *response_p)
{
    unsigned char entry;
    char etzv_response[SIZE_OF_ETZV_RESPONSE];
    char monotonic_time[SIZE_OF_MONOTONIC_TIME_STRING];
    char utc_time[SIZE_OF_REAL_TIME_STRING];
    AT_ParserState_s *parser_p = NULL;
    bool result = false;

    memset(etzv_response, 0, SIZE_OF_ETZV_RESPONSE);

    /* Loop through all parser states to find ETZR subscriptions. */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        /* Build report if ETZR is enabled. */
        if (parser_p->ETZR != ETZR_DISABLED) {
            int index = 0;

            /* Abort early if not all conditions have been met */
            if (!atc_enough_data_according_to_etzr_setting(response_p, parser_p->ETZR)) {
                ATC_LOG_E("%s: not enough event data for current ETZR setting (%d), discarding event!",
                          __FUNCTION__, parser_p->ETZR);
                goto exit;
            }

            /* Add time zone value with corresponding sign. */
            sprintf(&etzv_response[index], "*ETZV: \"");
            index += 8;

            if (0 == response_p->time_zone_sign) {
                sprintf(&etzv_response[index], "+");
            } else {
                sprintf(&etzv_response[index], "-");
            }

            index++;

            sprintf(&etzv_response[index], "%02d\"", response_p->time_zone_value); /* expressed in quarters of an hour */
            index += 3;

            if (parser_p->ETZR >= ETZR_TIMEZONE_TIME_TIMESTAMP) {
                if( response_p->year == TIME_INFO_NOT_AVAILABLE) {
                    result = atc_get_utc_time_string(utc_time, SIZE_OF_REAL_TIME_STRING);
                    if (!result) {
                        ATC_LOG_E("atc_get_utc_time_string failed!");
                        goto exit;
                    }

                    ATC_LOG_D("atc_get_utc_time_string return  %s",utc_time);
                    sprintf(&etzv_response[index],", \"%s\"",utc_time);
                } else {
                    /* Add time (network time on format YYYY/MM/DD,hh:mm:ss) */
                    sprintf(&etzv_response[index], ", \"2%03u/%02u/%02u,%02u:%02u:%02u\"",
                            response_p->year,
                            response_p->month,
                            response_p->day,
                            response_p->hour,
                            response_p->minute,
                            response_p->second);

                    /* Save NITZ time */
                    strncpy(old_nitz_time,&etzv_response[index+3],SIZE_OF_REAL_TIME_STRING-1);
                    ATC_LOG_D("NITZ time saved %s",old_nitz_time);
                }

                index += 23;
                /* Add timestamp (monotonic time mapped to format YYYY/MM/DD,hh:mm:ss, starting with 1970-01-01,00:00:00) */
                result = atc_get_monotonic_time_string(monotonic_time, SIZE_OF_MONOTONIC_TIME_STRING);

                if (!result) {
                    ATC_LOG_E("get_monotonic_time_string failed!");
                    goto exit;
                }

                if( response_p->year != TIME_INFO_NOT_AVAILABLE) {
                    /* Add monotonic time */
                    strcpy(old_monotonic_time,monotonic_time);

                    ATC_LOG_D("Monotonic Time saved %s", old_monotonic_time);
                }

                sprintf(&etzv_response[index], ", \"%s\"", monotonic_time);
                index += 23;
            }

            if (ETZR_TIMEZONE_TIME_TIMESTAMP_DAYLIGHT_SAVING == parser_p->ETZR) {
                sprintf(&etzv_response[index], ", \"%d\"", response_p->day_light_saving_time);
                index += 5;
            }

            /* Send the unsolicited response. */
            parser_send_unsolicited(parser_p, AT_TEXT_RESPONSE, (AT_CommandLine_t)etzv_response);
        }
    }

exit:
    return;
}


/*
 *===============================================================================
 *
 *   Function: AT_STAR_ETZR_Handle
 *
 *   INPUT:   parser_p   - Pointer to the current Parser_p state.
 *            info_text  - Pointer to a string buffer to put information text.
 *            message    - Response message.
 *
 *   RETURNS: A status code for the execution of the command is returned:
 *            AT_OK        - The command was successfully handled.
 *            AT_ERROR     - There was a general error.
 *            AT_CME_ERROR - There was a GSM specific error, the error code is
 *                           returned in InfoText.
 *
 *   The *ETZR function is used to enable/disable *ETZV UR code printing.
 *
 *   SET  - Sets the selected <n>:
 *          n=0 disable time reporting
 *          n=1 enable time reporting on the format *ETZV:<tz>
 *          n=2 enable time reporting on the format *ETZV:<tz>,<time>,<timestamp>
 *          n=3 enable time reporting on the format *ETZV:[<tz>],[<time>],<timestamp>[,<dst>]
 *
 *          for n=3, all parameters are always sent to keep it simple in AT as well as for clients.
 *
 *   READ - Returns the current <n>.
 *
 *   TEST - Writes the supported range for <n>.
 *
 *===============================================================================
 */
AT_Command_e AT_STAR_ETZR_Handle(AT_ParserState_s *parser_p,
                                 AT_CommandLine_t info_text,
                                 AT_ResponseMessage_t message)
{
    exe_cmee_error_t current_CMEE = CMEE_OK;

    switch (parser_p->Mode) {
    case AT_MODE_SET: {
        bool          error_flag      = false;
        ETZR_format_t report_format = ETZR_DISABLED;

        report_format = (ETZR_format_t) Parser_GetIntParam(parser_p,
                        &error_flag,
                        NULL);

        if (error_flag) {
            ATC_LOG_E("AT_STAR_ETZR_Handle(%d) - Not able to fetch parameter <n>",
                      parser_p->ContextID);
            return AT_ERROR;
        }

        if (!parser_p->NoMoreParams) {
            ATC_LOG_E("AT_STAR_ETZR_Handle(%d) - Too many parameters.",
                      parser_p->ContextID);
            return AT_ERROR;
        }

        if (report_format > ETZR_TIMEZONE_TIME_TIMESTAMP_DAYLIGHT_SAVING) {
            current_CMEE = CMEE_OPERATION_NOT_SUPPORTED;
            ATC_LOG_E("AT_STAR_ETZR_Handle(%d) - Value is outside range of <n>",
                      parser_p->ContextID);
            break;
        }

        parser_p->ETZR = report_format;

        break;
    }

    case AT_MODE_READ:
        AT_AddValue(info_text, ": 0", parser_p->ETZR);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;

    case AT_MODE_TEST:
        AT_AddRange(info_text, ": (0-0)", 0, ETZR_TIMEZONE_TIME_TIMESTAMP_DAYLIGHT_SAVING);
        Parser_SendResponse(parser_p, AT_NO_OVERRIDE, info_text);
        break;

    default:
        return AT_ERROR;
    }

    if (current_CMEE != CMEE_OK) {
        Parser_SetCMEE_ErrorType(parser_p, current_CMEE);
        return AT_CME_ERROR;
    }

    return AT_OK;
}
