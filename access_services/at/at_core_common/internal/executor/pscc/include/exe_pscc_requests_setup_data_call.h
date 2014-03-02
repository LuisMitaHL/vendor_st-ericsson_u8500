/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_PSCC_REQUESTS_SETUP_DATA_CALL_H
#define EXE_PSCC_REQUESTS_SETUP_DATA_CALL_H 1


#include "libstecom.h"
#include "mpl_config.h"
#include "pscc_msg.h"
#include "stecom_i.h"
#include "exe_pscc.h"
#include "exe_request_record.h"

/* Handler functions shared by several scenarios (step 1-7) */
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_1_send_pscc_create_request(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_2_3_receive_pscc_create_response(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_4_5_receive_pscc_set_response(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_1_step_6_receive_pscc_connect_response(exe_request_record_t *record_p);

/* Scenario 0 - successful establishment of data call (8-12) */
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_step_8_9_receive_pscc_connected_event(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_setup_data_call_scenario_0_step_10_11_12_receive_pscc_get_response(exe_request_record_t *record_p);

/* Scenario 1 - unsuccessful establishment of data call (8-14) */
exe_pscc_result_t pscc_request_setup_data_call_scenario_1_step_10_11_receive_pscc_get_response(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_setup_data_call_scenario_1_step_12_13_14_receive_pscc_destroy_response(exe_request_record_t *record_p);

/* Get connection status */
exe_pscc_result_t pscc_request_setup_data_call_send_pscc_get(exe_request_record_t *record_p);

/* Query connection */
exe_pscc_result_t pscc_request_setup_data_call_send_pscc_get_response(exe_request_record_t *record_p);

#endif /* EXE_PSCC_REQUESTS_SETUP_DATA_CALL_H */
