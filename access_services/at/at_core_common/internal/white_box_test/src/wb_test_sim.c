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

#define ESIMSR_TEST_STRING "*ESIMSR: (0,1)"
#define EPEE_TEST_STRING "*EPEE: (0,1)"
#define CPWD_TEST_STRING "+CPWD: (\"SC\",8),(\"P2\",8),(\"AO\",8),(\"OI\",8),(\"OX\",8),(\"AI\",8),(\"IR\",8),(\"AB\",8),(\"AG\",8),(\"AC\",8)"
#define CPIN_TEST_STRING_1 "+CPIN: (READY,SIM PIN,SIM PUK,PH-SIM PIN,SIM PIN2,SIM PUK2,PH-NET PIN,PH-NETSUB PIN,PH-SP PIN,PH-CORP PIN,PH-SIMLOCK PIN,PH-ESL PIN,BLOCKED)"
#define ECEXPIN_TEST_STRING_1 "*ECEXPIN: (\"READY\",\"SIM PIN\",\"SIM PUK\",\"PH-SIM PIN\",\"SIM PIN2\",\"SIM PUK2\",\"PH-NET PIN\",\"PH-NETSUB PIN\",\"PH-SP PIN\",\"PH-CORP PIN\",\"PH-SIMLOCK PIN\",\"PH-ESL PIN\",\"BLOCKED\")"

/* The white box test will send unsolicited events on all channels as the logic for checking
 * if the channel is active is part of code not included in the module tests
 */
#define NUMBER_OF_CHANNELS 4

void wb_testfcn_cpin_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CPIN_TEST_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define CPIN_READ_STRING_1 "+CPIN: READY"
void wb_testfcn_cpin_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CPIN_READ_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cpin_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cpin_set_wrong_pin(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 11"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_ecexpin_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    /* Same <code>s are supported as for CPIN testcommand */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ECEXPIN_TEST_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ecexpin_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define ECEXPIN_QUERY_STATE_STRING_1 "*ECEXPIN: READY"
void wb_testfcn_ecexpin_query_state(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ECEXPIN_QUERY_STATE_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ecexpin_set_puk(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ecexpin_set_incorrect_param_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ecexpin_set_incorrect_param_2(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_ecexpin_set_incorrect_param_3(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_csca_set_smsc(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_csca_set_smsc_failure(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 27"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csca_get_smsc(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_csca_get_smsc_1: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSCA: \"084545455\",129"));
        break;
    }
    case wb_testcase_csca_get_smsc_2: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSCA: \"+468994307\",145"));
        break;
    }
    case wb_testcase_csca_get_smsc_3: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSCA: \"+12345678\",145"));
        break;
    }
    case wb_testcase_csca_get_smsc_4: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSCA: \"+442587365\",145"));
        break;
    }
    case wb_testcase_csca_get_smsc_5: {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CSCA: \"084943799\",129"));
        break;
    }
    default: {
        WB_CHECK(0);
        break;
    }
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_csca_get_smsc_failure(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csca_test_is_supported(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define CRSM_GET_RESPONSE_READ_STRING_1 ": 144,0" /* "AA 03" is specified in wb_stub_sim.c*/
void wb_testfcn_crsm_read_get_resp(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_RESPONSE_READ_STRING_1));
}

#define CRSM_GET_RESPONSE_READ_STRING_2 "+CME ERROR: 50"
void wb_testfcn_crsm_read_get_resp_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_RESPONSE_READ_STRING_2));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_READ_FILE_BIN_STRING_1 ": 144,0" /* "AA 03" is specified in wb_stub_sim.c*/
void wb_testfcn_crsm_read_file_bin(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_READ_FILE_BIN_STRING_1));
}

#define CRSM_GET_READ_FILE_BIN_STRING_2 "+CME ERROR: 50"
void wb_testfcn_crsm_read_file_bin_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_READ_FILE_BIN_STRING_2));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_READ_FILE_RECORD_STRING_1 ": 144,0" /* "AA 03" is specified in wb_stub_sim.c*/
void wb_testfcn_crsm_read_file_rec(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_READ_FILE_RECORD_STRING_1));
}

#define CRSM_GET_READ_FILE_RECORD_STRING_2 "+CME ERROR: 50"
void wb_testfcn_crsm_read_file_rec_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_READ_FILE_RECORD_STRING_2));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_UPDATE_REC_STRING_1 ": 144,0"
void wb_testfcn_crsm_update_rec(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_UPDATE_REC_STRING_1));
}

