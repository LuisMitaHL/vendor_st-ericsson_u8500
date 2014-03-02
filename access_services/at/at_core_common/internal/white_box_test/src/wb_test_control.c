/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "atc_command_table.h"
#include "whitebox.h"

#define CMEC_READ_STRING "+CMEC: 2,0,0"
#define CMEC_TEST_STRING "+CMEC: (2),(0),(0)"

#define CMER_SET_ENABLE    "+CMER=3,0,0,1,0"
#define CMER_READ_ENABLED  "+CMER: 3,0,0,1,0"
#define CMER_SET_DISABLE   "+CMER=0,0,0,0,0"
#define CMER_READ_DISABLED "+CMER: 0,0,0,0,0"
#define CMER_TEST_STRING   "+CMER: (0,3),(0),(0),(0,1),(0)"

#define CMER_CIEV_TEST_STRING_VALUE_0   "+CIEV: 2,0"
#define CMER_CIEV_TEST_STRING_VALUE_1   "+CIEV: 2,1"
#define CMER_CIEV_TEST_STRING_VALUE_2   "+CIEV: 2,2"
#define CMER_CIEV_TEST_STRING_VALUE_3   "+CIEV: 2,3"
#define CMER_CIEV_TEST_STRING_VALUE_4   "+CIEV: 2,4"
#define CMER_CIEV_TEST_STRING_VALUE_5   "+CIEV: 2,5"

#define ERFSTATE_SET_ENABLE                 "*ERFSTATE=1"
#define ERFSTATE_READ_ENABLED               "*ERFSTATE: 1"
#define ERFSTATE_SET_DISABLE                "*ERFSTATE=0"
#define ERFSTATE_READ_DISABLED              "*ERFSTATE: 0"
#define ERFSTATE_TEST_STRING                "*ERFSTATE: (0-1)"

#define ERFSTATE_UNSOLICITED_OFF            "*ERFSTATE: 0"
#define ERFSTATE_UNSOLICITED_ON             "*ERFSTATE: 1"

