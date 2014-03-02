/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "atc.h"
#include "atc_log.h"
#include "atc_parser_util.h"


/*
 * Adds an unsolicited to the pending queue.
 */
static bool parser_pending_queue_add(AT_ParserState_s *parser_p,
                                     AT_Command_e override,
                                     AT_CommandLine_t response)
{
    bool result = false;
    atc_pending_queue_t *item_p = malloc(sizeof(atc_pending_queue_t));

    if (item_p == NULL) {
        goto exit;
    }

    item_p->override = override;
    item_p->response = (AT_CommandLine_t)malloc(strlen((char *)response) + 1);
    strcpy((char *)item_p->response, (char *)response);
    item_p->next = NULL;

    if (parser_p->pending_queue_p == NULL) {
        parser_p->pending_queue_p = item_p;
    } else {
        atc_pending_queue_t *temp_item_p = parser_p->pending_queue_p;

        while (temp_item_p->next != NULL) {
            temp_item_p = temp_item_p->next;
        }

        temp_item_p->next = item_p;
    }

    result = true;

exit:
    return result;
}

/*
 * Flushes the pending queue, i.e. sends all queued unsoliciteds if
 * the channel is no longer pending.
 */
void parser_pending_queue_flush()
{
    /* TODO May be extended to only flush one channel or all but one channel. */
    AT_ParserState_s *parser_p;
    unsigned char entry;

    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (!parser_p->IsPending && parser_p->pending_queue_p != NULL) {
            atc_pending_queue_t *item_p = parser_p->pending_queue_p;

            while (item_p != NULL) {
                Parser_SendResponse(parser_p, item_p->override, item_p->response);
                free(item_p->response);
                parser_p->pending_queue_p = item_p->next;
                free(item_p);
                item_p = parser_p->pending_queue_p;
            }
        }
    }
}


/*
 *=====================================================================
 *
 *  Function: parser_send_unsolicited
 *
 *  Input:  parser_p    - Pointer to the current parser state.
 *          override    - Response code that overrides the current
 *                        Parse->Command. AT_NO_OVERRIDE is not allowed
 *                        as it may cause wrong prefix to the response!
 *          response    - Pointer to the response string.
 *                        The pointer may be set to NULL if there is no
 *                        additional text to add.
 *
 *  Output: -
 *
 *      This function sends an unsolicited on the specified parser state
 *      if it is not pending. The request is put in a queue if the parser
 *      state is pending, and send later.
 *
 * ==================================================================
 */
void parser_send_unsolicited(AT_ParserState_s *parser_p, AT_Command_e override,
                             AT_CommandLine_t response)
{
    /* Add to queue if parser is pending. */
    if (parser_p->IsPending) {
        parser_pending_queue_add(parser_p, override, response);
    }
    /* Send unsolicited if not pending and not AT_NO_OVERRIDE. */
    else if (override != AT_NO_OVERRIDE) {
        Parser_SendResponse(parser_p, override, response);
    }
}

AT_Command_e parser_handle_exe_result(exe_request_result_t exe_res,
                                      AT_ParserState_s *parser_p)
{
    AT_Command_e result = AT_ERROR;

    switch (exe_res) {
    case EXE_PENDING:
        result = AT_PENDING;
        break;
    case EXE_SUCCESS:
        result = AT_OK;
        break;
    case EXE_FAILURE:
        result = AT_CME_ERROR;
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_ALLOWED);
        break;
    case EXE_NOT_SUPPORTED:
        result = AT_CME_ERROR;
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
        break;
    default:
        result = AT_CME_ERROR;
        Parser_SetCMEE_ErrorType(parser_p, CMEE_OPERATION_NOT_SUPPORTED);
        break;
    }

    return result;
}


