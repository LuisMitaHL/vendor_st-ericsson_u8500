/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_PSCC_REQUESTS_SEND_DATA_UPLINK_CALL_H_
#define EXE_PSCC_REQUESTS_SEND_DATA_UPLINK_CALL_H_

#include "libstecom.h"
#include "mpl_config.h"
#include "pscc_msg.h"
#include "stecom_i.h"
#include "exe_pscc.h"
#include "exe_request_record.h"

/* Scenario - Generation of data uplink command */
bool pscc_request_send_data_uplink_call_handle_request(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_send_data_uplink_call_step_1_send_pscc_set_request(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_send_data_uplink_call_step_2_send_pscc_generate_uplink_data_request(exe_request_record_t *record_p);

#endif /* EXE_PSCC_REQUESTS_SEND_DATA_UPLINK_CALL_H_ */