#define CRSM_GET_UPDATE_REC_STRING_2 "+CME ERROR: 24"
void wb_testfcn_crsm_update_rec_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_UPDATE_REC_STRING_2));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_UPDATE_REC_STRING_3 "+CME ERROR: 50"
void wb_testfcn_crsm_update_rec_1_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_UPDATE_REC_STRING_3));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_UPDATE_BIN_STRING_1 ": 144,0"
void wb_testfcn_crsm_update_bin(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_UPDATE_BIN_STRING_1));
}

#define CRSM_GET_UPDATE_BIN_STRING_2 "+CME ERROR: 24"
void wb_testfcn_crsm_update_bin_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_UPDATE_BIN_STRING_2));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_UPDATE_BIN_STRING_3 "+CME ERROR: 50"
void wb_testfcn_crsm_update_bin_1_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_UPDATE_BIN_STRING_3));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_READ_STAUS_STRING_1 ": 144,0"
void wb_testfcn_crsm_read_status(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_READ_STAUS_STRING_1));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CRSM_GET_READ_STAUS_STRING_2 "+CME ERROR: 50"
void wb_testfcn_crsm_read_status_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CRSM_GET_READ_STAUS_STRING_2));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_crsm_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cuad_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUAD: \"31323334323433343234\""));
}

void wb_testfcn_cuad_do_2(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cuad_do_3(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUAD: \"31323334323433343234\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUAD: \"31323334323433343234\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CUAD: \"31323334323433343234\""));
}

void wb_testfcn_cuad_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define ESIMSR_SET_0 "*ESIMSR=0"
#define ESIMSR_SET_1 "*ESIMSR=1"
#define ESIMSR_READ "*ESIMSR?"

void wb_testfcn_esimsr_set_read(char *at_string_p)
{
    /* Enable SIM State Reporting */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(ESIMSR_SET_1, strlen(ESIMSR_SET_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(ESIMSR_READ, strlen(ESIMSR_READ), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);

    if (wb_testcase_esimsr_0 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMSR: 1,7"));
    } else if (wb_testcase_esimsr_1 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMSR: 1,5"));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_esimsr_unsol(char *at_string_p)
{
    /* Enable SIM State Reporting */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(ESIMSR_SET_1, strlen(ESIMSR_SET_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Unsolicited */
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);

    if (wb_testcase_esimsr_2 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMSR: 7"));
    } else if (wb_testcase_esimsr_3 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMSR: 0"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esimsr_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ESIMSR_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define EPEE_SET_0 "*EPEE=0"
#define EPEE_SET_1 "*EPEE=1"
#define EPEE_READ "*EPEE?"

void wb_testfcn_epee_set_read(char *at_string_p)
{
    /* Enable SIM State Reporting */
    RESPONSE_BUFFER_LENGTH = 0;

    if (wb_testcase_epee_0 == CURRENT_TEST_CASE) {
        Parser_AT(EPEE_SET_0, strlen(EPEE_SET_0), AT_DATA_COMMAND, PARSER_p);
    } else if (wb_testcase_epee_1 == CURRENT_TEST_CASE) {
        Parser_AT(EPEE_SET_1, strlen(EPEE_SET_1), AT_DATA_COMMAND, PARSER_p);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(EPEE_READ, strlen(EPEE_READ), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_epee_0 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPEE: 0"));
    } else if (wb_testcase_epee_1 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPEE: 1"));
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_epee_unsol(char *at_string_p)
{
    Parser_AT(ESIMSR_SET_0, strlen(ESIMSR_SET_0), AT_DATA_COMMAND, PARSER_p);

    /* Enable SIM State Reporting */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(EPEE_SET_1, strlen(EPEE_SET_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Unsolicited */
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_EVENT, EXECUTOR.simclient_p);

    if (wb_testcase_epee_2 == CURRENT_TEST_CASE) {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPEV"));
    }

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_epee_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, EPEE_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


/* CPWD tests */

/* Test CPWD with old and new password */
void wb_testfcn_cpwd_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;

    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}

/* Test CPWD with old password. Shall return error as old and new password must be supplied. */
void wb_testfcn_cpwd_set_one(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: incorrect parameters"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    wb_testfcn_cmee_configure("+CMEE=0");
}

/* Test CPWD with new password. Shall return error as old and new password must be supplied. */
void wb_testfcn_cpwd_set_two(char *at_string_p)
{
    /* Enable SIM State Reporting */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}

void wb_testfcn_cpwd_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CPWD_TEST_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_clck_read_not_sup(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define CIMI_TEST_STRING_1 "OK"
void wb_testfcn_cimi_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CIMI_TEST_STRING_1));
}

#define CIMI_DO_STRING_1 "240991234567891"
void wb_testfcn_cimi_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CIMI_DO_STRING_1));
}

#define EPINR_TEST_STRING_1 "*EPINR: (1-4)"
void wb_testfcn_epinr_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, EPINR_TEST_STRING_1));
}

#define EPINR_SET_STRING_1 ": 3"
void wb_testfcn_epinr_set_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, EPINR_SET_STRING_1));
}

