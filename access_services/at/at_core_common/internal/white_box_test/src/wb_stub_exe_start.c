/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>

#include "whitebox.h"
#include "wb_test.h"
#include "exe.h"
#include "exe_p.h"
#include "exe_start.h"
#include "exe_internal.h"

#ifdef EXE_USE_CN_SERVICE
#include "exe_cn_client.h"
#endif

#ifdef EXE_USE_COPS_SERVICE
#include "exe_cops_client.h"
#endif

#ifdef EXE_USE_SMS_SERVICE
#include "exe_sms_client.h"
#endif

#ifdef EXE_USE_PSCC_SERVICE
#include "exe_pscc_client.h"
#endif

#ifdef EXE_USE_SIM_SERVICE
#include "exe_sim_client.h"
#endif

struct smsclient_s {
    int fd_request;
    int fd_event;
};


bool exe_start_init(exe_t *exe_p)
{
    return true;
}

bool exe_start_open_sessions(exe_t *exe_p)
{
    ATC_LOG_I("exe_start_open_sessions, enter %p", exe_p);

#ifdef EXE_USE_CN_SERVICE
    exe_p->cn_context_p = cn_client_open_session();

    if (NULL == exe_p->cn_context_p) {
        ATC_LOG_E("Error on open modem");
        goto error;
    }

    ATC_LOG_I("modem session opened");
#endif

#ifdef EXE_USE_COPS_SERVICE
    exe_p->copsclient_p = copsclient_open_session();
#endif
#ifdef EXE_USE_PSCC_SERVICE
    EXECUTOR.pscc_request_id = -1; /* No request pending */
    exe_p->psccclient_p = psccclient_open_session();
    /* Respond to the pscc_init_handler request that will be sent when starting the pscc client */
    wb_pscc_respond_with_just_client_tag(pscc_init_handler);

    if (NULL != exe_p->psccclient_p) {
        exe_start_set_status(EXE_START_PSCC, EXE_START_CONNECTED_READY);
    } else {
        exe_start_set_status(EXE_START_PSCC, EXE_START_NOT_CONNECTED);
    }

#endif
#ifdef EXE_USE_SIM_SERVICE
    exe_p->simclient_p = sim_client_open_session();

    if (NULL != exe_p->simclient_p) {
        exe_start_set_status(EXE_START_SIM, EXE_START_CONNECTED_READY);
    } else {
        exe_start_set_status(EXE_START_SIM, EXE_START_NOT_CONNECTED);
    }

#endif
#ifdef EXE_USE_SMS_SERVICE
    exe_p->smsclient_p = smsclient_open_session();

    if (NULL != exe_p->smsclient_p) {
        exe_start_set_status(EXE_START_SMS, EXE_START_CONNECTED_READY);
    } else {
        exe_start_set_status(EXE_START_SMS, EXE_START_NOT_CONNECTED);
    }

#endif
#ifdef EXE_USE_SIMPB_SERVICE
    exe_p->simpbclient_p = simpb_client_open_session();

    if (NULL != exe_p->simpbclient_p) {
        exe_start_set_status(EXE_START_SIMPB, EXE_START_CONNECTED_READY);
    } else {
        exe_start_set_status(EXE_START_SIMPB, EXE_START_NOT_CONNECTED);
    }

#endif
    return true;
error:
    return false;
}

exe_start_status_t exe_start_get_status(exe_start_services_t service)
{
    return EXE_START_CONNECTED_READY;
}

void exe_start_set_status(exe_start_services_t service, exe_start_status_t status)
{
}
