/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : tc_main.h
 * Description     : describes what tests that should be run
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#ifndef TC_MAIN_H
#define TC_MAIN_H

#include "common.h"
#include "tc_mal_basic.h"
#include "tc_mal_modify.h"
#include "tc_mal_cc.h"
#include "tc_mal_attach.h"

/**********************************************/
/********* Definitions of the testcases   *****/
/**********************************************/

const testcase_t testcases[] = {
    /* setup should be first */

   { "basic setup", tc_basic_first },


    /* basic tests */
    { "basic create,connect,disconnect,destroy", tc_basic_connect_disconnect },

    /* basic tests ipv4v6*/
    { "basic create,connect,disconnect,destroy ipv4v6",  tc_basic_connect_disconnect_ipv4v6 },
    { "basic create,connect,disconnect,destroy ipv6",  tc_basic_connect_disconnect_ipv6 },
    { "basic create,connect,disconnect,destroy ipv4",  tc_basic_connect_disconnect_ipv4 },

    { "basic modify", tc_modify },

    /* call control tests */
    { "cc allow", tc_cc_allow },
    { "cc hcmp_dcmp_on", tc_hcmp_dcmp_ON},
    { "cc not allow", tc_cc_not_allow },
    { "cc allow with modification", tc_cc_allow_mod },
    { "cc denied by gpds", tc_cc_denied_by_gpds },

    /* attach/detach tests */
    { "set attach mode - automatic",            tc_attach_set_automatic },
    { "set attach mode - manual",               tc_attach_set_manual },
    { "set attach mode - default",              tc_attach_set_default },
    { "set attach mode - fail",                 tc_attach_set_automatic_fail },
    { "query attach mode",                      tc_attach_query_mode },
    { "query attach status",                    tc_attach_query_status },
    { "attach",                                 tc_attach },
    { "attach fail",                            tc_attach_fail },
    { "detach",                                 tc_attach_detach },
    { "detach fail",                            tc_attach_detach_fail },
    { "attach when already attached",           tc_attach_already_attached },
    { "receive nwi detach",                     tc_attach_detach_received },
    { "detach when already detached",           tc_attach_already_detached },
    { "configure attach mode",                  tc_configure_attach_mode },
    { "configure attach mode fail 1",           tc_configure_attach_mode_fail_1 },
    { "configure attach mode fail 2",           tc_configure_attach_mode_fail_2 },
    { "configure filter icmp ping mode",        tc_configure_ping_block_mode },
    { "configure filter icmp ping mode fail 1", tc_configure_ping_block_mode_fail_1 },
    { "configure filter icmp ping mode fail 2", tc_configure_ping_block_mode_fail_2 },
    { "basic modify rel97",                     tc_rel97 },
    { "basic chap generation",                  tc_basic_connect_disconnect_chap_generation },
    { "basic chap",                             tc_basic_connect_disconnect_chap },
    { "basic pap",                              tc_basic_connect_disconnect_pap },
    { "tc_fetch_data_counter",                  tc_fetch_data_counters },
    { "tc_reset_data_counter",                  tc_reset_data_counters },
    { "basic uplink data generation",           tc_basic_connect_disconnect_uplink },
    { "list all connections",                   tc_list_all_connection },
    { "static ip for ipv4",                     tc_static_ip_ipv4 },
#if 1
    { "basic 255 connections", tc_basic_255connections },
    { "error scenarios", tc_error_scenarios },
#endif
    /* teardown should be last */
    { "basic teardown", tc_basic_last },
};

#endif // TC_MAIN_H
