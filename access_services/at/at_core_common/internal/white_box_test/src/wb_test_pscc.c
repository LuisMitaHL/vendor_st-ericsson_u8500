/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pscc_msg.h"
#include "sterc_msg.h"
#include "whitebox.h"
#include "atc_log.h"
#include "wb_test_pscc.h"

void send_qos_response(int *connid_p);
static void connect_and_disconnect_A();

char *wb_pscc_msg_id_string(int id)
{
    char *string_p;

    switch (id) {
    case pscc_create:
        string_p = "pscc_create";
        break;
    case pscc_destroy:
        string_p = "pscc_destroy";
        break;
    case pscc_connect:
        string_p = "pscc_connect";
        break;
    case pscc_disconnect:
        string_p = "pscc_disconnect";
        break;
    case pscc_set:
        string_p = "pscc_set";
        break;
    case pscc_get:
        string_p = "pscc_get";
        break;
    case pscc_list_all_connections:
        string_p = "pscc_list_all_connections";
        break;
    case pscc_modify:
        string_p = "pscc_modify";
        break;
    case pscc_generate_uplink_data:
        string_p = "pscc_generate_uplink_data";
        break;
    case pscc_init_handler:
        string_p = "pscc_init_handler";
        break;
    case pscc_event_connecting:
        string_p = "pscc_event_connecting";
        break;
    case pscc_event_connected:
        string_p = "pscc_event_connected";
        break;
    case pscc_event_disconnecting:
        string_p = "pscc_event_disconnecting";
        break;
    case pscc_event_disconnected:
        string_p = "pscc_event_disconnected";
        break;
    case pscc_event_failure:
        string_p = "pscc_event_failure";
        break;
    case pscc_event_modified:
        string_p = "pscc_event_modified";
        break;
    case pscc_event_data_counters_fetched:
        string_p = "pscc_event_data_counters_fetched";
        break;
    case pscc_event_data_counters_reset:
        string_p = "pscc_event_data_counters_reset";
        break;
    default:
        string_p = "<ERROR string not found, update wb_pscc_msg_id_string>";
        break;
    }

    return string_p;
}

char *wb_sterc_msg_id_string(int id)
{
    char *string_p;

    switch (id) {
    case sterc_create:
        string_p = "sterc_create";
        break;
    case sterc_destroy:
        string_p = "sterc_destroy";
        break;
    case sterc_get:
        string_p = "sterc_get";
        break;
    case sterc_event_disconnected:
        string_p = "sterc_event_disconnected";
        break;
    default:
        string_p = "<ERROR string not found, update wb_sterc_msg_id_string>";
        break;
    }

    return string_p;
}

bool wb_pscc_init_message(mpl_msg_type_t pscc_msg_type, int msg_id)
{
    EXECUTOR.pscc_param_list_p = NULL;
    EXECUTOR.pscc_msg_type = pscc_msg_type;
    bool result = false;

    switch (pscc_msg_type) {
    case mpl_msg_type_resp:

        if (EXECUTOR.pscc_request_id != msg_id) {
            printf("ERROR: wb_pscc_init_message, tried to respond to %s when old request is pending: %s\n", wb_pscc_msg_id_string(msg_id), wb_pscc_msg_id_string(EXECUTOR.pscc_request_id));
        } else if (-1 != EXECUTOR.pscc_event_id) {
            printf("ERROR: wb_pscc_init_message, tried to respond to %s when old event is pending: %s\n" , wb_pscc_msg_id_string(msg_id), wb_pscc_msg_id_string(EXECUTOR.pscc_event_id));
        } else {
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
            EXECUTOR.pscc_request_id = -1; /* No request pending */
            result = true;
        }

        break;
    case mpl_msg_type_event:

        if (-1 != EXECUTOR.pscc_event_id) {
            printf("ERROR: wb_pscc_init_message, tried to send event %s when old event is pending: %s\n" , wb_pscc_msg_id_string(msg_id), wb_pscc_msg_id_string(EXECUTOR.pscc_event_id));
        } else if (-1 != EXECUTOR.pscc_request_id) {
            printf("ERROR: wb_pscc_init_message, tried to send event %s when old request is pending: %s\n" , wb_pscc_msg_id_string(msg_id), wb_pscc_msg_id_string(EXECUTOR.pscc_request_id));
        } else {
            EXECUTOR.pscc_event_id = msg_id;
            result = true;
        }

        break;
    default:
        printf("ERROR: wb_pscc_init_message, unexpected pscc_msg_type %d\n", pscc_msg_type);
        break;
    }

    return result;
}

bool wb_sterc_init_message(mpl_msg_type_t sterc_msg_type, int msg_id)
{
    EXECUTOR.sterc_param_list_p = NULL;
    EXECUTOR.sterc_msg_type = sterc_msg_type;
    bool result = false;

    switch (sterc_msg_type) {
    case mpl_msg_type_resp:

        if (EXECUTOR.sterc_request_id != msg_id) {
            printf("ERROR: wb_sterc_init_message, tried to respond to %s when old request is pending: %s\n", wb_sterc_msg_id_string(msg_id), wb_sterc_msg_id_string(EXECUTOR.sterc_request_id));
        } else if (-1 != EXECUTOR.sterc_event_id) {
            printf("ERROR: wb_sterc_init_message, tried to respond to %s when old event is pending: %s\n" , wb_sterc_msg_id_string(msg_id), wb_sterc_msg_id_string(EXECUTOR.sterc_event_id));
        } else {
            mpl_add_param_to_list(&EXECUTOR.sterc_param_list_p, sterc_paramid_ct, &EXECUTOR.client_tag_p);
            EXECUTOR.sterc_request_id = -1; /* No request pending */
            result = true;
        }

        break;
    case mpl_msg_type_event:
        if (-1 != EXECUTOR.sterc_event_id) {
            printf("ERROR: wb_sterc_init_message, tried to send event %s when old event is pending: %s\n" , wb_sterc_msg_id_string(msg_id), wb_sterc_msg_id_string(EXECUTOR.sterc_event_id));
        } else if (-1 != EXECUTOR.pscc_request_id) {
            printf("ERROR: wb_sterc_init_message, tried to send event %s when old request is pending: %s\n" , wb_sterc_msg_id_string(msg_id), wb_sterc_msg_id_string(EXECUTOR.sterc_request_id));
        } else {
            EXECUTOR.sterc_event_id = msg_id;
            result = true;
        }
        break;
    default:
        printf("ERROR: wb_sterc_init_message, unexpected sterc_msg_type %d\n", sterc_msg_type);
        break;
    }

    return result;
}

