#if 0 /* replaced by Call&networking functions (migration needed) */
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "atc_command_list.h"
#include "atc_parser.h"

#include "whitebox.h"


static bool wb_testfcn_cusd_hex_charset(char **at_string_pp);

static void wb_testfcn_cscstest_cusd_send_ussd_helper(char *at_string_p, bool expecting_good_conversion);

static bool wb_testfcn_cusd_hex_charset(char **at_string_pp)
{
    bool return_value = false;

    if (wb_testcase_is_cusd_hex_charset == CURRENT_TEST_CASE) {
        return_value = true;

    }

    return return_value;
}

const char *cusd_ussd_string_hex_p = "757373645f737472";  /* Input data pattern encoded in hexadecimal notation. */
const char *cusd_ussd_string_ascii_p = "ussd_str";  /* Input data pattern encoded in ASCII notation. */

void wb_testfcn_cscstest_cusd_send_ussd_failure(char *at_string_p)
{
    wb_testfcn_cscstest_cusd_send_ussd_helper(at_string_p, false);
}

void wb_testfcn_cscstest_cusd_send_ussd(char *at_string_p)
{
    wb_testfcn_cscstest_cusd_send_ussd_helper(at_string_p, true);
}

static void wb_testfcn_cscstest_cusd_send_ussd_helper(char *at_string_p, bool expecting_good_conversion)
{
#ifdef EXE_USE_MAL_BACKEND
    int repetition_count = 0;
    const char *cusd_ussd_ptr = cusd_ussd_string_ascii_p;
    char *cusd_textbuffer_ptr;
    char *fcnname;

    RESPONSE_BUFFER_LENGTH = 0;

    if (wb_testfcn_cusd_hex_charset(&at_string_p)) {
        cusd_ussd_ptr = cusd_ussd_string_ascii_p;
    }

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(MAL_SS_ON_USSD, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);

    /*
     * Check the ussd_str that was left by the EXE
     */
    cusd_textbuffer_ptr = EXECUTOR.wb_test_ussd_str;

    while (check_for_string(cusd_textbuffer_ptr, cusd_ussd_ptr)) {
        repetition_count++;
    }

    if (expecting_good_conversion) {
        WB_CHECK(0 < repetition_count);
        WB_CHECK(!leftovers(cusd_textbuffer_ptr));

    } else {
        WB_CHECK(0 == repetition_count);
    }

#endif
}


void wb_testfcn_cusd_send_ussd(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(MAL_SS_ON_USSD, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);

    /* now check the response buffer */

    if (strchr(at_string_p, ',')) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD"));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#endif
}


void wb_testfcn_cusd_send_ussd_failure(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(MAL_SS_ON_USSD, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);

    /* now check the response buffer */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#endif
}

