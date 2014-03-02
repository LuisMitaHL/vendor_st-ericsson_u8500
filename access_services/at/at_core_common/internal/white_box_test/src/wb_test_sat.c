/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "whitebox.h"
#include "atc_command_list.h"
#include "atc_parser.h"

/* AT*STKC Test Functions */
void wb_testfcn_stkc_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_stkc_set_neg(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_stkc_set_exe_fail(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_stkc_set_3_params_neg(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


#define CSCS_READ_STRING "*STKC: 0,ABBA"
void wb_testfcn_stkc_read_zero(char *at_string_p) /* AT*STKC? */
{
    wb_testfcn_stkc_set("*STKC=0");
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CSCS_READ_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define STKC_READ_STRING_1 "*STKC: 1,ABBA"
void wb_testfcn_stkc_read_one(char *at_string_p) /* AT*STKC? */
{
    wb_testfcn_stkc_set("*STKC=1");
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, STKC_READ_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define STKC_READ_STRING_2 "*STKC: (0,1)"
void wb_testfcn_stkc_test(char *at_string_p) /* AT*STKC=? */
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, STKC_READ_STRING_2));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_stkr_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_stkr_set_param_error == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    } else if (wb_testcase_stkr_set_executer_error == CURRENT_TEST_CASE) {
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    } else { /* wb_testcase_stkr_set_ok */
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_stkr_test(char *at_string)  /* AT*STKR=? */
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eshlvocr_answer(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_eshlvocr_answer_wrong(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=1");
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    wb_testfcn_cmee_configure("+CMEE=0");
}
