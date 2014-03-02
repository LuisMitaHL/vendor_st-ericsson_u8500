/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "atc_command_list.h"
#include "atc_parser.h"

#include "whitebox.h"

extern int state;

void wb_testfcn_cops_network_register(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_net_callback(MAL_NET_CS_CONTROL_RESP, NULL, MAL_NET_SUCCESS, EXECUTOR.client_tag_p);
    EXECUTOR.mal_net_callback(MAL_NET_MODEM_REG_STATUS_IND, NULL, MAL_NET_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
#endif
}

void wb_testfcn_cops_network_deregister(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND

    /* Make sure CME ERROR codes are enabled */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", strlen("+CMEE=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_net_callback(MAL_NET_CS_CONTROL_RESP, NULL, MAL_NET_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
#endif
}

void wb_testfcn_cops_set_format(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
#endif
}

void wb_testfcn_cops_get_settings(char *at_string_p)
{

    /* Check with long alphanumeric operator name */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+COPS=3,0", strlen("+COPS=3,0"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COPS: 1,0,\"LongOperatorNLongOperatorNLongOperatorNLongOperatorN123\",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}

#define NETWORK_LIST_LEN 4
#define NET_MCC_MNC_STRING_LEN 8 /* CN_MCC_MNC_STRING_LENGTH */
void wb_testfcn_cops_network_search(char *at_string_p)
{

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    for (state = 0; state < 2 && PARSER_p->IsPending; state++) {
        EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    }

    /* Verify values */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COPS: (1,\"LongOperatorN.01\",\"ShortN.1\",\"Net.1\",2),(0,\"LongOperatorN.00\",\"ShortN.0\",\"Net.0\",0),(3,\"LongOperatorN.03\",\"ShortN.3\",\"Net.3\",2),(1,\"LongOperatorN.02\",\"ShortN.2\",\"Net.2\",2),(2,\"LongOperatorN.02\",\"ShortN.2\",\"Net.2\",0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define EPSB_SET_SUPPORTED_1 "*EPSB=0"
#define EPSB_SET_SUPPORTED_2 "*EPSB=1"
#define EPSB_SET_NOT_SUPPORTED "*EPSB=2"
#define EPSB_READ "*EPSB?"
#define EPSB_TEST "*EPSB=?"
void wb_testfcn_epsb(char *at_string_p)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;
    (void)at_string_p;

    /* First read and save current value for further clean up */
    Parser_AT(EPSB_READ, strlen(EPSB_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "*EPSB: default_value" to "*EPSB= default value"
         * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, CLEAN_UP_BUFFER_SIZE);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;

        /* Positive Test Cases */
        Parser_AT(EPSB_SET_SUPPORTED_1, strlen(EPSB_SET_SUPPORTED_1), AT_DATA_COMMAND, PARSER_p);

        if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            Parser_AT(EPSB_SET_SUPPORTED_2, strlen(EPSB_SET_SUPPORTED_2), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

            if (WB_CHECK(!leftovers(RESPONSE_BUFFER))) {

                /* Negative Test Cases */
                Parser_AT(EPSB_SET_NOT_SUPPORTED, strlen(EPSB_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));

                /* Now read previously set value */
                Parser_AT(EPSB_READ, strlen(EPSB_READ), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPSB: 1"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));

                /* Test Mode */
                Parser_AT(EPSB_TEST, strlen(EPSB_TEST), AT_DATA_COMMAND, PARSER_p);
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
                WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPSB: (0-1)"));
                WB_CHECK(!leftovers(RESPONSE_BUFFER));
            }
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_user_activity(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_user_activity_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

   // EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_user_activity_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    //EXECUTOR.cn_selector_callback_request(FD_CN_REQUEST, NULL);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EHSTACT: (1,2)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
