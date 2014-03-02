/*
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : tc_mal_basic.h
 * Description     : basic mal connectivity tests
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#ifndef TC_MAL_BASIC_H_
#define TC_MAL_BASIC_H_
#include "common.h"

psccd_test_state_type_t tc_basic_first(int state);
psccd_test_state_type_t tc_basic_connect_disconnect(int state);
psccd_test_state_type_t tc_basic_connect_disconnect_ipv4v6(int state);
psccd_test_state_type_t tc_basic_connect_disconnect_ipv6(int state);
psccd_test_state_type_t tc_basic_connect_disconnect_ipv4(int state);
psccd_test_state_type_t tc_basic_255connections(int state);
psccd_test_state_type_t tc_error_scenarios(int state);
psccd_test_state_type_t tc_fetch_data_counters(int state);
psccd_test_state_type_t tc_reset_data_counters(int state);
psccd_test_state_type_t tc_basic_last(int state);
psccd_test_state_type_t tc_basic_connect_disconnect_chap_generation(int state);
psccd_test_state_type_t tc_basic_connect_disconnect_chap(int state);
psccd_test_state_type_t tc_basic_connect_disconnect_pap(int state);
psccd_test_state_type_t tc_basic_connect_disconnect_uplink(int state);
psccd_test_state_type_t tc_list_all_connection(int state);
psccd_test_state_type_t tc_static_ip_ipv4(int state);


#endif //TC_MAL_BASIC_H_