#define EPINR_SET_STRING_2 "ERROR"
void wb_testfcn_epinr_set_2(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, EPINR_SET_STRING_2));
}

#define STKE_RESPONSE_STRING "*STKE: 30303030"
void wb_testfcn_stke_send(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, STKE_RESPONSE_STRING));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_stki_unsol(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->stkc_pc_enable = EXE_STKC_PC_ENABLE;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*STKI: 31313131"));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_stkn_unsol(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->stkc_pc_enable = EXE_STKC_PC_ENABLE;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*STKN: 32323232"));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_stkend_unsol(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->stkc_pc_enable = EXE_STKC_PC_ENABLE;
    RESPONSE_BUFFER[0] = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*STKEND"));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cnum_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNUM: \"\",\"112\",129"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CNUM: \"\",\"911\",129"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}

void wb_testfcn_cnum_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_write(char *at_string_p)
{
    /* Test with different combinations of legal strings */
    /* Define test values */
    char *teststrings[] = {"+CPOL=0,2,\"90210\",1,1,1",  /* legal, index 0 */
                           "+CPOL=1,2,\"90210\",1,1,1",  /* legal, index 1 */
                           "+CPOL=,2,\"90210\",0,0,0",   /* write to first free */
                           "+CPOL=4",     /*  remove item on index 4*/
                           "+CPOL=6,2",     /*  remove item on index 6*/
                           "+CPOL=8,2,,1,1,1",     /*  remove item on index 8*/
                           "+CPOL=1,2,\"021011\",1,1,1", /*legal, MNC+MMC 6 chars*/
                           "+CPOL=1,,\"90210\",1,0,1", /* Missing parameter for format, which defaults to 2 (valid)*/
                           "+CPOL=,2", /* Command to change format, ok as long as it is to "2" */
                          };
    int i = 0;

    for (i = 0; i < sizeof(teststrings) / sizeof(char *); i++) {
        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(teststrings[i], strlen(teststrings[i]), AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
    }

    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_write_illegal(char *at_string_p)
{
    /* Test with different combinations of illegal strings and combinations of strings */
    /* Define test values */
    char *teststrings[] = {"+CPOL=1,1,\"90210\", 1,1,1",  /* legal, but unsupported format */
                           "+CPOL=1,2,\"90210\", 1,2,1",   /* Illegal value on GSM_compact_act */
                           "+CPOL=1,2,\"90210\", 1,2",     /* Missing value for UTRAN_AcT*/
                           "+CPOL=1,2,\"90a10\", 1,0,1",   /* Illegal oper for format = 2*/
                           "+CPOL=1,2,\"9021011\", 1,1,1", /*String too long for format =2 */
                           "+CPOL=a,2,\"90210\", 1,1,1", /*Illegal index */
                           "+CPOL=1,2,\"9021\", 1,1,1", /*oper string too short*/
                           "+CPOL=1,2,\"90210\"",          /*AcT params must be present if oper is */
                           "+CPOL=a", /* Illegal index, nothing else set */
                           "+CPOL=500", /* Index > AT's definition of int (0xff) */
                           "+CPOL=500,2", /* invalid index, valid format*/
                           "+CPOL=", /*No parameters is not allowed either */
                           "+CPOL=1,2,\"90210\",1,1,1,0"  /* too many parameters */
                          };
    int i = 0;

    for (i = 0; i < sizeof(teststrings) / sizeof(char *); i++) {
        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(teststrings[i], strlen(teststrings[i]), AT_DATA_COMMAND, PARSER_p);
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
    }

    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_write_rejected(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    /* Do a proper write, but that is rejected by the sim interface. Callback method is set to
     * return parameters from sim stub to emulate a missing pwd case, i.e. code 16.  */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 16"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");

}

void wb_testfcn_plmn_write_retry(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_remove_element(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_read_two(char *at_string_p)
{

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPOL: 0,2,\"24012\",1,0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPOL: 2,2,\"789543\",0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_read_none(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPOL: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_read_failed(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_read_retry(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPOL: 0,2,\"24012\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_read_retry_failed(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CPOL: (0-9),(2)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_plmn_test_failed(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_csas_test(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "CSAS: (0-7)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_test_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "CSAS: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_test_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 313"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_test_3(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "CSAS: (0-252)"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_do(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_set_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_set_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 313"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_set_3(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 310"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_csas_set_4(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 313"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_test(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "CRES: (0-7)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_test_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "CRES: (0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_test_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 313"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_test_3(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "CRES: (0-252)"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_do(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_set_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_set_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 313"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_set_3(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 310"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cres_set_4(char *at_string_p)
{
    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CMS ERROR: 313"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}


#define CCHC_TEST_STRING_1 "OK"
void wb_testfcn_cchc_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CCHC_TEST_STRING_1));
}

#define CCHC_SET_STRING_1 "+CME ERROR: incorrect parameters"
void wb_testfcn_cchc_set_1(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CCHC_SET_STRING_1));

    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CCHC_SET_STRING_2 "OK"
void wb_testfcn_cchc_set_2(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CCHC_SET_STRING_2));

    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CCHO_TEST_STRING_1 "OK"
void wb_testfcn_ccho_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CCHO_TEST_STRING_1));
}

#define CCHO_SET_STRING_1 "+CME ERROR: unknown"
void wb_testfcn_ccho_set_1(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CCHO_SET_STRING_1));

    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CCHO_SET_STRING_2 ": 1"
void wb_testfcn_ccho_set_2(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CCHO_SET_STRING_2));

    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CGLA_TEST_STRING_1 "OK"
void wb_testfcn_cgla_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CGLA_TEST_STRING_1));
}

