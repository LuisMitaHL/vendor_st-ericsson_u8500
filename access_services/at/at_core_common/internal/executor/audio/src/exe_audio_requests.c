/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <assert.h>
#include <string.h>

#include "atc_log.h"

#include "exe.h"
#include "exe_internal.h"
#include "exe_request_record_p.h"
#include "exe_requests.h"
#ifdef EXE_USE_CN_SERVICE
#include "exe_cn_requests.h"
#endif
#ifndef _POSIX_SOURCE
/* Done to avoid redefinition of struct timeval and struct timespec when including ste_adm_client.h */
#define _POSIX_SOURCE
#endif

#include "ste_adm_client.h"

/********************************************************************
 * AUDIO
 ********************************************************************/

exe_request_result_t request_mute_set(exe_request_record_t *record_p)
{
    exe_cmut_response_t *cmut_response = record_p->request_data_p;
    int result;

    if (NULL == cmut_response) {
        ATC_LOG_E("request_mute_set: cmut_response is NULL");
        goto error;
    }

    result = ste_adm_client_set_cscall_upstream_mute(cmut_response->value);

    if (0 != result) {
        ATC_LOG_E("request_mute_set: ste_adm_client_set_cscall_upstream_mute failed with return code %d", result);
        goto error;
    }

    return EXE_SUCCESS;
error:
    return EXE_FAILURE;
}

exe_request_result_t request_mute_read(exe_request_record_t *record_p)
{
    exe_cmut_response_t *cmut_response = record_p->request_data_p;
    int result;
    int mute_enabled;

    result = ste_adm_client_get_cscall_upstream_mute(&mute_enabled);

    if (0 != result) {
        ATC_LOG_E("request_mute_read: ste_adm_client_get_cscall_upstream_mute failed with return code %d", result);
        goto error;
    }

    cmut_response->value = mute_enabled;
    record_p->response_data_p = (void *)cmut_response;
    return EXE_SUCCESS;
error:
    return EXE_FAILURE;
}

exe_request_result_t request_set_tty_mode(exe_request_record_t *record_p)
{
    int result = -1;
    ste_adm_tty_mode_t tty_mode;

    if (EXE_STATE_REQUEST == record_p->state) {

        exe_etty_t *etty_p = record_p->request_data_p;

        if (NULL == etty_p) {
            ATC_LOG_E("etty_p is NULL");
            goto error;
        }

        tty_mode = (ste_adm_tty_mode_t)etty_p->n;

        /* Synchronous request */
        result = ste_adm_client_set_cscall_tty_mode(tty_mode);

        if (0 != result) {
            ATC_LOG_E("ste_adm_client_set_cscall_tty_mode returned error:  %d", result);
            goto error;
        }
    }

#ifdef EXE_USE_CN_SERVICE
    /* The mode must also be set in the modem so forwarding the request to the mal handler */
    return request_modem_property_set(record_p);
#endif
error:
    return EXE_FAILURE;
}

exe_request_result_t request_get_tty_mode(exe_request_record_t *record_p)
{
    int result = -1;
    ste_adm_tty_mode_t tty_mode;
    exe_etty_t *response_p = (exe_etty_t *)record_p->request_data_p;

    if (NULL == response_p) {
        ATC_LOG_E("response_p is NULL");
        goto error;
    }

    /* Synchronous request */
    result = ste_adm_client_get_cscall_tty_mode(&tty_mode);

    if (0 != result) {
        ATC_LOG_E("ste_adm_client_get_cscall_tty_mode returned error:  %d", result);
        goto error;
    }

    response_p->n = (int)tty_mode;

    return EXE_SUCCESS;
error:
    return EXE_FAILURE;
}
