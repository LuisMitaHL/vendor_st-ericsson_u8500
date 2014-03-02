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
#include "exe_sms_client.h"

#include "whitebox.h"

#define CNMA_NACK "+CNMA=2"

#define CNMA_TEST_STRING_1 "+CNMA: (0-2)"
#define CMMS_TEST_STRING_1 "+CMMS: (0-2)"
#define SMS_PDU_1 "0001000d91445299034021f4000005cb309b5d06"
#define SMS_PDU_2 "000300F1"
#define CNMI_SET_SUPPORTED "+CNMI=2,2,2,1,0"
#define CNMI_SET_NOT_SUPPORTED "+CNMI=0,0,0,0,0"
#define CNMI_SET_CMT "+CNMI=2,2,2,1,0"
#define CNMI_SET_CMTI "+CNMI=2,1,2,1,0"
#define CNMI_SET_CDS "+CNMI=2,2,2,1,0"
#define CNMI_SET_CDSI "+CNMI=2,2,2,2,0"
#define CNMI_SET_CBM "+CNMI=2,2,2,2,0"
#define CSCB_SET "+CSCB=0, \"2,6- 9  , 30 - 40  \", \"2,3-7\""
#define CSCB_READ "+CSCB?"


#define CNMI_READ "+CNMI?"
#define CNMI_TEST "+CNMI=?"

void wb_clear_sms_subsription(void)
{
    exe_t *exe_p = (exe_t *)((uintptr_t)atc_get_exe_handle());
    smsclient_t *smsclient_p = (smsclient_t *)((uintptr_t)exe_get_smsclient(exe_p));
    smsclient_set_sms_subscription(smsclient_p, false);
}

