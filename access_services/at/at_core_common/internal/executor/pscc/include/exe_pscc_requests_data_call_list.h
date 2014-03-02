/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_PSCC_REQUESTS_DATA_CALL_LIST_H
#define EXE_PSCC_REQUESTS_DATA_CALL_LIST_H 1


#include "exe_request_record.h"
#include "exe_pscc.h"

#include "libstecom.h"
#include "mpl_config.h"
#include "pscc_msg.h"
#include "stecom_i.h"


exe_pscc_result_t pscc_data_call_list_step_2_3_receive_pscc_list_all_connections_response(exe_request_record_t *record_p);
exe_pscc_result_t pscc_data_call_list_step_4_5_receive_pscc_get_response(exe_request_record_t *record_p);
exe_pscc_result_t exe_pscc_list_all_connections_get(exe_request_record_t *record_p, int current_index);
exe_pscc_result_t exe_pscc_list_all_connections_add_from_iadb(exe_request_record_t *record_p);


#endif /* EXE_PSCC_REQUESTS_DATA_CALL_LIST_H */
