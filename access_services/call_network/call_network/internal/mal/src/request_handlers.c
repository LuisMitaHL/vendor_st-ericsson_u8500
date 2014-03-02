/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>

#ifdef USE_MAL_CS
#include "mal_call.h"
#include "mal_ss.h"
#endif

#ifdef USE_MAL_GSS
#include "mal_gss.h"
#endif

#ifdef USE_MAL_MCE
#include "mal_mce.h"
#endif

#ifdef USE_MAL_NET
#include "mal_net.h"
#endif

#ifdef USE_MAL_MIS
#include "mal_mis.h"
#endif

#ifdef USE_MAL_NVD
#include "mal_nvd.h"
#endif

#ifdef USE_MAL_RF
#include "mal_rf.h"
#endif

#ifdef USE_MAL_MTE
#include "mal_mte.h"
#endif

#ifdef USE_MAL_FTD
#include "mal_ftd.h"
#endif

#include "mal_utils.h"

#include "cn_client.h"
#include "cn_log.h"

#include "cnsocket.h"
#include "cnserver.h"

#include "cn_log.h"
#include "cn_utils.h"
#include "request_handling.h"
#include "cn_mal_assertions.h" /* do NOT remove this inclusion! */
#include "cn_mal_utils.h"
#include "cn_mal_dial_dtmf_handler.h"
#include "cn_pdc_ecc_list_handling.h"
#include "cn_ss_command_handling.h"
#include "cn_mal_utils.h"
#include "cn_pdc_internal.h"
#include "cn_event_trigger_level.h"
#include "cn_internal_handlers.h"
#include "plmn_main.h"
#include "cn_data_types.h"

#include "msup-client.h"

#ifndef ENABLE_MODULE_TEST
#include "cn_timer.h"
#endif

#ifdef USE_MAL_NET
/* Cached registration info used during cell handover */
extern int s_reg_info_timer_active;
extern cn_registration_info_t s_reg_info;

#endif

#ifdef USE_MAL_CS

typedef void (*ss_result_function)(request_status_t, void *, request_record_t *, cn_bool_t additional_results);
typedef request_status_t (*ss_command_function)(void *, request_record_t *);

typedef struct {
    ss_command_function command_function_p;
    ss_result_function  result_function_p;
    cn_ss_mal_command_t command;
    void *data_p;
    cn_bool_t          additional_results;
} cn_ss_mal_info_t;

/* Constants */
#define MAX_DTMF_DURATION_MS    (1000L)
#define MAX_DTMF_PAUSE_MS       (1000L)

#endif /* USE_MAL_CS */

#define L1CMD_ANTENNA_PATH                                 1 /* L1 command for selecting antenna path */
#define L1CMD_UARFCN_SCAN                                  2 /* L1 command for selecting UARFCN scan frequency */
#define INFO_PP_WCDMA_RX_DIVERSITY_SUPPORT_ID           5568 /* ppflag id for antenna path selection */
#define INFO_PP_WCDMA_RX_DIVERSITY_SUPPORT_VALUE           3 /* ppflag value for antenna path selection */

/* request states */
#define REQUEST_STATE_SET_MODE_MANUAL_RESPONSE          1000
#define REQUEST_STATE_SET_MODE_AUTOMATIC_RESPONSE       1001
#define REQUEST_STATE_MANUAL_REGISTRATION_RESPONSE      1002
#define REQUEST_STATE_HANGUP_LOOP                       1003
#define REQUEST_STATE_CALL_CLIR                         1004
#define REQUEST_STATE_SET_ANTENNA_PPFLAG_RESPONSE       1005
#define REQUEST_STATE_SELECT_ANTENNA_PATH_RESPONSE      1006
#define REQUEST_STATE_TEST_CARRIER_RESPONSE             1007
#define REQUEST_STATE_MODEM_SLEEP_RESPONSE              1008
#define REQUEST_STATE_MODEM_FORCED_SLEEP_RESPONSE       1009

#ifdef USE_MAL_FTD

#define FTD_REQUEST_ONESHOT_MEASUREMENT         0
#define FTD_REQUEST_START_PERIODIC_MEASUREMENTS 1
#define FTD_REQUEST_STOP_PERIODIC_MEASUREMENTS  2
#define FTD_REQUEST_RESET                       3
#endif /* USE_MAL_FTD */

#define CN_CALL_FORWARDING_REPLY_TIME_DEFAULT  20

/* static function */
static request_status_t query_colr_status(void *data_p, request_record_t *record_p);