/*****************************************************************************
* non_dial_characters_successfully_removed
*
* Remove all non dial characters, check that any modifier characters
* are located at the end of the dial string and verifies that the dial string is
* within MAX_DIGITS_IN_NUMBER range.
*
* Valid character: '*','#','+'(unless first digit in dial string),'A','B','C'
* and '0','1','2','3','4','5','6','7','8','9','p','w'
*
* Valid characters that should be ignored: 'D','W','T','P','!','@' and ','
*
* I/i and G/g is are optional modifier characters if present at the end of the
* string, they are ignored here so if applicable they need to be handled elsewhere.
* If they are not in the end the function will return an error.
*
* If any other characters not mentioned above is used the function will return false.
*
* in/out: dial_string point to the first position in the dial_string
*
*****************************************************************************/
bool non_dial_characters_successfully_removed(char *dial_string_p, exe_cmee_error_t *cme_err_p)
{
    bool  modifiers_checked = false;
    char *curr_pos = dial_string_p;
    char *pos = dial_string_p;

    /* ATD last dial number and phonebook memory dial features are not supposed to be handled here.
     * They should be handled at android level.
     */
    if ('L' == *pos || 'l' == *pos || '>' == *pos) {
        return false;
    }

    while ((*pos != ';') && (*pos != '\0') && (modifiers_checked == false)) {
        bool result = true;

        if (isdigit(*pos) || *pos == '*' || *pos == '#' || *pos == '+'
                || *pos == 'A' || *pos == 'B' || *pos == 'C' || *pos == 'p' || *pos == 'w') {
            /* Valid character */
            *curr_pos++ = *pos;
        } else {
            switch (*pos) {
            case 'I': {
                modifiers_checked = true;
                pos++;

                if (*pos == ';') {
                    pos--;
                } else if ((*pos == 'G') || (*pos == 'g')) {
                    /* Ignore */
                } else {
                    result = false; /* Invalid modifier characters */
                }

                break;
            }
            case 'i': {
                modifiers_checked = true;
                pos++;

                if (*pos == ';') {
                    pos--;
                } else if ((*pos == 'G') || (*pos == 'g')) {
                    /* Ignore */
                } else {
                    result = false; /* Invalid modifier characters */
                }

                break;
            }
            case 'G': {
                modifiers_checked = true;
                pos++;

                if (*pos == ';') {
                    pos--;
                } else {
                    result = false; /* Invalid modifier characters */
                }

                break;
            }
            case 'g': {
                modifiers_checked = true;
                pos++;

                if (*pos == ';') {
                    pos--;
                } else {
                    result = false; /* Invalid modifier characters */
                }

                break;
            }
            default:
                /* Every other non valid character or modifier should be ommited */
                break;
            }

            if (!result) {
                ATC_LOG_E("Dial string is erroneous; \"%s\"!!", dial_string_p);

                if (NULL != cme_err_p) {
                    *cme_err_p = CMEE_INVALID_CHARACTER_IN_DIAL_STRING;
                }

                return false;
            }
        }

        pos++;
    }

    /* Check that none of the combinations IG, Ig, ig or iG has occured in the middle of a dial string. */
    if ((*pos != ';') && (modifiers_checked == true)) {
        ATC_LOG_E("Dial modifiers 'I', 'i', 'G' or 'g' at wrong place in dial string; \"%s\"!!", dial_string_p);

        if (NULL != cme_err_p) {
            *cme_err_p = CMEE_INCORRECT_PARAMETERS;
        }

        return false;
    }

    /* We don't want the i/I or g/G so if they are present step back and point to the first
     * character after the valid phone number
     */
    if (modifiers_checked == true) {
        if (*(curr_pos - 1) == 'I' || *(curr_pos - 1) == 'i') {
            curr_pos--;
        }
    }

    /* CurrPos now points at the character after the last digit in the number to dial. */
    *curr_pos = '\0'; /* DialString is now a null terminated string
                         containing only valid characters (unless
                         there is a '+' in the start of the string). */

    if (((*dial_string_p != '+') && (strlen((char *) dial_string_p)
                                     > MAX_DIGITS_IN_NUMBER)) || ((*dial_string_p == '+')
                                             && (strlen((char *) dial_string_p)
                                                     > (MAX_DIGITS_IN_NUMBER + 1)))) {
        ATC_LOG_E("Dial string exceeds maximum length of %d characters!!", MAX_DIGITS_IN_NUMBER);

        if (NULL != cme_err_p) {
            *cme_err_p = CMEE_DIAL_STRING_TOO_LONG;
        }

        return false;
    }

    return true; /* Non dial characters successfully removed */
}