bool wb_pscc_respond_with_just_client_tag(int msg_id)
{
    bool result = wb_pscc_init_message(mpl_msg_type_resp, msg_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    return result;
}

static int conn_id = 7;
static int conn_id_b = 9;
static int conn_id_c = 10;
static int conn_id_d = 16;
static int conn_id_e = 4;
static int conn_id_f = 5;
static int conn_id_g = 6;
static int pdp_type = pscc_pdp_type_ipv4;
static int pdp_type_ipv4v6 = pscc_pdp_type_ipv4v6;
static int status_connected = pscc_connection_status_connected;
static int status_disconnected = pscc_connection_status_disconnected;
static int cause = 42;

void wb_testfcn_pscc(char *at_string)
{
    /* test EPPSD */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_a_test, strlen(eppsd_a_test), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* CGACT? (read), when no accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: 1,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* CGDCONT? (read), when no accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_read, strlen(cgdcont_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: 1,\"IP\",\"\",\"\",0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account F */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_f_create, strlen(cgdcont_f_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_a ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_read, strlen(cgdcont_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_a_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_b_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_f_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account A*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_a_on, strlen(eppsd_a_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: <?xml version=\"1.0\"?>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ip_address>192.168.0.3</ip_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<subnet_mask>255.255.255.255</subnet_mask>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<mtu>0</mtu>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<default_gateway>192.168.0.1</default_gateway>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.8</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.7</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<activation_cause>0</activation_cause>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "</connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that account A is active and account B is inactive */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_a ",1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* list params of defined contexts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_read, strlen(cgdcont_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    int hcmp = 1;
    int dcmp = 0;
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_apn, "apnA.se");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "123.0.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_hcmp, &hcmp);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dcmp, &dcmp);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: " cid_a ",\"IP\",\"apnA.se\",\"123.0.0.1\",0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: " cid_b ",\"IPV6\",\"apn.8.se\",\"\",0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_f_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* CGEV Network initiated disconnect of account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgerep, strlen(cgerep), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "123.0.0.1");
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CGEV_CHECK_STRING));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_a ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account A again */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_a_on, strlen(eppsd_a_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: <?xml version=\"1.0\"?>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ip_address>192.168.0.3</ip_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<subnet_mask>255.255.255.255</subnet_mask>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<mtu>0</mtu>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<default_gateway>192.168.0.1</default_gateway>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.8</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<activation_cause>0</activation_cause>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.7</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "</connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* manual disconnect */
    Parser_AT(eppsd_a_off, strlen(eppsd_a_off), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_a ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete the accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_delete, strlen(cgdcont_b_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_f_delete, strlen(cgdcont_f_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}


void wb_testfcn_pscc_ipv4v6(char *at_string)
{

    /* test EPPSD */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_a_test, strlen(eppsd_a_test), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* CGACT? (read), when no accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: 1,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* CGDCONT? (read), when no accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_read, strlen(cgdcont_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: 1,\"IP\",\"\",\"\",0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account E */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_e_create, strlen(cgdcont_e_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account F */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_f_create, strlen(cgdcont_f_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account G */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_g_create, strlen(cgdcont_g_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_e ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_g ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_read, strlen(cgdcont_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_e_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_f_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_g_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account E*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_e_on, strlen(eppsd_a_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: <?xml version=\"1.0\"?>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ip_address>192.168.0.3</ip_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<subnet_mask>255.255.255.255</subnet_mask>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<mtu>0</mtu>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<default_gateway>192.168.0.1</default_gateway>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.8</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.7</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<activation_cause>0</activation_cause>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "</connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that account E is active and account F and G are inactive */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_e ",1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_g ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* list params of defined contexts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_read, strlen(cgdcont_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    int hcmp = 1;
    int dcmp = 0;
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_apn, "apnA.se");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "123.0.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_hcmp, &hcmp);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dcmp, &dcmp);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: " cid_e ",\"IPV4V6\",\"apnA.se\",\"123.0.0.1\",0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: " cid_f ",\"IPV4V6\",\"apn.17.se\",\"\",0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_g_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* CGEV Network initiated disconnect of account E */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgerep, strlen(cgerep), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "123.0.0.1");
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CGEV_CHECK_STRING_IPV4V6));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have three inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_e ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_g ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account E again */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_e_on, strlen(eppsd_e_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: <?xml version=\"1.0\"?>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ip_address>192.168.0.3</ip_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<subnet_mask>255.255.255.255</subnet_mask>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<mtu>0</mtu>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<default_gateway>192.168.0.1</default_gateway>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.8</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.7</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<activation_cause>0</activation_cause>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "</connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* manual disconnect of E */
    Parser_AT(eppsd_e_off, strlen(eppsd_e_off), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_e);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have three inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_e ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_f ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_g ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account F */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_f_on, strlen(eppsd_f_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ipv6_dns_address, "2001:cdba::3257:9653");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ipv6_secondary_dns_address, "2001:cdba::3257:9654");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ipv6_address, "2001:cdba::3257:9652");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: <?xml version=\"1.0\"?>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ip_address>2001:cdba::3257:9652</ip_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<subnet_mask>0</subnet_mask>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<mtu>0</mtu>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<default_gateway>192.168.0.1</default_gateway>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>2001:cdba::3257:9653</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>2001:cdba::3257:9654</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<activation_cause>0</activation_cause>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "</connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account G */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_g_on, strlen(eppsd_g_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ipv6_dns_address, "2001:cdba::3257:9653");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ipv6_secondary_dns_address, "2001:cdba::3257:9654");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ipv6_address, "2001:cdba::3257:9652");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs1");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: <?xml version=\"1.0\"?>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ip_address>192.168.0.3</ip_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ipv6_address>2001:cdba::3257:9652</ipv6_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<subnet_mask>255.255.255.255</subnet_mask>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<mtu>0</mtu>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<default_gateway>192.168.0.1</default_gateway>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.8</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.7</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ipv6_dns_server>2001:cdba::3257:9653</ipv6_dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ipv6_dns_server>2001:cdba::3257:9654</ipv6_dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<activation_cause>0</activation_cause>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "</connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that account E is inactive and account F and G are active */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_e ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_g ",1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* list params of defined contexts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_read, strlen(cgdcont_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_apn, "apnA.se");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ipv6_address, "2001:cdba::3257:9652");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_hcmp, &hcmp);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dcmp, &dcmp);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_apn, "apn.17.se");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ipv6_address, "2001:cdba::3257:9652");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_hcmp, &hcmp);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dcmp, &dcmp);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: " cid_f ",\"IPV4V6\",\"apnA.se\",\"2001:cdba::3257:9652\",0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: " cid_g ",\"IPV4V6\",\"apn.17.se\",\"192.168.0.1\",\"2001:cdba::3257:9652\",0,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgdcont_e_read));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Check output of CGPADDR= */
	RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(CGPADDR_ALL, strlen(CGPADDR_ALL), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ipv6_address, "2001:cdba::3257:9652");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ipv6_address, "2001:cdba::3257:9652");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_e ",\"\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_f ",\"2001:cdba::3257:9652\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_g ",\"192.168.0.1\"" ",\"2001:cdba::3257:9652\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* CGEV Network initiated disconnect of account G */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgerep, strlen(cgerep), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type_ipv4v6);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "123.0.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ipv6_address, "2001:cdba::3257:9652");
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_g);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CGEV_CHECK_STRING_IPV4V6_1));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, CGEV_CHECK_STRING_IPV4V6_2));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that account E and G are inactive and account F is active */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_f);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_e ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_f ",1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_g ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete the accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_e_delete, strlen(cgdcont_e_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_f_delete, strlen(cgdcont_f_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_g_delete, strlen(cgdcont_g_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

}

