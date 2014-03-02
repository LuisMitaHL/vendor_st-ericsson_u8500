/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>

#include "atc_log.h"

#include "exe_p.h"
#include "exe_internal.h"
#include "exe_last_fail_cause.h"
#include "exe_requests.h"
#include "exe_request_record_p.h"


/********************************************************************
 * Misc
 ********************************************************************
 */

/*
 * Reads out a text string that describes the latest fail cause or disconnection
 * cause for network and call.
 */
exe_request_result_t request_ceer(exe_request_record_t *record_p)
{
    exe_ceer_request_t *request_data_p = (exe_ceer_request_t *)record_p->request_data_p;
    char *ceer_p = NULL;

    ATC_LOG_I("request_ceer");

    EXE_CHECK_GOTO_ERROR(NULL != record_p->request_data_p);

    ceer_p = &((exe_ceer_request_t *)record_p->request_data_p)->ceer_string[0];

    exe_get_last_fail_cause(record_p->exe_p, ceer_p);

    return EXE_SUCCESS;

error:
    return EXE_FAILURE;
}