/*
 *==============================================================================================
 *                                                                                             *
 *   Function: atc_enough_data_according_to_etzr_setting                                       *
 *                                                                                             *
 *   DESCRIPTION: Checks that enough data has been provided to be able to generate *ETZV       *
 *                events based on the given ETZR setting.                                      *
 *                                                                                             *
 *   INPUT:   response_p   - Pointer to ETZV event data                                        *
 *            etzr_setting - ETZR setting                                                      *
 *                                                                                             *
 *   RETURNS: true if enough data exist for *ETZV                                              *
 *            false if not enough data has been found                                          *
 *                                                                                             *
 *==============================================================================================
 */

bool atc_enough_data_according_to_etzr_setting(exe_etzv_response_t *response_p, ETZR_format_t etzr_setting)
{
    bool result = false; /* not enough data until proven below */

    if (!response_p) {
        ATC_LOG_E("%s: response_p is NULL!", __FUNCTION__);
        goto exit;
    }

    switch (etzr_setting) {
    case ETZR_TIMEZONE:

        if (response_p->time_zone_sign  != TIME_INFO_NOT_AVAILABLE &&
                response_p->time_zone_value != TIME_INFO_NOT_AVAILABLE) {
            result = true;
        }

        break;
    case ETZR_TIMEZONE_TIME_TIMESTAMP: /* the monotonic timestamp is not dependent on event data */

        if (response_p->time_zone_sign  != TIME_INFO_NOT_AVAILABLE &&
                response_p->time_zone_value != TIME_INFO_NOT_AVAILABLE &&
                response_p->year            != TIME_INFO_NOT_AVAILABLE &&
                response_p->month           != TIME_INFO_NOT_AVAILABLE &&
                response_p->day             != TIME_INFO_NOT_AVAILABLE &&
                response_p->hour            != TIME_INFO_NOT_AVAILABLE &&
                response_p->minute          != TIME_INFO_NOT_AVAILABLE &&
                response_p->second          != TIME_INFO_NOT_AVAILABLE) {
            result = true;
        }

        break;
    case ETZR_TIMEZONE_TIME_TIMESTAMP_DAYLIGHT_SAVING:

        if (response_p->time_zone_sign  != TIME_INFO_NOT_AVAILABLE &&
                response_p->time_zone_value != TIME_INFO_NOT_AVAILABLE) {
            if (response_p->day_light_saving_time == TIME_INFO_NOT_AVAILABLE) {
                response_p->day_light_saving_time = 0;
            }

            result = true;
        }

        break;
    case ETZR_DISABLED:
    default:
        ATC_LOG_E("%s: bad etzr_setting (%d)", __FUNCTION__, etzr_setting);
        break;
    }

exit:
    return result;
}

