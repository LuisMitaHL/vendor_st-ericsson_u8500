/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Test cases include file
 *
 * Author: Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *
 */

#ifndef TESTCASES_H_
#define TESTCASES_H_

simpb_test_state_type_t test_read_record(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t test_update_record(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t test_get_file_info(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t test_get_supported_pbs(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t test_set_pb(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t test_get_pb(int state, ste_simpb_t **ste_simpb_pp);

simpb_test_state_type_t test_unsolicted_events_CAT_PC_REFRESH_INDs(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t test_handle_pbr_caching(int state, ste_simpb_t **ste_simpb_pp);
simpb_test_state_type_t test_unsolicted_events_SIM_STATUS_STATE_CHANGED_IND(int state, ste_simpb_t **ste_simpb_pp);

#endif /* TESTCASES_H_ */
