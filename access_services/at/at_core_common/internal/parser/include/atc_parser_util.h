/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATC_PARSER_UTIL_H_
#define ATC_PARSER_UTIL_H_

#include "exe.h"
#include "atc_parser.h"

#define CC_EX_EMPTY 0xFF
#define SIZE_OF_UTC_TIME_STRING 19+1

/* To store the old NITZ time and old MONOTONIC time */
char old_nitz_time[SIZE_OF_UTC_TIME_STRING];
char old_monotonic_time[SIZE_OF_UTC_TIME_STRING];

AT_Command_e parser_handle_exe_result(exe_request_result_t exe_res,
                                      AT_ParserState_s *parser_p);

void parser_pending_queue_flush();
void parser_send_unsolicited(AT_ParserState_s *parser_p,
                             AT_Command_e override,
                             AT_CommandLine_t response);

bool non_dial_characters_successfully_removed(char *dial_string_p, exe_cmee_error_t *cme_err_p);

bool atc_enough_data_according_to_etzr_setting(exe_etzv_response_t *response_p, ETZR_format_t etzr_setting);
bool atc_get_monotonic_time_string(char *buffer_p, int buffer_size);
bool atc_get_utc_time_string(char *buffer_p, int buffer_size);
atc_charset_t map_exe_charset_to_atc_charset(exe_charset_t exe_charset);

#endif /* ATC_PARSER_UTIL_H_ */