/*
 *==============================================================================================
 *                                                                                             *
 *   Function: atc_get_monotonic_time_string                                                   *
 *                                                                                             *
 *   DESCRIPTION: Generation of a monotonic time string with format "yyyy/MM/dd,hh:mm:ss"      *
 *                Time starting point: 1970-01-01 00:00:00                                     *
 *                                                                                             *
 *   Monotonic time is not based on the system clock or a network clock. It can never be       *
 *   adjusted with respect to daylight savings, leap seconds and so on. It is incremented      *
 *   in a deterministic way based on a given reference point. This reference point is          *
 *   normally the starting point of the machine (this is how it is implemented in Linux).      *
 *                                                                                             *
 *   This time representation is not used to describe what time it is, but its usefulness      *
 *   comes when comparing timestamps. The comparison is always reliable, which isn't the       *
 *   case for normal timestamp comparisons (comparing a regular timestamp with another one     *
 *   that has been adjusted with respect to e.g. daylight saving gives incorrect               *
 *   information).                                                                             *
 *                                                                                             *
 *   Since the monotonic reference point of the system is based on the startup of the machine  *
 *   it isn't the same as the epoch reference time. Therefore a time translation is needed.    *
 *   This is done internally in this function.                                                 *
 *                                                                                             *
 *   INPUT:   buffer_p    - Pointer to a string buffer (ANSI/ASCII character set).             *
 *            buffer_size - Maximum size of buffer (including string termination character)    *
 *                                                                                             *
 *   RETURNS: true if the operation has succeeded and buffer_p has been updated.               *
 *            false if an error has occured                                                    *
 *                                                                                             *
 *==============================================================================================
 */
bool atc_get_monotonic_time_string(char *buffer_p, int buffer_size)
{
    int result = 0;
    time_t time_res = 0;
    struct timespec monotonic_time;
    struct tm time;

    result =  clock_gettime(CLOCK_MONOTONIC, &monotonic_time);

    if (-1 == result) {
        ATC_LOG_E("%s: clock_gettime failed!", __FUNCTION__);
        goto error;
    }

    /* A time string with format "yyyy/MM/dd,hh:mm:ss" is generated below. The monotonic reference
     * point is the time the machine was started (uptime). This is translated to a 1970/01/01
     * 00:00:00 reference point.
     *
     * The code below relies on the out of range handling of time.tm_sec in mktime() which is a very
     * convenient feature. The data type range has been analyzed and isn't a source of trouble. It
     * permits a machine uptime of floor( 2147483647 / (3600*24*365) ) = 68 years before overflowing
     * to 1970 again, which definitely won't be an issue for a mobile platform.
     */

    /* Create time structure needed by strftime() */
    time.tm_isdst = 0;           /* Daylight Savings flag */
    time.tm_year  = 1970 - 1900; /* Years since 1900      */
    time.tm_mon   = 0;           /* Month of year [0,11]  */
    time.tm_mday  = 1;           /* Day of month [1,31]   */
    time.tm_hour  = 0;           /* Hour [0,23]           */
    time.tm_min   = 0;           /* Minutes [0,59]        */
    time.tm_sec   = (int) monotonic_time.tv_sec;

    time_res = mktime(&time);

    if (-1 == time_res) {
        ATC_LOG_E("%s: mktime failed!", __FUNCTION__);
    }

    /* Create time string with format "yyyy/MM/dd,hh:mm:ss" */
    result = strftime(buffer_p, buffer_size, "%Y/%m/%d,%H:%M:%S", &time);

    if (0 == result) {
        ATC_LOG_E("%s: strftime failed!", __FUNCTION__);
        goto error;
    }

    return true;

error:
    return false;
}

/*
 *==============================================================================================
 *                                                                                             *
 *   Function: atc_get_utc_time_string                                                         *
 *                                                                                             *
 *   DESCRIPTION: Generation of a Universal time string with format "yyyy/MM/dd,hh:mm:ss"      *
 *                                                                                             *
 *                                                                                             *
 *   INPUT:   buffer_p    - Pointer to a string buffer (ANSI/ASCII character set).             *
 *            buffer_size - Maximum size of buffer (including string termination character)    *
 *                                                                                             *
 *   RETURNS: true if the operation has succeeded and buffer_p has been updated.               *
 *            false if an error has occured                                                    *
 *                                                                                             *
 *==============================================================================================
 */