void wb_testfcn_cgact_set_3gpp(char *at_string)
{
    /* Enable CGEREP for the unsolicited event... */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgerep, strlen(cgerep), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_a ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account A*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_a_on, strlen(cgact_a_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    if (wb_testcase_cgact_set_3gpp == CURRENT_TEST_CASE || wb_testcase_cgact_set_3gpp_nw_disconnect == CURRENT_TEST_CASE) {
        /* Successful connect */

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        /* check that account A is active and account B is inactive */
        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_a ",1"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_b ",0"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        if (wb_testcase_cgact_set_3gpp_nw_disconnect != CURRENT_TEST_CASE) {
            /* manual disconnect */
            int fail_cause = 34;
            RESPONSE_BUFFER_LENGTH = 0;
            Parser_AT(cgact_a_off, strlen(cgact_a_off), AT_DATA_COMMAND, PARSER_p);

            WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
            EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
            WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
            WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
            EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

            WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_cause, &fail_cause);
            EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

            WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
            EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));

            RESPONSE_BUFFER_LENGTH = 0;
            Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CEER: Deactivate Cause: SM34 (Service option temporarily out of order)"));
            WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));
        } else {
            /* network disconnect */

            RESPONSE_BUFFER_LENGTH = 0;
            WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "123.0.0.1");
            EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

            WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
            mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
            EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
            WB_CHECK(check_for_string(RESPONSE_BUFFER, CGEV_CHECK_STRING));
            WB_CHECK(!leftovers(RESPONSE_BUFFER));
        }
    } else {
        /* connect failure */
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_cause, &cause);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_destroy */

        WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

    }

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_a ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete the accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_delete, strlen(cgdcont_b_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cgact_set_two_cids(char *at_string)
{
    /* CGACT? (read), when no accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: 1,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_a ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account A and B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_a_and_b_on, strlen(cgact_a_and_b_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    /* Connect of B */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.167.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.167.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.167.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.167.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that account A and B are active */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */


    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_a ",1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: " cid_b ",1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* manual disconnect */
    Parser_AT(cgact_a_and_b_off, strlen(cgact_a_and_b_off), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    /* disconnect of B*/
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* check that we have two inactive accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_read, strlen(cgact_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_a ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGACT: "  cid_b ",0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgatt_gprs_update(char *at_string_p)
{
    int fail_cause = 34;
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_cgatt_attach == CURRENT_TEST_CASE) {
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_attach));
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_attached));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p);
    } else {
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_detach));
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_detached));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_cause, &fail_cause);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    if (wb_testcase_cgatt_detach == CURRENT_TEST_CASE) {
        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT("+CEER", strlen("+CEER"), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CEER: Deactivate Cause: SM34 (Service option temporarily out of order)"));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
    }
}

void wb_testfcn_cgatt_gprs_update_ignored(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.pscc_result = pscc_result_failed_operation_not_allowed;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_cgatt_attach_when_attached == CURRENT_TEST_CASE) {
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_attach));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    } else {
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_detach));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Restore default pscc result code */
    EXECUTOR.pscc_result = pscc_result_ok;
}


void wb_testfcn_cgatt_gprs_update_failed(char *at_string_p)
{
    /* Make sure CME ERROR codes are enabled */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (wb_testcase_cgatt_attach_fail == CURRENT_TEST_CASE) {
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_attach));
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_attach_failed));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p);
    } else {
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_detach));
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_detach_failed));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p);
    }

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 148"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cgatt_get_status(char *at_string_p)
{
    int attach_status = 0; /* pscc response 0 means attached (1 to cgatt) */
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_attach_status));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_attach_status, &attach_status);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGATT: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgatt_is_supported(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGATT: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgatt_gprs_abort(char *at_string_p)
{
    /* Test interrupt before response received */
    RESPONSE_BUFFER_LENGTH = 0;
    /*+CGATT=1*/
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);
    /* Interrupt attach/detach (by running another AT command or random string) */
    Parser_AT("S3=13", 5, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Test interrupt after response received, but before event received */
    RESPONSE_BUFFER_LENGTH = 0;
    EXECUTOR.pscc_request_id = -1;
    Parser_AT(at_string_p, strlen(at_string_p), AT_DATA_COMMAND, PARSER_p);

    if (strstr(at_string_p, "+CGATT=1")) {
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_attach));
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_attach_failed));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
    } else {
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_detach));
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_detach_failed));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_ct, &EXECUTOR.client_tag_p);
    }

    /* Interrupt attach/detach (by running another AT command or random string) */
    Parser_AT("S3=13", 5, AT_DATA_COMMAND, PARSER_p);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgcmod_set_1(char *at_string)
{
    /* Scenario 1
     * 1) Create 3 PS accounts.
     * 2) Connect to one of them.
     * 3) Change qos parameters for a connected account (using AT+CGEQREQ)
     * 4) Apply changes (AT+CGCMOD)
     * 5) Check that it has been changed (using AT+CGEQNEG)
     */

    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    add_and_connect_account_c();

    /* set qos parameters on account C */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_set_cid_c, strlen(cgeqreq_set_cid_c), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Apply changes */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgcmod_set, strlen(cgcmod_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_modify));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_modify */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_modified));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_modified */

    /*
     * TODO use AT+CGEQNEG to check that modified QoS parameters has been applied.
     *  (when AT+CGEQNEG has been implemented)
     */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    disconnect_and_delete_account_c();

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_delete, strlen(cgdcont_b_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_cgcmod_set_2(char *at_string)
{
    /* Scenario 2
     * Modify all available CID's (AT+CGCMOD=)
     * 1) Connect 2 accounts
     * 2) Apply changes (AT+CGCMOD)
     */
    add_and_connect_account_c();

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account A*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_a_on, strlen(cgact_a_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT+CGCMOD= */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgcmod_set_all, strlen(cgcmod_set_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_modify));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_modify */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_modified));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_modified */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_modify));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_modify */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_modified));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_modified */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Cleanup */
    disconnect_and_delete_account_c();

    /* Disconnect and remove account A */
    Parser_AT(cgact_a_off, strlen(cgact_a_off), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_cgcmod_set_3(char *at_string)
{
    /* Scenario 3
     * Modify two CID's (AT+CGCMOD=15,8)
     * 1) List connections.
     * 2) Fail to do QoS Modify procedure due to that one account is inactive
     */

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add and connect account C */
    add_and_connect_account_c();

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgcmod_set_b_and_c, strlen(cgcmod_set_b_and_c), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 3"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Clean up - Disconnect C account and delete both accounts */
    disconnect_and_delete_account_c();

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cgcmod_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account A and B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_a_and_b_on, strlen(cgact_a_and_b_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    /* Connect of B */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.167.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.167.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.167.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.167.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Do the CGCMOD=? command, which is identical to AT+CGACT? */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGCMOD: (" cid_b "," cid_a ")"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* disconnect account A */
    Parser_AT(cgact_a_and_b_off, strlen(cgact_a_and_b_off), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    /* disconnect account B*/
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete both accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_delete, strlen(cgdcont_b_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cgdcont(char *at_string)
{
    /* Make sure CME ERROR codes are enabled */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=1", strlen("+CMEE=1"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);

    switch(CURRENT_TEST_CASE) {
    case wb_testcase_cgdcont_1:
    case wb_testcase_cgdcont_2:
    case wb_testcase_cgdcont_3:
    case wb_testcase_cdcont_cleanup:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    break;
    case wb_testcase_cgdcont_4:
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
    break;
    default:
        WB_CHECK(0);
    break;
    }
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgdcont_read(char *at_string)
{
    /* Must be run before wb_testfcn_cgdcont when there are values stored
     * in the executor
     */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: 1,\"IP\",\"\",\"\",0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgdcont_parsing(char *at_string)
{
    char *teststrings[] = {
        "+CGDCONT= 12 ,\"IP\",\"apn.se\",\"www.pdp.bob\",0,0", /* all parameters */
        "+CGDCONT= 12 ,\"IP\",\"APN\",\"PDP\",0,0", /* upper case params */
        "+CGDCONT= 12 ,\"IP\",\"apn.se\",,0,1", /* missing PDP */
        "+CGDCONT= 12 ,\"IP\",,\"www.pdp.bob\",0,1", /* missing APN */
        "+CGDCONT= 12 ,\"IP\",,,0,1", /* missing APN and PDP */
        "+CGDCONT= 12 ,\"IP\",,,,1", /* missing APN and PDP and d_comp*/
        "+CGDCONT= 12 ,\"IPV6\",\"apn.se\",\"\",0,0", /* IPV6 */
        "+CGDCONT= 12 ,\"IPV4V6\",\"apn.se\",\"\",0,0", /* IPV4V6 */
        "+CGDCONT= 12 " /* delete account */
    };

    char *answerstrings[] = {
        "+CGDCONT: 12,\"IP\",\"apn.se\",\"www.pdp.bob\",0,0",
        "+CGDCONT: 12,\"IP\",\"APN\",\"PDP\",0,0",
        "+CGDCONT: 12,\"IP\",\"apn.se\",\"\",0,1",
        "+CGDCONT: 12,\"IP\",\"\",\"www.pdp.bob\",0,1",
        "+CGDCONT: 12,\"IP\",\"\",\"\",0,1",
        "+CGDCONT: 12,\"IP\",\"\",\"\",0,1",
        "+CGDCONT: 12,\"IPV6\",\"apn.se\",\"\",0,0",
        "+CGDCONT: 12,\"IPV4V6\",\"apn.se\",\"\",0,0",
        "+CGDCONT: 1,\"IP\",\"\",\"\",0,0",
    };

    int i = 0;

    for (i = 0; i < sizeof(teststrings) / sizeof(char *); i++) {
        /* check that call returns OK: */
        Parser_AT(teststrings[i], strlen(teststrings[i]), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        /* Check that the correct values can be read back: */
        Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

        WB_CHECK(check_for_string(RESPONSE_BUFFER, answerstrings[i]));
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
    }
}

void wb_testfcn_cgdcont_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: (1-25),\"IP\",,,(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: (1-25),\"IPV6\",,,(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGDCONT: (1-25),\"IPV4V6\",,,(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* ENAP test scenario
 * 1. Create account with +CGDCONT
 * 2. Activate enap  *ENAP=1,...
 *   a) activate account
 *   b) get account information
 *   c) activate nap connection
 * 3. Query nap connection *ENAP?
 * 4. Deactivate enap *ENAP=0,...
 *   a) deactivate nap connection
 *   b) deactivate account
 * 5. Activate account with +CGACT=1,<cid>
 * 6. Activate nap  *ENAP=1,...
 *   a) get account information
 *   b) activate nap connection
 * 7. Deactivate enap *ENAP=0,...
 *   a) deactivate nap connection
 *   b) deactivate account
 * 8. Delete account with +CGDCONT=<cid>
 */