void wb_testfcn_cnmi(char *at_string_p)
{
    char SET_CLEAN_UP[CLEAN_UP_BUFFER_SIZE];
    RESPONSE_BUFFER_LENGTH = 0;

    /* First read and save current value for further clean up */
    Parser_AT(CNMI_READ, strlen(CNMI_READ), AT_DATA_COMMAND, PARSER_p);

    if (WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"))) {

        /* Change "+CNMI: default_value" to "+CNMI= default value"
               * so it can be used as set command when clean-up */
        char *pch_p;
        strncpy(SET_CLEAN_UP, RESPONSE_BUFFER, CLEAN_UP_BUFFER_SIZE);
        pch_p = strchr(SET_CLEAN_UP, ':');
        *pch_p = '=';
        RESPONSE_BUFFER_LENGTH = 0;
        wb_clear_sms_subsription();
        /* Positive Test Cases */
        Parser_AT(CNMI_SET_SUPPORTED, strlen(CNMI_SET_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

        if (WB_CHECK(!leftovers(RESPONSE_BUFFER))) {

            /* Negative Test Cases */
            Parser_AT(CNMI_SET_NOT_SUPPORTED, strlen(CNMI_SET_NOT_SUPPORTED), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Now read previously set value */
            Parser_AT(CNMI_READ, strlen(CNMI_READ), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNMI: 2,2,2,1,0"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            /* Test Mode */
            Parser_AT(CNMI_TEST, strlen(CNMI_TEST), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNMI: (1,2),(0-3),(0,2),(0-2),(0)"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));
        }
    }

    /* Clean Up */
    Parser_AT(SET_CLEAN_UP, strlen(SET_CLEAN_UP), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cds(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    wb_clear_sms_subsription();
    Parser_AT(CNMI_SET_CDS, strlen(CNMI_SET_CDS), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sms_selector_callback_event(FD_SMS_EVENT, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CDS: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "3")); /* SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0791358508771010414243")); /* pdu */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cdsi(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    wb_clear_sms_subsription();
    Parser_AT(CNMI_SET_CDSI, strlen(CNMI_SET_CDSI), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    ATC_LOG_I("Before calling callback");
    EXECUTOR.sms_selector_callback_event(FD_SMS_EVENT, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CDSI: \"ME\",1")); /* CDSI: <mem>,<index> */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmt(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    wb_clear_sms_subsription();
    Parser_AT(CNMI_SET_CMT, strlen(CNMI_SET_CMT), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    ATC_LOG_I("Before calling callback");
    EXECUTOR.sms_selector_callback_event(FD_SMS_EVENT, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMT: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "3")); /* SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0791358508771010414243")); /* pdu */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmti(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    wb_clear_sms_subsription();
    Parser_AT(CNMI_SET_CMTI, strlen(CNMI_SET_CMTI), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    ATC_LOG_I("Before calling callback");
    EXECUTOR.sms_selector_callback_event(FD_SMS_EVENT, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMTI: \"ME\",2")); /* CMTI: <mem>,<index> */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cbm(char *at_string_p)
{
    char zeros[159];
    /* Fill array with zeros and finish with NULL termination */
    memset(zeros, '0', 158);
    zeros[158] = '\0';
    wb_clear_sms_subsription();
    Parser_AT(CNMI_SET_CBM, strlen(CNMI_SET_CBM), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sms_selector_callback_event(FD_SMS_EVENT, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CBM: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "9")); /* CBS length */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "C01000320F11414243")); /* pdu */
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cscb(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CSCB_SET, strlen(CSCB_SET), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CSCB_READ, strlen(CSCB_READ), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSCB: 0,\"40-50\",\"1-10\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgd(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgd_all(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgd_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGD: (0-5),(0-4)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgd_fail(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgd_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgf_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgf_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgf_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGF: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgf_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGF: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgs(char *at_string)
{
    struct AT_ResponseMessage_s AT_response_message;
    char *at_command = "+CMGS=19";

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command, strlen(at_command), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p = SMS_PDU_1;
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Need to set the context in transparent mode manually */
    CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGS: 3"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgs_failure(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgs_cms_error(char *at_string)
{
    struct AT_ResponseMessage_s AT_response_message;

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p
    = "07917283010010F5040BC87238880900F10000993092516195800AE8329BFD4697D9EC37";
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Need to set the context in transparent mode manually */
    CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 332"));
#else
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 304"));
#endif
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgs_read(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgs_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cmgw(char *at_string)
{
    struct AT_ResponseMessage_s AT_response_message;
    char *at_command = "+CMGW=19";

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command, strlen(at_command), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p = SMS_PDU_1;
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Need to set the context in transparent mode manually */
    CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGW: 3"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgw_failure(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_cmgw_failure_2 == CURRENT_TEST_CASE) {
        struct AT_ResponseMessage_s AT_response_message;
        AT_response_message.data_p = "";
        AT_response_message.exe_result_code = EXE_SUCCESS;
        AT_response_message.len = strlen(AT_response_message.data_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        /* Need to set the context in transparent mode manually */
        CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
        Parser_ParseCommandLine(PARSER_p, &AT_response_message);

        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    } else if (wb_testcase_cmgw_failure_3 == CURRENT_TEST_CASE) {
        struct AT_ResponseMessage_s AT_response_message;
        AT_response_message.data_p = "161"; /* character 'a' */
        AT_response_message.exe_result_code = EXE_SUCCESS;
        AT_response_message.len = strlen(AT_response_message.data_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        /* Need to set the context in transparent mode manually */
        CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
        Parser_ParseCommandLine(PARSER_p, &AT_response_message);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 304"));
    } else {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgw_read(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgw_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmms(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmms_failure(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 302"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmms_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CMMS_TEST_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmms_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMMS: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cnma_failure_1(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 302"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnma_set2(char *at_string_p)
{
    struct AT_ResponseMessage_s AT_response_message;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p = SMS_PDU_2;
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnma_read(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnma_set(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cnma_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CNMA_TEST_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cpms(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPMS: 10,15,10,15,10,15"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cpms_read(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPMS: \"SM\",10,15,\"SM\",10,15,\"SM\",10,15"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cpms_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPMS: (\"SM\",\"ME\"),(\"SM\",\"ME\"),(\"SM\",\"ME\")"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cpms_failure(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define CSMS_READ "+CSMS?"
void wb_testfcn_csms(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    switch (CURRENT_TEST_CASE) {

    case wb_testcase_csms_0:
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSMS: 1,1,1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        RESPONSE_BUFFER_LENGTH = 0;

        Parser_AT(CSMS_READ, strlen(CSMS_READ), AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSMS: 0,1,1,1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;

    case wb_testcase_csms_1:
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSMS: 1,1,1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        RESPONSE_BUFFER_LENGTH = 0;

        Parser_AT(CSMS_READ, strlen(CSMS_READ), AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSMS: 1,1,1,1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;

    case wb_testcase_csms_failure_1:
    case wb_testcase_csms_failure_2:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;

    case wb_testcase_csms_read_failure:
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSMS: 1,1,1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        RESPONSE_BUFFER_LENGTH = 0;

        Parser_AT(CSMS_READ, strlen(CSMS_READ), AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 500"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;

    case wb_testcase_csms_test:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSMS: (0-1)"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
        break;
    default:
        break;
    }
}

void wb_testfcn_esmsfull_set(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_esmsfull_set_0 == CURRENT_TEST_CASE) {
        /* respond to Request_SMS_DeliverReportSend*/
        EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CNMA_NACK, strlen(CNMA_NACK), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esmsfull_read(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_esmsfull_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!PARSER_p->IsPending);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESMSFULL: (0,1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgl_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: (0,1,2,3,4)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

static void wb_testfcn_cmgl_delete_all_pdus(void)
{
    char *at_command_delete_record_1 = "+CMGD=1";
    char *at_command_delete_record_2 = "+CMGD=2";
    char *at_command_delete_record_3 = "+CMGD=3";
    char *at_command_delete_record_4 = "+CMGD=4";

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_delete_record_1, strlen(at_command_delete_record_1), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_delete_record_2, strlen(at_command_delete_record_2), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_delete_record_3, strlen(at_command_delete_record_3), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_delete_record_4, strlen(at_command_delete_record_4), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define SMS_PDU_SENT   "07915892000000F001000B915892214365F7000021493A283D0795C3F33C88FE0601010101885EC6D341EDF27C1E3E97E72E"
#define SMS_PDU_UNSENT "07915892000000F001000B915892214365F7000021493A283D0795C3F33C88FE0602020202885EC6D341EDF27C1E3E97E72E"
#define SMS_PDU_READ   "0691640705809944068103030303F102041A082714040D050003120101A061391D1403"
#define SMS_PDU_UNREAD "0691640705809944068104040404F102041A082714040D050003120101A061391D1403"
static void wb_testfcn_cmgl_store_test_pdus(void)
{
    struct AT_ResponseMessage_s AT_response_message;
    char *at_command_write_sent_pdu = "+CMGW=42,3";
    char *at_command_write_unsent_pdu = "+CMGW=42,2";
    char *at_command_write_read_pdu = "+CMGW=28,1";
    char *at_command_write_unread_pdu = "+CMGW=28,0";

    /* Write some PDUs to storage. Assumes that the storage is currently empty. */
    /* "Sent" PDU */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_write_sent_pdu, strlen(at_command_write_sent_pdu), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p = SMS_PDU_SENT;
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    /* Need to set the context in transparent mode manually */
    CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGW: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* "Unsent" PDU */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_write_unsent_pdu, strlen(at_command_write_unsent_pdu), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p = SMS_PDU_UNSENT;
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    /* Need to set the context in transparent mode manually */
    CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGW: 2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* "Read" PDU */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_write_read_pdu, strlen(at_command_write_read_pdu), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p = SMS_PDU_READ;
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    /* Need to set the context in transparent mode manually */
    CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGW: 3"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* "Unread" PDU */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_write_unread_pdu, strlen(at_command_write_unread_pdu), AT_DATA_COMMAND, PARSER_p);
    AT_response_message.data_p = SMS_PDU_UNREAD;
    AT_response_message.exe_result_code = EXE_SUCCESS;
    AT_response_message.len = strlen(AT_response_message.data_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "> "));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    /* Need to set the context in transparent mode manually */
    CONTEXT_p->input_mode = ATC_INPUT_MODE_TRANSPARENT;
    Parser_ParseCommandLine(PARSER_p, &AT_response_message);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGW: 4"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgl(char *at_string_p)
{
    char *at_command_cmgl_unread  = "+CMGL=0";
    char *at_command_cmgl_read    = "+CMGL=1";
    char *at_command_cmgl_unsent  = "+CMGL=2";
    char *at_command_cmgl_sent    = "+CMGL=3";
    char *at_command_cmgl_all     = "+CMGL=4";
    char *at_command_cmgl_default = "+CMGL";

    /* Setup initial data */
    wb_testfcn_cmgl_store_test_pdus();

    /* List All (<stat> == 4) messages. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_all, strlen(at_command_cmgl_all), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Set Status */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "1,3,,42")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_SENT)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2,2,,42")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_UNSENT)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "3,1,,28")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_READ)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "4,0,,28")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_UNREAD)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* List All (<stat> == 4) messages. Any previously unread messages should now be read. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_all, strlen(at_command_cmgl_all), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "1,3,,42")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_SENT)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2,2,,42")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_UNSENT)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "3,1,,28")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_READ)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    /* PDU in record 4 was previously Unread. The status should now have changed to read. */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "4,1,,28")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_UNREAD)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Delete all PDUs from storage */
    wb_testfcn_cmgl_delete_all_pdus();

    /* Setup initial data */
    wb_testfcn_cmgl_store_test_pdus();

    /* List using default <stat>; i.e. unread messages. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_default, strlen(at_command_cmgl_default), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Set Status */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "4,0,,28")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_UNREAD)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* List using default <stat>; i.e. unread messages. There should be no messages found. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_default, strlen(at_command_cmgl_default), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* List unread (<stat> == 0) messages. There should be no messages found. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_unread, strlen(at_command_cmgl_unread), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* List read (<stat> == 1) messages. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_read, strlen(at_command_cmgl_read), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "3,1,,28")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_READ)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "4,1,,28")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_UNREAD)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* List unsent (<stat> == 2) messages. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_unsent, strlen(at_command_cmgl_unsent), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "2,2,,42")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_UNSENT)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* List sent (<stat> == 3) messages. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_sent, strlen(at_command_cmgl_sent), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Read */
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p); /* Request / Response SMS Find */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "1,3,,42")); /* index + status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, SMS_PDU_SENT)); // pdu
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Delete all PDUs from storage */
    wb_testfcn_cmgl_delete_all_pdus();

    /* List All (<stat> == 4) messages. None should be found. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_all, strlen(at_command_cmgl_all), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgl_failure(char *at_string)
{
    char *at_command_cmgl_invalid_stat    = "+CMGL=5";
    char *at_command_cmgl_too_many_params = "+CMGL=4,1";
    char *at_command_cmgl_invalid_param   = "+CMGL?";

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_invalid_stat, strlen(at_command_cmgl_invalid_stat), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_too_many_params, strlen(at_command_cmgl_too_many_params), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_command_cmgl_invalid_param, strlen(at_command_cmgl_invalid_param), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgr(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMGR: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0,,3")); /*status + SMS length excluding SMSC addr */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "0791358508771010414243")); /* pdu */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgr_failure(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmgr_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgsms_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgsms_set_fail(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgsms_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGSMS: 3"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgsms_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGSMS: (0,1,3)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmss_set(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMSS: 3"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmss_set_fail(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmss_set_new_recipient(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    EXECUTOR.sms_selector_callback_request(FD_SMS_REQUEST, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMSS: 3"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cmss_set_param_fail(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 303"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ciev_storage_status(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    wb_testfcn_cmer_set("+CMER=3,0,0,1,0");
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sms_selector_callback_event(FD_SMS_EVENT, EXECUTOR.smsclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CIEV: 10,1"));
}

