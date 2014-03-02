/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef TC_MAL_ATTACG_H_
#define TC_MAL_ATTACH_H_
#include "common.h"

psccd_test_state_type_t tc_attach_set_automatic(int state);
psccd_test_state_type_t tc_attach_set_manual(int state);
psccd_test_state_type_t tc_attach_set_default(int state);
psccd_test_state_type_t tc_attach_set_automatic_fail(int state);
psccd_test_state_type_t tc_attach_query_mode(int state);
psccd_test_state_type_t tc_attach_query_status(int state);
psccd_test_state_type_t tc_attach(int state);
psccd_test_state_type_t tc_attach_fail(int state);
psccd_test_state_type_t tc_attach_already_attached(int state);
psccd_test_state_type_t tc_attach_detach(int state);
psccd_test_state_type_t tc_attach_detach_received(int state);
psccd_test_state_type_t tc_attach_detach_fail(int state);
psccd_test_state_type_t tc_attach_already_detached(int state);
psccd_test_state_type_t tc_configure_attach_mode(int state);
psccd_test_state_type_t tc_configure_attach_mode_fail_1(int state);
psccd_test_state_type_t tc_configure_attach_mode_fail_2(int state);
psccd_test_state_type_t tc_configure_ping_block_mode(int state);
psccd_test_state_type_t tc_configure_ping_block_mode_fail_1(int state);
psccd_test_state_type_t tc_configure_ping_block_mode_fail_2(int state);


#endif //TC_MAL_ATTACH_H_