void wb_testfcn_enap(char *at_string)
{
    int enap_conn_id = 4;
    int handle = 3;
    RESPONSE_BUFFER_LENGTH = 0;

    /* 1. */
    /* Create an account:AT+CGDCONT=5,"ip","apn.enap.se","www.enap_pdp",0,0 */
    Parser_AT(enap_account_create, strlen(enap_account_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    /* 2.*/
    /* Enap SET: AT*ENAP=1,5,1 */
    Parser_AT(enap_nap_connect, strlen(enap_nap_connect), AT_DATA_COMMAND, PARSER_p);

    /* 2 a) */
    /* Init message which will be used by request */
    /* pscc_create as a response type */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    /* add enap_conn_id to the message */
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    /* Triger request_nap_connect */
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    /* 1. Init message which will be used by request */
    /* 2. Add pscc_paramid_ct to the message, pscc_set as a response type */
    /* 3. Triger request_nap_connect */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    /* 1. Init message which will be used by request */
    /* 2. Add pscc_paramid_ct to the message, pscc_connect as a response type */
    /* 3. Triger request_nap_connect */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    /* Init message which will be used by request */
    /* pscc_event_connecting as a event type */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    /* add enap_conn_id to the message */
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    /* Triger request_nap_connect */
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p);

    /* Init message which will be used by request */
    /* pscc_event_connected as a event type */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    /* add enap_conn_id to the message */
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    /* Triger request_nap_connect */
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p);

    /* 2 b) */
    /* Init message which will be used by request */
    /* pscc_get as a response type */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    /* add pscc_paramid_gw_address,pscc_paramid_dns_address, pscc_paramid_secondary_dns_address, pscc_paramid_own_ip_address  to the message */
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    /* Triger request_nap_connect */
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 2 c) */
    /* Init message which will be used by request */
    /* sterc_create as a response type */
    WB_CHECK(wb_sterc_init_message(mpl_msg_type_resp, sterc_create));
    /* add sterc_paramid_handle to the message */
    mpl_add_param_to_list(&EXECUTOR.sterc_param_list_p, sterc_paramid_handle, &handle);
    /* Triger request_nap_connect */
    EXECUTOR.sterc_selector_callback_request(FD_STERC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 3. */
    /* Enap READ: AT*ENAP? */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(enap_nap_read, strlen(enap_nap_read), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_sterc_init_message(mpl_msg_type_resp, sterc_get));
    EXECUTOR.sterc_selector_callback_request(FD_STERC_REQUEST, EXECUTOR.psccclient_p); /* response to sterc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*ENAP: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 4. */
    /* Enap SET: AT*ENAP=0,5 */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(enap_nap_disconnect, strlen(enap_nap_disconnect), AT_DATA_COMMAND, PARSER_p);

    /* 4 a) deactivate nap connection */
    /* Init message which will be used by request */
    /* sterc_destroy as a response type */
    WB_CHECK(wb_sterc_init_message(mpl_msg_type_resp, sterc_destroy));
    /* Triger request_nap_connect */
    EXECUTOR.sterc_selector_callback_request(FD_STERC_REQUEST, EXECUTOR.psccclient_p);

    /* 4 b) deactivate PDP account */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 5. */
    /* Activate PDP context: AT+CGACT=1,5 */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(enap_account_activate, strlen(enap_account_activate), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 6.*/
    /* Enap SET: AT*ENAP=1,5,1 */
    Parser_AT(enap_nap_connect, strlen(enap_nap_connect), AT_DATA_COMMAND, PARSER_p);

    /* 6 a) get account information */
    /* Init message which will be used by request */
    /* pscc_get as a response type */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    /* add pscc_paramid_gw_address,pscc_paramid_dns_address, pscc_paramid_secondary_dns_address, pscc_paramid_own_ip_address  to the message */
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    /* Triger request_nap_connect */
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 6 b) activate nap connection */
    /* Init message which will be used by request */
    /* sterc_create as a response type */
    WB_CHECK(wb_sterc_init_message(mpl_msg_type_resp, sterc_create));
    /* add sterc_paramid_handle to the message */
    mpl_add_param_to_list(&EXECUTOR.sterc_param_list_p, sterc_paramid_handle, &handle);
    /* Triger request_nap_connect */
    EXECUTOR.sterc_selector_callback_request(FD_STERC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 7. */
    /* Enap SET: AT*ENAP=0,5 */
    Parser_AT(enap_nap_disconnect, strlen(enap_nap_disconnect), AT_DATA_COMMAND, PARSER_p);

    /* 7 a) deactivate nap connection */
    /* Init message which will be used by request */
    /* sterc_destroy as a response type */
    WB_CHECK(wb_sterc_init_message(mpl_msg_type_resp, sterc_destroy));
    /* Triger request_nap_connect */
    EXECUTOR.sterc_selector_callback_request(FD_STERC_REQUEST, EXECUTOR.psccclient_p);

    /* 7 b) deactivate PDP account */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &enap_conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* 8. */
    /* Cgdcont SET: AT+CGDCONT=<cid> */
    /* delete the account  */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(enap_account_delete, strlen(enap_account_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

/* Test functions for CGEREP */
void wb_testfcn_cgerep_set(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgerep_set_failure(char *at_string)
{
    /* Make sure CME ERROR codes are enabled. */
    wb_testfcn_cmee_configure("+CMEE=1");

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Make sure CME ERROR codes are disabled. */
    wb_testfcn_cmee_configure("+CMEE=0");
}

void wb_testfcn_cgerep_read(char *at_string)
{
    (void)wb_testfcn_cgerep_set(cgerep);

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, strlen(at_string), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEREP: 1,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgerep_test(char *at_string)
{
    size_t at_string_length = strlen(at_string);
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(at_string, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEREP: (0-1),(0)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgeqreq(char *at_string)
{
    /* add account A using +CGDCONT - default qos paramters should be added*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B using +CGDCONT - default qos paramters should be added*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* read req qos parameters from account A, should be default parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_read, strlen(cgeqreq_read), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 1,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 8,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 24,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set qos parameters on account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_set, strlen(cgeqreq_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* read qos parameters from account A, should be the new qos parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_read, strlen(cgeqreq_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 1,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 8,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 24,3,5696,14000,5000,14000,0,1502,\"1E4\",\"1E5\",1,4,2,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove the created accounts:*/
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    Parser_AT(cgdcont_b_delete, strlen(cgdcont_b_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Test the test command */
#define cgeqreq_test "+CGEQREQ=?"
    Parser_AT(cgeqreq_test, strlen(cgeqreq_test), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: \"IP\",(0-4),(0-5696),(0-21000),(0-5696),(0-21000),(0-2),(0-1520),(\"0E0\",\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"1E1\"),(\"0E0\",\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\"),(0-3),(0-4000),(0-3),(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: \"IPV6\",(0-4),(0-5696),(0-21000),(0-5696),(0-21000),(0-2),(0-1520),(\"0E0\",\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"1E1\"),(\"0E0\",\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\"),(0-3),(0-4000),(0-3),(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: \"IPV4V6\",(0-4),(0-5696),(0-21000),(0-5696),(0-21000),(0-2),(0-1520),(\"0E0\",\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"1E1\"),(\"0E0\",\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\"),(0-3),(0-4000),(0-3),(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgqmin(char *at_string)
{
    /* As per all other, some test has left a created, non-active account with id=1. */
    /* Make sure CME ERROR codes are disabled */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT("+CMEE=0", strlen("+CMEE=0"), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Temporarily remove it. */
    Parser_AT(remove_1, strlen(remove_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Verify that read of cgqmin returns ok and no pdp contexts. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgqmin_read_all, strlen(cgqmin_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Attempt to set QMIN when cid is not defined: */
    Parser_AT(cgqmin_max_set, strlen(cgqmin_max_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Put back account number 1 */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(create_1, strlen(create_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Verify that read of cgqmin returns one pdp context with default parameters. */
    Parser_AT(cgqmin_read_all, strlen(cgqmin_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account A using +CGDCONT */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set maximal parameters to account A */
    Parser_AT(cgqmin_max_set, strlen(cgqmin_max_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read parameters for all accounts */
    Parser_AT(cgqmin_read_all, strlen(cgqmin_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgqmin_max_response));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set with some params left out */
    Parser_AT(cgqmin_set_leftouts, strlen(cgqmin_set_leftouts), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read parameters for all accounts */
    Parser_AT(cgqmin_read_all, strlen(cgqmin_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgqmin_leftouts_response));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set with no params */
    Parser_AT(cgqmin_set_reset, strlen(cgqmin_set_reset), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read parameters for all accounts */
    Parser_AT(cgqmin_read_all, strlen(cgqmin_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgqmin_set_reset_response));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove account A */
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Test the test command */
#define cgqmin_test "+CGQMIN=?"
    Parser_AT(cgqmin_test, strlen(cgqmin_test), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: \"IP\",(0-3),(0-4),(0-5),(0-9),(0-18,31)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: \"IPV6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQMIN: \"IPV4V6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgqreq(char *at_string)
{
    /* As per all other, some test has left a created, non-active account with id=1. */

    /* Temporarily remove it. */
    Parser_AT(remove_1, strlen(remove_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Verify that read of cgqreq returns ok and no pdp contexts. */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgqreq_read_all, strlen(cgqreq_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Attempt to set QREQ when cid is not defined: */
    Parser_AT(cgqreq_max_set, strlen(cgqreq_max_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Put back account number 1 */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(create_1, strlen(create_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Verify that read of cgqreq returns one pdp context with default parameters. */
    Parser_AT(cgqreq_read_all, strlen(cgqreq_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account A using +CGDCONT */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set maximal parameters to account A */
    Parser_AT(cgqreq_max_set, strlen(cgqreq_max_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read parameters for all accounts */
    Parser_AT(cgqreq_read_all, strlen(cgqreq_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgqreq_max_response));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set with some params left out */
    Parser_AT(cgqreq_set_leftouts, strlen(cgqreq_set_leftouts), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read parameters for all accounts */
    Parser_AT(cgqreq_read_all, strlen(cgqreq_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgqreq_leftouts_response));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set with no params */
    Parser_AT(cgqreq_set_reset, strlen(cgqreq_set_reset), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Read parameters for all accounts */
    Parser_AT(cgqreq_read_all, strlen(cgqreq_read_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: 1,0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, cgqreq_set_reset_response));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove account A */
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Test the test command */
#define cgqreq_test "+CGQREQ=?"
    Parser_AT(cgqreq_test, strlen(cgqreq_test), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: \"IP\",(0-3),(0-4),(0-5),(0-9),(0-18,31)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: \"IPV6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGQREQ: \"IPV4V6\",(0-3),(0-4),(0-5),(0-9),(0-18,31)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgeqmin(char *at_string)
{
    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));


    /* read min qos parameters from account A, no parameters should return as accounts does not exist*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_read, strlen(cgeqmin_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 1,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 8,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 24,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set min qos parameters on account A*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_set, strlen(cgeqmin_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* read qos parameters from account A, should be the new qos parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_read, strlen(cgeqmin_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 1,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 8,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 24,3,5696,14000,5000,14000,0,1502,\"1E4\",\"1E5\",1,4,2,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove the created accounts:*/
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    Parser_AT(cgdcont_b_delete, strlen(cgdcont_b_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Test the test command */
#define cgeqmin_test "+CGEQMIN=?"
    Parser_AT(cgeqmin_test, strlen(cgeqmin_test), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: \"IP\",(0-3),(0-5696),(0-21000),(0-5696),(0-21000),(0-1),(0-1520),(\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"1E1\"),(\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\"),(0-2),(0-4000),(0-3),(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: \"IPV6\",(0-3),(0-5696),(0-21000),(0-5696),(0-21000),(0-1),(0-1520),(\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"1E1\"),(\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\"),(0-2),(0-4000),(0-3),(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: \"IPV4V6\",(0-3),(0-5696),(0-21000),(0-5696),(0-21000),(0-1),(0-1520),(\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"1E1\"),(\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\"),(0-2),(0-4000),(0-3),(0-1),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgeqmin_partialset(char *at_string)
{
    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set maximum parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_set_maximum, strlen(cgeqmin_set_maximum), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Verify maximum values */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_read, strlen(cgeqmin_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 1,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 24,3,5696,21000,5696,21000,1,1502,\"1E1\",\"5E2\",2,4000,3,1,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set minimum parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_set_minimum, strlen(cgeqmin_set_minimum), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /*Verify minimum values */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_read, strlen(cgeqmin_read), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 1,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 24,0,0,0,0,0,0,0,\"1E6\",\"6E8\",0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set partial set of parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_set_partial, strlen(cgeqmin_set_partial), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* read qos parameters from account A, should be the new qos parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqmin_read, strlen(cgeqmin_read), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 1,3,0,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQMIN: 24,1,2345,0,0,0,0,0,\"\",\"\",2,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove the created accounts:*/
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_cgeqreq_partialset(char *at_string)
{
    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set maximum parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_set_maximum, strlen(cgeqreq_set_maximum), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Verify maximum values */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_read, strlen(cgeqreq_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 1,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 24,4,5696,21000,5696,21000,2,1502,\"1E1\",\"5E2\",3,4000,3,1,1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set minimum parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_set_minimum, strlen(cgeqreq_set_minimum), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /*Verify minimum values */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_read, strlen(cgeqreq_read), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 1,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 24,0,0,0,0,0,0,0,\"1E6\",\"6E8\",0,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Set partial set of parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_set_partial, strlen(cgeqreq_set_partial), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* read qos parameters from account A, should be the new qos parameters */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqreq_read, strlen(cgeqreq_read), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 1,4,0,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQREQ: 24,1,2345,0,0,0,2,0,\"\",\"\",3,0,0,0,0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove the created accounts:*/
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgeqneg(char *at_string_p)
{
    /* Test negative testcases first: */
#define cgeqneg_set_illegal "+CGEQNEG=26"
    Parser_AT(cgeqneg_set_illegal, strlen(cgeqneg_set_illegal), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#define cgeqneg_set_very_illegal "+CGEQNEG=260"
    Parser_AT(cgeqneg_set_very_illegal, strlen(cgeqneg_set_very_illegal), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#define cgeqneg_set_mix_with_illegal "+CGEQNEG=3,42,24"
    Parser_AT(cgeqneg_set_mix_with_illegal, strlen(cgeqneg_set_mix_with_illegal), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#define cgeqneg_set_mix_with_illegal_2 "+CGEQNEG=42,24"
    Parser_AT(cgeqneg_set_mix_with_illegal_2, strlen(cgeqneg_set_mix_with_illegal_2), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#define cgeqneg_set_empty "+CGEQNEG="
    Parser_AT(cgeqneg_set_empty, strlen(cgeqneg_set_empty), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Unfortunate precondition: Account with cid=1 already exists. */

    /*Case 1: Run test command for no pdp contexts.*/
    Parser_AT(cgeqneg_test, strlen(cgeqneg_test), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQNEG: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));


    /* Run set command for no pdp contexts. */
    Parser_AT(cgeqneg_set, strlen(cgeqneg_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQNEG: "));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

#define only_cgact_a_and_b_on "+CGACT=1," cid_a "," cid_b
    /* connect accounts A & B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(only_cgact_a_and_b_on, strlen(only_cgact_a_and_b_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    /* Connect of B */
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.167.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.167.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.167.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.167.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */


    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));


    /* Add account C*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_c_create, strlen(cgdcont_c_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Run test command for two active and one inactive pdp context. */
    /*Case 1: Run test command for no pdp contexts.*/
    Parser_AT(cgeqneg_test, strlen(cgeqneg_set), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQNEG: (" cid_a "," cid_b ")"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Request qos data for _no_ accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqneg_set_none, strlen(cgeqneg_set_none), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Request qos data for one of the active accounts */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqneg_set, strlen(cgeqneg_set), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    send_qos_response(&conn_id);
    send_qos_response(&conn_id_b);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQNEG: 8,1,5696,21000,4992,21000,1,1400,\"1E5\",\"4E3\",2,100,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));


    /* Request qos data for both active accounts: */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgeqneg_set_all, strlen(cgeqneg_set_all), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    send_qos_response(&conn_id);
    send_qos_response(&conn_id_b);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQNEG: 8,1,5696,21000,4992,21000,1,1400,\"1E5\",\"4E3\",2,100,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGEQNEG: 24,1,5696,21000,4992,21000,1,1400,\"1E5\",\"4E3\",2,100,2"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));


    /* Cleanup:*/
    /* disconnect a and b */
#define only_cgact_a_and_b_off "+CGACT=0," cid_a "," cid_b
    Parser_AT(only_cgact_a_and_b_off, strlen(only_cgact_a_and_b_off), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    /* disconnect of B*/
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_b);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */


    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove account c: */
    Parser_AT(cgdcont_c_delete, strlen(cgdcont_c_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove account b:*/
    Parser_AT(cgdcont_b_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove account a:*/
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Remove account c:*/
    Parser_AT(cgdcont_c_delete, strlen(cgdcont_c_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgpaddr_set(char *at_string)
{
    int len = strlen(at_string);
    RESPONSE_BUFFER_LENGTH = 0;
    add_and_connect_account_c();

    /* Send AT+CGPADDR=10,15 */
    Parser_AT(at_string, len, AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.5");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    /* Check response */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_c ",\"192.168.0.5\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Cleanup */
    disconnect_and_delete_account_c();
}

void wb_testfcn_cgpaddr_set_two_cids(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    int len = strlen(at_string);

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account B */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_b_create, strlen(cgdcont_b_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Send AT+CGPADDR=8,10,24 */
    Parser_AT(at_string, len, AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

    /* Check response */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_b ",\"\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_a ",\"www.pdp_a\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cgpaddr_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    int len = strlen(at_string);
    /* Expected pre-condition:
     * Account A created,
     * Account B created,
     * Account C created and activated.
     */

    add_and_connect_account_c();

    /* Send AT+CGPADDR=? */
    Parser_AT(at_string, len, AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_list_all_connections));

    /* Check response */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: (" cid_b "," cid_c "," cid_a")"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Cleanup */
    disconnect_and_delete_account_c();
}
void wb_testfcn_cgpaddr_all(char *at_string)
{
    /* Expected pre-condition:
     * Account A created,
     * Account B created,
     */
    RESPONSE_BUFFER_LENGTH = 0;
    int len = strlen(at_string);

    add_and_connect_account_c();
    Parser_AT(at_string, len, AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_list_all_connections));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_list_all_connections */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.5");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_c ",\"192.168.0.5\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_a ",\"www.pdp_a\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CGPADDR: " cid_b ",\"\""));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Cleanup */
    disconnect_and_delete_account_c();
}

void wb_testfcn_cr_set(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    size_t at_string_length = strlen(at_string_p);
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Restoring to CR mode to 0 */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cr_set_0, strlen(cr_set_0), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cr_set_neg(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    size_t at_string_length = strlen(at_string_p);
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cr_read_0(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cr_set_0, strlen(cr_set_0), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;
    size_t at_string_length = strlen(at_string_p);
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CR: 0"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cr_read_1(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cr_set_1, strlen(cr_set_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    RESPONSE_BUFFER_LENGTH = 0;
    size_t at_string_length = strlen(at_string_p);
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CR: 1"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Restoring to CR mode to 0 */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cr_set_0, strlen(cr_set_0), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_cr_test(char *at_string_p)
{
    RESPONSE_BUFFER_LENGTH = 0;
    size_t at_string_length = strlen(at_string_p);
    Parser_AT(at_string_p, at_string_length, AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CR: (0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_pscc_with_report(char *at_string_p)
{
    /* Activate service reporting */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cr_set_1, strlen(cr_set_1), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* add account C */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_c_create, strlen(cgdcont_c_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account C*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_c_on, strlen(cgact_c_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.5");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CR: GPRS"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Deactivate service reporting */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cr_set_0, strlen(cr_set_0), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    disconnect_and_delete_account_c();
}

void add_and_connect_account_c()
{
    /* add account C */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_c_create, strlen(cgdcont_c_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account C*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_c_on, strlen(cgact_c_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.5");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void disconnect_and_delete_account_c()
{
    RESPONSE_BUFFER_LENGTH = 0;
    /* disconnect account C */
    Parser_AT(cgact_c_off, strlen(cgact_c_off), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_c);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete account C */

    Parser_AT(cgdcont_c_delete, strlen(cgdcont_c_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void add_and_connect_account_d()
{
    /* add account D */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_d_create, strlen(cgdcont_d_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* connect account D*/
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_d_on, strlen(cgact_d_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_d);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_d);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_d);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_d);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.5");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void disconnect_and_delete_account_d()
{
    /* disconnect account D */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgact_d_off, strlen(cgact_d_off), AT_DATA_COMMAND, PARSER_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_d);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id_d);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete account D */
    Parser_AT(cgdcont_d_delete, strlen(cgdcont_d_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}


void wb_testfcn_euplink(char *at_string)
{
    /* AT*EUPLINK positive scenario 1. (Default size)
     * 1. Create account D using AT+CGDCONT.
     * 2. Connect to account D using AT+CGACT.
     * 3. Send uplink data using AT*EUPLINK with default size (512) bytes.
     * 4. Disconnect to account D using AT+CGACT.
     * 5. Delete account D.
     */

    add_and_connect_account_d();

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(euplink_d_def, strlen(euplink_d_def), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_generate_uplink_data));
    disconnect_and_delete_account_d();

    /* AT*EUPLINK positive scenario 2. (Max size)
     * 1. Create account D using AT+CGDCONT.
     * 2. Connect to account D using AT+CGACT.
     * 3. Do AT*EUPLINK with max size (262143) bytes.
     * 4. Disconnect to account D using AT+CGACT.
     * 5. Delete account D.
     */

    add_and_connect_account_d();
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(euplink_d_max, strlen(euplink_d_max), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_generate_uplink_data));
    disconnect_and_delete_account_d();

    /* AT*EUPLINK negative scenario 1 (non-active account).
     * 1. Create account D using AT+CGDCONT.
     * 2. Send uplink data using AT*EUPLINK with default size (512) bytes.
     *    This should fail due to inactive PDP context.
     * 3. Delete account D.
     */

    /* add account D */
    RESPONSE_BUFFER_LENGTH = 0;

    Parser_AT(cgdcont_d_create, strlen(cgdcont_d_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(euplink_d_def, strlen(euplink_d_def), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));

    /* Simulate failure when sending pscc_generate_uplink_data request */
    EXECUTOR.pscc_result = pscc_result_failed_operation_not_allowed;

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_generate_uplink_data));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete account D */
    Parser_AT(cgdcont_d_delete, strlen(cgdcont_d_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Restore default pscc result code */
    EXECUTOR.pscc_result = pscc_result_ok;

    /* AT*EUPLINK negative scenario 2 (no account).
     * 1. Send uplink data using AT*EUPLINK using unknown CID.
     * This should fail since no account exists based on CID.
     */

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(euplink_d_def, strlen(euplink_d_def), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EUPLINK negative scenario 3 (wrong pdp).
     * 1. Create account C using AT+CGDCONT.
     * 2. Connect to account C using AT+CGACT.
     * 3. Send uplink data using AT*EUPLINK using account C.
     * This should fail since this account uses the wrong pdp type.
     * 4. Disconnect to account C using AT+CGACT.
     * 5. Delete account C.
     */

    add_and_connect_account_c();
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(euplink_c_def, strlen(euplink_c_def), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    disconnect_and_delete_account_c();
}

void send_qos_response(int *conn_id_p)
{
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    static int traffic_class = 2;
    static int max_bru = 208;
    static int max_bru_e = 0; /* => 5696 */
    static int max_brd = 254;
    static int max_brd_e = 79; /* => 21000 */
    static int gua_bru = 197;
    static int gua_bru_e = 0; /* => 5000 */
    static int gua_brd = 254;
    static int gua_brd_e = 79; /* => 21000 */
    static int delivery_order = 1; /* YES */
    static int max_sdu_size = 140; /* 1400 octets */
    static int sdu_err_ratio = 5; /*1E5 */
    static int residual_ber = 4; /* 4E3 */
    static int delivery_err_SDU = 1; /* No detect (2=yes, 3=no) */
    static int transfer_delay = 10; /* 100 ms */
    static int traffic_handling_prio = 2; /* Priority level 2 */


    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, conn_id_p);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_traffic_class, &traffic_class);

    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_max_bitrate_uplink, &max_bru);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_extended_max_bitrate_uplink, &max_bru_e);

    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_max_bitrate_downlink, &max_brd);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_extended_max_bitrate_downlink, &max_brd_e);

    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_guaranteed_bitrate_uplink, &gua_bru);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink, &gua_bru_e);

    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_guaranteed_bitrate_downlink, &gua_brd);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink, &gua_brd_e);

    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_delivery_order, &delivery_order);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_max_sdu_size, &max_sdu_size);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_sdu_error_ratio, &sdu_err_ratio);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_residual_ber, &residual_ber);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_delivery_erroneous_sdu, &delivery_err_SDU);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_transfer_delay, &transfer_delay);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_neg_qos_traffic_handling_priority, &traffic_handling_prio);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
}

void wb_testfcn_d_ps(char *at_string)
{
    int handle = 3;
    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set ATD using account A */
    RESPONSE_BUFFER_LENGTH = 0;
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_d_ps:
        Parser_AT(atd_ps_set, strlen(atd_ps_set), AT_DATA_COMMAND, PARSER_p);
        break;
    case wb_testcase_d_ps_l2p:
        Parser_AT(atd_ps_set_l2p, strlen(atd_ps_set_l2p), AT_DATA_COMMAND, PARSER_p);
        break;
    case wb_testcase_d_ps_cid:
        Parser_AT(atd_ps_set_a, strlen(atd_ps_set_a), AT_DATA_COMMAND, PARSER_p);
        break;
    case wb_testcase_d_ps_l2p_cid:
        Parser_AT(atd_ps_set_a_l2p, strlen(atd_ps_set_a_l2p), AT_DATA_COMMAND, PARSER_p);
        break;
    case wb_testcase_d_ps_conn:
        /* Activate PDP */
        Parser_AT(cgact_a_on, strlen(cgact_a_on), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */

        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");

        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));

        RESPONSE_BUFFER_LENGTH = 0;
        Parser_AT(atd_ps_set, strlen(atd_ps_set), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "CONNECT"));
        break;
    default:
        break;
    }
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    if (CURRENT_TEST_CASE != wb_testcase_d_ps_conn)
    {
        WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
        mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
        EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_create */
        WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    }
    WB_CHECK(wb_sterc_init_message(mpl_msg_type_resp, sterc_create));
    mpl_add_param_to_list(&EXECUTOR.sterc_param_list_p, sterc_paramid_handle, &handle);
    EXECUTOR.sterc_selector_callback_request(FD_STERC_REQUEST, EXECUTOR.psccclient_p);
    if (CURRENT_TEST_CASE != wb_testcase_d_ps_conn)
    {
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "CONNECT"));
        WB_CHECK(!leftovers(RESPONSE_BUFFER));
    }

    WB_CHECK(wb_sterc_init_message(mpl_msg_type_event, sterc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.sterc_param_list_p, sterc_paramid_handle, &handle);
    EXECUTOR.sterc_selector_callback_event(FD_STERC_EVENT, EXECUTOR.psccclient_p); /* sterc_event_disconnected */

    WB_CHECK(wb_sterc_init_message(mpl_msg_type_resp, sterc_destroy));
    EXECUTOR.sterc_selector_callback_request(FD_STERC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_d_ps_failure(char *at_string)
{
    int handle = 3;
    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set ATD using account A */
    RESPONSE_BUFFER_LENGTH = 0;
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_d_ps_failure:
        Parser_AT(atd_ps_set_bad_sc, strlen(atd_ps_set_bad_sc), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 4"));
        break;
    case wb_testcase_d_ps_failure_1:
        Parser_AT(atd_ps_set_bad_l2p, strlen(atd_ps_set_bad_l2p), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
        break;
    case wb_testcase_d_ps_failure_2:
        Parser_AT(atd_ps_set_bad_cid_0, strlen(atd_ps_set_bad_cid_0), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
        break;
    case wb_testcase_d_ps_failure_3:
        Parser_AT(atd_ps_set_bad_cid, strlen(atd_ps_set_bad_cid), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "+CME ERROR: 50"));
        break;
    case wb_testcase_d_ps_failure_4:
        Parser_AT(atd_ps_set_b_wrong_cid, strlen(atd_ps_set_b_wrong_cid), AT_DATA_COMMAND, PARSER_p);
        WB_CHECK(check_for_string(RESPONSE_BUFFER, "NO CARRIER"));
        break;
    default:
        break;
    }
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

void wb_testfcn_eiaauw_set(char *at_string)
{
    /* AT*EIAAUW positive scenario 1. (auth_prot ALL=00000111=7)
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account A.
     * 3. Connect to account A using AT*EPPSD.
     * 4. Disconnect to account A using AT*EPPSD.
     * 5. Delete account A.
     */

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_a_auth_all, strlen(eiaauw_set_a_auth_all), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    connect_and_disconnect_A();

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EIAAUW positive scenario 2. (auth_prot CHAP=00000101=5)
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account A.
     * 3. Connect to account A using AT*EPPSD.
     * 4. Disconnect to account A using AT*EPPSD.
     * 5. Delete account A.
     */

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_a_auth_chap, strlen(eiaauw_set_a_auth_chap), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    connect_and_disconnect_A();

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EIAAUW positive scenario 3. (auth_prot NONE=00000001=1)
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account A.
     * 3. Connect to account A using AT*EPPSD.
     * 4. Disconnect to account A using AT*EPPSD.
     * 5. Delete account A.
     */

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_a_auth_none, strlen(eiaauw_set_a_auth_none), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    connect_and_disconnect_A();

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EIAAUW positive scenario 4. (auth_prot PAP=00000011=3)
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account A.
     * 3. Connect to account A using AT*EPPSD.
     * 4. Disconnect to account A using AT*EPPSD.
     * 5. Delete account A.
     */

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_a_auth_pap, strlen(eiaauw_set_a_auth_pap), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    connect_and_disconnect_A();

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EIAAUW positive scenario 5. (empty pap/chap credentials)
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account A.
     * 3. Connect to account A using AT*EPPSD.
     * 4. Disconnect to account A using AT*EPPSD.
     * 5. Delete account A.
     */

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_a_empty, strlen(eiaauw_set_a_empty), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    connect_and_disconnect_A();

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EIAAUW negative scenario 1. (auth_prot unspecified=0)
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account A.
     *    Fail due to unspecified auth protocol.
     * 3. Delete account A.
     */

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_a_auth_err, strlen(eiaauw_set_a_auth_err), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EIAAUW negative scenario 2. (bearer=2 not supported (CS))
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account A.
     *    Fail due to unsupported bearer.
     * 3. Delete account A.
     */

    /* add account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_a_bearer_err, strlen(eiaauw_set_a_bearer_err), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* AT*EIAAUW negative scenario 3. (Bad CID)
     * 1. Create account A using AT+CGDCONT.
     * 2. Set auth parameters to account C.
     *    Fail due to wrong CID.
     * 3. Delete account A.
     */
    /* add account A */

    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_create, strlen(cgdcont_a_create), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* set auth parameters to account C */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_set_c_err, strlen(eiaauw_set_c_err), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "ERROR"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* delete account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(cgdcont_a_delete, strlen(cgdcont_a_delete), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
void wb_testfcn_eiaauw_test(char *at_string)
{
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eiaauw_test, strlen(eiaauw_test), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EIAAUW: (1-25),(1-4),\"\",\"\",(00001-11111),(0-1)"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}

static void connect_and_disconnect_A()
{
    /* connect account A */
    RESPONSE_BUFFER_LENGTH = 0;
    Parser_AT(eppsd_a_on, strlen(eppsd_a_on), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_create));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);

    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_set));
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_connect));

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_connected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_connected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */

    WB_CHECK(check_for_string(RESPONSE_BUFFER, "*EPPSD: <?xml version=\"1.0\"?>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<ip_address>192.168.0.3</ip_address>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<subnet_mask>255.255.255.255</subnet_mask>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<mtu>0</mtu>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<default_gateway>192.168.0.1</default_gateway>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.8</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<dns_server>192.168.0.7</dns_server>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "<activation_cause>0</activation_cause>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "</connection_parameters>"));
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));

    /* Disconnect account A */
    Parser_AT(eppsd_a_off, strlen(eppsd_a_off), AT_DATA_COMMAND, PARSER_p);
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_get));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_pdp_type, &pdp_type);
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_gw_address, "192.168.0.1");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_dns_address, "192.168.0.8");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_secondary_dns_address, "192.168.0.7");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_own_ip_address, "192.168.0.3");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_netdev_name, "gprs0");
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connection_status, &status_connected);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p); /* response to pscc_get */
    WB_CHECK(wb_pscc_respond_with_just_client_tag(pscc_disconnect));
    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnecting));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnecting */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_event, pscc_event_disconnected));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_event(FD_PSCC_EVENT, EXECUTOR.psccclient_p); /* pscc_event_disconnected */

    WB_CHECK(wb_pscc_init_message(mpl_msg_type_resp, pscc_destroy));
    mpl_add_param_to_list(&EXECUTOR.pscc_param_list_p, pscc_paramid_connid, &conn_id);
    EXECUTOR.pscc_selector_callback_request(FD_PSCC_REQUEST, EXECUTOR.psccclient_p);
    WB_CHECK(check_for_string(RESPONSE_BUFFER, "OK"));
    WB_CHECK(!leftovers(RESPONSE_BUFFER));
}
