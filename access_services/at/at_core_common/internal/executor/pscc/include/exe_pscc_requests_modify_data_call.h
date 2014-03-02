/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef EXE_PSCC_REQUESTS_MODIFY_DATA_CALL_H
#define EXE_PSCC_REQUESTS_MODIFY_DATA_CALL_H 1


#include "exe_request_record.h"
#include "libstecom.h"
#include "mpl_config.h"
#include "pscc_msg.h"
#include "stecom_i.h"

/* Handler functions shared by request modify scenario 0 and 1 (step 1-2) */
exe_pscc_result_t pscc_request_modify_data_call_scenario_0_1_step_1_send_pscc_modify_request(exe_request_record_t *record_p);


#endif /* EXE_PSCC_REQUESTS_MODIFY_DATA_CALL_H */
