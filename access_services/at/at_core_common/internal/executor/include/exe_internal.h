/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __exe_internal_h__
#define __exe_internal_h__ (1)


#include "exe_request_record.h"

#ifdef EXE_USE_COPS_SERVICE
#include "exe_cops_client.h"
#endif
#ifdef EXE_USE_CN_SERVICE
#include "exe_cn_client.h"
#endif
#ifdef EXE_USE_PSCC_SERVICE
#include "exe_pscc_client.h"
#endif
#ifdef EXE_USE_SMS_SERVICE
#include "exe_sms_client.h"
#endif
#ifdef EXE_USE_SIM_SERVICE
#include "exe_sim_client.h"
#endif
#ifdef EXE_USE_SIMPB_SERVICE
#include "exe_simpb_client.h"
#endif

#define EXE_INVALID_LAI_1 (0x0000) /* Reserved value to indicate that no valid LAI exists, as defined by 3GPP 23.003 */
#define EXE_INVALID_LAI_2 (0xFFFE) /* Reserved value to indicate that no valid LAI exists, as defined by 3GPP 23.003 */


#ifdef EXE_USE_COPS_SERVICE
copsclient_t *exe_get_copsclient(exe_t *exe_p);
#endif
#ifdef EXE_USE_CN_SERVICE
cn_context_t *exe_get_cn_client(exe_t *exe);
exe_vtd_t exe_get_dtmf_duration(exe_t *exe_p);
void exe_set_dtmf_duration(exe_t *exe_p, exe_vtd_t dtmf_duration);
#endif
#ifdef EXE_USE_PSCC_SERVICE
psccclient_t *exe_get_psccclient(exe_t *exe_p);
#endif
#ifdef EXE_USE_SMS_SERVICE
smsclient_t *exe_get_smsclient(exe_t *exe_p);
#endif
#ifdef EXE_USE_SIM_SERVICE
sim_client_t *exe_get_sim_client(exe_t *exe);
#endif
#ifdef EXE_USE_SIMPB_SERVICE
simpb_client_t *exe_get_simpb_client(exe_t *exe);
#endif


exe_t *exe_get_exe(void);
void exe_set_last_fail_cause(exe_t *exe_p, void *last_fail_cause_p);
void exe_get_last_fail_cause(exe_t *exe_p, char *error_report_p);

/* Requester states common for all clients.
 * Range 0 - 9 is reserved for common states. */
#define EXE_STATE_REQUEST     0       /* Initial request from caller. */
#define EXE_STATE_RESPONSE    1       /* Generic response state. */

void exe_request_complete(exe_request_record_t *request_p, exe_request_result_t result, void *response_p);
void exe_event(exe_event_t event, void *event_data_p);

/* Requester states common for all clients. */
#define EXE_REQUEST     0       /* Initial request from caller. */
#define EXE_RESPONSE    1       /* Generic response state. */
#define EXE_ABORT       999     /* Abort the execution of the request. */

bool exe_check(bool a, char *file, int line);
#define EXE_CHECK(a) exe_check(a, __FILE__, __LINE__)
#define EXE_CHECK_GOTO_ERROR(a) {if (!exe_check(a, __FILE__, __LINE__))  goto error;}

#endif /* __exe_internal_h__ */