void wb_testfcn_cusd_cancel_ussd(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(MAL_SS_ON_USSD, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
#endif
}

void wb_testfcn_cusd_get_settings(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
#endif
}

void wb_testfcn_cusd_get_supported_n(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: (0-2)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
#endif
}

void wb_testfcn_cusd_unsolicited(char *at_string_p)
{
#ifdef EXE_USE_MAL_BACKEND
    mal_ss_ril_ussd_info response = { 5, "#*99*#" };
    RESPONSE_BUFFER_LENGTH = 0;
    /* Enable +CUSD result code presentation */
    Parser_AT("+CUSD=1", strlen("+CUSD=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.mal_ss_callback(MAL_SS_ON_USSD, &response, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUSD: 5,#*99*#"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
#endif
}

void wb_testfcn_ccwa_set_1(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_ccwa_set3 == CURRENT_TEST_CASE) {
        EXECUTOR.mal_call_callback(NULL, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccwa_set_neg(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_ccwa_set_query(char *at_string_p)
{
    uint32_t    response[2];
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    response[0] = 1; /* enabled */
    response[1] = 5; /* classinfo (status) */

    EXECUTOR.mal_call_callback(NULL, &response, MAL_SUCCESS, EXECUTOR.client_tag_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 1,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 0,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 1,4"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 0,16"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 0,32"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 0,64"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 0,128"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccwa_read(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccwa_test(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCWA: (0,1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccfc_set_1(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_ss_callback(EXE_CCFC, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccfc_set_2(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);

    /* Setup CME so we get error codes */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", 7, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Test */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_ss_callback(EXE_CCFC, NULL, MAL_NOT_SUPPORTED, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccfc_set_query(char *at_string_p)
{
    /* Setting up the faked query response from MAL */
    size_t at_string_length = strlen(at_string_p);

    /* Test with content in MAL response */
    {
        mal_ss_call_fwd_info *response_p  = (mal_ss_call_fwd_info *)calloc(1, sizeof(mal_ss_call_fwd_info));
        mal_ss_call_forward_info *fwrd_infos_p = (mal_ss_call_forward_info *)calloc(3, sizeof(mal_ss_call_forward_info));

        response_p->num = 3;
        response_p->call_fwd_info = fwrd_infos_p;

        response_p->call_fwd_info[0].status       = EXE_CCFC_MODE_ENABLE;
        response_p->call_fwd_info[0].reason       = EXE_CCFC_REASON_MOBILE_BUSY;
        response_p->call_fwd_info[0].serviceClass = EXE_CLASSX_VOICE;
        response_p->call_fwd_info[0].toa          = 145;
        response_p->call_fwd_info[0].number = (char *)calloc(1, 28);
        sprintf(response_p->call_fwd_info[0].number, "+01555123123");
        response_p->call_fwd_info[0].timeSeconds  = EXE_CCFC_TIME_DEFAULT + 10;

        response_p->call_fwd_info[1].status       = EXE_CCFC_STATUS_ENABLED;
        response_p->call_fwd_info[1].reason       = EXE_CCFC_REASON_NO_REPLY;
        response_p->call_fwd_info[1].serviceClass = EXE_CLASSX_DATA;
        response_p->call_fwd_info[1].toa          = 129;
        response_p->call_fwd_info[1].number = (char *)calloc(1, 28);
        sprintf(response_p->call_fwd_info[1].number, "555234234");
        response_p->call_fwd_info[1].timeSeconds  = EXE_CCFC_TIME_DEFAULT - 5;

        response_p->call_fwd_info[2].status       = EXE_CCFC_STATUS_DISABLED;
        response_p->call_fwd_info[2].reason       = EXE_CCFC_REASON_UNCONDITIONAL;
        response_p->call_fwd_info[2].serviceClass = EXE_CLASSX_FAX;
        response_p->call_fwd_info[2].toa          = 161;
        response_p->call_fwd_info[2].number       = NULL;
        response_p->call_fwd_info[2].timeSeconds  = EXE_CCFC_TIME_DEFAULT;

        /* Parsing AT command */

        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.mal_ss_callback(EXE_CCFC, response_p, MAL_SUCCESS, EXECUTOR.client_tag_p);

        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 1,1,\"+01555123123\",145"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 1,2,\"555234234\",129,,,15"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0,4"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        free(response_p->call_fwd_info[0].number);
        free(response_p->call_fwd_info[1].number);
        free(response_p);
        free(fwrd_infos_p);
    }

    /* Test with no classes in MAL response */
    {
        mal_ss_call_fwd_info *response_p  = (mal_ss_call_fwd_info *)calloc(1, sizeof(mal_ss_call_fwd_info));

        response_p->num = 0;
        response_p->call_fwd_info = NULL;

        /* Parsing AT command */

        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.mal_ss_callback(EXE_CCFC, response_p, MAL_SUCCESS, EXECUTOR.client_tag_p);

        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: 0"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

        free(response_p);
    }
}

void wb_testfcn_ccfc_set_register(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);

    /* Setup CME so we get error codes */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", 7, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* TEST MAL_SUCCESS*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_ss_callback(EXE_CCFC, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* TEST MAL_NOT_SUPPORTED*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_ss_callback(EXE_CCFC, NULL, MAL_NOT_SUPPORTED, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* TEST MAL_FAILED*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_ss_callback(EXE_CCFC, NULL, MAL_FAIL, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 100"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Stop CMEEs */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=0", 7, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ccfc_test(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CCFC: (0-5)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cnap_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cnap_set_0:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_cnap_set_1:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_cnap_set_2: /* Invalid value */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnap_read(char *at_string_p)
{
    int clip_read = 0;
    char cnap_resp[20];

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    /* CLIP requires the same provisioning information as CNAP so we re-use CLIPs call */
    EXECUTOR.mal_call_callback(EXE_CLIP_READ, &clip_read, MAL_SUCCESS, EXECUTOR.client_tag_p);

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cnap_read_0:
        sprintf(cnap_resp, "+CNAP: 0,%d", clip_read);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, cnap_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    case wb_testcase_cnap_read_1:
        sprintf(cnap_resp, "+CNAP: 1,%d", clip_read);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, cnap_resp));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        break;
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnap_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNAP: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnap_unsolicited(char *at_string_p)
{
    int entry;
    AT_ParserState_s *parser_p;
    mal_call_context call_context;
    memset(&call_context, 0, sizeof(call_context));
    call_context.name = "No Name";
    call_context.namePresentation = 1;

    /* Turn off +CLIP */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);

        if (parser_p->CLIP) {
            parser_p->CLIP = 0;
        }

        if (parser_p->ECAM) {
            parser_p->ECAM = 0;
        }
    }

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.mal_call_callback(MAL_CALL_RING, &call_context, MAL_SUCCESS, 0);

    /* Drain all "RING"s, one for each parser */
    for (entry = 0; entry < MAX_NO_OF_OPEN_CHANNELS; entry++) {
        parser_p = ParserStateTable_GetParserState_ByEntry(entry);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "RING"));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNAP: \"No Name\",0"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_colr_do(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLR: 2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_colr_test(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define COLP_READ           "+COLP?"
#define COLP_SET_NO_VAL     "+COLP="
#define COLP_SET_0          "+COLP=0"
#define COLP_SET_1          "+COLP=1"
void wb_testfcn_colp(char *at_string_p)
{
    /* SET DISABLE */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_SET_0, strlen(COLP_SET_0), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* READ  */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_READ, strlen(COLP_READ), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLP: 0,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* SET NO VAL */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_SET_NO_VAL, strlen(COLP_SET_NO_VAL), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* READ  */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(COLP_READ, strlen(COLP_READ), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLP: 1,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_colp_test(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+COLP: (0,1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


#define CSSN_SET_SUPPORTED "+CSSN=1,1"
#define CSSN_SET_NOT_SUPPORTED "+CSSN=1,2"
#define CSSN_READ "+CSSN?"
#define CSSN_TEST "+CSSN=?"
void wb_testfcn_cssn(void)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;

    /* First read and save current value for further clean up */
    Parser_AT(CSSN_READ, strlen(CSSN_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "+CSSN: default_value" to "+CSSN= default value"
         * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, CLEAN_UP_BUFFER_SIZE);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;

        /* Positive Test Cases */
        Parser_AT(CSSN_SET_SUPPORTED, strlen(CSSN_SET_SUPPORTED), AT_DATA_COMMAND, PARSER_p);

        if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Negative Test Cases */
            Parser_AT(CSSN_SET_NOT_SUPPORTED, strlen(CSSN_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Now read previously set value */
            Parser_AT(CSSN_READ, strlen(CSSN_READ), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSSN: 1,1"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Test Mode */
            Parser_AT(CSSN_TEST, strlen(CSSN_TEST), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSSN: (0,1),(0,1)"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cpwd_call_barring(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);

    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(0, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_clck_call_barring_lock(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;

    wb_testfcn_cmee_configure("+CMEE=2");

    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(0, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_clck_call_barring_unlock(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    RESPONSE_BUFFER_LENGTH = 0;

    wb_testfcn_cmee_configure("+CMEE=2");

    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(0, NULL, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_clck_call_barring_query(char *at_string_p)
{
    size_t at_string_length = strlen(at_string_p);
    mal_ss_service_op_class service_op_class;

    /* Setup response from MAL */
    service_op_class.service_class = 7;
    service_op_class.service_op = EXE_CLCK_STATUS_ACTIVE;

    /* Do the QUERY */
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.mal_call_callback(0, &service_op_class, MAL_SUCCESS, EXECUTOR.client_tag_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 1,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 1,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 1,4"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 0,8"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 0,16"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 0,64"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 0,128"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}

#endif /* #if 0 */
