/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _POSIX_SOURCE
/* Done to avoid redefinition of struct timeval and struct timespec when including ste_adm_client.h */
#define _POSIX_SOURCE
#endif
#include "ste_adm_client.h"
#include "whitebox.h"

void wb_testfcn_cmut_set(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmut_set_neg(char *at_string)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CMUT_READ_STRING "+CMUT: 1"
void wb_testfcn_cmut_read(char *at_string)
{
    wb_testfcn_cmut_set("+CMUT=1");
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMUT_READ_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define CMUT_TEST_STRING "+CMUT: (0,1)"
void wb_testfcn_cmut_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMUT_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#if 0
void wb_testfcn_etty_set(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER[0]);

    EXECUTOR.mal_call_callback(EXE_ETTY_SET, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
#endif

void wb_testfcn_etty_set_neg(char *at_string)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_etty_set_4 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define ETTY_READ_STRING "*ETTY: 1"
void wb_testfcn_etty_read(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ETTY_READ_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define ETTY_TEST_STRING "*ETTY: (0-3)"
void wb_testfcn_etty_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ETTY_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