bool atc_get_utc_time_string(char *buffer_p, int buffer_size)
{
    time_t old_monotonic, old_nitz, new_monotonic;
    double diff;
    struct tm old_monotonic_tm;
    struct tm new_monotonic_tm;
    struct tm old_nitz_tm;
    struct tm *new_nitz_tm;
    char monotonic_time[19+1];
    char *tz = getenv("TZ");
    int result = 0;
    time_t time_res = 0;
    struct tm time;
    bool retval = false;

    /* Calculate the current UTC*/
    setenv("TZ", "", 1);
    tzset();

    memset(&old_monotonic_tm, 0, sizeof(struct tm));
    memset(&old_nitz_tm, 0, sizeof(struct tm));
    memset(&new_monotonic_tm, 0, sizeof(struct tm));
    memset(buffer_p, 0,buffer_size );

    result = atc_get_monotonic_time_string(monotonic_time, buffer_size);

    ATC_LOG_D("old_nitz_time return  %s",old_nitz_time);

    /* Parse string format yyyy/MM/dd,hh:mm:ss */
    strptime(old_monotonic_time, "%Y/%m/%d,%H:%M:%S", &old_monotonic_tm);
    strptime(old_nitz_time, "%Y/%m/%d,%H:%M:%S", &old_nitz_tm);
    strptime(monotonic_time, "%Y/%m/%d,%H:%M:%S", &new_monotonic_tm);

    /* Convert struct tm format to time_t */
    old_monotonic = mktime(&old_monotonic_tm);
    old_nitz = mktime(&old_nitz_tm);
    new_monotonic = mktime(&new_monotonic_tm);

    /* This should never happen.old_monotonic or old_nitz or new_monotonic are never set */
    if (0 > old_monotonic || 0 > old_nitz || old_monotonic > new_monotonic) {
        ATC_LOG_E("old_monotonic/old_nitz never set Or old_monotonic is ahead of new_monotonic!");
        retval = false;
        goto exit;
    }

    /* Find new monotonic clock/old monotonic time diff and add diff to NITZ time */
    diff = difftime(new_monotonic, old_monotonic);

    if (diff > 0) {
        old_nitz += (time_t)diff;
        ATC_LOG_D("NITZ is %.2lf seconds old, adjusting time", diff);
        ATC_LOG_D("New value of NITZ in seconds = %d", old_nitz);

        /* Convert back time_t to struct tm format */
        new_nitz_tm = gmtime(&old_nitz);

        /* Converting back to string format yyyy/MM/dd,hh:mm:ss */
        strftime(buffer_p, buffer_size, "%Y/%m/%d,%H:%M:%S", new_nitz_tm);
        retval = true;
    } else {
        ATC_LOG_D("No adjustment needed %f", diff);
        strcpy(buffer_p,old_nitz_time);
        retval = true;
    }

exit:
    if (tz)
        setenv("TZ", tz, 1);
    else
        unsetenv("TZ");

    tzset();

    return retval;
}

/*==============================================================================================
 *                                                                                             *
 *   Function: map_exe_charset_to_atc_charset                                                  *
 *                                                                                             *
 *   DESCRIPTION: Maps exe_charset_t enums to atc_charset_t enums                              *
 *                                                                                             *
 *                                                                                             *
 *   INPUT:   exe_charset_t  - Character set enum value                                        *
 *                                                                                             *
 *   RETURNS: Corresponding atc_charset_t enum value                                           *
 *                                                                                             *
 *==============================================================================================
 */
atc_charset_t map_exe_charset_to_atc_charset(exe_charset_t exe_charset)
{
    switch (exe_charset) {
    case EXE_CHARSET_UTF8:
        return ATC_CHARSET_UTF8;
    case EXE_CHARSET_HEX:
        return ATC_CHARSET_HEX;
    case EXE_CHARSET_GSM7:
        return ATC_CHARSET_GSM7;
    case EXE_CHARSET_GSM8:
        return ATC_CHARSET_GSM8;
    case EXE_CHARSET_UCS2:
        return ATC_CHARSET_UCS2;
    default:
        return ATC_CHARSET_UNDEF_BINARY;
    }
}
