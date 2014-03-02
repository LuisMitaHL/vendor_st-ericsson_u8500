/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_PSCC_REQUESTS_DEACTIVATE_DATA_CALL_H
#define EXE_PSCC_REQUESTS_DEACTIVATE_DATA_CALL_H 1

#include "exe_request_record.h"
#include "libstecom.h"
#include "mpl_config.h"
#include "pscc_msg.h"
#include "stecom_i.h"

exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_1_send_pscc_disconnect_request(exe_request_record_t *record_p);
exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_2_receive_disconnect_response(exe_request_record_t *record_p);
exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_3_4_receive_disconnected_event(exe_request_record_t *record_p);
exe_request_result_t pscc_request_deactivate_data_call_scenario_0_step_5_6_7_receive_destroy_response(exe_request_record_t *record_p);


#endif /* EXE_PSCC_REQUESTS_DEACTIVATE_DATA_CALL_H */