#define CGLA_SET_STRING_1 "+CME ERROR: incorrect parameters"
void wb_testfcn_cgla_set_1(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CGLA_SET_STRING_1));

    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CGLA_SET_STRING_2 "+CGLA: 8,\"B4B5B6B7\""
void wb_testfcn_cgla_set_2(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CGLA_SET_STRING_2));

    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CSIM_RESPONSE_STRING_1 "OK"
void wb_testfcn_csim_test(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CSIM_RESPONSE_STRING_1));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CSIM_RESPONSE_STRING_2 "+CME ERROR: 50"
void wb_testfcn_csim_set_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CSIM_RESPONSE_STRING_2));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CSIM_RESPONSE_STRING_3 "+CSIM: 8,\"B4B5B6B7\""
void wb_testfcn_csim_set_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CSIM_RESPONSE_STRING_3));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CSIM_RESPONSE_STRING_4 "+CME ERROR: 50"
void wb_testfcn_csim_set_3(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CSIM_RESPONSE_STRING_4));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CSIM_RESPONSE_STRING_5 "+CSIM: 8,\"B4B5B6B7\""
void wb_testfcn_csim_set_4(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER_LENGTH);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CSIM_RESPONSE_STRING_5));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_esimrf_test(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMRF: (0,1)"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_esimrf_read(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMRF: 0"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_esimrf_set_1(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_esimrf_set_2(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_esimrf_unsol_1(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMRF: 0,0"));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esimrf_unsol_2(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ""));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_esimrf_unsol_3(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ""));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_esimrf_unsol_4(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMRF: 0,3"));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esimrf_unsol_5(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, ""));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esimrf_unsol_6(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMRF: 0,1,12258"));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esimrf_unsol_7(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMRF: 0,1,12258,\"FEDCBA\""));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_esimrf_unsol_8(char *at_string_p)
{
    /* Unsolicited */
    int i = 0;
    PARSER_p->ESIMRF = 1;
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ESIMRF: 0,1,12258,\"FEDCBA1234\""));
    clean_buffer(RESPONSE_BUFFER);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}

#define EFSIM_TEST_STRING_1 "OK"
void wb_testfcn_efsim_test(char *at_string_p)
{
    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, EFSIM_TEST_STRING_1));
}

#define EFSIM_READ_STRING "<?xml version=\"1.0\"?>"
void wb_testfcn_efsim_read(char *at_string_p)
{
    wb_testfcn_cmee_configure("+CMEE=2");

    RESPONSE_BUFFER[0] = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(0 == RESPONSE_BUFFER[0]);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, EFSIM_READ_STRING));

    wb_testfcn_cmee_configure("+CMEE=0");
}
