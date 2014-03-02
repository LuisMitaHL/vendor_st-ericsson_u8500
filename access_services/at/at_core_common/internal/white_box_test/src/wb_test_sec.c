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

extern int state;

#define CLCK_TEST_STRING_1 "+CLCK: (\"SC\",\"AO\",\"OI\",\"OX\",\"AI\",\"IR\",\"AB\",\"AG\",\"AC\",\
\"PN\",\"PU\",\"PP\",\"PC\",\"FD\",\"EL\")"

void wb_testfcn_clck_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CLCK_TEST_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define CLCK_READ_STRING_1 "ERROR"
void wb_testfcn_clck_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CLCK_READ_STRING_1));
}

void wb_testfcn_clck_set_enable_neg(char *at_string_p) {

    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    for (state = 0; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    }
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_clck_query_neg(char *at_string_p) {
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    for (state = 0; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    }
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_clck_set_enable(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    for (state = 0; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    }
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_clck_set_disable(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    for (state = 0; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    }
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_clck_set_disable_pin(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 16"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_clck_set_disable_pin_fd(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    for (state = 0; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    }
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 17"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_clck_set_disable_puk(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 12"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_clck_set_disable_puk_fd(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    for (state = 0; state < 3 && PARSER_p->IsPending; state++) {
        EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);
    }
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 18"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

#define CLCK_SET_QUERY_STRING_1 "+CLCK: 2"
void wb_testfcn_clck_set_query(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.sim_selector_callback_request(FD_SIM_REQUEST, EXECUTOR.simclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, CLCK_SET_QUERY_STRING_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_clck_set_query_pn(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CLCK: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

#define CLCK_SET_QUERY_NEG_STRING_1 "ERROR"
void wb_testfcn_clck_set_query_neg(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CLCK_SET_QUERY_NEG_STRING_1));
}

void wb_testfcn_ecpschblkr_do(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    char *response = "DEADBEEF";
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, response));
}

void wb_testfcn_cops_rsa_authenticate(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    char *auth_init = "*ECPSAUTHU=1,0";
    char *auth_send_data1 = "*ECPSAUTHDU=\"DEAD\"";
    char *auth_send_data2 = "*ECPSAUTHDU=\"BEEF\"";
    char *auth_complete = "*ECPSAUTHU=2";

    Parser_AT(auth_init, strlen(auth_init), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    Parser_AT(auth_send_data1, strlen(auth_send_data1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    Parser_AT(auth_send_data2, strlen(auth_send_data2), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    Parser_AT(auth_complete, strlen(auth_complete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_simlock_authenticate(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    char *auth_init = "*ECPSAUTHU=1,1";
    char *auth_send_data = "*ECPSAUTHDU=,\"0B\",\"0E\",\"0E\",\"0E\",\"0F\"";
    char *auth_complete = "*ECPSAUTHU=2";

    Parser_AT(auth_init, strlen(auth_init), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    Parser_AT(auth_send_data, strlen(auth_send_data), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    Parser_AT(auth_complete, strlen(auth_complete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_auth_fail(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
}

void wb_testfcn_cops_auth(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_imei(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_imei_fail(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
}

void wb_testfcn_cops_simlock(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_simlock_fail(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
}

void wb_testfcn_cops_simlock_data(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    char *simlock_init = "*ECPSSIMLOCK=1";
    char *simlock_send_data = "*ECPSSIMLOCKD=\"00010203040506070809\"";
    char *simlock_send_data2 = "*ECPSSIMLOCKD=\"0A0B0C0D0E0F\"";
    char *simlock_init_send = "*ECPSSIMLOCK=2";

    Parser_AT(simlock_init, strlen(simlock_init), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    Parser_AT(simlock_send_data, strlen(simlock_send_data), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    Parser_AT(simlock_send_data2, strlen(simlock_send_data2), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    Parser_AT(simlock_init_send, strlen(simlock_init_send), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_simlock_data_odd(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    char *simlock_init = "*ECPSSIMLOCK=1";
    char *simlock_send_data = "*ECPSSIMLOCKD=\"0102F\"";
    char *simlock_init_send = "*ECPSSIMLOCK=2";

    Parser_AT(simlock_init, strlen(simlock_init), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    Parser_AT(simlock_send_data, strlen(simlock_send_data), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    Parser_AT(simlock_init_send, strlen(simlock_init_send), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_simlock_data_read(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    char *response = "0A0B0C0D0E0F";
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, response));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
}

void wb_testfcn_cops_simlock_data_fail(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
}
