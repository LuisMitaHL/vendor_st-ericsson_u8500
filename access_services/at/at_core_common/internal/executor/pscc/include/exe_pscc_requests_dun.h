/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_PSCC_REQUESTS_DUN_H_
#define EXE_PSCC_REQUESTS_DUN_H_

#include "exe_request_record.h"
#include "libstecom.h"
#include "mpl_config.h"
#include "pscc_msg.h"
#include "stecom_i.h"

/* Connect DUN */
exe_pscc_result_t pscc_request_dun_step_1_send_pscc_create_request(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_dun_step_2_3_receive_pscc_create_response(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_dun_step_4_5_send_sterc_create_request(exe_request_record_t *record_p, int cid);
exe_pscc_result_t pscc_request_dun_step_6_send_sterc_receive_create_response(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_dun_step_7_8_receive_sterc_event(exe_request_record_t *record_p, int sterc_handle);
exe_pscc_result_t pscc_request_dun_step_9_10_receive_sterc_destroy_response(exe_request_record_t *record_p);
exe_pscc_result_t pscc_request_dun_step_11_receive_pscc_destroy_response(exe_request_record_t *record_p);


#endif /* EXE_PSCC_REQUESTS_DUN_H_ */