request_status_t handle_request_rf_on(void *data_p,
                                      request_record_t *record_p)
{
#ifndef USE_MAL_MCE
    (void) data_p;
    CN_LOG_I("MAL MCE disabled");
    send_response(record_p->client_id, CN_RESPONSE_RF_ON, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_modem_status_t modem_status;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_mce_rf_on -> request");
        result = mal_mce_rf_on(request_record_get_modem_tag(record_p));

        if (MAL_MCE_SUCCESS != result) {
            CN_LOG_E("mal_mce_rf_on failed!");
            send_response(record_p->client_id, CN_RESPONSE_RF_ON, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_mce_rf_on <- response");

        if (MAL_MCE_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mce_rf_on (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_RF_ON, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_mce_rf_on not found!");
            send_response(record_p->client_id, CN_RESPONSE_RF_ON, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        modem_status = *((mal_mce_status *)record_p->response_data_p);
        send_response(record_p->client_id, CN_RESPONSE_RF_ON, CN_SUCCESS, record_p->client_tag, sizeof(modem_status), &modem_status);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_RF_ON, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_MCE */
}

request_status_t handle_request_rf_off(void *data_p,
                                       request_record_t *record_p)
{
#ifndef USE_MAL_MCE
    (void) data_p;
    CN_LOG_I("MAL MCE disabled");
    send_response(record_p->client_id, CN_RESPONSE_RF_OFF, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_modem_status_t modem_status;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_mce_rf_off -> request");
        result = mal_mce_rf_off(request_record_get_modem_tag(record_p));

        if (MAL_MCE_SUCCESS != result) {
            CN_LOG_E("mal_mce_rf_off failed!");
            send_response(record_p->client_id, CN_RESPONSE_RF_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_mce_rf_off <- response");

        if (MAL_MCE_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mce_rf_off (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_RF_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_mce_rf_off not found!");
            send_response(record_p->client_id, CN_RESPONSE_RF_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        modem_status = *((mal_mce_status *)record_p->response_data_p);
        send_response(record_p->client_id, CN_RESPONSE_RF_OFF, CN_SUCCESS, record_p->client_tag, sizeof(modem_status), &modem_status);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_RF_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;

#endif /* USE_MAL_MCE */
}

request_status_t handle_request_rf_status(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_MCE
    (void) data_p;
    CN_LOG_I("MAL MCE disabled");
    send_response(record_p->client_id, CN_RESPONSE_RF_STATUS, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_rf_status_t rf_status;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_mce_rf_status_query -> request");
        result = mal_mce_rf_status_query(request_record_get_modem_tag(record_p));

        if (MAL_MCE_SUCCESS != result) {
            CN_LOG_E("mal_mce_rf_status_query failed!");
            send_response(record_p->client_id, CN_RESPONSE_RF_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_mce_rf_status_query <- response");

        if (MAL_MCE_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mce_rf_status_query (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_RF_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_mce_rf_status_query not found!");
            send_response(record_p->client_id, CN_RESPONSE_RF_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        rf_status = *((mal_mce_rf_status *)record_p->response_data_p);
        send_response(record_p->client_id, CN_RESPONSE_RF_STATUS, CN_SUCCESS, record_p->client_tag, sizeof(rf_status), &rf_status);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_RF_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;

#endif /* USE_MAL_MCE */
}

request_status_t handle_request_registration_control(void *data_p,
        request_record_t *record_p)
{


#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL  NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_CONTROL, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_sint32_t result = 0;
        mal_net_nw_access_conf_data mal_request_data;
        cn_request_network_access_config_data_t *cn_request_data_p = NULL;

        CN_LOG_D("mal_net_configure_network_access -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        cn_request_data_p = (cn_request_network_access_config_data_t *) data_p;

        mal_request_data.reg_conf = cn_request_data_p->config_data.registration_conf;
        mal_request_data.roam_conf = cn_request_data_p->config_data.roaming_conf;

        result = mal_net_configure_network_access(&mal_request_data, request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_configure_network_access failed!");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_CONTROL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_configure_network_access <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_configure_network_access (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_CONTROL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_CONTROL, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_CONTROL, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_modem_reset(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_MCE
    (void) data_p;
    CN_LOG_I("MAL MCE disabled");
    send_response(record_p->client_id, CN_RESPONSE_RESET_MODEM, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    /*** NOTE: Synchronous MAL call ***/
    CN_LOG_D("mal_mce_reset -> synchronous request");

    result = mal_mce_reset(request_record_get_modem_tag(record_p));

    if (MAL_MCE_SUCCESS != result) {
        CN_LOG_E("mal_mce_reset failed!");
        send_response(record_p->client_id, CN_RESPONSE_RESET_MODEM, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_RESET_MODEM, CN_SUCCESS, record_p->client_tag, 0, NULL);

    return REQUEST_STATUS_DONE;

error:
    return status;
#endif /* USE_MAL_MCE */
}

request_status_t handle_request_modem_reset_with_dump(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_MTE
    (void) data_p;
    CN_LOG_I("MAL MTE disabled");
    send_response(record_p->client_id, CN_RESPONSE_RESET_MODEM_WITH_DUMP, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_request_reset_cpu_type_t *request_data_p = (cn_request_reset_cpu_type_t *) data_p;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    CN_LOG_D("mal_mte_reset_modem_with_dump -> request");

    result = mal_mte_reset_modem_with_dump(request_data_p->type, request_record_get_modem_tag(record_p));

    if (MAL_MTE_SUCCESS != result) {
        CN_LOG_E("mal_mte_reset_modem_with_dump failed!");
        send_response(record_p->client_id, CN_RESPONSE_RESET_MODEM_WITH_DUMP, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_RESET_MODEM_WITH_DUMP, CN_SUCCESS, record_p->client_tag, 0, NULL);

    return REQUEST_STATUS_DONE;

error:
    return status;
#endif /* USE_MAL_MTE */
}

request_status_t handle_request_set_preferred_network_type(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_GSS
    (void) data_p;
    CN_LOG_I("MAL GSS disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_request_set_preferred_network_type_t *request_data_p = (cn_request_set_preferred_network_type_t *) data_p;
    request_status_t status = REQUEST_STATUS_ERROR;
    mal_call_context *call_context_p = NULL;
    cn_sint32_t nr_of_calls = 0;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {

        CN_LOG_D("mal_call_request_getcalllist -> synchronous request");

        /*** NOTE: Synchronous MAL calls ***/
        mal_call_request_getcalllist(NULL, (void **)&call_context_p, &nr_of_calls);

        if (call_context_p) {
            mal_call_request_freecalllist(call_context_p);
        }

        if (nr_of_calls == 0) {

            CN_LOG_D("mal_gss_set_preferred_network_type -> request");
            result = mal_gss_set_preferred_network_type(request_data_p->type, request_record_get_modem_tag(record_p));

            if (MAL_GSS_MODE_NOT_SUPPORTED == result || MAL_GSS_SERVICE_NOT_ALLOWED == result) {
                CN_LOG_E("mal_gss_set_preferred_network_type failed, not supported");
                send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_REQUEST_NOT_SUPPORTED, record_p->client_tag, 0, NULL);
                goto error;
            }

            if (MAL_GSS_SUCCESS != result) {
                CN_LOG_E("mal_gss_set_preferred_network_type failed");
                send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            record_p->state = REQUEST_STATE_RESPONSE;

            return REQUEST_STATUS_PENDING;
        } else {
            CN_LOG_E("mal_gss_set_preferred_network_type: Operation Not Supported during an Ongoing CS Call");
            send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_gss_set_preferred_network_type <- response");

        if (MAL_GSS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_gss_set_preferred_network_type (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:

    return status;
#endif /* USE_MAL_GSS */
}

request_status_t handle_request_get_preferred_network_type(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_GSS
    (void) data_p;
    CN_LOG_I("MAL GSS disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_network_type_t network_type;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_gss_get_preferred_network_type -> request");
        result = mal_gss_get_preferred_network_type(request_record_get_modem_tag(record_p));

        if (MAL_GSS_SUCCESS != result) {
            CN_LOG_E("mal_gss_get_preferred_network_type failed");
            send_response(record_p->client_id, CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_gss_get_preferred_network_type <- response");

        if (MAL_GSS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_gss_get_preferred_network_type (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_gss_get_preferred_network_type not found!");
            send_response(record_p->client_id, CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        network_type = *((mal_gss_network_type *)record_p->response_data_p);

        send_response(record_p->client_id, CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_SUCCESS, record_p->client_tag, sizeof(network_type), &network_type);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_GET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_GSS */
}

request_status_t handle_request_registration_state_normal(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_registration_info_t *reg_info_p = NULL;
    mal_net_registration_info *mal_reg_info_p = NULL;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        if (0 != s_reg_info_timer_active) {
            /* Cell handover (probably) in progress. Use cached registration formation. */
            CN_LOG_I("Using cached registration information -> synchronous");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_SUCCESS, record_p->client_tag, sizeof(s_reg_info), &s_reg_info);

            return REQUEST_STATUS_DONE;
        } else {
            CN_LOG_D("mal_net_req_registration_state_normal -> request");
            result = mal_net_req_registration_state_normal(request_record_get_modem_tag(record_p));

            if (MAL_NET_SUCCESS != result) {
                CN_LOG_E("mal_net_req_registration_state_normal failed");
                send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            record_p->state = REQUEST_STATE_RESPONSE;

            return REQUEST_STATUS_PENDING;
        }
    }
    case REQUEST_STATE_RESPONSE: {
        const char *long_operator_name_p = NULL;
        const char *short_operator_name_p = NULL;
        CN_LOG_D("mal_net_req_registration_state_normal <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_req_registration_state_normal (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_req_registration_state_normal not found!");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (0 != s_reg_info_timer_active) {
            /* Cell handover (probably) in progress. Use cached registration formation. */
            CN_LOG_I("Using cached registration information -> synchronous");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_SUCCESS, record_p->client_tag, sizeof(s_reg_info), &s_reg_info);

            return REQUEST_STATUS_DONE;
        }

        reg_info_p = calloc(1, sizeof(cn_registration_info_t));

        if (!reg_info_p) {
            CN_LOG_E("calloc failed for reg_info_p!");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        mal_reg_info_p = (mal_net_registration_info *)record_p->response_data_p;
        reg_info_p->reg_status = mal_reg_info_p->reg_status;
        reg_info_p->gprs_attached = mal_reg_info_p->gprs_attached;
        reg_info_p->cs_attached = mal_reg_info_p->cs_attached;
        reg_info_p->rat = mal_reg_info_p->rat;
        reg_info_p->cid = mal_reg_info_p->cid;
        reg_info_p->lac = mal_reg_info_p->lac;

        if (mal_reg_info_p->mcc_mnc_string) {
            (void)strncpy(reg_info_p->mcc_mnc, (const char *)mal_reg_info_p->mcc_mnc_string, sizeof(reg_info_p->mcc_mnc));

            if (plmn_get_operator_name((const char *)mal_reg_info_p->mcc_mnc_string, mal_reg_info_p->lac,
                                       &long_operator_name_p, &short_operator_name_p) > 0) {

                if (long_operator_name_p) {
                    (void)strncpy(reg_info_p->long_operator_name, long_operator_name_p, sizeof(reg_info_p->long_operator_name));
                }

                if (short_operator_name_p) {
                    (void)strncpy(reg_info_p->short_operator_name, short_operator_name_p, sizeof(reg_info_p->short_operator_name));
                }
            }
        }

        if (cn_pdc_set_current_mcc((char *) mal_reg_info_p->mcc_mnc_string)) {
            if (!cn_pdc_util_clear_emergency_numbers_by_origin(CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED |
                    CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED |
                    CN_EMERGENCY_NUMBER_ORIGIN_SIM |
                    CN_EMERGENCY_NUMBER_ORIGIN_NETWORK |
                    CN_EMERGENCY_NUMBER_ORIGIN_MEMORY, TRUE)) {
                CN_LOG_E("new mcc, couldn't clear number network list!");
                free(reg_info_p);
                goto error;
            }
        }

        send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_SUCCESS, record_p->client_tag, sizeof(*reg_info_p), reg_info_p);

        /* Keep a copy of the latest reported modem registration information */
        memcpy(&s_reg_info, reg_info_p, sizeof(s_reg_info));

        free(reg_info_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_NORMAL, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_registration_state_gprs(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_registration_info_t *reg_info_p = NULL;
    mal_net_registration_info *mal_reg_info_p = NULL;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        if (0 != s_reg_info_timer_active) {
            /* Cell handover (probably) in progress. Use cached registration formation. */
            CN_LOG_I("Using cached registration information -> synchronous");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_SUCCESS, record_p->client_tag, sizeof(s_reg_info), &s_reg_info);

            return REQUEST_STATUS_DONE;
        } else {
            CN_LOG_D("mal_net_req_registration_state_gprs -> request");
            result = mal_net_req_registration_state_gprs(request_record_get_modem_tag(record_p));

            if (MAL_NET_SUCCESS != result) {
                CN_LOG_E("mal_net_req_registration_state_gprs failed");
                send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            record_p->state = REQUEST_STATE_RESPONSE;

            return REQUEST_STATUS_PENDING;
        }
    }
    case REQUEST_STATE_RESPONSE: {
        const char *long_operator_name_p = NULL;
        const char *short_operator_name_p = NULL;
        CN_LOG_D("mal_net_req_registration_state_gprs <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_req_registration_state_gprs (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_req_registration_state_gprs not found!");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (0 != s_reg_info_timer_active) {
            /* Cell handover (probably) in progress. Use cached registration formation. */
            CN_LOG_I("Using cached registration information -> synchronous");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_SUCCESS, record_p->client_tag, sizeof(s_reg_info), &s_reg_info);

            return REQUEST_STATUS_DONE;
        }

        reg_info_p = calloc(1, sizeof(cn_registration_info_t));

        if (!reg_info_p) {
            CN_LOG_E("calloc failed for reg_info_p!");
            send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        mal_reg_info_p = (mal_net_registration_info *)record_p->response_data_p;
        reg_info_p->reg_status = mal_reg_info_p->reg_status;
        reg_info_p->gprs_attached = mal_reg_info_p->gprs_attached;
        reg_info_p->cs_attached = mal_reg_info_p->cs_attached;
        reg_info_p->rat = mal_reg_info_p->rat;
        reg_info_p->cid = mal_reg_info_p->cid;
        reg_info_p->lac = mal_reg_info_p->lac;

        if (mal_reg_info_p->mcc_mnc_string) {
            (void)strncpy(reg_info_p->mcc_mnc, (const char *)mal_reg_info_p->mcc_mnc_string, sizeof(reg_info_p->mcc_mnc));

            if (plmn_get_operator_name((const char *)mal_reg_info_p->mcc_mnc_string, mal_reg_info_p->lac,
                                       &long_operator_name_p, &short_operator_name_p) > 0) {

                if (long_operator_name_p) {
                    (void)strncpy(reg_info_p->long_operator_name, long_operator_name_p, sizeof(reg_info_p->long_operator_name));
                }

                if (short_operator_name_p) {
                    (void)strncpy(reg_info_p->short_operator_name, short_operator_name_p, sizeof(reg_info_p->short_operator_name));
                }
            }
        }

        if (cn_pdc_set_current_mcc((char *) mal_reg_info_p->mcc_mnc_string)) {
            if (!cn_pdc_util_clear_emergency_numbers_by_origin(CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED |
                    CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED |
                    CN_EMERGENCY_NUMBER_ORIGIN_SIM |
                    CN_EMERGENCY_NUMBER_ORIGIN_NETWORK |
                    CN_EMERGENCY_NUMBER_ORIGIN_MEMORY, TRUE)) {
                CN_LOG_E("new mcc, couldn't clear number network list!");
                free(reg_info_p);
                goto error;
            }
        }

        send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_SUCCESS, record_p->client_tag, sizeof(*reg_info_p), reg_info_p);

        /* Keep a copy of the latest reported modem registration information */
        memcpy(&s_reg_info, reg_info_p, sizeof(s_reg_info));

        free(reg_info_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_REGISTRATION_STATE_GPRS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_network_emergency_numbers_update(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    (void) record_p;
    CN_LOG_I("MAL CS disabled");
    return REQUEST_STATUS_ERROR;
#else
    cn_emergency_number_list_internal_t *cn_list_p = NULL;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        mal_call_emergency_number_list *mal_list_p = NULL;
        uint16_t n = 0;
        uint8_t i;
        uint8_t j;

        CN_LOG_D("mal_net_req_registration_state_normal -> request");

        REQUIRE_VALID_RECORD(data_p);

        result = mal_net_req_registration_state_normal(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_req_registration_state_normal failed");
            goto error;
        }

        mal_list_p = (mal_call_emergency_number_list *) data_p;

        /* Only store network numbers */
        for (i = 0; i < mal_list_p->num_of_numbers; i++) {
            if (CN_EMERGENCY_NUMBER_ORIGIN_NETWORK == mal_list_p->emergency_numbers[i].emer_num_origin) {
                n++;
            }
        }

        if (n > 0) {
            n = sizeof(cn_emergency_number_list_internal_t) + (n - 1) * sizeof(cn_emergency_number_config_internal_t);
        } else {
            n = sizeof(cn_emergency_number_list_internal_t);
        }

        cn_list_p = (cn_emergency_number_list_internal_t *) calloc(1, n);

        if (NULL == cn_list_p) {
            CN_LOG_D("could not allocate list");
            goto error;
        }

        cn_list_p->num_of_emergency_numbers = mal_list_p->num_of_numbers;

        for (i = 0, j = 0; i < mal_list_p->num_of_numbers; i++) {
            if (CN_EMERGENCY_NUMBER_ORIGIN_NETWORK == mal_list_p->emergency_numbers[i].emer_num_origin) {
                cn_list_p->emergency_number[j].origin = mal_list_p->emergency_numbers[i].emer_num_origin;
                cn_list_p->emergency_number[j].service_type = mal_list_p->emergency_numbers[i].srvc_cat;
                n = (CN_EMERGENCY_NUMBER_STRING_LENGTH < strlen((char *) mal_list_p->emergency_numbers[i].emergency_num)) ? CN_EMERGENCY_NUMBER_STRING_LENGTH : strlen((char *) mal_list_p->emergency_numbers[i].emergency_num);
                (void) memmove(cn_list_p->emergency_number[j].emergency_number, mal_list_p->emergency_numbers[i].emergency_num, n);
                cn_list_p->emergency_number[j].emergency_number[n] = '\0';
                j++;
            }
        }

        cn_list_p->num_of_emergency_numbers = j;

        record_p->request_data_p = cn_list_p;

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        mal_net_registration_info *reg_info_p = NULL;
        uint16_t n = 0;
        uint8_t i = 0;

        CN_LOG_D("mal_net_req_registration_state_normal <- response");

        REQUIRE_VALID_REQUEST_DATA(record_p->request_data_p);
        REQUIRE_VALID_RESPONSE_DATA(record_p->response_data_p);

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_req_registration_state_normal (result:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_req_registration_state_normal not found!");
            goto error;
        }

        reg_info_p = (mal_net_registration_info *) record_p->response_data_p;

        (void) cn_pdc_set_current_mcc((char *) reg_info_p->mcc_mnc_string);

        cn_list_p = (cn_emergency_number_list_internal_t *) record_p->request_data_p;

        for (i = 0; i < cn_list_p->num_of_emergency_numbers; i++) {
            if (NULL != reg_info_p->mcc_mnc_string) {
                n = (CN_MCC_STRING_LENGTH < strlen(cn_list_p->emergency_number[i].emergency_number)) ? CN_MCC_STRING_LENGTH : strlen(cn_list_p->emergency_number[i].emergency_number);
                (void) memmove(cn_list_p->emergency_number[i].mcc, reg_info_p->mcc_mnc_string, n);
                cn_list_p->emergency_number[i].mcc[n] = '\0';
            } else {
                (void) memmove(cn_list_p->emergency_number[i].mcc, "-", 1);
                cn_list_p->emergency_number[i].mcc[1] = '\0';
            }

            /* mcc range is not applicable for network emergency numbers */
            (void) memmove(cn_list_p->emergency_number[i].mcc_range, "-", 1);
            cn_list_p->emergency_number[i].mcc_range[1] = '\0';
        }

        /* Remove everything but the client defined emergency numbers */
        if (!cn_pdc_util_clear_emergency_numbers_by_origin(CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED |
                CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED |
                CN_EMERGENCY_NUMBER_ORIGIN_SIM |
                CN_EMERGENCY_NUMBER_ORIGIN_NETWORK |
                CN_EMERGENCY_NUMBER_ORIGIN_MEMORY, FALSE)) {
            CN_LOG_E("couldn't clear network number sublist!");
            goto error;
        }

        if (!cn_pdc_util_add_emergency_numbers_to_list(cn_list_p)) {
            CN_LOG_E("couldn't add emergency numbers to list");
            goto error;
        }


        free(cn_list_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        break;
    }
    }

error:

    free(cn_list_p);
    return status;
#endif /* USE_MAL_CALL */
}

request_status_t handle_request_cell_info(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_CELL_INFO, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_cell_info_t *cell_info_p = NULL;
    mal_net_cell_info *mal_cell_info_p = NULL;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_req_cell_info -> request");
        result = mal_net_req_cell_info(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_req_cell_info failed");
            send_response(record_p->client_id, CN_RESPONSE_CELL_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_req_cell_info <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_req_cell_info (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_CELL_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_req_cell_info not found!");
            send_response(record_p->client_id, CN_RESPONSE_CELL_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        cell_info_p = calloc(1, sizeof(cn_cell_info_t));

        if (!cell_info_p) {
            CN_LOG_E("calloc failed for cell_info_p!");
            send_response(record_p->client_id, CN_RESPONSE_CELL_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        mal_cell_info_p = (mal_net_cell_info *)record_p->response_data_p;

        cell_info_p->gsm_frame_nbr      = mal_cell_info_p->gsm_frame_nbr;
        cell_info_p->bands_available    = mal_cell_info_p->bands_available;
        cell_info_p->current_cell_id    = mal_cell_info_p->current_cell_id;
        cell_info_p->current_ac         = mal_cell_info_p->current_ac;
        cell_info_p->country_code       = mal_cell_info_p->country_code;
        cell_info_p->network_code       = mal_cell_info_p->network_code;

        if (mal_cell_info_p->mcc_mnc_string) {
            (void)strncpy(cell_info_p->mcc_mnc, (const char *)mal_cell_info_p->mcc_mnc_string, CN_MAX_STRING_SIZE);
        }

        /* We assume these types to be identical. */
        cell_info_p->rat                = mal_cell_info_p->rat;
        cell_info_p->network_type       = mal_cell_info_p->network_type;
        cell_info_p->service_status     = mal_cell_info_p->service_status;

        send_response(record_p->client_id, CN_RESPONSE_CELL_INFO, CN_SUCCESS, record_p->client_tag, sizeof(*cell_info_p), cell_info_p);
        free(cell_info_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_CELL_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_automatic_network_registration(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_register -> request");

        result = mal_net_register(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_register failed (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_register <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_register failed (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_AUTOMATIC_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_manual_network_registration(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_MANUAL_NETWORK_REGISTRATION, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    cn_request_manual_network_registration_t *new_request_data_p = NULL;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_manual_network_registration_t *request_data_p = (cn_request_manual_network_registration_t *) data_p;
        mal_net_manual_reg_info reg_info;
        int matches = 0;

        reg_info.mcc_mnc = calloc(1, sizeof(request_data_p->plmn_operator));
        reg_info.rat = request_data_p->act;

        if (!reg_info.mcc_mnc) {
            CN_LOG_E("calloc failed for reg_info.mcc_mnc!");
            goto error;
        }

        /* If the operator is provided as an alphanumeric string it must first be
         * converted into a MCC+MNC string. To do that it is necessary to search
         * a list of PLMN operator names and see if there is a match.
         *
         * Since some operators use the exact same name for more than one network,
         * the first thing that is searched is the result from the latest network
         * search. If no match is found there, the PLMN operator list is searched.
         */
        switch (request_data_p->format) {
        case CN_OPERATOR_FORMAT_LONG:
            matches = cn_util_search_latest_network_search(request_data_p->plmn_operator, NULL, request_data_p->act, reg_info.mcc_mnc);

            if (1 > matches) {
                matches = plmn_get_operator_mcc_mnc(request_data_p->plmn_operator, NULL, reg_info.mcc_mnc);
            }

            break;
        case CN_OPERATOR_FORMAT_SHORT:
            matches = cn_util_search_latest_network_search(request_data_p->plmn_operator, NULL, request_data_p->act, reg_info.mcc_mnc);

            if (1 > matches) {
                matches = plmn_get_operator_mcc_mnc(NULL, request_data_p->plmn_operator, reg_info.mcc_mnc);
            }

            break;
        case CN_OPERATOR_FORMAT_NUMERIC:
            /* Straight copy. No conversion is necessary */
            strncpy(reg_info.mcc_mnc, request_data_p->plmn_operator, sizeof(request_data_p->plmn_operator));
            matches = 1;
            break;
        default:
            CN_LOG_E("Unknown operator format: %d!", request_data_p->format);
            break;
        }

        if (1 > matches) {
            free(reg_info.mcc_mnc);
            CN_LOG_E("No match found for operator: \"%s\"!", request_data_p->plmn_operator);
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        CN_LOG_D("mal_net_manual_register -> request");
        result = mal_net_manual_register(&reg_info, request_record_get_modem_tag(record_p));

        free(reg_info.mcc_mnc);

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_manual_register failed (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_MANUAL_REGISTRATION_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_MANUAL_REGISTRATION_RESPONSE: {
        CN_LOG_D("mal_net_manual_register <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_manual_register failed (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

error:

    if (REQUEST_STATE_REQUEST == record_p->state) {
        free(new_request_data_p);
    }

    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_manual_network_registration_with_automatic_fallback(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(
        record_p->client_id,
        CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK,
        CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    cn_request_manual_network_registration_with_automatic_fallback_t *new_request_data_p = NULL;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_manual_network_registration_with_automatic_fallback_t *request_data_p = (cn_request_manual_network_registration_with_automatic_fallback_t *) data_p;
        mal_net_manual_reg_info reg_info;
        int matches = 0;

        reg_info.mcc_mnc = calloc(1, sizeof(request_data_p->plmn_operator));
        reg_info.rat = request_data_p->act;

        if (!reg_info.mcc_mnc) {
            CN_LOG_E("calloc failed for reg_info.mcc_mnc!");
            goto error;
        }

        /* If the operator is provided as an alphanumeric string it must first be
         * converted into a MCC+MNC string. To do that it is necessary to search
         * a list of PLMN operator names and see if there is a match.
         *
         * Since some operators use the exact same name for more than one network,
         * the first thing that is searched is the result from the latest network
         * search. If no match is found there, the PLMN operator list is searched.
         */
        switch (request_data_p->format) {
        case CN_OPERATOR_FORMAT_LONG:
            matches = cn_util_search_latest_network_search(request_data_p->plmn_operator, NULL, request_data_p->act, reg_info.mcc_mnc);

            if (1 > matches) {
                matches = plmn_get_operator_mcc_mnc(request_data_p->plmn_operator, NULL, reg_info.mcc_mnc);
            }

            break;
        case CN_OPERATOR_FORMAT_SHORT:
            matches = cn_util_search_latest_network_search(request_data_p->plmn_operator, NULL, request_data_p->act, reg_info.mcc_mnc);

            if (1 > matches) {
                matches = plmn_get_operator_mcc_mnc(NULL, request_data_p->plmn_operator, reg_info.mcc_mnc);
            }

            break;
        case CN_OPERATOR_FORMAT_NUMERIC:
            /* Straight copy. No conversion is necessary */
            strncpy(reg_info.mcc_mnc, request_data_p->plmn_operator, sizeof(request_data_p->plmn_operator));
            matches = 1;
            break;
        default:
            CN_LOG_E("Unknown operator format: %d!", request_data_p->format);
            break;
        }

        if (1 > matches) {
            free(reg_info.mcc_mnc);
            CN_LOG_E("No match found for operator: \"%s\"!", request_data_p->plmn_operator);
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        CN_LOG_D("mal_net_manual_register -> request");
        result = mal_net_manual_register(&reg_info, request_record_get_modem_tag(record_p));

        free(reg_info.mcc_mnc);

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_manual_register failed (result:%d)", result);
            goto automatic_registration;
        }

        record_p->state = REQUEST_STATE_MANUAL_REGISTRATION_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_MANUAL_REGISTRATION_RESPONSE: {
        CN_LOG_D("mal_net_manual_register <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_manual_register failed (result:%d)", record_p->response_error_code);
            goto automatic_registration;
        }

        send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    case REQUEST_STATE_SET_MODE_AUTOMATIC_RESPONSE: {
        CN_LOG_D("mal_net_set_mode <- response (REQUEST_STATE_SET_MODE_AUTOMATIC_RESPONSE)");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_set_mode failed (result:%d)!", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_SUCCESS_AUTOMATIC_FALLBACK, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

automatic_registration: {
        int32_t mode = 0; /* AUTOMATIC SELECTION MODE */
        CN_LOG_W("Falling back to mal_net_set_mode with AUTOMATIC MODE ");

        result = mal_net_set_mode(&mode, request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_set_mode failed (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_REGISTRATION_WITH_AUTOMATIC_FALLBACK, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_SET_MODE_AUTOMATIC_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }

error:

    if (REQUEST_STATE_REQUEST == record_p->state) {
        free(new_request_data_p);
    }

    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_network_deregister(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_NETWORK_DEREGISTER,
                  CN_FAILURE, record_p->client_tag, 0, NULL);

    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_deregister -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_net_deregister(request_record_get_modem_tag(record_p));

        if (MAL_MCE_SUCCESS != result) {
            CN_LOG_E("mal_net_deregister failed!");
            send_response(record_p->client_id, CN_RESPONSE_NETWORK_DEREGISTER, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_deregister <- response");

        if (MAL_MCE_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_deregister failed (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_NETWORK_DEREGISTER, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_NETWORK_DEREGISTER, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_NETWORK_DEREGISTER, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_net_query_mode(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_registration_info_t *reg_info_p = NULL;
    mal_net_registration_info *mal_reg_info_p = NULL;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_query_mode -> request");

        result = mal_net_query_mode(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_query_mode failed (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        const char *long_operator_name_p = NULL;
        const char *short_operator_name_p = NULL;
        CN_LOG_D("mal_net_query_mode <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_query_mode failed (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_query_mode not found!");
            send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        reg_info_p = calloc(1, sizeof(cn_registration_info_t));

        if (!reg_info_p) {
            CN_LOG_E("calloc failed for reg_info_p!");
            send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        mal_reg_info_p = (mal_net_registration_info *)record_p->response_data_p;
        reg_info_p->reg_status = (cn_modem_reg_status_t)mal_reg_info_p->reg_status;
        reg_info_p->gprs_attached = mal_reg_info_p->gprs_attached;
        reg_info_p->cs_attached = mal_reg_info_p->cs_attached;
        reg_info_p->rat = mal_reg_info_p->rat;
        reg_info_p->cid = mal_reg_info_p->cid;
        reg_info_p->lac = mal_reg_info_p->lac;

        if (mal_reg_info_p->mcc_mnc_string) {
            (void)strncpy(reg_info_p->mcc_mnc, (const char *)mal_reg_info_p->mcc_mnc_string, sizeof(reg_info_p->mcc_mnc));

            if (plmn_get_operator_name((const char *)mal_reg_info_p->mcc_mnc_string, mal_reg_info_p->lac,
                                       &long_operator_name_p, &short_operator_name_p) > 0) {

                if (long_operator_name_p) {
                    (void)strncpy(reg_info_p->long_operator_name, long_operator_name_p, sizeof(reg_info_p->long_operator_name));
                }

                if (short_operator_name_p) {
                    (void)strncpy(reg_info_p->short_operator_name, short_operator_name_p, sizeof(reg_info_p->short_operator_name));
                }
            }
        }

        if (cn_pdc_set_current_mcc((char *) mal_reg_info_p->mcc_mnc_string)) {
            if (!cn_pdc_util_clear_emergency_numbers_by_origin(CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED |
                    CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED |
                    CN_EMERGENCY_NUMBER_ORIGIN_SIM |
                    CN_EMERGENCY_NUMBER_ORIGIN_NETWORK |
                    CN_EMERGENCY_NUMBER_ORIGIN_MEMORY, TRUE)) {
                CN_LOG_E("new mcc, couldn't clear number network list!");
                free(reg_info_p);
                goto error;
            }
        }

        if (mal_reg_info_p->selection_mode == MAL_NET_SELECT_MODE_MANUAL) {
            reg_info_p->search_mode = CN_NETWORK_SELECTION_MODE_MANUAL;
        } else {
            reg_info_p->search_mode = CN_NETWORK_SEARCH_MODE_AUTOMATIC;
        }

        CN_LOG_D("selection_mode=%d, search_mode=%d", mal_reg_info_p->selection_mode, reg_info_p->search_mode);

        send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE, CN_SUCCESS, record_p->client_tag, sizeof(*reg_info_p), reg_info_p);
        free(reg_info_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_NET_QUERY_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_manual_network_search(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_manual_search -> request");

        result = mal_net_manual_search(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_manual_search failed (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        cn_manual_network_search_data_t *network_search_data_p = NULL;
        mal_net_manual_search_data *mal_data_p = NULL;
        mal_net_network_node *node_p = NULL;
        const char *long_operator_name_p = NULL;
        const char *short_operator_name_p = NULL;
        cn_uint32_t i = 0;
        cn_uint32_t size = 0;

        CN_LOG_D("mal_net_manual_search <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_manual_search failed (result:%d)", record_p->response_error_code);

            if (MAL_NET_INTERRUPTED == record_p->response_error_code) {
                /* Interrupted request */
                send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_REQUEST_INTERRUPTED, record_p->client_tag, 0, NULL);
                status = REQUEST_STATUS_DONE;
            }
            else if (MAL_NET_CAUSE_NET_NOT_FOUND == record_p->response_error_code) {
                /* No network */
                send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_REQUEST_CAUSE_NET_NOT_FOUND , record_p->client_tag, 0, NULL);
                status = REQUEST_STATUS_DONE;
            }
            else {
                send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
            }

            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_manual_search not found!");
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        mal_data_p = (mal_net_manual_search_data *) record_p->response_data_p;

        size = sizeof(cn_manual_network_search_data_t) + sizeof(cn_network_info_t) * (mal_data_p->num_of_networks - 1);
        network_search_data_p = calloc(1, size);

        if (!network_search_data_p) {
            CN_LOG_E("calloc failed for network_search_data_p!");
            send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        network_search_data_p->num_of_networks = mal_data_p->num_of_networks;

        for (i = 0, node_p = mal_data_p->head;
                node_p && i < mal_data_p->num_of_networks;
                node_p = node_p->next, i++) {
            network_search_data_p->cn_network_info[i].index = node_p->index;
            network_search_data_p->cn_network_info[i].network_status = node_p->network_status;
            network_search_data_p->cn_network_info[i].band_info = node_p->band_info;
            network_search_data_p->cn_network_info[i].network_type = node_p->network_type;
            network_search_data_p->cn_network_info[i].umts_available = node_p->umts_available;
            network_search_data_p->cn_network_info[i].current_lac = node_p->current_lac;
            network_search_data_p->cn_network_info[i].country_code = node_p->country_code;
            network_search_data_p->cn_network_info[i].network_code = node_p->network_code;

            (void)strncpy(network_search_data_p->cn_network_info[i].mcc_mnc, (const char *)node_p->mcc_mnc,
                          sizeof(network_search_data_p->cn_network_info[i].mcc_mnc));

            if (plmn_get_operator_name((const char *)node_p->mcc_mnc, node_p->current_lac,
                                       &long_operator_name_p, &short_operator_name_p) > 0) {
                if (long_operator_name_p) {
                    (void)strncpy(network_search_data_p->cn_network_info[i].long_op_name, long_operator_name_p,
                                  sizeof(network_search_data_p->cn_network_info[i].long_op_name));
                }

                if (short_operator_name_p) {
                    (void)strncpy(network_search_data_p->cn_network_info[i].short_op_name, short_operator_name_p,
                                  sizeof(network_search_data_p->cn_network_info[i].short_op_name));
                }
            }
        }

        send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_SUCCESS, record_p->client_tag, size, network_search_data_p);

        /* Store the latest network search result. This information will be used if and when
         * an attempt to manually register to a network using the long or short operator name
         * is made.
         */
        cn_util_set_latest_network_search(network_search_data_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_MANUAL_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_interrupt_network_search(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_INTERRUPT_NETWORK_SEARCH, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_interrupt_search -> request");

        result = mal_net_interrupt_search(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_interrupt_search failed (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_INTERRUPT_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_interrupt_search <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_interrupt_search failed (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_INTERRUPT_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_INTERRUPT_NETWORK_SEARCH, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_INTERRUPT_NETWORK_SEARCH, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;

#endif /* USE_MAL_NET */
}

request_status_t handle_request_current_call_list(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_CURRENT_CALL_LIST,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    mal_call_context *call_context_p = NULL;
    cn_call_list_t *call_list_p = NULL;
    cn_sint32_t nr_of_calls = 0;
    cn_sint32_t result = 0;
    cn_sint32_t i = 0;
    cn_sint32_t size = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    CN_LOG_D("mal_call_request_getcalllist -> synchronous request");

    /*** NOTE: Synchronous MAL calls ***/
    result = mal_call_request_getcalllist(NULL, (void **)&call_context_p, &nr_of_calls);

    if (MAL_SUCCESS != result) {
        CN_LOG_E("mal_call_request_getcalllist failed (result:%d)", result);
        send_response(record_p->client_id, CN_RESPONSE_CURRENT_CALL_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    size = sizeof(cn_call_list_t);

    if (nr_of_calls > 0) {
        size += sizeof(cn_call_context_t) * (nr_of_calls - 1);
    }

    call_list_p = calloc(1, size);

    if (!call_list_p) {
        CN_LOG_E("calloc failed for call_list_p");
        send_response(record_p->client_id, CN_RESPONSE_CURRENT_CALL_LIST, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    call_list_p->nr_of_calls = nr_of_calls;

    if (call_context_p) {
        for (i = 0; i < nr_of_calls; i++) {
            util_call_context(&(call_context_p[i]), &(call_list_p->call_context[i]));
        }

        mal_call_request_freecalllist(call_context_p);
    }

    send_response(record_p->client_id, CN_RESPONSE_CURRENT_CALL_LIST, CN_SUCCESS, record_p->client_tag, size, call_list_p);
    free(call_list_p);

    return REQUEST_STATUS_DONE;

error:
    mal_call_request_freecalllist(call_context_p);
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_dial(void *data_p,
                                     request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_DIAL, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t          status = REQUEST_STATUS_ERROR;
    cn_sint32_t               result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        mal_call_dailup mal_dialup;
        mal_call_uus uus_data;
        cn_request_dial_t *request_data_p = (cn_request_dial_t *) data_p;

        REQUIRE_VALID_REQUEST_DATA(data_p);

        /* Extract any DTMF sequence from the dialing string and store during dial request. */
        record_p->request_data_p = mddh_util_extract_and_remove_dtmf(data_p);

        switch (request_data_p->dial.call_type) {
        case CN_CALL_TYPE_EMERGENCY_CALL:
            mal_dialup.call_type = MAL_CALL_TYPE_EMERGENCY;
            break;
        case CN_CALL_TYPE_VOICE_CALL:
            mal_dialup.call_type = MAL_CALL_TYPE_VOICE;
            break;
        default:
            CN_LOG_E("unknown request_data_p->dial.call_type! (%d)", request_data_p->dial.call_type);
            send_response(record_p->client_id, CN_RESPONSE_DIAL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
            break;
        }

        mal_dialup.phone_number     = (char *)    request_data_p->dial.phone_number;
        mal_dialup.clir             = (int32_t)   request_data_p->dial.clir;
        mal_dialup.bc_length        = (uint8_t)   request_data_p->dial.bc_length;
        mal_dialup.bc_data          = (uint8_t *) request_data_p->dial.bc_data;

        if (request_data_p->dial.user_to_user_len > 0) {
            uus_data.uus = (uint8_t *) request_data_p->dial.user_to_user;
            uus_data.uus_length = (uint8_t) request_data_p->dial.user_to_user_len;
            mal_dialup.uus_info = &uus_data;
        } else {
            mal_dialup.uus_info = NULL;
        }

        CN_LOG_D("call_type: %d , clir: %d , phone_number: \"%s\" bc_length: %d", mal_dialup.call_type,
                 mal_dialup.clir, mal_dialup.phone_number, mal_dialup.bc_length);

        CN_LOG_D("mal_call_request_dialup -> request");

        result = mal_call_request_dialup(&mal_dialup, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_dialup failed (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_DIAL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        cn_response_dial_t response_dial;
        cn_request_dtmf_string_t *dtmf_p = (cn_request_dtmf_string_t *)record_p->request_data_p;
        mal_call_context *mal_call_context_p = (mal_call_context *) record_p->response_data_p;
        memset(&response_dial, 0, sizeof(response_dial));

        CN_LOG_D("mal_call_request_dialup <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_DIAL, "mal_call_request_dialup");
            goto error;
        }

        record_p->request_data_p = NULL; /* If we get this far, then we only reference the DTMF string using dtmf_p */

        /* Associate DTMF sequence (if any) with the call id */
        if (mal_call_context_p && dtmf_p) {
            if (mddh_associate_dtmf_with_call_id(mal_call_context_p->callId, dtmf_p) != 0) {
                free(dtmf_p); /* Free the memory if association fails... */
            }
        }

        response_dial.service_type = CN_SERVICE_TYPE_AFTER_SAT_CC_VOICE_CALL;

        if (mal_call_context_p) {
            response_dial.data.call_id = (cn_uint8_t)(mal_call_context_p->callId & 0xFF);
        }

        send_response(record_p->client_id, CN_RESPONSE_DIAL, CN_SUCCESS, record_p->client_tag, sizeof(response_dial), &response_dial);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_DIAL, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:

    if (record_p) {
        free(record_p->request_data_p);
        record_p->request_data_p = NULL;
    }

    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_hangup(void *data_p,
                                       request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_HANGUP, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_hangup_t *request_data_p = (cn_request_hangup_t *)data_p;
        int matching_calls = 0;
        cn_call_context_t call_context;
        cn_uint8_t call_id = 0;

        REQUIRE_VALID_REQUEST_DATA(data_p);

        if (CN_CALL_STATE_FILTER_NONE != request_data_p->filter) {
            matching_calls = util_get_call_by_filter(&request_data_p->filter, &call_context);

            if (0 == matching_calls) {
                /* No calls matching filter to hang up on, return success */
                send_response(record_p->client_id, CN_RESPONSE_HANGUP, CN_SUCCESS, record_p->client_tag, 0, NULL);

                return REQUEST_STATUS_DONE;

            } else if (1 < matching_calls) {
                /* More than one call matching, use call ID bit fields */
                if (CN_CALL_STATE_FILTER_ALL != request_data_p->filter) {
                    call_id = MAL_CALL_MODEM_ID_CONFERENCE;
                } else {
                    call_id = MAL_CALL_MODEM_ID_ALL;
                }
            } else {
                /* When there's only a single match, use the call ID returned by the filter function */
                call_id = call_context.call_id;
            }

        } else {
            call_id = request_data_p->call_id;
        }

        CN_LOG_D("mal_call_request_hangup -> request, call_id=0x%X", call_id);

        result = mal_call_request_hangup(&call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_hangup call_id=0x%X, failed (result:%d)", call_id, result);
            send_response(record_p->client_id, CN_RESPONSE_HANGUP, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }

    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_hangup <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_HANGUP, "mal_call_request_hangup");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_HANGUP, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }

    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_HANGUP, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_swap_calls(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SWAP_CALLS, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_swap_calls_t *request_data_p = (cn_request_swap_calls_t *)data_p;

        CN_LOG_D("mal_call_request_swap_call -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_swap_call(&request_data_p->active_call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_swap_call failed (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SWAP_CALLS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_swap_call <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_SWAP_CALLS, "mal_call_request_swap_call");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SWAP_CALLS, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SWAP_CALLS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_hold_call(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_HOLD_CALL, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_hold_call_t *request_data_p = (cn_request_hold_call_t *)data_p;

        CN_LOG_D("mal_call_request_hold -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_hold(&request_data_p->active_call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_hold (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_HOLD_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_hold <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_HOLD_CALL, "mal_call_request_hold");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_HOLD_CALL, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_HOLD_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}
request_status_t handle_request_resume_call(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_RESUME_CALL,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_resume_call_t *request_data_p = (cn_request_resume_call_t *)data_p;

        CN_LOG_D("mal_call_request_resume -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_resume(&request_data_p->hold_call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_resume (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_RESUME_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_resume <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_RESUME_CALL, "mal_call_request_resume");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_RESUME_CALL, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_RESUME_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}
request_status_t handle_request_conference_call(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_conference_call_t *request_data_p = (cn_request_conference_call_t *) data_p;

        CN_LOG_D("mal_call_request_conference_call -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_conference_call(&request_data_p->active_call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_conference_call (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_conference_call <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_CONFERENCE_CALL, "mal_call_request_conference_call");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_conference_call_split(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL_SPLIT,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_conference_call_split_t *request_data_p = (cn_request_conference_call_split_t *) data_p;

        CN_LOG_D("mal_call_request_conference_call_split -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_conference_call_split(&request_data_p->active_call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_conference_call_split (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL_SPLIT, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_conference_call_split <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_CONFERENCE_CALL_SPLIT, "mal_call_request_conference_call_split");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL_SPLIT, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_CONFERENCE_CALL_SPLIT, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_explicit_call_transfer(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_EXPLICIT_CALL_TRANSFER,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_explicit_call_transfer_t *request_data_p = (cn_request_explicit_call_transfer_t *) data_p;

        CN_LOG_D("mal_call_request_explicit_call_transfer -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_explicit_call_transfer(&request_data_p->call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_explicit_call_transfer (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_EXPLICIT_CALL_TRANSFER, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_explicit_call_transfer <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_EXPLICIT_CALL_TRANSFER, "mal_call_request_explicit_call_transfer");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_EXPLICIT_CALL_TRANSFER, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_EXPLICIT_CALL_TRANSFER, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_answer_call(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_ANSWER_CALL,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_answer_call_t *request_data_p = (cn_request_answer_call_t *) data_p;

        CN_LOG_D("mal_call_request_answer -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_answer(&request_data_p->call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_answer (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_ANSWER_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_answer <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_ANSWER_CALL, "mal_call_request_answer");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_ANSWER_CALL, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_ANSWER_CALL, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_ussd(void *data_p,
                                     request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_USSD, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_response_ussd_t rsp;
    cn_sint32_t result = 0;
    memset(&rsp, 0, sizeof(rsp));


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_ussd_t *request_data_p = (cn_request_ussd_t *) data_p;
        mal_ss_ussd_data ss_ussd_data;

        CN_LOG_D("mal_ss_request_ussd -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        memset(&ss_ussd_data, 0, sizeof(ss_ussd_data));
        ss_ussd_data.length = request_data_p->ussd_info.length; /**< Length of the USSD string */
        ss_ussd_data.dcs = request_data_p->ussd_info.dcs; /**< Data coding scheme as per 3GPP TS 23.038, Chapter 5 'Cell Broadcast Data Coding Scheme' */
        ss_ussd_data.ussd_str = request_data_p->ussd_info.ussd_string; /**< USSD string */
        ss_ussd_data.suppress_ussd_on_end = request_data_p->ussd_info.sat_initiated; /**<Valid when USSD received_type is USSD_END for SAT initiated USSD */

        CN_LOG_D("USSD data: length=%d, dcs =%d, string = %s, suppress_ussd_on_end = %d", ss_ussd_data.length , ss_ussd_data.dcs, ss_ussd_data.ussd_str, ss_ussd_data.suppress_ussd_on_end);

        result = mal_ss_request_ussd(&ss_ussd_data, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            util_convert_ss_error_type(1, NULL, &rsp.ss_error); /* Use value 1 to provoke convert function to generate internal error code */
            CN_LOG_E("mal_ss_request_ussd (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        mal_ss_error_type_value *mal_ss_error_p = NULL;
        mal_ss_service_response *ss_service_response_p = (mal_ss_service_response *) record_p->response_data_p;

        CN_LOG_D("mal_ss_request_ussd <- response");


        if (ss_service_response_p != NULL) {
            mal_ss_error_p = ss_service_response_p->error_info;
        }  else if (MAL_SUCCESS != record_p->response_error_code) {
            goto error;
        }


        /* data_p is NULL in case of successfull response. In case of failed response, it contains ss_error_type. */
        util_convert_ss_error_type(record_p->response_error_code, mal_ss_error_p, &rsp.ss_error);
        rsp.session_id = (uint32_t) request_record_get_modem_tag(record_p);

        if (mal_ss_error_p != NULL) {
            CN_LOG_D("mal_ss_request_ussd -> mal_ss_error_p->error_type 0x%x", mal_ss_error_p->error_type);
            CN_LOG_D("mal_ss_request_ussd -> mal_ss_error_p->error_value 0x%x", mal_ss_error_p->error_value);
        }

        send_response(record_p->client_id, CN_RESPONSE_USSD, CN_SUCCESS, record_p->client_tag, sizeof(rsp), &rsp);

        return REQUEST_STATUS_DONE;
    }
    default: {
        util_convert_ss_error_type(1, NULL, &rsp.ss_error); /* Use value 1 to provoke convert function to generate internal error code */
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    send_response(record_p->client_id, CN_RESPONSE_USSD, CN_FAILURE, record_p->client_tag, sizeof(rsp), &rsp);
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_ussd_abort(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_USSD_ABORT, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_ss_cancel_ussd -> request");

        result = mal_ss_cancel_ussd(request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_cancel_ussd (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_USSD_ABORT, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_cancel_ussd <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_cancel_ussd (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_USSD_ABORT, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_USSD_ABORT, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_USSD_ABORT, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_rssi_value(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_RSSI_VALUE, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_get_rssi -> request");

        result = mal_net_get_rssi(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_get_rssi (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_RSSI_VALUE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        cn_signal_info_t     rssi_data;
        mal_net_signal_info *mal_rssi_data_p;

        CN_LOG_D("mal_net_get_rssi <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_get_rssi (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_RSSI_VALUE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_get_rssi not found!");
            send_response(record_p->client_id, CN_RESPONSE_RSSI_VALUE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        memset(&rssi_data, 0, sizeof(rssi_data));
        mal_rssi_data_p    = (mal_net_signal_info *)record_p->response_data_p;
        rssi_data.rssi_dbm = mal_rssi_data_p->rssi_dbm;
        rssi_data.ber      = mal_rssi_data_p->ber;
        rssi_data.ecno     = mal_rssi_data_p->ecno;
        rssi_data.rat      = mal_rssi_data_p->rat;

        send_response(record_p->client_id, CN_RESPONSE_RSSI_VALUE, CN_SUCCESS, record_p->client_tag, sizeof(rssi_data), &rssi_data);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_RSSI_VALUE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

#ifdef USE_MAL_CS
static request_status_t query_clip_status(void *data_p,
        request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_ss_request_query_clip -> request");

        result = mal_ss_request_query_clip(request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_request_query_clip (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_request_query_clip <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_request_query_clip (result:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_request_query_clip not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    return status;
}
#endif

request_status_t handle_request_clip_status(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_CLIP_STATUS,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    status = query_clip_status(data_p, record_p);

    if (REQUEST_STATUS_DONE == status) {
        if (NULL == record_p->response_data_p) {
            CN_LOG_E("handle_request_clip_status: response_p is NULL\n");
            send_response(record_p->client_id, CN_RESPONSE_CLIP_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            status = REQUEST_STATUS_ERROR;
        } else {
            mal_ss_service_response *response_p = (mal_ss_service_response *)record_p->response_data_p;
            cn_clip_status_t clip_status;
            clip_status = (cn_clip_status_t) response_p->response[0];
            send_response(record_p->client_id, CN_RESPONSE_CLIP_STATUS, CN_SUCCESS, record_p->client_tag, sizeof(clip_status), &clip_status);
        }
    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_CLIP_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    }

    return status;
#endif /* USE_MAL_CS */
}

#ifdef USE_MAL_CS
static request_status_t query_cnap_status(void *data_p,
        request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_ss_calling_name_presentation_query -> request");

        result = mal_ss_calling_name_presentation_query(request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_calling_name_presentation_query (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_calling_name_presentation_query <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_calling_name_presentation_query (result:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_calling_name_presentation_query not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    return status;
}
#endif

request_status_t handle_request_colr_status(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_CLIP_STATUS,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    status = query_colr_status(data_p, record_p);

    if (REQUEST_STATUS_DONE == status) {
        mal_ss_service_response *response_p = (mal_ss_service_response *)record_p->response_data_p;
        cn_colr_status_t colr_status;

        if (!response_p) {
            CN_LOG_E("response_p is NULL!");
            send_response(record_p->client_id, CN_RESPONSE_COLR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto exit;
        }

        response_p = (mal_ss_service_response *)record_p->response_data_p;
        colr_status = (cn_colr_status_t) response_p->response[0];

        send_response(record_p->client_id, CN_RESPONSE_COLR_STATUS, CN_SUCCESS, record_p->client_tag, sizeof(colr_status), &colr_status);

    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_COLR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    }

exit:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_cnap_status(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_CNAP_STATUS,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    status = query_cnap_status(data_p, record_p);


    if (REQUEST_STATUS_DONE == status) {
        if (NULL == record_p->response_data_p) {
            send_response(record_p->client_id, CN_RESPONSE_CNAP_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            status = REQUEST_STATUS_ERROR;
        } else {
            cn_cnap_status_t cnap_status;
            mal_ss_service_response *response_p = (mal_ss_service_response *)record_p->response_data_p;
            cnap_status = (cn_cnap_status_t) response_p->response[0];
            send_response(record_p->client_id, CN_RESPONSE_CNAP_STATUS, CN_SUCCESS, record_p->client_tag, sizeof(cnap_status), &cnap_status);
        }
    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_CNAP_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
    }

    return status;
#endif /* USE_MAL_CS */
}

#ifdef USE_MAL_CS
static request_status_t query_clir_ss_status(void *data_p,
        request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_ss_request_get_clir -> request");

        result = mal_ss_request_get_clir(request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_request_get_clir (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_request_get_clir <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("response data for mal_ss_request_get_clir returned error %d!", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_request_get_clir not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_CLIR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
}

static request_status_t query_clir_status(void *data_p,
        request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_call_request_get_clir -> request");

        /* Query call api for modem property (n param) */
        result = mal_call_request_get_clir(request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_get_clir (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_CALL_CLIR;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_CALL_CLIR: {
        cn_clir_setting_t *n_p = NULL;
        CN_LOG_D("mal_call_request_get_clir <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p,
                                             CN_RESPONSE_CLIR_STATUS,
                                             "mal_call_request_get_clir");
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_call_request_get_clir not found!");
            goto error;
        }

        CN_LOG_D("mal_ss_request_get_clir -> request");

        n_p = calloc(1, sizeof(cn_clir_setting_t));

        if (NULL == n_p) {
            CN_LOG_E("calloc failed for cn_clir_setting_t");
            goto error;
        }

        memmove(n_p, (record_p->response_data_p), sizeof(cn_clir_setting_t));

        record_p->request_data_p = n_p;
        /* Query ss api for network status  (m param)*/
        result = mal_ss_request_get_clir(request_record_get_modem_tag(
                                             record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_request_get_clir (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_request_get_clir <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_request_get_clir failed! (response_error_code:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_request_get_clir not found!");
            goto error;
        }

        if (!record_p->request_data_p) {
            CN_LOG_E("request_data_p data for mal_ss_request_get_clir not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_CLIR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
}

static request_status_t query_colr_status(void *data_p, request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_ss_request_query_colr -> request");

        result = mal_ss_request_query_colr(request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_request_query_colr (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_request_query_colr <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_request_query_colr (result:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_request_query_colr not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    return status;
}
#endif /* USE_MAL_CS */

request_status_t handle_request_clir_status(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_CLIR_STATUS,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    mal_ss_service_response *response_p = NULL;


    status = query_clir_status(data_p, record_p);

    if (REQUEST_STATUS_DONE == status) {
        if (NULL == record_p->response_data_p || NULL == record_p->request_data_p) {
            send_response(record_p->client_id, CN_RESPONSE_CLIR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            status = REQUEST_STATUS_ERROR;
        } else {
            cn_clir_t clir;
            clir.setting = *((cn_clir_setting_t *)record_p->request_data_p); /* n param, TS 27.007 7.7 */
            response_p = (mal_ss_service_response *)record_p->response_data_p;
            clir.status = response_p->response[0]; /* m param, TS 27.007 7.7 */
            send_response(record_p->client_id, CN_RESPONSE_CLIR_STATUS, CN_SUCCESS, record_p->client_tag, sizeof(clir), &clir);
        }

        free(record_p->request_data_p);
    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_CLIR_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
        free(record_p->request_data_p);
    }

    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_set_clir(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_CLIR, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_set_clir_t *request_data_p = (cn_request_set_clir_t *) data_p;

        CN_LOG_D("mal_call_request_set_clir -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_call_request_set_clir(&request_data_p->clir_setting, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_set_clir (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SET_CLIR, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_set_clir <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_SET_CLIR, "mal_call_request_set_clir");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_CLIR, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_CLIR, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}


request_status_t handle_request_sleep_test_mode(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_MTE
    (void) data_p;
    CN_LOG_I("MAL MTE disabled");
    send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_request_sleep_mode_t *request_data_p = (cn_request_sleep_mode_t *) data_p;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {

        if (CN_SLEEP_TEST_MODE_ON == request_data_p->sleep_mode) {
            CN_LOG_D("Request to enable Sleep Mode");
            CN_LOG_D("mal_mte_sleep_test_mode -> request");
            result = mal_mte_sleep_test_mode(CN_MODEM_TEST_SLEEP_MODE_ALL_ALLOWED, CN_CPU_ALL, request_record_get_modem_tag(record_p));

            if (MAL_MTE_SUCCESS != result) {
                CN_LOG_E("mal_mte_sleep_test_mode failed!");
                send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            record_p->state = REQUEST_STATE_RESPONSE;
            return REQUEST_STATUS_PENDING;

        } else if (CN_SLEEP_TEST_MODE_OFF == request_data_p->sleep_mode) {
            CN_LOG_D("Request to disable Sleep Mode");
            CN_LOG_D("mal_mte_sleep_test_mode -> request");
            result = mal_mte_sleep_test_mode(CN_MODEM_TEST_SLEEP_MODE_NO_SLEEP, CN_CPU_ALL, request_record_get_modem_tag(record_p));

            if (MAL_MTE_SUCCESS != result) {
                CN_LOG_E("mal_mte_sleep_test_mode failed!");
                send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            record_p-> state = REQUEST_STATE_MODEM_SLEEP_RESPONSE;
            return REQUEST_STATUS_PENDING;
        }

        send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    case REQUEST_STATE_MODEM_SLEEP_RESPONSE: {

        CN_LOG_D("mal_mte_sleep_test_mode <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        CN_LOG_D("mal_mte_forced_sleep_mode_set -> request");

        result = mal_mte_forced_sleep_mode_set(CN_MODEM_TEST_SLEEP_MODE_ALL_ALLOWED, CN_CPU_ALL, request_record_get_modem_tag(record_p));

        if (MAL_MTE_SUCCESS != result) {
            CN_LOG_E("mal_mte_forced_sleep_mode_set failed!");
            send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_MODEM_FORCED_SLEEP_RESPONSE;
        return REQUEST_STATUS_PENDING;
    }

    case REQUEST_STATE_MODEM_FORCED_SLEEP_RESPONSE: {
        CN_LOG_D("mal_mte_forced_sleep_mode_set <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_SUCCESS, record_p->client_tag, 0, NULL);
        return REQUEST_STATUS_DONE;
    }

    case REQUEST_STATE_RESPONSE: {

        CN_LOG_D("mal_mte_sleep_test_mode <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_SUCCESS, record_p->client_tag, 0, NULL);
        return REQUEST_STATUS_DONE;
    }

    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SLEEP_TEST_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_MTE */
}


#ifdef USE_MAL_CS
static request_status_t query_call_forward(
    void *data_p,
    request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_call_forward_info_t *request_data_p = (cn_request_call_forward_info_t *) data_p;

        CN_LOG_D("mal_ss_query_call_forward -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        CN_LOG_D("%s:%s(%d): status:%d", __FILE__, __FUNCTION__, __LINE__, request_data_p->call_forward_info.status);
        CN_LOG_D("%s:%s(%d): reason:%d", __FILE__, __FUNCTION__, __LINE__, request_data_p->call_forward_info.reason);
        CN_LOG_D("%s:%s(%d): class:%d", __FILE__, __FUNCTION__, __LINE__, request_data_p->call_forward_info.service_class);
        CN_LOG_D("%s:%s(%d): toa:%d", __FILE__, __FUNCTION__, __LINE__, request_data_p->call_forward_info.toa);
        CN_LOG_D("%s:%s(%d): number:(%s)", __FILE__, __FUNCTION__, __LINE__, request_data_p->call_forward_info.number);
        CN_LOG_D("%s:%s(%d): time_seconds:(%d)", __FILE__, __FUNCTION__, __LINE__, request_data_p->call_forward_info.time_seconds);

        result = mal_ss_query_call_forward(&request_data_p->call_forward_info, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_query_call_forward (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_query_call_forward <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_query_call_forward (result:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_query_call_forward not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }

error:
    return status;
}
#endif /* USE_MAL_CS */

request_status_t handle_request_query_call_forward(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_FORWARD,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    status = query_call_forward(data_p, record_p);

    if (REQUEST_STATUS_DONE == status) {
        if (NULL == record_p->response_data_p) {
            send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_FORWARD, CN_FAILURE, record_p->client_tag, 0, NULL);
            status = REQUEST_STATUS_ERROR;
        } else {
            cn_sint32_t num_list = 0;
            cn_sint32_t i = 0;
            cn_sint32_t size = 0;
            mal_ss_call_fwd_info *mal_call_fwd_info_p = NULL;
            cn_call_forward_info_list_t *call_forward_info_list_p = NULL;
            mal_ss_service_response *response_p = (mal_ss_service_response *)record_p->response_data_p;

            mal_call_fwd_info_p = (mal_ss_call_fwd_info *)response_p->call_forward_info;
            num_list = mal_call_fwd_info_p->num;

            size = sizeof(cn_call_forward_info_list_t);

            if (num_list > 0) {
                size += sizeof(cn_call_forward_info_t) * (num_list - 1);
            }

            call_forward_info_list_p = calloc(1, size);

            if (!call_forward_info_list_p) {
                CN_LOG_E("calloc failed for call_forward_info_list_p");
                status = REQUEST_STATUS_ERROR;
                goto error;
            }

            call_forward_info_list_p->num = num_list;

            for (i = 0; i < num_list; i++) {
                call_forward_info_list_p->call_fwd_info[i].status = mal_call_fwd_info_p->call_fwd_info[i].status;
                call_forward_info_list_p->call_fwd_info[i].reason = mal_call_fwd_info_p->call_fwd_info[i].reason;
                call_forward_info_list_p->call_fwd_info[i].service_class = mal_call_fwd_info_p->call_fwd_info[i].serviceClass;
                call_forward_info_list_p->call_fwd_info[i].toa = mal_call_fwd_info_p->call_fwd_info[i].toa;
                call_forward_info_list_p->call_fwd_info[i].time_seconds = mal_call_fwd_info_p->call_fwd_info[i].timeSeconds;

                if (mal_call_fwd_info_p->call_fwd_info[i].number) {
                    (void)strncpy(call_forward_info_list_p->call_fwd_info[i].number,
                                  (const char *)mal_call_fwd_info_p->call_fwd_info[i].number, CN_MAX_STRING_SIZE);
                }
            }

            send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_FORWARD, CN_SUCCESS, record_p->client_tag, size, call_forward_info_list_p);

            free(call_forward_info_list_p);
        }
    } else if (REQUEST_STATUS_ERROR == status) {
        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_query_call_forward (result:%d)", record_p->response_error_code);

            if (MAL_NOT_SUPPORTED == record_p->response_error_code) {
                send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_FORWARD, CN_REQUEST_NOT_SUPPORTED, record_p->client_tag, 0, NULL);
            } else {
                send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_FORWARD, CN_FAILURE, record_p->client_tag, 0, NULL);
            }
        } else {
            send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_FORWARD, CN_FAILURE, record_p->client_tag, 0, NULL);
        }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

#ifdef USE_MAL_CS
static request_status_t set_call_forward(
    void *data_p,
    request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_call_forward_info_t *request_data_p = (cn_request_call_forward_info_t *) data_p;
        mal_ss_call_forward_info call_forward_info;

        CN_LOG_D("mal_ss_set_call_forward -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        call_forward_info.status = request_data_p->call_forward_info.status;
        call_forward_info.reason = request_data_p->call_forward_info.reason;
        call_forward_info.serviceClass = request_data_p->call_forward_info.service_class;
        call_forward_info.toa = request_data_p->call_forward_info.toa;
        call_forward_info.timeSeconds = request_data_p->call_forward_info.time_seconds;
        call_forward_info.number = request_data_p->call_forward_info.number;

        /* If TIME for reason NO REPLY when making REGISTRATION is not set, we set time to default */
        if (call_forward_info.status == CN_CALL_FORWARDING_STATUS_REGISTRATION
                && call_forward_info.reason == CN_CALL_FORWARDING_REASON_NO_REPLY
                && call_forward_info.timeSeconds == 0) {
            call_forward_info.timeSeconds = CN_CALL_FORWARDING_REPLY_TIME_DEFAULT;
        }

        CN_LOG_D("call_forward_info.status=%d", call_forward_info.status);
        CN_LOG_D("call_forward_info.reason=%d", call_forward_info.reason);
        CN_LOG_D("call_forward_info.serviceClass : %d", call_forward_info.serviceClass);
        CN_LOG_D("call_forward_info.toa : %d", call_forward_info.toa);
        CN_LOG_D("call_forward_info.timeSeconds : %d", call_forward_info.timeSeconds);
        CN_LOG_D("call_forward_info.number : \"%s\"", call_forward_info.number);
        result = mal_ss_set_call_forward(&call_forward_info, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_set_call_forward (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_set_call_forward <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_set_call_forward (result:%d)", record_p->response_error_code);
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }

error:
    return status;
}
#endif /* USE_MAL_CS */

request_status_t handle_request_set_call_forward(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_CALL_FORWARD,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    status = set_call_forward(data_p, record_p);


    if (REQUEST_STATUS_DONE == status) {
        send_response(record_p->client_id, CN_RESPONSE_SET_CALL_FORWARD, CN_SUCCESS, record_p->client_tag, 0, NULL);
    } else if (REQUEST_STATUS_ERROR == status) {
        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_set_call_forward (result:%d)", record_p->response_error_code);

            if (MAL_NOT_SUPPORTED == record_p->response_error_code) {
                send_response(record_p->client_id, CN_RESPONSE_SET_CALL_FORWARD, CN_REQUEST_NOT_SUPPORTED, record_p->client_tag, 0, NULL);
            } else {
                send_response(record_p->client_id, CN_RESPONSE_SET_CALL_FORWARD, CN_FAILURE, record_p->client_tag, 0, NULL);
            }
        } else {
            send_response(record_p->client_id, CN_RESPONSE_SET_CALL_FORWARD, CN_FAILURE, record_p->client_tag, 0, NULL);
        }
    }

    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_set_modem_property(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_MODEM_PROPERTY,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        mal_call_modem_property mal_prop;
        cn_request_set_modem_property_t *request_data_p = NULL;

        CN_LOG_D("mal_call_request_modem_property_set -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        request_data_p = (cn_request_set_modem_property_t *) data_p;
        mal_prop = (mal_call_modem_property) request_data_p->modem_property.type;

        switch (request_data_p->modem_property.type) {
        case CN_MODEM_PROPERTY_TYPE_CTM: {
            mal_call_modem_prop_ctm mal_ctm = (mal_call_modem_prop_ctm) request_data_p->modem_property.value.ctm;
            result = mal_call_request_modem_property_set(&mal_prop, &mal_ctm, request_record_get_modem_tag(record_p));
            break;
        }
        case CN_MODEM_PROPERTY_TYPE_CLIR: {
            uint8_t n = (uint8_t) request_data_p->modem_property.value.clir.clir_setting; /* "n" parameter from TS 27.007 */
            result = mal_call_request_modem_property_set(&mal_prop, &n, request_record_get_modem_tag(record_p));
            break;
        }
        case CN_MODEM_PROPERTY_TYPE_ALS: {
            mal_call_modem_prop_als mal_als = (mal_call_modem_prop_als) request_data_p->modem_property.value.als.current_line;
            result = mal_call_request_modem_property_set(&mal_prop, &mal_als, request_record_get_modem_tag(record_p));
            break;
        }
        default:
            CN_LOG_D("unknown modem property type! (type:%d)", request_data_p->modem_property.type);
            send_response(record_p->client_id, CN_RESPONSE_SET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_modem_property_set (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_modem_property_set <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_SET_MODEM_PROPERTY, "mal_call_request_modem_property_set");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_MODEM_PROPERTY, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_get_modem_property(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    cn_modem_property_t *modem_property_p = NULL;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        mal_call_modem_property mal_prop;
        cn_request_get_modem_property_t *request_data_p = NULL;

        CN_LOG_D("mal_call_request_modem_property_get -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        request_data_p = (cn_request_get_modem_property_t *) data_p;
        mal_prop = (mal_call_modem_property) request_data_p->type;

        record_p->request_data_p = calloc(1, sizeof(cn_modem_property_t));

        if (!record_p->request_data_p) {
            CN_LOG_E("calloc failed for record_p->request_data_p!");
            send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        memcpy(record_p->request_data_p, request_data_p, sizeof(cn_modem_property_t));

        switch (request_data_p->type) {
        case CN_MODEM_PROPERTY_TYPE_CTM:
        case CN_MODEM_PROPERTY_TYPE_CLIR:
        case CN_MODEM_PROPERTY_TYPE_ALS:
            result = mal_call_request_modem_property_get(&mal_prop, request_record_get_modem_tag(record_p));
            break;
        default:
            CN_LOG_D("unknown modem property type! (type:%d)", request_data_p->type);
            free(record_p->request_data_p);
            send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_modem_property_get (result:%d)", result);
            free(record_p->request_data_p);
            send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        modem_property_p = (cn_modem_property_t *)record_p->request_data_p;
        modem_property_p->type = request_data_p->type;

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        cn_modem_property_t modem_property;

        CN_LOG_D("mal_call_request_modem_property_get <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_call_request_modem_property_get (result:%d)", record_p->response_error_code);
            free(record_p->request_data_p);
            send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_call_request_modem_property_get not found!");
            free(record_p->request_data_p);
            send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->request_data_p) {
            CN_LOG_E("response data for mal_call_request_modem_property_get not found!");
            send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        /* Find out what property type was in the request to begin with */
        modem_property.type = ((cn_modem_property_t *)record_p->request_data_p)->type;

        switch (modem_property.type) {
        case CN_MODEM_PROPERTY_TYPE_CTM: {
            mal_call_modem_prop_ctm *mal_ctm_p = (mal_call_modem_prop_ctm *) record_p->response_data_p;
            modem_property.value.ctm = (cn_modem_prop_value_ctm_t) * mal_ctm_p;
            break;
        }
        case CN_MODEM_PROPERTY_TYPE_CLIR: {
            uint32_t *mal_response_p = (uint32_t *) record_p->response_data_p;
            modem_property.value.clir.clir_setting = mal_response_p[0]; /* "n" parameter from 3GPP 27.007: parameter sets the adjustment for outgoing calls */
            modem_property.value.clir.service_status = mal_response_p[1]; /* "m" parameter from 3GPP 27.007: parameter shows the subscriber CLIR service status in the network */
            break;
        }
        case CN_MODEM_PROPERTY_TYPE_ALS: {
            mal_call_modem_prop_als_resp *mal_als_p = (mal_call_modem_prop_als_resp *) record_p->response_data_p;
            modem_property.value.als.current_line = (cn_als_line_t) mal_als_p->current_line;
            modem_property.value.als.supported_lines = (cn_supported_als_lines_t) mal_als_p->supported_lines;
            break;
        }
        default:
            CN_LOG_D("unknown modem property type! (type:%d)", modem_property.type);
            free(record_p->request_data_p);
            send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_SUCCESS, record_p->client_tag, sizeof(modem_property), &modem_property);
        free(record_p->request_data_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        free(record_p->request_data_p);
        send_response(record_p->client_id, CN_RESPONSE_GET_MODEM_PROPERTY, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_set_cssn(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_CSSN, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_request_set_cssn_t *request_data_p = (cn_request_set_cssn_t *)data_p;
    request_status_t status = REQUEST_STATUS_DONE;
    cn_sint32_t result = 0;


    REQUIRE_VALID_REQUEST_DATA(data_p);
    REQUIRE_VALID_RECORD(record_p);

    CN_LOG_D("mal_call_set_supp_svc_notification -> synchronous request");

    /* Synchronous request */
    result = mal_call_set_supp_svc_notification((void *)&request_data_p->cssn_setting);

    if (MAL_SUCCESS != result) {
        CN_LOG_E("mal_call_set_supp_svc_notification (result:%d)!", result);
        send_response(record_p->client_id, CN_RESPONSE_SET_CSSN, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_SET_CSSN, CN_SUCCESS, record_p->client_tag, 0, NULL);

    return REQUEST_STATUS_DONE;

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_dtmf_send(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        int no_of_calls = 0;
        cn_sint32_t call_id = 0;
        cn_call_context_t call_context;
        cn_call_state_filter_t filter = CN_CALL_STATE_FILTER_NONE;
        cn_request_dtmf_send_t *request_data_p = (cn_request_dtmf_send_t *) data_p;
        mal_call_dtmf_info dtmf_info;

        REQUIRE_VALID_REQUEST_DATA(data_p);

        /* Get active or alerting call */
        filter = CN_CALL_STATE_FILTER_ACTIVE | CN_CALL_STATE_FILTER_ALERTING;
        no_of_calls = util_get_call_by_filter(&filter, &call_context);

        if (1 > no_of_calls) {
            CN_LOG_E("no active or alerting call!");
            send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        /* An active call must carry speech or emergency or multimedia to allow DTMF */
        if (CN_CALL_STATE_ACTIVE == call_context.call_state) {
            if (CN_CALL_MODE_SPEECH != call_context.mode &&
                    CN_CALL_MODE_EMERGENCY != call_context.mode &&
                    CN_CALL_MODE_MULTIMEDIA != call_context.mode) {
                CN_LOG_E("active call is not a speech or emergency or multimedia call!");
                send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            /* Alerting call must have the mode set, which implies that the progress indicator IE has been received */
        } else {
            if (CN_CALL_MODE_UNKNOWN == call_context.mode) {
                CN_LOG_E("alerting call is has no mode set!");
                send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }
        }

        call_id = call_context.call_id;

        switch (request_data_p->dtmf_string.string_type) {
        case CN_DTMF_STRING_TYPE_ASCII:
            dtmf_info.string_type = UTF8_STRING;
            break;
        case CN_DTMF_STRING_TYPE_BCD:
            dtmf_info.string_type = BCD_STRING;
            break;
        default:
            CN_LOG_E("Unknown dtmf string type");
            send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        dtmf_info.length = request_data_p->dtmf_string.length;
        dtmf_info.dtmf_string = request_data_p->dtmf_string.dtmf_string;

        if (MAX_DTMF_DURATION_MS > request_data_p->dtmf_string.duration_time) {
            dtmf_info.dtmf_duration_time = request_data_p->dtmf_string.duration_time;
        } else {
            dtmf_info.dtmf_duration_time = MAX_DTMF_DURATION_MS;
        }

        if (MAX_DTMF_PAUSE_MS > request_data_p->dtmf_string.pause_time) {
            dtmf_info.dtmf_pause_time = request_data_p->dtmf_string.pause_time;
        } else {
            dtmf_info.dtmf_pause_time = MAX_DTMF_PAUSE_MS;
        }

        CN_LOG_D("mal_call_request_dtmf_send -> request");
        result = mal_call_request_dtmf_send(&call_id, &dtmf_info, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_dtmf_send (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_dtmf_send <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_DTMF_SEND, "mal_call_request_dtmf_send");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_DTMF_SEND, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_dtmf_start(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        int no_of_calls = 0;
        cn_sint32_t call_id = 0;
        cn_call_context_t call_context;
        cn_call_state_filter_t filter = CN_CALL_STATE_FILTER_NONE;
        cn_request_dtmf_start_t *request_data_p = (cn_request_dtmf_start_t *) data_p;

        REQUIRE_VALID_REQUEST_DATA(data_p);

        /* Get active or alerting call */
        filter = CN_CALL_STATE_FILTER_ACTIVE | CN_CALL_STATE_FILTER_ALERTING;
        no_of_calls = util_get_call_by_filter(&filter, &call_context);

        if (1 > no_of_calls) {
            CN_LOG_E("no active or alerting call!");
            send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        /* An active call must carry speech or emergency or multimedia to allow DTMF */
        if (CN_CALL_STATE_ACTIVE == call_context.call_state) {
            if (CN_CALL_MODE_SPEECH != call_context.mode &&
                    CN_CALL_MODE_EMERGENCY != call_context.mode &&
                    CN_CALL_MODE_MULTIMEDIA != call_context.mode) {
                CN_LOG_E("active call is not a speech or emergency or multimedia call!");
                send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            /* Alerting call must have the mode set, which implies that the progress indicator IE has been received */
        } else {
            if (CN_CALL_MODE_UNKNOWN == call_context.mode) {
                CN_LOG_E("alerting call is has no mode set!");
                send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }
        }

        call_id = call_context.call_id;

        CN_LOG_D("mal_call_request_dtmf_start -> request");
        result = mal_call_request_dtmf_start(&call_id, &request_data_p->character, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_dtmf_start (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_dtmf_start <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_DTMF_START, "mal_call_request_dtmf_start");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_DTMF_START, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_dtmf_stop(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        int no_of_calls = 0;
        cn_sint32_t call_id = 0;
        cn_call_context_t call_context;
        cn_call_state_filter_t filter = CN_CALL_STATE_FILTER_NONE;

        (void)data_p;

        /* Get active or alerting call */
        filter = CN_CALL_STATE_FILTER_ACTIVE | CN_CALL_STATE_FILTER_ALERTING;
        no_of_calls = util_get_call_by_filter(&filter, &call_context);

        if (1 > no_of_calls) {
            CN_LOG_E("no active or alerting call!");
            send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        /* An active call must carry speech or emergency or multimedia to allow DTMF */
        if (CN_CALL_STATE_ACTIVE == call_context.call_state) {
            if (CN_CALL_MODE_SPEECH != call_context.mode &&
                    CN_CALL_MODE_EMERGENCY != call_context.mode &&
                    CN_CALL_MODE_MULTIMEDIA != call_context.mode) {
                CN_LOG_E("active call is not a speech or emergency or multimedia call!");
                send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            /* Alerting call must have the mode set, which implies that the progress indicator IE has been received */
        } else {
            if (CN_CALL_MODE_UNKNOWN == call_context.mode) {
                CN_LOG_E("alerting call is has no mode set!");
                send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }
        }

        call_id = call_context.call_id;

        CN_LOG_D("mal_call_request_dtmf_stop -> request");
        result = mal_call_request_dtmf_stop(&call_id, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_call_request_dtmf_stop (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_call_request_dtmf_stop <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            util_call_failure_response_cause(data_p, record_p, CN_RESPONSE_DTMF_STOP, "mal_call_request_dtmf_stop");
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_DTMF_STOP, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

#ifdef USE_MAL_CS
static request_status_t set_call_waiting(void *data_p, request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        int data[2] = {0, 0};
        cn_request_set_call_waiting_t *request_data_p = (cn_request_set_call_waiting_t *) data_p;

        CN_LOG_D("mal_ss_set_call_waiting -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        data[0] = request_data_p->call_waiting.setting;
        data[1] = request_data_p->call_waiting.service_class;
        result = mal_ss_set_call_waiting(data, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_set_call_waiting (result:%d)", result);
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_set_call_waiting <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_set_call_waiting (result:%d)", record_p->response_error_code);
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    return status;
}
#endif /* USE_MAL_CS */

request_status_t handle_request_set_call_waiting(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_CALL_WAITING,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    status = set_call_waiting(data_p, record_p);


    if (REQUEST_STATUS_DONE == status) {
        send_response(record_p->client_id, CN_RESPONSE_SET_CALL_WAITING, CN_SUCCESS, record_p->client_tag, 0, NULL);
    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_SET_CALL_WAITING, CN_FAILURE, record_p->client_tag, 0, NULL);
    }

    return status;
#endif /* USE_MAL_CS */
}

#ifdef USE_MAL_CS
static request_status_t query_call_waiting(void *data_p,
        request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_get_call_waiting_t *request_data_p = (cn_request_get_call_waiting_t *) data_p;

        CN_LOG_D("mal_ss_query_call_waiting -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_ss_query_call_waiting(&request_data_p->service_class, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_query_call_waiting (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_query_call_waiting <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_query_call_waiting (result:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_query_call_waiting not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    return status;
}
#endif /* USE_MAL_CS */

request_status_t handle_request_get_call_waiting(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_GET_CALL_WAITING,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    status = query_call_waiting(data_p, record_p);

    if (REQUEST_STATUS_DONE == status) {
        if (NULL == record_p->response_data_p) {
            send_response(record_p->client_id, CN_RESPONSE_GET_CALL_WAITING, CN_FAILURE, record_p->client_tag, 0, NULL);
            status = REQUEST_STATUS_ERROR;
        } else {
            cn_call_waiting_t call_waiting;
            mal_ss_service_response *response_p = (mal_ss_service_response *)record_p->response_data_p;

            call_waiting.setting = response_p->response[0];
            call_waiting.service_class = response_p->response[1];

            send_response(record_p->client_id, CN_RESPONSE_GET_CALL_WAITING, CN_SUCCESS, record_p->client_tag, sizeof(call_waiting), &call_waiting);
        }

    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_GET_CALL_WAITING, CN_FAILURE, record_p->client_tag, 0, NULL);
    }

    return status;
#endif /* USE_MAL_CS */
}

#define EXE_SS_BAD_PASSWORD                     ( ((SS_ERROR_TYPE_GSM_PASSWORD) << 8 ) | MAL_SS_GSM_BAD_PASSWORD )
#define EXE_SS_BAD_PASSWORD_FORMAT              ( ((SS_ERROR_TYPE_GSM_PASSWORD) << 8 ) | MAL_SS_GSM_BAD_PASSWORD_FORMAT )
#define EXE_SS_GSM_MAX_PW_ATT_VIOLATION         ( ((SS_ERROR_TYPE_GSM) << 8 )          | 0x0000002b )
#define EXE_SS_GSM_UNEXPECTED_DATA_VALUE        ( ((SS_ERROR_TYPE_GSM) << 8 )          | 0x00000024 )
#define EXE_SS_GSM_NEGATIVE_PASSWORD_CHECK      ( ((SS_ERROR_TYPE_GSM) << 8 )          | 0x00000026 )
request_status_t handle_request_change_barring_password(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_CHANGE_BARRING_PASSWORD, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_change_barring_password_t *request_data_p = (cn_request_change_barring_password_t *) data_p;

        CN_LOG_D("mal_ss_change_barring_password -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_ss_change_barring_password(request_data_p->facility,
                                                request_data_p->old_passwd, request_data_p->new_passwd, request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_change_barring_password (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_CHANGE_BARRING_PASSWORD, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        cn_error_code_t error_code;
        mal_ss_service_response *response_p = NULL;
        CN_LOG_D("mal_ss_change_barring_password <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_change_barring_password (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_CHANGE_BARRING_PASSWORD, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        response_p = (mal_ss_service_response *)record_p->response_data_p;

        if (NULL != response_p && NULL != response_p->error_info) {
            mal_ss_error_type_value
            *mal_error_struct_p = response_p->error_info;

            /* MAL error code separated in two values, merge the two values into on to facilitate easy decoding */
            int mal_error_code = (((mal_error_struct_p->error_type << 8) & 0x0000ff00) | (mal_error_struct_p->error_value & 0x000000ff));

            switch (mal_error_code) {
            case EXE_SS_BAD_PASSWORD: /* fall through intentional */
            case EXE_SS_BAD_PASSWORD_FORMAT:
            case EXE_SS_GSM_UNEXPECTED_DATA_VALUE:
            case EXE_SS_GSM_MAX_PW_ATT_VIOLATION:
            case EXE_SS_GSM_NEGATIVE_PASSWORD_CHECK:
                error_code = CN_REQUEST_INCORRECT_PASSWORD;
                break;
            default:
                error_code = CN_REQUEST_NOT_ALLOWED;
                break;
            }
        } else {
            /* No response data indicates success */
            error_code = CN_SUCCESS;
        }

        send_response(record_p->client_id, CN_RESPONSE_CHANGE_BARRING_PASSWORD, error_code, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_CHANGE_BARRING_PASSWORD, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

error:
    return status;
#endif /* USE_MAL_CS */
}

#ifdef USE_MAL_CS
static request_status_t query_call_barring(void *data_p,
        request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_query_call_barring_t *request_data_p =
            (cn_request_query_call_barring_t *) data_p;

        CN_LOG_D("mal_ss_query_call_barring -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_ss_query_call_barring(
                     request_data_p->facility,
                     &request_data_p->service_class,
                     request_record_get_modem_tag(
                         record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_query_call_barring (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_query_call_barring <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_query_call_barring (result:%d)", record_p->response_error_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_ss_query_call_barring not found!");
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }

error:
    return status;
}
#endif /* USE_MAL_CS */

request_status_t handle_request_query_call_barring(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_BARRING,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    status = query_call_barring(data_p, record_p);

    if (REQUEST_STATUS_DONE == status) {
        if (NULL == record_p->response_data_p) {
            send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_BARRING, CN_FAILURE, record_p->client_tag, 0, NULL);
            status = REQUEST_STATUS_ERROR;
        } else {
            mal_ss_service_response *response_p = NULL;
            cn_call_barring_t call_barring;
            response_p = (mal_ss_service_response *)(record_p->response_data_p);
            call_barring.setting = response_p->response[0];
            call_barring.service_class = response_p->response[1];

            send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_BARRING, CN_SUCCESS, record_p->client_tag, sizeof(call_barring), &call_barring);
        }
    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_QUERY_CALL_BARRING, CN_FAILURE, record_p->client_tag, 0, NULL);
    }

    return status;
#endif /* USE_MAL_CS */
}

#ifdef USE_MAL_CS
static request_status_t set_call_barring(void *data_p,
        request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {

        cn_request_set_call_barring_t *request_data_p =
            (cn_request_set_call_barring_t *) data_p;

        CN_LOG_D("mal_ss_set_call_barring -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_ss_set_call_barring(
                     request_data_p->facility,
                     &request_data_p->call_barring,
                     request_data_p->passwd,
                     request_record_get_modem_tag(
                         record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_ss_set_call_barring (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_ss_set_call_barring <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_ss_set_call_barring (result:%d)", record_p->response_error_code);
            goto error;
        }

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }

error:
    return status;
}
#endif /* USE_MAL_CS */

request_status_t handle_request_set_call_barring(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_CALL_BARRING,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    status = set_call_barring(data_p, record_p);

    if (REQUEST_STATUS_DONE == status) {
        cn_error_code_t error_code;

        mal_ss_service_response *response_p = (mal_ss_service_response *)record_p->response_data_p;

        if (NULL != response_p && NULL != response_p->error_info) {
            mal_ss_error_type_value
            *mal_error_struct_p = response_p->error_info;

            /* MAL error code separated in two values, merge the two values into on to facilitate easy decoding */
            int mal_error_code = (((mal_error_struct_p->error_type << 8) & 0x0000ff00) | (mal_error_struct_p->error_value & 0x000000ff));

            switch (mal_error_code) {
            case EXE_SS_BAD_PASSWORD: /* fall through intentional */
            case EXE_SS_BAD_PASSWORD_FORMAT:
            case EXE_SS_GSM_UNEXPECTED_DATA_VALUE:
            case EXE_SS_GSM_MAX_PW_ATT_VIOLATION:
            case EXE_SS_GSM_NEGATIVE_PASSWORD_CHECK:
                error_code = CN_REQUEST_INCORRECT_PASSWORD;
                break;
            default:
                error_code = CN_REQUEST_NOT_ALLOWED;
                break;
            }
        } else {
            /* No response data indicates success */
            error_code = CN_SUCCESS;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_CALL_BARRING, error_code, record_p->client_tag, 0, NULL);
    } else if (REQUEST_STATUS_ERROR == status) {
        send_response(record_p->client_id, CN_RESPONSE_SET_CALL_BARRING, CN_FAILURE, record_p->client_tag, 0, NULL);
    }

    return status;
#endif /* USE_MAL_CS */
}

request_status_t handle_request_set_product_profile_flag(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_MIS
    (void) data_p;
    CN_LOG_I("MAL MIS disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_set_product_profile_flag_t *request_data_p = (cn_request_set_product_profile_flag_t *) data_p;

        CN_LOG_D("mal_mis_set_pp_flag -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_mis_set_pp_flag(request_data_p->flag_id, request_data_p->flag_value, request_record_get_modem_tag(record_p));

        if (MAL_MIS_SUCCESS != result) {
            CN_LOG_E("mal_ss_set_call_waiting (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_mis_set_pp_flag <- response");

        if (MAL_MIS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mis_set_pp_flag (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_PRODUCT_PROFILE_FLAG, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_MIS */
}

request_status_t handle_request_set_l1_parameter(
    void *data_p,
    request_record_t *record_p)
{
#if (!defined(USE_MAL_MIS) || !defined(USE_MAL_MTE) || !defined(USE_MAL_NET))
    (void) data_p;
    CN_LOG_I("MAL MIS, MAL MTE or MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_set_l1_parameter_t *request_data_p = (cn_request_set_l1_parameter_t *) data_p;
        REQUIRE_VALID_REQUEST_DATA(data_p);

        if (L1CMD_ANTENNA_PATH == request_data_p->command) { /* selecting antenna path */
            CN_LOG_D("mal_mis_set_pp_flag -> request");

            record_p->request_data_p = calloc(1, sizeof(cn_request_set_l1_parameter_t));

            if (NULL == record_p->request_data_p) {
                CN_LOG_E("calloc failed for record_p->request_data_p!");
                send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            memcpy(record_p->request_data_p, request_data_p, sizeof(cn_request_set_l1_parameter_t));

            result = mal_mis_set_pp_flag(INFO_PP_WCDMA_RX_DIVERSITY_SUPPORT_ID, INFO_PP_WCDMA_RX_DIVERSITY_SUPPORT_VALUE,
                                         request_record_get_modem_tag(record_p));

            if (MAL_MIS_SUCCESS != result) {
                CN_LOG_E("mal_mis_set_pp_flag (result:%d)", result);
                send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }

            record_p->state = REQUEST_STATE_SET_ANTENNA_PPFLAG_RESPONSE;
        } else if (L1CMD_UARFCN_SCAN == request_data_p->command) {
            CN_LOG_D("mal_net_test_carrier -> request");
            mal_net_test_carrier_data carrier_data;
            carrier_data.carrier_operation = (request_data_p->parameter > 0) ? MAL_NET_TEST_CARRIER_SET : MAL_NET_TEST_CARRIER_CLEAR;
            carrier_data.carrier_type = MAL_NET_CARRIER_TYPE_WCDMA;
            carrier_data.band_selection = MAL_NET_GSM900_1800;
            carrier_data.channel_nbr = request_data_p->parameter;

            result = mal_net_test_carrier(&carrier_data, request_record_get_modem_tag(record_p));

            record_p->state = REQUEST_STATE_TEST_CARRIER_RESPONSE;
        } else { /* to add handling of another L1 command just add it here along with an additional state */
            CN_LOG_E("invalid command parameter");
            goto error; /* incorrect parameter, should never happen - filtered by AT parser */
        }

        return REQUEST_STATUS_PENDING;
    }

    case REQUEST_STATE_SET_ANTENNA_PPFLAG_RESPONSE: {
        CN_LOG_D("mal_mis_set_pp_flag <- response");

        if (MAL_MIS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mis_set_pp_flag (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        CN_LOG_D("mal_mte_select_antenna_path -> request");

        cn_request_set_l1_parameter_t *request_data_p = (cn_request_set_l1_parameter_t *) record_p->request_data_p;

        result = mal_mte_select_antenna_path(request_data_p->parameter, request_record_get_modem_tag(record_p));

        if (MAL_MTE_SUCCESS != result) {
            CN_LOG_E("mal_mte_select_antenna_path (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_SELECT_ANTENNA_PATH_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }

    case REQUEST_STATE_SELECT_ANTENNA_PATH_RESPONSE: {
        CN_LOG_D("mal_mte_select_antenna_path <- response");

        if (MAL_MTE_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mte_select_antenna_path (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_SUCCESS, record_p->client_tag, 0, NULL);

        free(record_p->request_data_p);
        return REQUEST_STATUS_DONE;
    }

    case REQUEST_STATE_TEST_CARRIER_RESPONSE: {
        CN_LOG_D("mal_net_test_carrier <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_test_carrier (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_SUCCESS, record_p->client_tag, 0, NULL);
        return REQUEST_STATUS_DONE;
    }

    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_L1_PARAMETER, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    free(record_p->request_data_p);
    return status;
#endif /* USE_MAL_MIS and USE_MAL_MTE and USE_MAL_NET */
}

request_status_t handle_request_set_user_activity_status(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_GSS
    (void) data_p;
    CN_LOG_I("MAL GSS disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_set_user_activity_status_t *request_data_p = (cn_request_set_user_activity_status_t *) data_p;
        mal_gss_env_info_t env_info;
        CN_LOG_D("mal_gss_set_environment_information -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        memset(&env_info, 0, sizeof(mal_gss_env_info_t));
        env_info.user_activity_mode = request_data_p->user_status;
        env_info.battery_info = request_data_p->battery_status;
        result = mal_gss_set_environment_information(&env_info, request_record_get_modem_tag(record_p));

        if (MAL_GSS_SUCCESS != result) {
            CN_LOG_E("mal_gss_set_environment_information (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_gss_set_environment_information <- response");

        if (MAL_GSS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_gss_set_environment_information (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_GSS */
}

request_status_t handle_request_set_neighbour_cells_reporting(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_request_set_neighbour_cells_reporting_t *request_data_p = (cn_request_set_neighbour_cells_reporting_t *) data_p;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_REQUEST_DATA(data_p);
    REQUIRE_VALID_RECORD(record_p);

    /*** NOTE: Synchronous MAL call ***/
    result = mal_net_set_control_unsol_neighbour_cell_info(request_data_p->enable_reporting, request_data_p->rat_type);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("mal_net_set_control_unsol_neighbour_cell_info failed! (result:%d)", result);
        send_response(record_p->client_id, CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_SET_NEIGHBOUR_CELLS_REPORTING, CN_SUCCESS, record_p->client_tag, 0, NULL);

    return REQUEST_STATUS_DONE;

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_get_neighbour_cells_reporting(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_request_get_neighbour_cells_reporting_t *request_data_p = (cn_request_get_neighbour_cells_reporting_t *) data_p;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    uint8_t mode = 0;
    cn_bool_t reporting_enabled = 0;


    REQUIRE_VALID_REQUEST_DATA(data_p);
    REQUIRE_VALID_RECORD(record_p);

    /*** NOTE: Synchronous MAL call ***/
    result = mal_net_get_control_unsol_neighbour_cell_info(&mode, request_data_p->rat_type);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("mal_net_set_control_unsol_neighbour_cell_info failed! (result:%d)", result);
        send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    reporting_enabled = (cn_bool_t) mode;
    send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_REPORTING, CN_SUCCESS, record_p->client_tag, sizeof(reporting_enabled), &reporting_enabled);

    return REQUEST_STATUS_DONE;

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_get_neighbour_cells_basic_extd_info(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    CN_LOG_D("Entering handle_request_get_neighbour_cells_basic_extd_info");
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_get_neighbour_cells_info_t *request_data_p = (cn_request_get_neighbour_cells_info_t *) data_p;
        mal_net_neighbour_request_type request_type = MAL_NET_CELL_INFO_STATE_NEIGH_ECID_FETCH;

        if(cn_util_map_rat_to_neigh_rat(s_reg_info.rat) != request_data_p->rat_type){
            CN_LOG_E("RAT Mismatch");
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        CN_LOG_D("mal_net_get_neighbour_cell_ids -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_net_get_neighbour_cell_ids(request_data_p->rat_type, request_record_get_modem_tag(record_p), request_type);

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_get_neighbour_cell_ids failed! (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        cn_uint32_t size = 0;
        mal_net_neighbour_cells_info *mal_neighbour_cells_info = NULL;
        cn_neighbour_cells_info_t    *cn_neighbour_cells_info = NULL;

        CN_LOG_D("mal_net_get_neighbour_cell_ids <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            cn_error_code_t error_code = CN_FAILURE;
            CN_LOG_E("mal_net_get_neighbour_cell_ids failed! (result:%d)", record_p->response_error_code);
            if(record_p->response_error_code == MAL_NET_INTERRUPTED){
                error_code = CN_REQUEST_INTERRUPTED;
            }
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, error_code, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_get_neighbour_cell_ids not found!");
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        mal_neighbour_cells_info = (mal_net_neighbour_cells_info *)record_p->response_data_p;

        cn_neighbour_cells_info = calloc(1,sizeof(cn_neighbour_cells_info_t));

        result = util_copy_neighbour_cells_info_data(cn_neighbour_cells_info, mal_neighbour_cells_info, &size);

        if (-1 == result) {
            CN_LOG_E("util_copy_neighbour_cells_info_data failed!");
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            free(cn_neighbour_cells_info);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_SUCCESS, record_p->client_tag, size, cn_neighbour_cells_info);
        free(cn_neighbour_cells_info);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_COMPLETE_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_get_neighbour_cells_extd_info(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    CN_LOG_D("Entering handle_request_get_neighbour_cells_extd_info");
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_get_neighbour_cells_info_t *request_data_p = (cn_request_get_neighbour_cells_info_t *) data_p;
        mal_net_neighbour_request_type request_type = MAL_NET_CELL_INFO_STATE_ECID_FETCH;

        if(cn_util_map_rat_to_neigh_rat(s_reg_info.rat) != request_data_p->rat_type){
            CN_LOG_E("RAT Mismatch");
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }
        CN_LOG_D("mal_net_get_neighbour_cell_ids -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_net_get_neighbour_cell_ids(request_data_p->rat_type, request_record_get_modem_tag(record_p), request_type);

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_get_neighbour_cell_ids failed! (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        cn_uint32_t size = 0;
        mal_net_neighbour_cells_info *mal_neighbour_cells_info = NULL;
        cn_neighbour_cells_info_t    *cn_neighbour_cells_info = NULL;

        CN_LOG_D("mal_net_get_neighbour_cell_ids <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            cn_error_code_t error_code = CN_FAILURE;
            CN_LOG_E("mal_net_get_neighbour_cell_ids failed! (result:%d)", record_p->response_error_code);
            if(record_p->response_error_code == MAL_NET_INTERRUPTED){
                error_code = CN_REQUEST_INTERRUPTED;
            }
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, error_code, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_get_neighbour_cell_ids not found!");
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        mal_neighbour_cells_info = (mal_net_neighbour_cells_info *)record_p->response_data_p;

        cn_neighbour_cells_info = calloc(1,sizeof(cn_neighbour_cells_info_t));

        result = util_copy_neighbour_cells_info_data(cn_neighbour_cells_info, mal_neighbour_cells_info, &size);

        if (-1 == result) {
            CN_LOG_E("util_copy_neighbour_cells_info_data failed!");
            send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
            free(cn_neighbour_cells_info);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_SUCCESS, record_p->client_tag, size, cn_neighbour_cells_info);
        free(cn_neighbour_cells_info);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_GET_NEIGHBOUR_CELLS_EXTD_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_set_event_reporting(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_EVENT_REPORTING,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    cn_request_set_event_reporting_t *request_data_p = (cn_request_set_event_reporting_t *) data_p;


    REQUIRE_VALID_RECORD(record_p);
    REQUIRE_VALID_REQUEST_DATA(data_p);

    /*** NOTE: Synchronous MAL call ***/
    CN_LOG_D("synchronous call to mal_net_set_ind_state, type=%d, enable_reporting=%d",
             request_data_p->type, request_data_p->enable_reporting);
    result = mal_net_set_ind_state((mal_net_ind_type) request_data_p->type, request_data_p->enable_reporting);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("mal_net_set_ind_state failed! (result:%d)", result);
        send_response(record_p->client_id, CN_RESPONSE_SET_EVENT_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_SET_EVENT_REPORTING, CN_SUCCESS, record_p->client_tag, 0, NULL);

    return REQUEST_STATUS_DONE;

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_get_rab_status(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_GET_RAB_STATUS,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    cn_rab_status_t rab_status = 0;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    /*** NOTE: Synchronous MAL call ***/
    result = mal_net_get_rab_status((mal_net_rab_status_type *)&rab_status);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("mal_net_get_rab_status failed! (result:%d)", result);
        send_response(record_p->client_id, CN_RESPONSE_GET_RAB_STATUS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_GET_RAB_STATUS, CN_SUCCESS, record_p->client_tag, sizeof(rab_status), &rab_status);

    return REQUEST_STATUS_DONE;

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_baseband_version(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_MIS
    (void) data_p;
    CN_LOG_I("MAL MIS disabled");
    send_response(record_p->client_id, CN_RESPONSE_BASEBAND_VERSION,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {

        CN_LOG_D("mal_mis_req_baseband_version -> request");

        (void)data_p;

        result = mal_mis_req_baseband_version(request_record_get_modem_tag(record_p));

        if (MAL_MIS_SUCCESS != result) {
            CN_LOG_E("mal_mis_req_baseband_version (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_BASEBAND_VERSION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_mis_req_baseband_version <- response");
        cn_baseband_version_t baseband_version;

        if (MAL_MIS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mis_req_baseband_version (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_BASEBAND_VERSION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("mal_mis_req_baseband_version, no response data");
            send_response(record_p->client_id, CN_RESPONSE_BASEBAND_VERSION, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        strncpy(baseband_version, record_p->response_data_p,
                sizeof(cn_baseband_version_t));
        baseband_version[sizeof(cn_baseband_version_t) - 1] = '\0';

        send_response(record_p->client_id, CN_RESPONSE_BASEBAND_VERSION,
                      CN_SUCCESS, record_p->client_tag,
                      sizeof(cn_baseband_version_t), baseband_version);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_BASEBAND_VERSION, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_MIS */
}

request_status_t handle_request_get_pp_flags(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_MIS
    (void) data_p;
    CN_LOG_I("MAL MIS disabled");
    send_response(record_p->client_id, CN_RESPONSE_GET_PP_FLAGS,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_get_pp_flags_t *request_data_p = (cn_request_get_pp_flags_t *) data_p;

        CN_LOG_D("mal_mis_req_pp_flags -> request");

        result = mal_mis_req_pp_flags(request_data_p->read_all_flags, request_data_p->flag_id,
                                      request_record_get_modem_tag(record_p));

        if (MAL_MIS_SUCCESS != result) {
            CN_LOG_E("mal_mis_req_pp_flags (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_GET_PP_FLAGS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_mis_req_pp_flags <- response");
        cn_pp_flag_list_t *pp_flag_list_p = NULL;
        size_t size;

        if (MAL_MIS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mis_req_pp_flags (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_GET_PP_FLAGS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("mal_mis_req_pp_flags, no response data");
            send_response(record_p->client_id, CN_RESPONSE_GET_PP_FLAGS, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        pp_flag_list_p = (cn_pp_flag_list_t*)record_p->response_data_p;

        if (0 < pp_flag_list_p->count) {
            size = sizeof(cn_pp_flag_list_t) + (pp_flag_list_p->count - 1) * sizeof(cn_pp_flag_t);
        } else {
            size = sizeof(cn_pp_flag_list_t);
        }

        send_response(record_p->client_id, CN_RESPONSE_GET_PP_FLAGS,
                      CN_SUCCESS, record_p->client_tag,
                      size, pp_flag_list_p);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_GET_PP_FLAGS, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_MIS */
}

request_status_t handle_request_set_signal_info_config(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_CONFIG,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_set_signal_info_config_t *request_data_p = (cn_request_set_signal_info_config_t *) data_p;
        cn_sint32_t result = 0;
        mal_net_rssi_conf_data rssi_conf_data;
        uint8_t mode = 0;

        CN_LOG_D("mal_net_configure_rssi -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        /*** NOTE: Synchronous MAL call ***/
        result = mal_net_get_encsq_mode(&mode);

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_get_encsq_mode failed! (result:%d)", result);
            goto error;
        }

        memset(&rssi_conf_data, 0, sizeof(rssi_conf_data));
        rssi_conf_data.num_of_segments = (uint8_t)request_data_p->config.num_of_segments;
        rssi_conf_data.hysteresis_down = (uint8_t)request_data_p->config.hysteresis_down;
        rssi_conf_data.hysteresis_up = (uint8_t)request_data_p->config.hysteresis_up;
        rssi_conf_data.border[0] = (uint8_t)request_data_p->config.border[0];
        rssi_conf_data.border[1] = (uint8_t)request_data_p->config.border[1];
        rssi_conf_data.border[2] = (uint8_t)request_data_p->config.border[2];
        rssi_conf_data.border[3] = (uint8_t)request_data_p->config.border[3];
        rssi_conf_data.ccsq_mode = (uint8_t)request_data_p->config.ccsq_mode;

        result = mal_net_configure_rssi(&rssi_conf_data, request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_configure_rssi failed! (result:%d)", result);
            goto error;
        }

        /* If RSSI IND mode is disabled, synchronous MAL call, return CN_SUCCESS */
        if (CN_ENCSQ_DISABLE == mode) {
            send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_CONFIG, CN_SUCCESS, record_p->client_tag, 0, NULL);

            return REQUEST_STATUS_DONE;
        } else {
            record_p->state = REQUEST_STATE_RESPONSE;
            return REQUEST_STATUS_PENDING;
        }
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_configure_rssi <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_configure_rssi failed! (result:%d)", record_p->response_error_code);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_CONFIG, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);

    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_get_signal_info_config(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_GET_SIGNAL_INFO_CONFIG,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    mal_net_rssi_conf_data rssi_conf_data;;
    cn_signal_info_config_t response_data;


    REQUIRE_VALID_REQUEST_DATA(data_p);
    REQUIRE_VALID_RECORD(record_p);

    /*** NOTE: Synchronous MAL call ***/
    result = mal_net_get_rssi_conf_values(&rssi_conf_data);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("mal_net_get_rssi_conf_values failed! (result:%d)", result);
        goto error;
    }

    response_data.ccsq_mode = (cn_uint8_t)rssi_conf_data.ccsq_mode;
    response_data.border[0] = (cn_uint8_t)rssi_conf_data.border[0];
    response_data.border[1] = (cn_uint8_t)rssi_conf_data.border[1];
    response_data.border[2] = (cn_uint8_t)rssi_conf_data.border[2];
    response_data.border[3] = (cn_uint8_t)rssi_conf_data.border[3];

    send_response(record_p->client_id, CN_RESPONSE_GET_SIGNAL_INFO_CONFIG, CN_SUCCESS, record_p->client_tag, sizeof(response_data), &response_data);

    return REQUEST_STATUS_DONE;

error:
    send_response(record_p->client_id, CN_RESPONSE_GET_SIGNAL_INFO_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);

    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_set_signal_info_reporting(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SET_SIGNAL_INFO_REPORTING, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_sint32_t result = 0;
        cn_request_set_signal_info_reporting_t *request_data_p = (cn_request_set_signal_info_reporting_t *)data_p;
        uint8_t rssi_mode;

        CN_LOG_D("mal_net_enable_disable_rssi -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        rssi_mode = (request_data_p->mode == CN_RSSI_EVENT_REPORTING_ENABLE) ? 1 : 0;

        result = mal_net_enable_disable_rssi(rssi_mode, request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_enable_disable_rssi failed! (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_enable_disable_rssi <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_enable_disable_rssi failed! (result:%d)", record_p->response_error_code);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_REPORTING, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default:
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }

error:
    send_response(record_p->client_id, CN_RESPONSE_SET_SIGNAL_INFO_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);

    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_get_signal_info_reporting(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_GET_SIGNAL_INFO_REPORTING, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    uint8_t mode = 0;


    REQUIRE_VALID_REQUEST_DATA(data_p);
    REQUIRE_VALID_RECORD(record_p);

    /*** NOTE: Synchronous MAL call ***/
    result = mal_net_get_encsq_mode(&mode);

    if (MAL_NET_SUCCESS != result) {
        CN_LOG_E("mal_net_get_encsq_mode failed! (result:%d)", result);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_GET_SIGNAL_INFO_REPORTING, CN_SUCCESS, record_p->client_tag, sizeof(mode), &mode);

    return REQUEST_STATUS_DONE;

error:
    send_response(record_p->client_id, CN_RESPONSE_GET_SIGNAL_INFO_REPORTING, CN_FAILURE, record_p->client_tag, 0, NULL);

    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_get_timing_advance(
    void *data_p,
    request_record_t *record_p)
{
#ifndef USE_MAL_GSS
    (void) data_p;
    CN_LOG_I("MAL GSS disabled");
    send_response(record_p->client_id, CN_RESPONSE_TIMING_ADVANCE,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        mal_error_type result = MAL_FAIL;

        CN_LOG_D("mal_gss_get_timing_advance_value -> request");

        result = mal_gss_get_timing_advance_value(request_record_get_modem_tag(record_p));

        if (MAL_SUCCESS != result) {
            CN_LOG_E("mal_gss_get_timing_advance_value failed (%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_TIMING_ADVANCE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        mal_gss_timing_advance_info *mal_timing_advance_p = NULL;
        cn_timing_advance_info_t cn_timing_advance_info;

        CN_LOG_D("mal_gss_get_timing_advance_value <- response");

        if (MAL_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_gss_get_timing_advance_value! (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_TIMING_ADVANCE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        REQUIRE_VALID_RESPONSE_DATA(record_p->response_data_p);

        mal_timing_advance_p = (mal_gss_timing_advance_info *)record_p->response_data_p;
        memset(&cn_timing_advance_info, 0, sizeof(cn_timing_advance_info));
        cn_timing_advance_info.validity = mal_timing_advance_p->validity;
        cn_timing_advance_info.value = mal_timing_advance_p->value;

        send_response(record_p->client_id, CN_RESPONSE_TIMING_ADVANCE, CN_SUCCESS, record_p->client_tag, sizeof(cn_timing_advance_info), &cn_timing_advance_info);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_TIMING_ADVANCE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_GSS */

}

request_status_t handle_request_get_nmr_info(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id, CN_RESPONSE_NMR_INFO, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_nmr_info_t *request_data_p = (cn_request_nmr_info_t *) data_p;
        cn_sint32_t result = 0;

        CN_LOG_D("mal_net_req_nmr_info -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_net_req_nmr_info(request_data_p->rat,
                                      request_data_p->utran,
                                      request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("handle_request_get_nmr_info failed! (result:%d)", result);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;

    }
    case REQUEST_STATE_RESPONSE: {
        mal_net_nmr_info *response_data_p = NULL;
        cn_nmr_info_t cn_response;

        CN_LOG_D("mal_net_req_nmr_info <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_req_nmr_info failed! (result:%d)", record_p->response_error_code);
            goto error;
        }

        response_data_p = ((mal_net_nmr_info *)record_p->response_data_p);

        REQUIRE_VALID_RESPONSE_DATA(response_data_p);

        memset(&cn_response, 0, sizeof(cn_response));
        cn_response.rat = response_data_p->rat;

        if (CN_NMR_RAT_TYPE_GERAN == cn_response.rat) {
            int size = sizeof(cn_uint16_t) * response_data_p->geran_info.num_of_channels;

            if (CN_MAX_NMR_ARFCN_SIZE < size) {
                CN_LOG_E("mal_net_req_nmr_info failed! num_of_channels > max size");
                goto error;
            }

            cn_response.nmr_info.geran_info.num_of_channels = response_data_p->geran_info.num_of_channels;
            memcpy(cn_response.nmr_info.geran_info.measurement_results, response_data_p->geran_info.measurement_results, CN_MAX_NMR_RES_SIZE);
            memcpy(cn_response.nmr_info.geran_info.ARFCN_list, response_data_p->geran_info.ARFCN_list, size);
        } else {
            int size = response_data_p->utran_info.mm_report_len;

            if (CN_MAX_NMR_REPORT_SIZE < size) {
                CN_LOG_E("mal_net_req_nmr_info failed! mm_report_len > max size");
                goto error;
            }

            cn_response.nmr_info.utran_info.mm_report_len = response_data_p->utran_info.mm_report_len;
            memcpy(cn_response.nmr_info.utran_info.measurements_report, response_data_p->utran_info.measurements_report, size);
        }

        send_response(record_p->client_id, CN_RESPONSE_NMR_INFO, CN_SUCCESS, record_p->client_tag, sizeof(cn_response), &cn_response);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    send_response(record_p->client_id, CN_RESPONSE_NMR_INFO, CN_FAILURE, record_p->client_tag, 0, NULL);

    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_set_hsxpa_mode(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_GSS
    (void) data_p;
    CN_LOG_I("MAL GSS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_HSXPA_MODE,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_request_hsxpa_t *request_data_p = (cn_request_hsxpa_t *) data_p;
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {

        CN_LOG_D("mal_gss_set_hsxpa_mode -> request");

        REQUIRE_VALID_REQUEST_DATA(data_p);

        result = mal_gss_set_hsxpa_mode(request_data_p->hsxpa_mode, request_record_get_modem_tag(record_p));

        if (MAL_GSS_MODE_NOT_SUPPORTED == result || MAL_GSS_SERVICE_NOT_ALLOWED == result) {
            CN_LOG_E("mal_gss_set_hsxpa_mode failed, not supported");
            send_response(record_p->client_id, CN_RESPONSE_SET_HSXPA_MODE, CN_REQUEST_NOT_SUPPORTED, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (MAL_GSS_SUCCESS != result) {
            CN_LOG_E("mal_gss_set_hsxpa_mode failed");
            send_response(record_p->client_id, CN_RESPONSE_SET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_gss_set_hsxpa_mode <- response");

        if (MAL_GSS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_gss_set_hsxpa_mode (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SET_HSXPA_MODE, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_GSS */
}

request_status_t handle_request_get_hsxpa_mode(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_GSS
    (void) data_p;
    CN_LOG_I("MAL GSS disabled");
    send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE,
                  CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;
    cn_hsxpa_mode_t hsxpa_mode;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {

        CN_LOG_D("mal_gss_get_hsxpa_mode -> request");

        (void)data_p;

        result = mal_gss_get_hsxpa_mode(request_record_get_modem_tag(record_p));

        if (MAL_GSS_MODE_NOT_SUPPORTED == result || MAL_GSS_SERVICE_NOT_ALLOWED == result) {
            CN_LOG_E("mal_gss_get_hsxpa_mode failed, not supported");
            send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE, CN_REQUEST_NOT_SUPPORTED, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (MAL_GSS_SUCCESS != result) {
            CN_LOG_E("mal_gss_get_hsxpa_mode failed");
            send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_gss_get_hsxpa_mode <- response");

        if (MAL_GSS_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("handle_request_get_hsxpa_mode (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("mal_gss_get_hsxpa_mode, no response data");
            send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        hsxpa_mode = *((mal_gss_hsxpa_mode *)record_p->response_data_p);
        send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE, CN_SUCCESS, record_p->client_tag, sizeof(hsxpa_mode), &hsxpa_mode);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_GET_HSXPA_MODE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_GSS */
}

#ifdef USE_MAL_CS
static void handle_generic_ss_response(request_status_t status,
                                       void *data_p,
                                       request_record_t *record_p, cn_bool_t additional_results)
{
    (void)data_p;
    mal_ss_service_response *response_p = (mal_ss_service_response *)record_p->response_data_p;
    cn_ss_response_t cn_response;


    memset(&cn_response, 0, sizeof(cn_ss_response_t));

    if (REQUEST_STATUS_PENDING == status) {
        /* No result yet */
        return;
    }

    if (status == REQUEST_STATUS_ERROR) {

        if (NULL != record_p->response_data_p && NULL != response_p->error_info) {
            /* We have more specific error probably from network return success and error struct */
            if (NULL != response_p->additional_info) {
                /* We have data also should not be the case return error */
                CN_LOG_E("Request status = REQUEST_STATUS_ERROR with additional_info");
                goto error;
            }
        } else {
            CN_LOG_E("Request status = REQUEST_STATUS_ERROR with no error info");
            goto error;
        }
    }

    if (NULL != response_p->additional_info) {
        if (CN_SS_MAX_ADDITIONAL_RESULT_LENGTH < response_p->additional_info->return_result_length) {
            CN_LOG_E("response_p->additional_info is larger than CN_SS_MAX_ADDITIONAL_RESULT_LENGTH");
            goto error;
        }

        if (additional_results) {
            cn_response.additional_result.length = response_p->additional_info->return_result_length;


            memcpy(cn_response.additional_result.additional_result_data, response_p->additional_info->return_result, cn_response.additional_result.length);
        }

        send_response(record_p->client_id, CN_RESPONSE_SS, CN_SUCCESS,
                      record_p->client_tag, sizeof(cn_response),
                      &cn_response);
    } else if (NULL != response_p->error_info) {
        util_convert_ss_error_type(record_p->response_error_code, response_p->error_info, &(cn_response.error_code));
        send_response(record_p->client_id, CN_RESPONSE_SS, CN_SUCCESS,
                      record_p->client_tag, sizeof(cn_response),
                      &cn_response);
    } else if (status == REQUEST_STATUS_DONE) {
        send_response(record_p->client_id, CN_RESPONSE_SS, CN_SUCCESS,
                      record_p->client_tag, sizeof(cn_response),
                      &cn_response);
    } else {
        /* No error code, no error struct and no result, return error */
        CN_LOG_E("handle_generic_ss_response: No info available.\n");
        goto error;
    }

    return;

error:
    send_response(record_p->client_id, CN_RESPONSE_SS, CN_FAILURE,
                  record_p->client_tag, 0, NULL);

}
#endif /* USE_MAL_CS */

request_status_t handle_request_ss_command(void *data_p,
        request_record_t *record_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
    send_response(record_p->client_id, CN_RESPONSE_SS_COMMAND, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    cn_ss_mal_info_t *ss_state_info_p = NULL;
    cn_ss_string_compounds_t *ss_data_p = NULL;
    request_status_t status = REQUEST_STATUS_ERROR;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        REQUIRE_VALID_REQUEST_DATA(data_p);
        char *facility_p = NULL;
        cn_call_forwarding_reason_t reason = 0;
        cn_sint32_t mode = 0;
        cn_sint32_t result = 0;

        cn_request_ss_command_t *request_command_p = (cn_request_ss_command_t *)data_p;
        CN_LOG_D("MMI string = %s", request_command_p->ss_command.mmi_string);
        ss_data_p = decode_ss_string((char *)request_command_p->ss_command.mmi_string);

        if (!ss_data_p) {
            CN_LOG_E("decode_ss_string failed!");
            goto error;
        }

        ss_state_info_p = calloc(1, sizeof(*ss_state_info_p));

        if (!ss_state_info_p) {
            CN_LOG_E("calloc failed for ss_state_info_p!");
            goto error;
        }

        record_p->ss_request_info_p = ss_state_info_p;
        ss_state_info_p->additional_results = request_command_p->ss_command.additional_results;

        result = cn_util_convert_cn_procedure_type_to_mal_mode(ss_data_p, &mode);

        if (0 != result) {
            CN_LOG_E("cn_util_convert_cn_procedure_type_to_mal_mode failed!");
            goto error;
        }

        result = cn_util_convert_cn_ss_type_to_mal_cf_reason(ss_data_p, &reason);

        if (0 != result) {
            CN_LOG_E("cn_util_convert_cn_ss_type_to_mal_cf_reason failed!");
            goto error;
        }

        result = cn_util_convert_cn_ss_type_to_mal_ss_command(ss_data_p, &ss_state_info_p->command);

        if (0 != result) {
            CN_LOG_E("cn_util_convert_cn_ss_type_to_mal_ss_command failed!");
            goto error_not_allowed;
        }

        result = cn_util_convert_cn_ss_type_to_mal_ss_facility(ss_data_p, &facility_p);

        if (0 != result) {
            CN_LOG_E("cn_util_convert_cn_ss_type_to_mal_ss_facility failed!");
            goto error;
        }

        ss_state_info_p->result_function_p = handle_generic_ss_response; /* Default handler overridden below when needed */

        switch (ss_state_info_p->command) {
        case SS_MAL_SET_BARRING: {
            cn_sint32_t class = 0;

            cn_request_set_call_barring_t *barring_data_p = calloc(1, sizeof(cn_request_set_call_barring_t));

            if (NULL == barring_data_p) {
                CN_LOG_E("calloc failed for barring_data_p");
                goto error;
            }

            ss_state_info_p->data_p = barring_data_p;
            strncpy(barring_data_p->facility, facility_p, CN_MAX_STRING_SIZE);
            class = cn_util_convert_ss_service_class_to_cn_class(atoi(ss_data_p->supplementary_info_b));

            if (0 > class) {
                CN_LOG_E("No class available\n");
                goto error;
            }

            barring_data_p->call_barring.service_class = class;

            if (CN_SS_PROCEDURE_TYPE_REGISTRATION == ss_data_p->procedure_type ||
                    CN_SS_PROCEDURE_TYPE_ACTIVATION == ss_data_p->procedure_type) {
                barring_data_p->call_barring.setting = CN_CALL_BARRING_ACTIVATION;
            } else {
                barring_data_p->call_barring.setting = CN_CALL_BARRING_DEACTIVATION;
            }

            strncpy(barring_data_p->passwd, ss_data_p->supplementary_info_a, CN_MAX_STRING_SIZE);

            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->command_function_p = set_call_barring;
                status = set_call_barring(barring_data_p, record_p);
            } else { /* originated as CN_REQUEST_SET_CALL_BARRING (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_set_call_barring;
                status = handle_request_set_call_barring(barring_data_p, record_p);
                free(barring_data_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_QUERY_BARRING: {
            cn_sint32_t class = 0;
            cn_request_query_call_barring_t *barring_data_p = calloc(1, sizeof(cn_request_query_call_barring_t));
            ss_state_info_p->data_p = barring_data_p;

            if (NULL == barring_data_p) {
                CN_LOG_E("calloc failed for barring_data_p");
                goto error;
            }

            strncpy(barring_data_p->facility, facility_p, CN_MAX_STRING_SIZE);
            class = cn_util_convert_ss_service_class_to_cn_class(atoi(ss_data_p->supplementary_info_b));

            if (0 > class) {
                CN_LOG_E("wrong service class");
                goto error_not_supported;
            }

            barring_data_p->service_class = class;

            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->command_function_p = query_call_barring;
                status = query_call_barring(barring_data_p, record_p);
            } else { /* originated as CN_REQUEST_QUERY_CALL_BARRING (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_query_call_barring;
                status = handle_request_query_call_barring(barring_data_p, record_p);
                free(barring_data_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_SET_FORWARDING: {
            cn_sint32_t class = 0;
            cn_request_call_forward_info_t *forward_data_p = calloc(1, sizeof(cn_request_call_forward_info_t));
            ss_state_info_p->data_p = forward_data_p;

            if (NULL == forward_data_p) {
                CN_LOG_E("calloc failed for forward_data_p");
                goto error_not_supported;
            }

            strncpy(forward_data_p->call_forward_info.number, ss_data_p->supplementary_info_a, CN_MAX_STRING_SIZE);
            class = cn_util_convert_ss_service_class_to_cn_class(atoi(ss_data_p->supplementary_info_b));

            if (0 > class) {
                CN_LOG_E("wrong service class");
                goto error_not_supported;
            }

            forward_data_p->call_forward_info.service_class = class;
            forward_data_p->call_forward_info.time_seconds =  atoi(ss_data_p->supplementary_info_c);
            forward_data_p->call_forward_info.reason = reason;
            forward_data_p->call_forward_info.status = mode;
            forward_data_p->call_forward_info.toa = convert_toa(request_command_p->ss_command.type_of_number, request_command_p->ss_command.numbering_plan_id);
            CN_LOG_D("type_of_number = %d", request_command_p->ss_command.type_of_number);
            CN_LOG_D("numbering_plan_id = %d", request_command_p->ss_command.numbering_plan_id);
            CN_LOG_D("toa = %d", forward_data_p->call_forward_info.toa);

            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->command_function_p = set_call_forward;
                status = set_call_forward(forward_data_p, record_p);
            } else { /* originated as CN_REQUEST_SET_CALL_FORWARD (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_set_call_forward;
                status = handle_request_set_call_forward(forward_data_p, record_p);
                free(forward_data_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_QUERY_FORWARDING: {
            cn_sint32_t class = 0;
            cn_request_call_forward_info_t *forward_data_p = calloc(1, sizeof(cn_request_call_forward_info_t));

            if (NULL == forward_data_p) {
                CN_LOG_E("calloc failed for forward_data_p");
                goto error;
            }

            ss_state_info_p->data_p = forward_data_p;
            class = cn_util_convert_ss_service_class_to_cn_class(atoi(ss_data_p->supplementary_info_b));

            if (0 > class) {
                CN_LOG_E("wrong service class");
                goto error_not_supported;
            }

            forward_data_p->call_forward_info.service_class = class;
            forward_data_p->call_forward_info.reason = reason;
            forward_data_p->call_forward_info.status = mode;

            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->command_function_p = query_call_forward;
                status = query_call_forward(forward_data_p, record_p);
            } else { /* originated as CN_REQUEST_QUERY_CALL_FORWARD (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_query_call_forward;
                status = handle_request_query_call_forward(forward_data_p, record_p);
                free(forward_data_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_SET_WAITING: {
            cn_sint32_t class = 0;
            cn_request_set_call_waiting_t *waiting_data_p = calloc(1, sizeof(cn_request_set_call_waiting_t));

            if (NULL == waiting_data_p) {
                CN_LOG_E("calloc failed for waiting_data_p");
                goto error;
            }

            ss_state_info_p->data_p = waiting_data_p;
            class = cn_util_convert_ss_service_class_to_cn_class(atoi(ss_data_p->supplementary_info_a));

            if (0 > class) {
                CN_LOG_E("wrong service class");
                goto error_not_supported;
            }

            waiting_data_p->call_waiting.service_class = class;

            if (CN_SS_PROCEDURE_TYPE_ACTIVATION == ss_data_p->procedure_type ||
                    CN_SS_PROCEDURE_TYPE_REGISTRATION == ss_data_p->procedure_type) {
                waiting_data_p->call_waiting.setting = CN_CALL_WAITING_SERVICE_ENABLED;
            } else {
                waiting_data_p->call_waiting.setting = CN_CALL_WAITING_SERVICE_DISABLED;
            }

            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->command_function_p = set_call_waiting;
                status = set_call_waiting(waiting_data_p, record_p);
            } else { /* originated as CN_REQUEST_SET_CALL_WAITING (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_set_call_waiting;
                status = handle_request_set_call_waiting(waiting_data_p, record_p);
                free(waiting_data_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_QUERY_WAITING: {
            cn_sint32_t class = 0;
            cn_request_get_call_waiting_t *waiting_data_p = calloc(1, sizeof(cn_request_get_call_waiting_t));

            if (NULL == waiting_data_p) {
                CN_LOG_E("calloc failed for waiting_data_p");
                goto error;
            }

            ss_state_info_p->data_p = waiting_data_p;
            class = cn_util_convert_ss_service_class_to_cn_class(atoi(ss_data_p->supplementary_info_a));

            if (0 > class) {
                CN_LOG_E("wrong service class");
                goto error_not_supported;
            }

            waiting_data_p->service_class = class;

            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->command_function_p = query_call_waiting;
                status = query_call_waiting(waiting_data_p, record_p);
            } else { /* originated as CN_REQUEST_GET_CALL_WAITING (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_get_call_waiting;
                status = handle_request_get_call_waiting(waiting_data_p, record_p);
                free(waiting_data_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_QUERY_CLIR: {
            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->data_p = NULL;
                ss_state_info_p->command_function_p = query_clir_ss_status;
                status = query_clir_ss_status(NULL, record_p);
            } else { /* originated as CN_REQUEST_CLIR_STATUS (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_clir_status;
                status = handle_request_clir_status(NULL, record_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_QUERY_CLIP: {
            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->data_p = NULL;
                ss_state_info_p->command_function_p = query_clip_status;
                status = query_clip_status(NULL, record_p);
            } else { /* originated as CN_REQUEST_CLIP_STATUS (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_clip_status;
                status = handle_request_clip_status(NULL, record_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_QUERY_COLR: {
            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->data_p = NULL;
                ss_state_info_p->command_function_p = query_colr_status;
                status = query_colr_status(NULL, record_p);
            } else { /* originated as CN_REQUEST_COLR_STATUS (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_colr_status;
                status = handle_request_colr_status(NULL, record_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        case SS_MAL_QUERY_CNAP: {
            if (CN_REQUEST_SS == record_p->message_type) {
                ss_state_info_p->data_p = NULL;
                ss_state_info_p->command_function_p = query_cnap_status;
                status = query_cnap_status(NULL, record_p);
            } else { /* originated as CN_REQUEST_CNAP_STATUS (before PDC handling) and the client expect a related response */
                record_p->request_handler_p = handle_request_cnap_status;
                status = handle_request_cnap_status(NULL, record_p);
                free(ss_state_info_p);
                ss_state_info_p = NULL;
            }
        }
        break;
        default:
            CN_LOG_E("Unknown case");
            goto error;
            break;
        }

        if (REQUEST_STATUS_ERROR == status) {
            CN_LOG_E("REQUEST_STATUS_ERROR = status\n");
            goto error;
        }

        break;
    } /* end of REQUEST_STATE_REQUEST case */
    default: {
        ss_state_info_p = (cn_ss_mal_info_t *) record_p->ss_request_info_p;

        if (NULL == ss_state_info_p->command_function_p) {
            CN_LOG_E("ss_state_info_p->command_function_p is NULL!");
            goto error;
        }

        if (NULL == ss_state_info_p->result_function_p) {
            CN_LOG_E("ss_state_info_p->result_function_p is NULL!");
            goto error;
        }

        status = ss_state_info_p->command_function_p(ss_state_info_p->data_p, record_p);
        ss_state_info_p->result_function_p(status, ss_state_info_p->data_p, record_p, ss_state_info_p->additional_results);
    }
    } /* end of switch statement */

    goto cleanup;

error_not_allowed:
    status = REQUEST_STATUS_ERROR;
    send_response(record_p->client_id, get_response_message_type_from_request(record_p->message_type), CN_REQUEST_NOT_ALLOWED, record_p->client_tag, 0, NULL);
    goto cleanup;
error_not_supported:
    status = REQUEST_STATUS_ERROR;
    send_response(record_p->client_id, get_response_message_type_from_request(record_p->message_type), CN_REQUEST_NOT_SUPPORTED, record_p->client_tag, 0, NULL);
    goto cleanup;
error:
    status = REQUEST_STATUS_ERROR;
    send_response(record_p->client_id, get_response_message_type_from_request(record_p->message_type), CN_FAILURE, record_p->client_tag, 0, NULL);
    goto cleanup;

cleanup:

    if (REQUEST_STATUS_PENDING != status) {
        if (NULL != ss_state_info_p) {
            free(ss_state_info_p->data_p);
            free(ss_state_info_p);
        }
    }

    free(ss_data_p);

    return status;
#endif /* USE_MAL_CS */
}


request_status_t handle_request_reg_status_event_config(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SET_USER_ACTIVITY_STATUS, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_reg_status_event_config_t *request_data_p = (cn_request_reg_status_event_config_t *) data_p;
        mal_net_reg_status_set_mode reg_status_mode = MAL_NET_MSG_SEND_MODE_REG_STATUS_CHANGE;
        mal_net_ind_type ind_type = MAL_NET_SELECT_MODEM_REG_STATUS_IND;
        mal_net_ind_state ind_state = MAL_NET_IND_DISABLE;

        /* Parameter check */
        if (!(CN_REG_STATUS_TRIGGER_LEVEL_UNDEFINED <= request_data_p->trigger_level &&
                CN_REG_STATUS_TRIGGER_LEVEL_ALL_PARAMS >= request_data_p->trigger_level)) {
            CN_LOG_E("trigger_level out of range! (%d)", request_data_p->trigger_level);
            send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        /* Update table with specified trigger level. If this function is invoked due to a client disconnection *
         * the table is not updated (a table entry has instead been removed). In that case                      *
         * CN_REG_STATUS_TRIGGER_LEVEL_UNDEFINED is specified.                                                  */
        if (CN_REG_STATUS_TRIGGER_LEVEL_UNDEFINED != request_data_p->trigger_level) {
            result = etl_update_trigger_level_table(ETL_TABLE_REG_STATUS, record_p->client_id, request_data_p->trigger_level);

            if (!result) {
                CN_LOG_E("update_trigger_level_table failed!");
                send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
                goto error;
            }
        }

        request_data_p->trigger_level = etl_determine_reg_status_trigger_level();

        /* if no trigger is specified unsubscribe event */
        if (CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM == request_data_p->trigger_level) {
            CN_LOG_D("ind_state: MAL_NET_IND_DISABLE");
            ind_state = MAL_NET_IND_DISABLE;
        } else { /* if trigger is specified, subscribe to event (previous setting is not tracked) */
            CN_LOG_D("ind_state: MAL_NET_IND_ENABLE");
            ind_state = MAL_NET_IND_ENABLE;
        }

        /*** NOTE: Synchronous MAL call ***/
        result = mal_net_set_ind_state(ind_type, ind_state);

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_set_ind_state failed! (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        /* if the event has been unsubscribed unsubscribed, no additional action is needed. Return response to client */
        if (CN_REG_STATUS_TRIGGER_LEVEL_NO_PARAM == request_data_p->trigger_level) {
            send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_SUCCESS, record_p->client_tag, 0, NULL);
            return REQUEST_STATUS_DONE;
        }

        /* if the event has been subscribed, continue specifying trigger level */
        result = util_convert_cn_trigger_level_to_mal(request_data_p->trigger_level, &reg_status_mode);

        if (!result) {
            CN_LOG_E("util_convert_cn_trigger_level_to_mal failed!");
            send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        CN_LOG_D("mal_net_configure_reg_status -> request");

        result = mal_net_configure_reg_status(reg_status_mode, request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_configure_reg_status failed! (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_net_configure_reg_status <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_configure_reg_status failed (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_REG_STATUS_EVENT_CONFIG, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}


request_status_t handle_request_rat_name(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_RAT_NAME, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_net_req_current_rat_name -> request");

        result = mal_net_req_current_rat_name(request_record_get_modem_tag(record_p));

        if (MAL_NET_SUCCESS != result) {
            CN_LOG_E("mal_net_req_current_rat_name failed! (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_RAT_NAME, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;
        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        mal_net_rat_name *mal_rat_name_p = (mal_net_rat_name *)record_p->response_data_p;
        cn_rat_name_t rat_name = CN_RAT_NAME_UNKNOWN;

        CN_LOG_D("mal_net_req_current_rat_name <- response");

        if (MAL_NET_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_net_req_current_rat_name failed! (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_RAT_NAME, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (!record_p->response_data_p) {
            CN_LOG_E("response data for mal_net_req_current_rat_name not found!");
            send_response(record_p->client_id, CN_RESPONSE_RAT_NAME, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        rat_name = *mal_rat_name_p;
        CN_LOG_D("rat name: %d", rat_name);

        send_response(record_p->client_id, CN_RESPONSE_RAT_NAME, CN_SUCCESS, record_p->client_tag, sizeof(rat_name), &rat_name);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_RAT_NAME, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_NET */
}

request_status_t handle_request_modem_power_off(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_MCE
    (void) data_p;
    CN_LOG_I("MAL MCE disabled");
    send_response(record_p->client_id, CN_RESPONSE_MODEM_POWER_OFF, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        CN_LOG_D("mal_mce_modem_switch_off -> request");

        result = mal_mce_modem_switch_off(request_record_get_modem_tag(record_p));

        if (MAL_MCE_SUCCESS != result) {
            CN_LOG_E("mal_mce_modem_switch_off failed! (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_MODEM_POWER_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;
        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_mce_modem_switch_off <- response");

        if (MAL_MCE_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_mce_modem_switch_off failed! (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_MODEM_POWER_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_MODEM_POWER_OFF, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_MODEM_POWER_OFF, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_MCE */
}

request_status_t handle_request_send_tx_back_off_event(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_RF
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
    send_response(record_p->client_id,
                  CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_FAILURE,
                  record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    cn_sint32_t result = 0;

    (void)data_p;
    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        cn_request_send_tx_back_off_event_t *request_data_p = (cn_request_send_tx_back_off_event_t *)data_p;
        mal_rf_tx_back_off_event tx_back_off_event;

        REQUIRE_VALID_REQUEST_DATA(data_p);

        CN_LOG_D("mal_rf_send_tx_back_off_event -> request");

        tx_back_off_event = request_data_p->event;
        result = mal_rf_send_tx_back_off_event(tx_back_off_event, request_record_get_modem_tag(record_p));

        if (MAL_RF_SUCCESS != result) {
            CN_LOG_E("mal_rf_send_tx_back_off_event failed! (result:%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;
        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_rf_send_tx_back_off_event <- response");

        if (MAL_RF_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_rf_send_tx_back_off_event failed! (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        send_response(record_p->client_id, CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_SUCCESS, record_p->client_tag, 0, NULL);

        return REQUEST_STATUS_DONE;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SEND_TX_BACK_OFF_EVENT, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    return status;
#endif /* USE_MAL_RF */
}

#ifdef USE_MAL_FTD
static void empage_update_pageno(int32_t page)
{
    int32_t oldpage;

    oldpage = get_empage_activepage();
    set_empage_activepage((int)page);

    if (oldpage != page) {
        set_empage_seqnr(1);
    }
}

int empage_timer_expired(void *data_p)
{
    (void) data_p;
    cn_sint32_t result = mal_ftd_stop_dump_once_report(get_empage_activepage());

    if (MAL_FTD_SUCCESS != result) {
        CN_LOG_E("error calling mal_ftd_stop_dump_once_report (%d)", result);
        return -1;
    };

    return 0;
}
#endif

request_status_t handle_request_set_empage(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_FTD
    (void) data_p;
    CN_LOG_E("MAL FTD disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_EMPAGE, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t  status = REQUEST_STATUS_ERROR;

    REQUIRE_VALID_RECORD(record_p);

    /*** NOTE: Synchronous MAL call ***/
    cn_sint32_t       result       = 0;
    cn_empage_t *cn_request_data_p = NULL;
    int32_t page;
    int32_t mode;
    int32_t timervalue;

    REQUIRE_VALID_REQUEST_DATA(data_p);

    cn_request_data_p = (cn_empage_t *) data_p;
    mode              = cn_request_data_p->mode;
    page              = cn_request_data_p->page;
    timervalue        = cn_request_data_p->timer;

    switch (mode) {
    case FTD_REQUEST_ONESHOT_MEASUREMENT:
        CN_LOG_D("mal_ftd_activate_request -> synchronous request (oneshot mode)");
#ifndef ENABLE_MODULE_TEST

        /* Start special supervision timer */
        if (cn_timer_start(timervalue * 1000, empage_timer_expired, NULL) < 0) {
            CN_LOG_E("can't start empage supervision timer");
            goto exit;
        }

#endif
        empage_update_pageno(page);
        set_empage_urc_issue(true);
        set_empage_write_tofile(true);
        result = mal_ftd_activate_request(page, (uint8_t)mode);
        break;
    case FTD_REQUEST_START_PERIODIC_MEASUREMENTS:
        CN_LOG_D("mal_ftd_activate_request -> synchronous request (periodic mode)");

        /* Make sure that there is no ongoing activity to deactivate subscribed resources */
        if (get_empage_deactivation_in_progress()) {
            CN_LOG_E("Can't perform request, still awaiting stop request to be finalized.");
            send_response(record_p->client_id, CN_RESPONSE_SET_EMPAGE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto exit;
        }

        empage_update_pageno(page);
        set_empage_urc_issue(false);
        set_empage_write_tofile(true);
        set_empage_activation_in_progress(true);
        result = mal_ftd_activate_request(page, (uint8_t)mode);
        break;
    case FTD_REQUEST_STOP_PERIODIC_MEASUREMENTS:
        CN_LOG_D("mal_ftd_deactivate_request -> synchronous request");

        /* Make sure that there is no ongoing activity to deactivate subscribed resources */
        if (get_empage_activation_in_progress()) {
            CN_LOG_E("Can't perform request, still awaiting start request to be finalized.");
            send_response(record_p->client_id, CN_RESPONSE_SET_EMPAGE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto exit;
        }

        set_empage_write_tofile(false);
        set_empage_deactivation_in_progress(true);
        result = mal_ftd_deactivate_request(get_empage_activepage());
        break;
    case FTD_REQUEST_RESET:
        CN_LOG_D("resetting em");
        set_empage_seqnr(1);
        set_empage_activation_in_progress(false);
        set_empage_deactivation_in_progress(false);
        result = MAL_FTD_SUCCESS;
        break;
    default:
        CN_LOG_E("Erroneous mode, can't perform mal_ftd_request");
        goto error;
    }

    if (MAL_FTD_SUCCESS != result) {
        CN_LOG_E("mal_ftd_request failed (%d) !", result);
        send_response(record_p->client_id, CN_RESPONSE_SET_EMPAGE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }

    send_response(record_p->client_id, CN_RESPONSE_SET_EMPAGE, CN_SUCCESS, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_DONE;

error:
    set_empage_urc_issue(false);
    set_empage_write_tofile(false);
exit:
    return status;
#endif /* USE_MAL_FTD */
}

request_status_t handle_request_set_default_nvmd(void *data_p, request_record_t *record_p)
{
#ifndef USE_MAL_NVD
    (void) data_p;
    CN_LOG_E("MAL NVD disabled");
    send_response(record_p->client_id, CN_RESPONSE_SET_DEFAULT_NVMD, CN_FAILURE, record_p->client_tag, 0, NULL);
    return REQUEST_STATUS_ERROR;
#else
    request_status_t status = REQUEST_STATUS_ERROR;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        mal_nvd_error_type result = MAL_NVD_FAIL;

        if (data_p) {
            /* Notify MSUP that the modem is being reset for legitimate reasons,
             * and that the modem should be restarted, and no core dump is needed.
             */
            if (1 > msup_send_notification(MSUP_FLAG_RESET_IMMINENT |
                                           MSUP_FLAG_FORCE_MSR_AFTER_NEXT_RESET |
                                           MSUP_FLAG_NO_COREDUMP_AFTER_NEXT_RESET)) {
                CN_LOG_E("msup_send_notification failed!");
            }

            /* Indicator to make a post-reset */
            record_p->request_data_p = malloc(1);
        }

        CN_LOG_D("mal_nvd_set_default -> request");

        result = mal_nvd_set_default(request_record_get_modem_tag(record_p));

        if (MAL_NVD_SUCCESS != result) {
            CN_LOG_E("mal_nvd_set_default failed (%d)", result);
            send_response(record_p->client_id, CN_RESPONSE_SET_DEFAULT_NVMD, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_RESPONSE;
        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_RESPONSE: {
        CN_LOG_D("mal_nvd_set_default <- response");

        if (MAL_NVD_SUCCESS != record_p->response_error_code) {
            CN_LOG_E("mal_nvd_set_default failed! (result:%d)", record_p->response_error_code);
            send_response(record_p->client_id, CN_RESPONSE_SET_DEFAULT_NVMD, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        /* Create NVD indicator file in the file system */
        CN_LOG_D("creating NVD indicator file");
        cn_util_create_empty_file_in_fs(NVD_INDICATOR_PATH);

        /* Indicator to make a post-reset */
        if (record_p->request_data_p) {
            free(record_p->request_data_p);
            record_p->request_data_p = NULL;

            record_p->request_handler_p = handle_request_modem_reset;

            /* Invoke request handler */
            status = handle_request_modem_reset(NULL, record_p);

            if (REQUEST_STATUS_PENDING != status) {
                request_record_free(record_p);
            }

        } else {
            send_response(record_p->client_id, CN_RESPONSE_SET_DEFAULT_NVMD, CN_SUCCESS, record_p->client_tag, 0, NULL);
            status = REQUEST_STATUS_DONE;
        }

        return status;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_DEFAULT_NVMD, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:
    free(record_p->request_data_p);
    return status;
#endif /* USE_MAL_NVD */

}

void set_default_non_volatile_modem_data()
{
    request_status_t  status = REQUEST_STATUS_ERROR;
    request_record_t *record_p = NULL;

    /* Only write default non-volatile modem data the first time after flash */
    if (!cn_util_path_present_in_fs(NVD_INDICATOR_PATH)) {
        CN_LOG_D("setting default data");

        /* Create internal request record and bind it to the set default nvmd handler */
        /* Client id -1 indicates internal */
        record_p = request_record_create(CN_REQUEST_SET_DEFAULT_NVMD, 0, -1);

        if (!record_p) {
            CN_LOG_E("request_record_create failed!");
            goto exit;
        }

        record_p->request_handler_p = handle_request_set_default_nvmd;

        /* Invoke request handler with reset */
        status = handle_request_set_default_nvmd((void *)TRUE, record_p);

        if (REQUEST_STATUS_PENDING != status) {
            request_record_free(record_p);
        }
    } else {
        CN_LOG_D("data already defined, skipping setup procedure");
    }

exit:
    return;
}