/* Test functions for ME CONTROL */
#define AND_C_SET_SUPPORTED "&C1"
#define AND_C_SET_NOT_SUPPORTED "&C2"
#define AND_C_SET_NOT_SUPPORTED2 "&C=1"
#define AND_C_READ "&C?"
#define AND_C_TEST "&C=?"
void wb_testfcn_and_c(char *at_string_p)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;
    (void)at_string_p;

    /* First read and save current value for further clean up */
    Parser_AT(AND_C_READ, strlen(AND_C_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "&C: default_value" to "&C= default value"
         * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, RESPONSE_BUFFER_LENGTH + 1);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;

        /* Positive Test Cases */
        Parser_AT(AND_C_SET_SUPPORTED, strlen(AND_C_SET_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

        if (WB_CHECK(!leftovers(RESPONSE_BUFFER))) {

            /* Negative Test Cases */
            Parser_AT(AND_C_SET_NOT_SUPPORTED, strlen(AND_C_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Negative Test Cases */
            Parser_AT(AND_C_SET_NOT_SUPPORTED2, strlen(AND_C_SET_NOT_SUPPORTED2), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Now read previously set value */
            Parser_AT(AND_C_READ, strlen(AND_C_READ), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "&C: 1"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Test Mode */
            Parser_AT(AND_C_TEST, strlen(AND_C_TEST), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "&C: (0-1)"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define AND_D_SET_SUPPORTED "&D1"
#define AND_D_SET_NOT_SUPPORTED "&D3"
#define AND_D_READ "&D?"
#define AND_D_TEST "&D=?"
void wb_testfcn_and_d(char *at_string_p)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;
    (void)at_string_p;

    /* First read and save current value for further clean up */
    Parser_AT(AND_D_READ, strlen(AND_D_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "&D: default_value" to "&D= default value"
         * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, RESPONSE_BUFFER_LENGTH + 1);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;

        /* Positive Test Cases */
        Parser_AT(AND_D_SET_SUPPORTED, strlen(AND_D_SET_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

        if (WB_CHECK(!leftovers(RESPONSE_BUFFER))) {

            /* Negative Test Cases */
            Parser_AT(AND_D_SET_NOT_SUPPORTED, strlen(AND_D_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Now read previously set value */
            Parser_AT(AND_D_READ, strlen(AND_D_READ), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "&D: 1"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Test Mode */
            Parser_AT(AND_D_TEST, strlen(AND_D_TEST), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "&D: (0-2)"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmec(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_cmec_failure(char *at_string_p)
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

void wb_testfcn_cmec_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMEC_READ_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_cmec_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMEC_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmer_set(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmer_set_neg(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);

    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));  /* CMEE_INCORRECT_PARAMETERS */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Disable CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cmer_read(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_ENABLE, strlen(CMER_SET_ENABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_READ_ENABLED));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));


    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_DISABLE, strlen(CMER_SET_DISABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_READ_DISABLED));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmer_test(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmer_unsol_ciev_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_ENABLE, strlen(CMER_SET_ENABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_CIEV_TEST_STRING_VALUE_0));
}

void wb_testfcn_cmer_unsol_ciev_2(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_ENABLE, strlen(CMER_SET_ENABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_CIEV_TEST_STRING_VALUE_1));
}

void wb_testfcn_cmer_unsol_ciev_3(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_ENABLE, strlen(CMER_SET_ENABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_CIEV_TEST_STRING_VALUE_2));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmer_unsol_ciev_4(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_ENABLE, strlen(CMER_SET_ENABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_CIEV_TEST_STRING_VALUE_3));
}

void wb_testfcn_cmer_unsol_ciev_5(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_ENABLE, strlen(CMER_SET_ENABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_CIEV_TEST_STRING_VALUE_4));
}

void wb_testfcn_cmer_unsol_ciev_6(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_ENABLE, strlen(CMER_SET_ENABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMER_CIEV_TEST_STRING_VALUE_5));
}

void wb_testfcn_cmer_unsol_ciev_disabled(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CMER_SET_DISABLE, strlen(CMER_SET_DISABLE), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    WB_CHECK(!check_for_string(RESPONSE_BUFFER, "+CIEV: "));
}

void wb_testfcn_cgmr_do(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", strlen("+CMEE=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (CURRENT_TEST_CASE == wb_testcase_cgmr_fail || CURRENT_TEST_CASE == wb_testcase_gmr_fail) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "\"WhiteBoxSystem\n\""));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgmr_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgmi_do(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", strlen("+CMEE=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (CURRENT_TEST_CASE == wb_testcase_cgmi_fail) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "\"WhiteBoxManufacturer\n\n\""));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgmi_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgmm_do(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", strlen("+CMEE=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (CURRENT_TEST_CASE == wb_testcase_cgmm_fail || CURRENT_TEST_CASE == wb_testcase_gmm_fail) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "\"WhiteBoxModel\""));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgmm_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define CRC_SET_SUPPORTED_1 "+CRC=0"
#define CRC_SET_SUPPORTED_2 "+CRC=1"
#define CRC_SET_NOT_SUPPORTED "+CRC=2"
#define CRC_READ "+CRC?"
#define CRC_TEST "+CRC=?"
void wb_testfcn_crc(char *at_string_p)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;

    /* First read and save current value for further clean up */
    Parser_AT(CRC_READ, strlen(CRC_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "+CRC: default_value" to "+CRC= default value"
         * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, RESPONSE_BUFFER_LENGTH + 1);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;

        /* Positive Test Cases */
        Parser_AT(CRC_SET_SUPPORTED_1, strlen(CRC_SET_SUPPORTED_1), AT_DATA_COMMAND, PARSER_p);

        if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            Parser_AT(CRC_SET_SUPPORTED_2, strlen(CRC_SET_SUPPORTED_2), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

            if (WB_CHECK(!leftovers(RESPONSE_BUFFER))) {

                /* Negative Test Cases */
                Parser_AT(CRC_SET_NOT_SUPPORTED, strlen(CRC_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));

                /* Now read previously set value */
                Parser_AT(CRC_READ, strlen(CRC_READ), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CRC: 1"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));

                /* Test Mode */
                Parser_AT(CRC_TEST, strlen(CRC_TEST), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CRC: (0-1)"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));
            }
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* Test functions for AT CONTROL */
void wb_testfcn_star(char *at_string_p)
{
    int i;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (i = 0; i < AT_COMMAND_TABLE_LENGTH; i++) {
        if (!(AT_CommandTable[i].Class & AT_CLASS_DEBUG) && (AT_CommandTable[i].Mode != AT_MODE_NONE)) {
            WB_CHECK(check_for_string(RESPONSE_BUFFER, AT_CommandTable[i].String));
        }
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_star_neg(char *at_string_p)
{
    int i;
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_star_test(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_star_test_neg(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_f(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_f_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_f_test(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "&F: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ok(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_error(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_e_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "1")); /* was just set to default */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_e_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "E: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_e(char *at_string_p) /* default 1 */
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_q_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "Q: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_q_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "Q: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_v0(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_v_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "V: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_num_error(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_v_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "V: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_z_do_neg(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_z_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_z_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "Z: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* Test functions for ME CONTROL */
void wb_testfcn_cmee_set_0(char *at_string_p)
{
    char *failing_at_command_p = "+CMEC=1,1,1";
    char *read_cmee_command_p = "+CMEE?";

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(read_cmee_command_p, strlen(read_cmee_command_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMEE: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(failing_at_command_p, strlen(failing_at_command_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmee_set_1(char *at_string_p)
{
    char *failing_at_command_p = "+CMEC=1,1,1";

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(failing_at_command_p, strlen(failing_at_command_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cmee_set_2(char *at_string_p)
{
    char *failing_at_command_p = "+CMEC=1,1,1";

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(failing_at_command_p, strlen(failing_at_command_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: operation not supported"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cmee_failure(char *at_string_p)
{
    char *read_cmee_command_p = "+CMEE?";

    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(read_cmee_command_p, strlen(read_cmee_command_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMEE: 2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cmee_read(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMEE: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmee_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMEE: (0-2)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmee_configure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* *ERFSTATE */
void wb_testfcn_erfstate_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    if (wb_testcase_erfstate_error == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_erfstate_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    if (wb_testcase_erfstate_read_enabled == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ERFSTATE_READ_ENABLED));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ERFSTATE_READ_DISABLED));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_erfstate_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ERFSTATE_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_erfstate_unsolicited(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (PARSER_p->IsPending) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_SIGNAL_INFO */

    if (wb_testcase_erfstate_unsolicited_on == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ERFSTATE_UNSOLICITED_ON));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, ERFSTATE_UNSOLICITED_OFF));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/* Test functions for S0 */
void wb_testfcn_s0(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s0_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Enable automatic answer after '7' rings */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S0=7", strlen("S0=7"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S0?", strlen("S0?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset automatic answer to disabled (no args)*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S0=", strlen("S0="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S0?", strlen("S0?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s0_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s0_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s0_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S0: (0-0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* Test functions for S3 */
void wb_testfcn_s3(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s3_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set newline character to 'B' */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S3=66", strlen("S3=66"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S3?", strlen("S3?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "66"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset new line character to <CR> (no args) */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S3=", strlen("S3="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S3?", strlen("S3?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "13"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s3_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s3_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "13"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s3_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S3: (0-127)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/* Test functions for S4 */
void wb_testfcn_s4(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s4_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set response formatting character to 'B' */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S4=66", strlen("S4=66"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S4?", strlen("S4?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "66"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset response formatting character to <LF> (no args) */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S4=", strlen("S4="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S4?", strlen("S4?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "10"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_s4_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s4_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "10"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s4_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S4: (0-127)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/* Test functions for S5 */
void wb_testfcn_s5(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s5_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set command line editing character to 'B' */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S5=66", strlen("S5=66"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S5?", strlen("S5?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "66"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset command line editing character to BS (no args) */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S5=", strlen("S5="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S5?", strlen("S5?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "8"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s5_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s5_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "8"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s5_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S5: (0-127)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/* Test functions for S6 */
void wb_testfcn_s6_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s6_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S6: (2-10)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s6_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s6_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set pause before blind dialing to 10 secs */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S6=10", strlen("S6=10"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S6?", strlen("S6?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "10"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset pause before blind dialing to 2 secs (no args) */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S6=", strlen("S6="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S6?", strlen("S6?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/* Test functions for S7 */
void wb_testfcn_s7_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "50"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s7_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S7: (1-255)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s7_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s7_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set connection completion timeout to 255 secs */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S7=255", strlen("S7=255"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S7?", strlen("S7?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "255"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset connection completion timeout to 50 secs (no args) */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S7=", strlen("S7="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S7?", strlen("S7?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "50"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* Test functions for S8 */
void wb_testfcn_s8_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s8_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S8: (0-255)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s8_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set comma dial modifier time to 255 secs */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S8=255", strlen("S8=255"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S8?", strlen("S8?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "255"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset comma dial modifier time to 2 secs (no args) */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S8=", strlen("S8="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S8?", strlen("S8?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s8_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* Test functions for S10 */
void wb_testfcn_s10_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s10_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "S10: (1-254)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s10_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_s10_noargs(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set automatic disconnect delay control to 254 (1/10)'s secs */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S10=254", strlen("S10=254"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S10?", strlen("S10?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "254"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Reset automatic disconnect delay control 2 (1/10)'s secs (no args) */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S10=", strlen("S10="), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("S10?", strlen("S10?"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define X_SET_SUPPORTED_DEFAULT "X"
#define X_SET_SUPPORTED_1 "X=0"
#define X_SET_SUPPORTED_2 "X=4"
#define X_SET_NOT_SUPPORTED "X=5"
#define X_READ "X?"
#define X_TEST "X=?"
void wb_testfcn_x(char *at_string_p)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;
    (void)at_string_p;

    /* First read and save current value for further clean up */
    Parser_AT(X_READ, strlen(X_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "X: default_value" to "X= default value"
         * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, RESPONSE_BUFFER_LENGTH + 1);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;

        /* Positive Test Cases */

        /* set, using default value */
        Parser_AT(X_SET_SUPPORTED_DEFAULT, strlen(X_SET_SUPPORTED_DEFAULT), AT_DATA_COMMAND, PARSER_p);

        if (!WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            return;
        }

        Parser_AT(X_READ, strlen(X_READ), AT_DATA_COMMAND, PARSER_p);

        if (!WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            return;
        }

        /* Default value (4) should be returned */
        if (!WB_CHECK(check_for_string(RESPONSE_BUFFER, "X: 4"))) {
            return;
        }

        Parser_AT(X_SET_SUPPORTED_1, strlen(X_SET_SUPPORTED_1), AT_DATA_COMMAND, PARSER_p);

        if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            Parser_AT(X_SET_SUPPORTED_2, strlen(X_SET_SUPPORTED_2), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

            if (WB_CHECK(!leftovers(RESPONSE_BUFFER))) {

                /* Negative Test Cases */
                Parser_AT(X_SET_NOT_SUPPORTED, strlen(X_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));

                /* Now read previously set value */
                Parser_AT(X_READ, strlen(X_READ), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "X: 4"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));

                /* Test Mode */
                Parser_AT(X_TEST, strlen(X_TEST), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "X: (0-4)"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));
            }
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cscs_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cscs_set_noparameters(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");
    RESPONSE_BUFFER_LENGTH = 0;

    if (wb_testcase_cscs_set_noparameters == CURRENT_TEST_CASE) {
        PARSER_p->cscs_charset = ATC_CHARSET_MAX;
        Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

        if (CHARACTER_SET_DEFAULT != PARSER_p->cscs_charset) {
            WB_CHECK(false);
            PARSER_p->cscs_charset = CHARACTER_SET_DEFAULT;
        }

        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        return;
    }

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    switch (CURRENT_TEST_CASE) {

    case wb_testcase_cscs_short_failure:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;

    case wb_testcase_cscs_short_escape:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;

    case wb_testcase_cscs_short_empty:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        /* Make sure CME ERROR codes are disabled. */
        wb_testfcn_cmee_configure("+CMEE=0");
        break;

    case wb_testcase_cscs_short_onequote:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;

    default:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;
    }

    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cscs_set_neg(char *at_string_p)
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

#define CSCS_READ_UTF8_STRING "+CSCS: \"UTF-8\""
#define CSCS_READ_HEX_STRING "+CSCS: \"HEX\""

void wb_testfcn_cscs_read(char *at_string_p) /* AT+CSCS? */
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_cscs_read_hex == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, CSCS_READ_HEX_STRING));
    }

    if (wb_testcase_cscs_read_utf_8 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, CSCS_READ_UTF8_STRING));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#ifdef AT_PLUS_CSCS_CHARSET_HEX_ENABLED
#define CSCS_TEST_STRING "+CSCS: (\"UTF-8\",\"HEX\")"
#else
#define CSCS_TEST_STRING "+CSCS: (\"UTF-8\")"
#endif


void wb_testfcn_cscs_test(char *at_string_p) /* AT+CSCS=? */
{

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CSCS_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define ECAM_SET_SUPPORTED_1 "*ECAM=0"
#define ECAM_SET_SUPPORTED_2 "*ECAM=1"
#define ECAM_READ "*ECAM?"
#define ECAM_TEST "*ECAM=?"
void wb_testfcn_ecam(char *at_string_p)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;
    (void)at_string_p;

    /* First read and save current value for further clean up */
    Parser_AT(ECAM_READ, strlen(ECAM_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "*ECAM: default_value" to "*ECAM= default value"
         * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, RESPONSE_BUFFER_LENGTH + 1);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;

        /* Positive Test Cases */
        Parser_AT(ECAM_SET_SUPPORTED_1, strlen(ECAM_SET_SUPPORTED_1), AT_DATA_COMMAND, PARSER_p);

        if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            Parser_AT(ECAM_SET_SUPPORTED_2, strlen(ECAM_SET_SUPPORTED_2), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

            if (WB_CHECK(!leftovers(RESPONSE_BUFFER))) {

                /* Now read previously set value */
                Parser_AT(ECAM_READ, strlen(ECAM_READ), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECAM: 1"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));

                /* Test Mode */
                Parser_AT(ECAM_TEST, strlen(ECAM_TEST), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ECAM: (0-1)"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));
            }
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define CUUS1_READ_STRING "+CUUS1: 0,1,1,\"7E02ABBA\""
#define CUUS1_TEST_STRING "+CUUS1: (0), (0-1), (0-1), , (0-1)"
#define CUUSI_CHECK_STRING "+CUUS1U: 1,\"7E02ABBA\""

void wb_testfcn_cuus1_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_cuus1_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT("+CUUS1=0,1,1,\"7E02ABBA\"", strlen("+CUUS1=0,1,1,\"7E02ABBA\""), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CUUS1_READ_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}
void wb_testfcn_cuus1_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CUUS1_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_cuus1_neg(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (CURRENT_TEST_CASE == wb_testcase_cuus1_neg_0) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));

    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cuus1_unsol(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.cn_selector_callback_event(FD_CN_EVENT, NULL); /* CN_EVENT_CALL_STATE_CHANGED */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CRING: VOICE"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CUUSI_CHECK_STRING));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
