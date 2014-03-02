/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "exe_request_record.h"

#ifndef EXE_CSPSA_REQUESTS_H
#define EXE_CSPSA_REQUESTS_H 1

exe_request_result_t request_write_value(exe_request_record_t *record_p);
exe_request_result_t request_read_value(exe_request_record_t *record_p);

#endif /* EXE_CSPSA_REQUESTS_H */
