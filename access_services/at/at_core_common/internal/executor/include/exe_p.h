/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_P_H
#define EXE_P_H 1

#include "exe_last_fail_cause.h"

#ifdef EXE_USE_COPS_SERVICE
#include "exe_cops_client.h"
#endif

#ifdef EXE_USE_CN_SERVICE
#include "exe_cn_client.h"
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

#ifdef EXE_USE_SIMPB_SERVICE
#include "exe_simpb_client.h"
#endif

struct exe_s {
#ifdef EXE_USE_COPS_SERVICE
    copsclient_t *copsclient_p;
#endif

#ifdef EXE_USE_CN_SERVICE
    cn_context_t *cn_context_p;
    exe_vtd_t dtmf_duration;
#endif
#ifdef EXE_USE_SMS_SERVICE
    smsclient_t *smsclient_p;
#endif
#ifdef EXE_USE_PSCC_SERVICE
    psccclient_t *psccclient_p;
#endif
#ifdef EXE_USE_SIM_SERVICE
    sim_client_t *simclient_p;
#endif
#ifdef EXE_USE_SIMPB_SERVICE
    simpb_client_t *simpbclient_p;
#endif
    exe_last_fail_cause_t last_fail;
};

#endif

