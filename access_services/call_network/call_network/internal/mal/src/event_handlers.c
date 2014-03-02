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
#endif /* USE_MAL_CS */

#ifdef USE_MAL_GSS
#include "mal_gss.h"
#endif /* USE_MAL_GSS */

#ifdef USE_MAL_MCE
#include "mal_mce.h"
#endif /* USE_MAL_MCE */

#ifdef USE_MAL_NET
#include "mal_net.h"
#endif /* USE_MAL_NET */

#ifdef USE_MAL_FTD
#include "mal_ftd.h"
#include <time.h>
#endif /* USE_MAL_FTD */

#include "mal_utils.h"

#include "cn_client.h"
#include "cn_log.h"

#include "cnsocket.h"
#include "cnserver.h"

#include "cn_log.h"
#include "cn_macros.h"
#include "event_handling.h"
#include "request_handling.h"
#include "cn_mal_assertions.h" /* do NOT remove this inclusion! */
#include "cn_mal_utils.h"
#include "cn_mal_dial_dtmf_handler.h"
#include "cn_pdc_ecc_list_handling.h"
#include "plmn_main.h"

#ifndef ENABLE_MODULE_TEST
#include "cn_timer.h"
#endif

#define MODEM_REGISTRATION_TIMEOUT_MS   (3000L)

#ifdef USE_MAL_NET
#define RING_TIMER_TIMEOUT_MS           (3000L)

int s_reg_info_timer_active             = 0;
cn_registration_info_t s_reg_info       = {
    .reg_status             = CN_NOT_REG_NOT_SEARCHING,
    .gprs_attached          = 0,
    .cs_attached            = 0,
    .rat                    = CN_RAT_TYPE_UNKNOWN,
    .cid                    = 0,
    .lac                    = 0,
    .search_mode            = CN_NETWORK_SEARCH_MODE_AUTOMATIC,
    .mcc_mnc[0]             = '\0',
    .long_operator_name[0]  = '\0',
    .short_operator_name[0] = '\0',
};

cn_net_detailed_fail_cause_t cn_net_detailed_fail_cause = {CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET,0};

static int modem_registration_status_timer_expired(void *data_p);
#endif

#ifdef USE_MAL_FTD
/* NOTE    : When running module tests (on host) the file is stored in
 *           device/vendor/st-ericsson/access_services/call_network/call_network/internal/test/
 *           ie same location as test.txt is stored that contains the module test log.
 * */
#ifndef HAVE_ANDROID_OS
#define EM_MEASUREMENT_FILE "em_measurements.txt"
#else
#define EM_MEASUREMENT_FILE "/data/misc/em_measurements.txt"
#endif

#define EM_MEASUREMENT_MAX_LENGTH   200

#define EM_URC_ERROR_INDICATOR        0
#define EM_URC_ERROR_EVENT            0

#define EM_URC_ACTIVATE_INDICATOR   101
#define EM_URC_DEACTIVATE_INDICATOR 102
#define EM_URC_RESULT_SUCCESS         0
#define EM_URC_RESULT_FAILURE         1

#ifndef ENABLE_MODULE_TEST
extern int empage_timer_expired(void *data_p);
#endif
#endif /* USE_MAL_FTD */

/*
 * Local function prototypes
 */

void handle_event_radio_status(void *data_p)
{
#ifndef USE_MAL_MCE
    (void) data_p;
    CN_LOG_I("MAL MCE disabled");
#else
    cn_rf_status_t rf_status;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    rf_status = *((mal_mce_rf_status *)data_p);

    result = send_event(CN_EVENT_RADIO_STATUS, CN_SUCCESS, sizeof(cn_rf_status_t), &rf_status);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_RADIO_STATUS!");
        goto exit;
    }

exit:
    return;
#endif /* USE_MAL_MCE */
}


#ifdef USE_MAL_NET
static void modem_registration_status_update(cn_registration_info_t *reg_info_p)
{
    int result;

#ifndef ENABLE_MODULE_TEST

    /* Kill any timer if it is running */
    if (0 != s_reg_info_timer_active) {
        if (cn_timer_stop(modem_registration_status_timer_expired) < 0) {
            CN_LOG_E("can't stop modem registration status timer");
        }

        s_reg_info_timer_active = 0;
    }

#endif

    /* Update emergency number list if country has changed */
    if (cn_pdc_set_current_mcc((char *) reg_info_p->mcc_mnc)) {
        if (!cn_pdc_util_clear_emergency_numbers_by_origin(CN_EMERGENCY_NUMBER_ORIGIN_UNSPECIFIED |
                CN_EMERGENCY_NUMBER_ORIGIN_HARD_CODED |
                CN_EMERGENCY_NUMBER_ORIGIN_SIM |
                CN_EMERGENCY_NUMBER_ORIGIN_NETWORK |
                CN_EMERGENCY_NUMBER_ORIGIN_MEMORY, TRUE)) {
            CN_LOG_E("new mcc, couldn't clear number network list!");
        }
    }

    /* Send the event up the chain */
    result = send_event(CN_EVENT_MODEM_REGISTRATION_STATUS, CN_SUCCESS, sizeof(*reg_info_p), reg_info_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_MODEM_REGISTRATION_STATUS!");
    }

    /* Keep a copy of the latest reported modem registration information */
    memcpy(&s_reg_info, reg_info_p, sizeof(s_reg_info));
}


static int modem_registration_status_timer_expired(void *data_p)
{
    cn_registration_info_t *reg_info_p = (cn_registration_info_t *)data_p;
    int result=0;

    if (NULL == reg_info_p) {
        CN_LOG_E("reg_info_p is NULL");

        return -1;
    }

    modem_registration_status_update(reg_info_p);
    if (cn_net_detailed_fail_cause.cause != 0) {
        result = send_event(CN_EVENT_NET_DETAILED_FAIL_CAUSE, CN_SUCCESS,
                            sizeof(cn_net_detailed_fail_cause), &cn_net_detailed_fail_cause);
        cn_net_detailed_fail_cause.class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET;
        cn_net_detailed_fail_cause.cause = 0;
        if (-1 == result) {
            CN_LOG_E("error sending CN_EVENT_NET_DETAILED_FAIL_CAUSE!");
        }
    }

    return 0;
}
#endif


void handle_event_modem_registration_status(void *data_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
#else
    cn_registration_info_t *reg_info_p = NULL;
    mal_net_registration_info *mal_reg_info_p = NULL;
    const char *long_operator_name_p = NULL;
    const char *short_operator_name_p = NULL;
#ifndef ENABLE_MODULE_TEST
    cn_timer_callback_data_t callback_data = {NULL, 0};
#endif


    REQUIRE_VALID_EVENT_DATA(data_p);

    reg_info_p = calloc(1, sizeof(*reg_info_p));

    if (!reg_info_p) {
        CN_LOG_E("calloc failed for reg_info_p!");
        goto exit;
    }

    mal_reg_info_p = (mal_net_registration_info *)data_p;
    reg_info_p->reg_status = mal_reg_info_p->reg_status;
    reg_info_p->gprs_attached = mal_reg_info_p->gprs_attached;
    reg_info_p->rat = mal_reg_info_p->rat;
    reg_info_p->cid = mal_reg_info_p->cid;
    reg_info_p->lac = mal_reg_info_p->lac;
    reg_info_p->cs_attached = mal_reg_info_p->cs_attached;

    if (mal_reg_info_p->mcc_mnc_string) {
        (void)strncpy(reg_info_p->mcc_mnc, (const char *)mal_reg_info_p->mcc_mnc_string, CN_MAX_STRING_SIZE);

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

    /*
     * Handle temporary loss of CS/PS attach during handover between cells.
     *
     * During handover between cells the modem will attach CS and PS to the
     * new cell in a two step operation. This means that for a short while
     * the modem may report either CS attach or PS attach as false.
     *
     * Handle this situation by delaying reporting the registration status
     * event if it shows a degradation in terms of CS or PS attachment
     * status, until we get a new registration event, or the timer expires.
     */
    if ((s_reg_info.cs_attached == reg_info_p->cs_attached || reg_info_p->cs_attached) &&
            (s_reg_info.gprs_attached == reg_info_p->gprs_attached || reg_info_p->gprs_attached)) {
        /* CS/PS attach is same or better than previously. Report without delay. */
        modem_registration_status_update(reg_info_p);
    } else if (s_reg_info.rat == reg_info_p->rat && s_reg_info.cid == reg_info_p->cid) {
        /* CS/PS attach has degraded, but no cell or radio-technology change. Report without delay. */
        modem_registration_status_update(reg_info_p);
    } else {
        /* CS/PS attach has degraded, and cell or radio-technology has changed. Delay reporting. */
#ifndef ENABLE_MODULE_TEST

        /* Stop any previously running modem registration status timer */
        if (0 != s_reg_info_timer_active) {
            if (cn_timer_stop(modem_registration_status_timer_expired) < 0) {
                CN_LOG_E("can't stop modem registration status timer");
            }

            s_reg_info_timer_active = 0;
        }

        /* Start modem registration status timer */
        callback_data.data_p = reg_info_p;
        callback_data.size_of_data = sizeof(cn_registration_info_t);

        if (cn_timer_start(MODEM_REGISTRATION_TIMEOUT_MS,
                           modem_registration_status_timer_expired, &callback_data) < 0) {
            CN_LOG_E("can't start modem registration status timer");
            modem_registration_status_update(reg_info_p);
            cn_net_detailed_fail_cause.cause = 0;
            cn_net_detailed_fail_cause.class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET;
        } else {
            s_reg_info_timer_active = 1;
        }

#else
    modem_registration_status_update(reg_info_p);
#endif
    }

    free(reg_info_p);

exit:
    return;
#endif /* USE_MAL_NET */
}

void handle_event_time_info(void *data_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
#else
    mal_net_time_info *mal_time_info_p = NULL;
    cn_time_info_t    *time_info_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    time_info_p = calloc(1, sizeof(*time_info_p));

    if (!time_info_p) {
        CN_LOG_E("calloc failed for time_info_p!");
        goto exit;
    }

    mal_time_info_p = (mal_net_time_info *)data_p;

    time_info_p->year                  = mal_time_info_p->year;
    time_info_p->month                 = mal_time_info_p->month;
    time_info_p->day                   = mal_time_info_p->day;
    time_info_p->hour                  = mal_time_info_p->hour;
    time_info_p->minute                = mal_time_info_p->minute;
    time_info_p->second                = mal_time_info_p->second;
    time_info_p->time_zone_sign        = mal_time_info_p->time_zone_sign;
    time_info_p->time_zone_value       = mal_time_info_p->time_zone_value;
    time_info_p->day_light_saving_time = mal_time_info_p->day_light_saving_time;

    result = send_event(CN_EVENT_TIME_INFO, CN_SUCCESS, sizeof(*time_info_p), time_info_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_TIME_INFO!");
    }

    free(time_info_p);

exit:
    return;
#endif /* USE_MAL_NET */
}


void handle_event_name_info(void *data_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
#else
    mal_net_name_info *mal_name_info_p = NULL;
    cn_network_name_info_t *name_info_p = NULL;
    size_t length;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    mal_name_info_p = (mal_net_name_info *)data_p;

    /* Register received NITZ name in PLMN list */
    /* NOTE: string_encoding_t is aligned with dcs. See 3GPP TS 24.008 Network Name information element, table 10.5.94 */
    if (mal_name_info_p->mcc_mnc_string) {
        plmn_add_nitz_name((const char *)mal_name_info_p->mcc_mnc_string,
                           (const char *)mal_name_info_p->full_operator_name.text_p,
                           mal_name_info_p->full_operator_name.length,
                           mal_name_info_p->full_operator_name.spare,
                           (string_encoding_t)mal_name_info_p->full_operator_name.dcs,
                           (const char *)mal_name_info_p->short_operator_name.text_p,
                           mal_name_info_p->short_operator_name.length,
                           mal_name_info_p->short_operator_name.spare,
                           (string_encoding_t)mal_name_info_p->short_operator_name.dcs);
    }

    /* Send event to upper layers */
    name_info_p = calloc(1, sizeof(*name_info_p));

    if (!name_info_p) {
        CN_LOG_E("calloc failed for name_info_p!");
        goto exit;
    }

    if (mal_name_info_p->mcc_mnc_string) {
        strncpy(name_info_p->mcc_mnc, (const char *)mal_name_info_p->mcc_mnc_string, CN_MCC_MNC_STRING_LENGTH);
    }

    if (mal_name_info_p->full_operator_name.text_p && mal_name_info_p->full_operator_name.length > 0) {
        if (mal_name_info_p->full_operator_name.length > CN_MAX_STRING_BUFF) {
            length = CN_MAX_STRING_BUFF;
        } else {
            length = mal_name_info_p->full_operator_name.length;
        }

        memcpy(name_info_p->name[CN_NETWORK_NAME_LONG].text, mal_name_info_p->full_operator_name.text_p, length);
        name_info_p->name[CN_NETWORK_NAME_LONG].length = length;
        name_info_p->name[CN_NETWORK_NAME_LONG].add_ci = (cn_bool_t)mal_name_info_p->full_operator_name.add_ci;
        name_info_p->name[CN_NETWORK_NAME_LONG].dcs = (cn_network_name_dcs_t)mal_name_info_p->full_operator_name.dcs;
        name_info_p->name[CN_NETWORK_NAME_LONG].spare = mal_name_info_p->full_operator_name.spare;
    }

    if (mal_name_info_p->short_operator_name.text_p && mal_name_info_p->short_operator_name.length > 0) {
        if (mal_name_info_p->short_operator_name.length > CN_MAX_STRING_BUFF) {
            length = CN_MAX_STRING_BUFF;
        } else {
            length = mal_name_info_p->short_operator_name.length;
        }

        memcpy(name_info_p->name[CN_NETWORK_NAME_SHORT].text, mal_name_info_p->short_operator_name.text_p, length);
        name_info_p->name[CN_NETWORK_NAME_SHORT].length = length;
        name_info_p->name[CN_NETWORK_NAME_SHORT].add_ci = (cn_bool_t)mal_name_info_p->short_operator_name.add_ci;
        name_info_p->name[CN_NETWORK_NAME_SHORT].dcs = (cn_network_name_dcs_t)mal_name_info_p->short_operator_name.dcs;
        name_info_p->name[CN_NETWORK_NAME_LONG].spare = mal_name_info_p->short_operator_name.spare;
    }

    result = send_event(CN_EVENT_NETWORK_INFO, CN_SUCCESS, sizeof(*name_info_p), name_info_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_NETWORK_INFO!");
    }

    free(name_info_p);

exit:
    return;
#endif /* USE_MAL_NET */
}

#ifdef USE_MAL_CS
static int ring_timer_expired(void *data_p)
{
    int result;
    cn_call_context_t *call_context_p = (cn_call_context_t *)data_p;

    if (NULL == call_context_p) {
        CN_LOG_E("call_context_p is NULL");

        return -1;
    }

    call_context_p->number_of_rings++;
    call_context_p->updated = 0; /* Reset updated flag */

    result = send_event(CN_EVENT_RING, CN_SUCCESS, sizeof(cn_call_context_t), call_context_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_RING!");

        return -1;
    };

    return 0;
}
#endif

void handle_event_ring(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_call_context *mal_call_context_p = NULL;
    cn_call_context_t *call_context_p = NULL;
    int result = 0;
#ifndef ENABLE_MODULE_TEST
    cn_timer_callback_data_t callback_data = {NULL, 0};
#endif


    REQUIRE_VALID_EVENT_DATA(data_p);

    call_context_p = calloc(1, sizeof(*call_context_p));

    if (!call_context_p) {
        CN_LOG_E("calloc failed for call_context_p!");
        goto exit;
    }

    mal_call_context_p = (mal_call_context *)data_p;
    util_call_context(mal_call_context_p, call_context_p);

    result = send_event(CN_EVENT_RING, CN_SUCCESS, sizeof(*call_context_p), call_context_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_RING!");
    }

#ifndef ENABLE_MODULE_TEST

    /* Start RING timer */
    callback_data.data_p = call_context_p;
    callback_data.size_of_data = sizeof(cn_call_context_t);

    if (cn_timer_start(RING_TIMER_TIMEOUT_MS, ring_timer_expired, &callback_data) < 0) {
        CN_LOG_E("can't start ring timer");
    }

#endif

    free(call_context_p);

exit:
    return;
#endif /* USE_MAL_CS */
}


void handle_event_ring_waiting(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_call_context *mal_call_context_p = NULL;
    cn_call_context_t *call_context_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    call_context_p = calloc(1, sizeof(*call_context_p));

    if (!call_context_p) {
        CN_LOG_E("calloc failed for call_context_p!");
        goto exit;
    }

    mal_call_context_p = (mal_call_context *)data_p;
    util_call_context(mal_call_context_p, call_context_p);

    result = send_event(CN_EVENT_RING_WAITING, CN_SUCCESS, sizeof(*call_context_p), call_context_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_RING_WAITING!");
    }

    free(call_context_p);

exit:
    return;
#endif /* USE_MAL_CS */
}


void handle_event_call_state_changed(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_call_context *mal_call_context_p = NULL;
    cn_call_context_t *call_context_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    call_context_p = calloc(1, sizeof(*call_context_p));

    if (!call_context_p) {
        CN_LOG_E("calloc failed for call_context_p!");
        goto exit;
    }

    mal_call_context_p = (mal_call_context *)data_p;
    util_call_context(mal_call_context_p, call_context_p);

    mddh_dtmf_handle_call_state_change(call_context_p->call_id, call_context_p->call_state);

    result = send_event(CN_EVENT_CALL_STATE_CHANGED, CN_SUCCESS,
                        sizeof(*call_context_p), call_context_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_CALL_STATE_CHANGED!");
    }

#ifndef ENABLE_MODULE_TEST

    /* Stop RING timer */
    if (CN_CALL_STATE_INCOMING == call_context_p->prev_call_state &&
            (CN_CALL_STATE_ACTIVE == call_context_p->call_state ||
             CN_CALL_STATE_IDLE == call_context_p->call_state)) {
        if (cn_timer_stop(ring_timer_expired) < 0) {
            CN_LOG_E("ring timer can't be stopped");
        }
    }

#endif

    if (call_context_p) {
        free(call_context_p);
    }

exit:
    return;
#endif /* USE_MAL_CS */
}

void handle_event_modem_emergency_number_indication(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    request_status_t status = REQUEST_STATUS_ERROR;
    request_record_t *record_p = NULL;
    mal_call_emergency_number_list *list_p = NULL;

    REQUIRE_VALID_EVENT_DATA(data_p);

    list_p = (mal_call_emergency_number_list *) data_p;

    record_p = request_record_create(CN_REQUEST_NETWORK_EMERGENCY_NUMBERS_UPDATE, 0, -1);

    if (!record_p) {
        CN_LOG_E("ERROR, not possible to create request record!");
        goto exit;
    }

    record_p->request_handler_p = handle_request_network_emergency_numbers_update;

    status = handle_request_network_emergency_numbers_update(data_p, record_p);

    if (REQUEST_STATUS_PENDING != status) {
        CN_LOG_E("ERROR, handling network emergency numbers update request failed!");
        request_record_free(record_p);
    }

exit:
    return;
#endif
}

void handle_event_net_modem_detailed_fail_cause(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    uint8_t *mal_net_error_p = NULL;
    cn_net_detailed_fail_cause_t *cn_net_detailed_fail_cause_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    mal_net_error_p = (uint8_t *)data_p;

    cn_net_detailed_fail_cause_p = calloc(1, sizeof(*cn_net_detailed_fail_cause_p));

    if (NULL == cn_net_detailed_fail_cause_p) {
        CN_LOG_E("NULL == cn_net_error_cause_p");
        goto exit;
    }

    cn_net_detailed_fail_cause_p->class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_NET;
    cn_net_detailed_fail_cause_p->cause = *mal_net_error_p;

    if (s_reg_info_timer_active == 0) {
        result = send_event(CN_EVENT_NET_DETAILED_FAIL_CAUSE, CN_SUCCESS,
                        sizeof(*cn_net_detailed_fail_cause_p), cn_net_detailed_fail_cause_p);
    }
    else {
        cn_net_detailed_fail_cause.class = cn_net_detailed_fail_cause_p->class;
        cn_net_detailed_fail_cause.cause = cn_net_detailed_fail_cause_p->cause;
    }

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_NET_DETAILED_FAIL_CAUSE!");
    }

    free(cn_net_detailed_fail_cause_p);

exit:
    return;
#endif /* USE_MAL_CS */
}

void handle_event_ss_detailed_fail_cause(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_ss_error_type_value *mal_ss_error_type_value_p = NULL;
    cn_net_detailed_fail_cause_t *cn_net_detailed_fail_cause_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    cn_net_detailed_fail_cause_p = calloc(1, sizeof(*cn_net_detailed_fail_cause_p));

    if (NULL == cn_net_detailed_fail_cause_p) {
        CN_LOG_E("NULL == cn_net_error_cause_p");
        goto exit;
    }

    mal_ss_error_type_value_p = (mal_ss_error_type_value *)data_p;

    switch (mal_ss_error_type_value_p->error_type) {

    case SS_ERROR_TYPE_GSM:

        cn_net_detailed_fail_cause_p->class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS;
        cn_net_detailed_fail_cause_p->cause = mal_ss_error_type_value_p->error_value;

        result = send_event(CN_EVENT_NET_DETAILED_FAIL_CAUSE, CN_SUCCESS,
                            sizeof(*cn_net_detailed_fail_cause_p), cn_net_detailed_fail_cause_p);

        if (-1 == result) {
            CN_LOG_E("error sending CN_EVENT_NET_DETAILED_FAIL_CAUSE!");
        }

        break;

    case SS_ERROR_TYPE_MISC:

        if (MAL_SS_SERVICE_REQUEST_RELEASED == mal_ss_error_type_value_p->error_value) {
            cn_net_detailed_fail_cause_p->class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS;
            cn_net_detailed_fail_cause_p->cause = MAL_SS_SERVICE_REQUEST_RELEASED;
        } else if (MAL_SS_SERVICE_UNKNOWN_ERROR == mal_ss_error_type_value_p->error_value) {
            cn_net_detailed_fail_cause_p->class = CN_NET_DETAILED_FAIL_CAUSE_CLASS_SS;
            cn_net_detailed_fail_cause_p->cause = MAL_SS_SERVICE_UNKNOWN_ERROR;
        } else {
            break; /* Do not propagate any other SS_ERROR_TYPE_MISC errors */
        }

        CN_LOG_D("mal_ss_error_type_value_p error_type=SS_ERROR_TYPE_MISC, propagated as SS_ERROR_TYPE_GSM");
        result = send_event(CN_EVENT_NET_DETAILED_FAIL_CAUSE, CN_SUCCESS,
                            sizeof(*cn_net_detailed_fail_cause_p), cn_net_detailed_fail_cause_p);

        if (-1 == result) {
            CN_LOG_E("error sending CN_EVENT_NET_DETAILED_FAIL_CAUSE!");
        }

        break;

    case SS_ERROR_NONE:
    case SS_ERROR_TYPE_GSM_PASSWORD:
    case SS_ERROR_TYPE_GSM_MSG:
    case SS_ERROR_TYPE_MM:
        /* Ignored error, do not propagate upwards
         */
        break;
    default:
        CN_LOG_E("Invalid mal_ss_error_type_value_p error_type=%d", mal_ss_error_type_value_p->error_type);

    }

    free(cn_net_detailed_fail_cause_p);

exit:
    return;
#endif /* USE_MAL_CS */
}

void handle_event_ss_status_ind(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_ss_ussd_status_ind_info *status_p = NULL;
    cn_ss_status_info_t *ss_status_ind_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    ss_status_ind_p = calloc(1, sizeof(*ss_status_ind_p));

    if (NULL == ss_status_ind_p) {
        CN_LOG_E("NULL == ss_status_ind_p");
        goto exit;
    }

    status_p = (mal_ss_ussd_status_ind_info *)data_p;

    switch (status_p->ussd_status_ind_type) {
    case MAL_SS_STATUS_REQUEST_USSD_BUSY:
        ss_status_ind_p->type = CN_SS_STATUS_INFO_TYPE_USSD_BUSY;
        break;
    case MAL_SS_STATUS_REQUEST_USSD_STOP:
        ss_status_ind_p->type = CN_SS_STATUS_INFO_TYPE_USSD_STOP;
        break;
    case MAL_SS_STATUS_REQUEST_USSD_FAILED:
        ss_status_ind_p->type = CN_SS_STATUS_INFO_TYPE_USSD_FAILED;
        break;
    default:
        CN_LOG_E("Unknown status indication");
        goto exit;
        break;
    }

    util_convert_ss_error_type(0, status_p->error_info, &ss_status_ind_p->ss_error);

    ss_status_ind_p->session_id = (uint32_t) status_p->ussd_session_id;

    CN_LOG_D("handle_event_ss_status_ind -> rsp_p.session_id %d", ss_status_ind_p->session_id);

    result = send_event(CN_EVENT_SS_STATUS_INFO, CN_SUCCESS,
                        sizeof(*ss_status_ind_p), ss_status_ind_p);

exit:
    free(ss_status_ind_p);

    return;
#endif
}

void handle_event_call_supp_svc_notification(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_supp_svc_notification *mal_supp_svc_notification_p = NULL;
    cn_supp_svc_notification_t *cn_supp_svc_notification_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    cn_supp_svc_notification_p = calloc(1, sizeof(*cn_supp_svc_notification_p));

    if (!cn_supp_svc_notification_p) {
        CN_LOG_E("calloc failed for cn_supp_svc_notification_p!");
        goto exit;
    }

    mal_supp_svc_notification_p = (mal_supp_svc_notification *)data_p;

    cn_supp_svc_notification_p->notification_type = mal_supp_svc_notification_p->notificationType;
    cn_supp_svc_notification_p->code = mal_supp_svc_notification_p->code;
    cn_supp_svc_notification_p->index = mal_supp_svc_notification_p->index;
    cn_supp_svc_notification_p->address_type = mal_supp_svc_notification_p->type;

    if (mal_supp_svc_notification_p->number) {
        (void)strncpy(cn_supp_svc_notification_p->number,
                      (const char *)mal_supp_svc_notification_p->number, CN_MAX_STRING_SIZE);
    }

    result = send_event(CN_EVENT_CALL_SUPP_SVC_NOTIFICATION, CN_SUCCESS,
                        sizeof(*cn_supp_svc_notification_p), cn_supp_svc_notification_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_CALL_SUPP_SVC_NOTIFICATION!");
    }

    free(cn_supp_svc_notification_p);

exit:
    return;
#endif /* USE_MAL_CS */
}


void handle_event_call_cnap(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_call_context *mal_call_context_p = NULL;
    cn_call_context_t *cn_call_context_p = NULL;
    int result = 0;
#ifndef ENABLE_MODULE_TEST
    cn_timer_callback_data_t callback_data = {NULL, 0};
#endif

    REQUIRE_VALID_EVENT_DATA(data_p);

    cn_call_context_p = calloc(1, sizeof(*cn_call_context_p));

    if (!cn_call_context_p) {
        CN_LOG_E("calloc failed for cn_call_context_p!");
        goto exit;
    }

    mal_call_context_p = (mal_call_context *)data_p;
    util_call_context(mal_call_context_p, cn_call_context_p);

    cn_call_context_p->number_of_rings = 1;
    cn_call_context_p->updated = 1; /* Call-context is updated */

    result = send_event(CN_EVENT_RING, CN_SUCCESS,
                        sizeof(*cn_call_context_p), cn_call_context_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_RING!");
    }

#ifndef ENABLE_MODULE_TEST

    /* Stop and then restart the RING timer */
    if (cn_timer_stop(ring_timer_expired) < 0) {
        CN_LOG_E("ring timer can't be stopped");
    } else {
        callback_data.data_p = cn_call_context_p;
        callback_data.size_of_data = sizeof(cn_call_context_t);

        if (cn_timer_start(RING_TIMER_TIMEOUT_MS, ring_timer_expired, &callback_data) < 0) {
            CN_LOG_E("can't start ring timer");
        }
    }

#endif

    free(cn_call_context_p);

exit:
    return;
#endif /* USE_MAL_CS */
}


void handle_event_ussd(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    cn_ussd_info_t *ussd_info_p = NULL;
    mal_ss_ussd_info *mal_ussd_data_p = NULL;
    int result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    mal_ussd_data_p = (mal_ss_ussd_info *) data_p;

    ussd_info_p = calloc(1, sizeof(*ussd_info_p));

    if (!ussd_info_p) {
        CN_LOG_E("calloc failed for ussd_info_p!");
        goto exit;
    }

    ussd_info_p->type = mal_ussd_data_p->ussd_type_code;

    switch (mal_ussd_data_p->ussd_received_type) {
    case MAL_SS_GSM_USSD_REQUEST:
        ussd_info_p->received_type = CN_USSD_RECEIVED_TYPE_GSM_USSD_REQUEST;
        break;
    case MAL_SS_GSM_USSD_NOTIFY:
        ussd_info_p->received_type = CN_USSD_RECEIVED_TYPE_GSM_USSD_NOTIFY;
        break;
    case MAL_SS_GSM_USSD_COMMAND:
        ussd_info_p->received_type = CN_USSD_RECEIVED_TYPE_GSM_USSD_COMMAND;
        break;
    case MAL_SS_GSM_USSD_END:
        ussd_info_p->received_type = CN_USSD_RECEIVED_TYPE_GSM_USSD_END;
        break;
    default:
        CN_LOG_E("Received invalid ussd received type (0x%x)!", mal_ussd_data_p->ussd_received_type);
        goto exit;
    }

    ussd_info_p->length = mal_ussd_data_p->ussd_data.length;
    ussd_info_p->dcs = mal_ussd_data_p->ussd_data.dcs;
    ussd_info_p->session_id = (uint32_t) mal_ussd_data_p->ussd_session_id;
    ussd_info_p->me_initiated = mal_ussd_data_p->is_ussd_ue_terminated; /* 1 == ME initiated, 0 == Network initiated */
    ussd_info_p->suppress_ussd_on_end = mal_ussd_data_p->suppress_ussd_on_end; /* Valid when received_type is CN_USSD_RECEIVED_TYPE_GSM_USSD_END for SAT initiated USSD */

    if (mal_ussd_data_p->ussd_data.length > 0) {
        memmove(ussd_info_p->ussd_string, (const char *)mal_ussd_data_p->ussd_data.ussd_str, CN_MAX_STRING_SIZE);
    }

    CN_LOG_D("handle_event_ussd -> rsp_p.session_id %d", ussd_info_p->session_id);

    result = send_event(CN_EVENT_USSD, CN_SUCCESS, sizeof(*ussd_info_p), ussd_info_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_USSD!");
    }

exit:
    free(ussd_info_p);

    return;
#endif /* USE_MAL_CS */
}


void handle_event_neighbour_cells_info(void *data_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
#else
    mal_net_neighbour_cells_info *mal_neighbour_cells_info_p = NULL;
    cn_neighbour_cells_info_t    *neighbour_cells_info_p = NULL;
    cn_uint32_t size = 0;
    cn_sint32_t result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    mal_neighbour_cells_info_p = (mal_net_neighbour_cells_info *)data_p;

    neighbour_cells_info_p = calloc(1, sizeof(cn_neighbour_cells_info_t));

    if (!neighbour_cells_info_p) {
        CN_LOG_E("calloc failed for neighbour_cells_info_p!");
        goto exit;
    }

    result = util_copy_neighbour_cells_info_data(neighbour_cells_info_p, mal_neighbour_cells_info_p, &size);

    if (-1 == result) {
        CN_LOG_E("util_copy_neighbour_cells_info_data failed!");
        free(neighbour_cells_info_p);
        goto exit;
    }

    result = send_event(CN_EVENT_NEIGHBOUR_CELLS_INFO, CN_SUCCESS, size, neighbour_cells_info_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_NEIGHBOUR_CELLS_INFO!");
    }

    free(neighbour_cells_info_p);

exit:
    return;
#endif /* USE_MAL_NET */
}


void handle_event_signal_info(void *data_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
#else
    cn_signal_info_t *signal_info_p = NULL;
    mal_net_signal_info *mal_signal_info_p = NULL;
    cn_sint32_t result = 0;
    int i = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    signal_info_p = calloc(1, sizeof(*signal_info_p));

    if (!signal_info_p) {
        CN_LOG_E("calloc failed for signal_info_p!");
        goto exit;
    }

    mal_signal_info_p = (mal_net_signal_info *)data_p;

    signal_info_p->rssi_dbm              = mal_signal_info_p->rssi_dbm;
    signal_info_p->no_of_signal_bars     = mal_signal_info_p->no_of_signal_bars;
    signal_info_p->ber                   = mal_signal_info_p->ber;
    signal_info_p->num_of_segments       = mal_signal_info_p->num_of_segments;
    signal_info_p->ecno                  = mal_signal_info_p->ecno;
    signal_info_p->rat                   = mal_signal_info_p->rat;

    for (i = 0; i < CN_RSSI_CONF_MAX_SEGMENTS; i++) {
        signal_info_p->border[i]         = mal_signal_info_p->border[i];
    }

    result = send_event(CN_EVENT_SIGNAL_INFO, CN_SUCCESS, sizeof(*signal_info_p), signal_info_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_TIME_INFO!");
    }

    free(signal_info_p);

exit:
    return;
#endif /* USE_MAL_NET */
}


void handle_event_cell_info(void *data_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
#else
    cn_cell_info_t *cell_info_p = NULL;
    mal_net_cell_info *mal_cell_info_p = NULL;
    cn_sint32_t result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    cell_info_p = calloc(1, sizeof(*cell_info_p));

    if (!cell_info_p) {
        CN_LOG_E("calloc failed for cell_info_p!");
        goto exit;
    }

    mal_cell_info_p = (mal_net_cell_info *)data_p;

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

    result = send_event(CN_EVENT_CELL_INFO, CN_SUCCESS, sizeof(*cell_info_p), cell_info_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_CELL_INFO");
    }

    free(cell_info_p);

exit:
    return;
#endif /* USE_MAL_NET */
}


void handle_event_generate_local_comfort_tones(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    mal_call_alert_tone mal_tone_generation = 0;
    cn_comfort_tone_generation_t tone_generation = 0;
    cn_sint32_t result = 0;


    REQUIRE_VALID_EVENT_DATA(data_p);

    mal_tone_generation = *((mal_call_alert_tone *)data_p);
    tone_generation = (cn_comfort_tone_generation_t) mal_tone_generation;

    result = send_event(CN_EVENT_GENERATE_LOCAL_COMFORT_TONES, CN_SUCCESS, sizeof(tone_generation), &tone_generation);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_GENERATE_LOCAL_COMFORT_TONES!");
    }

exit:
    return;
#endif
}

void handle_event_radio_info(void *data_p)
{
#ifndef USE_MAL_CS
    (void) data_p;
    CN_LOG_I("MAL CS disabled");
#else
    cn_sint32_t result = 0;
    cn_rab_status_t rab_status = 0;
    mal_net_rab_status_type mal_rab_status = 0;

    REQUIRE_VALID_EVENT_DATA(data_p);

    mal_rab_status = *((mal_net_rab_status_type *)data_p);
    rab_status = (cn_rab_status_t) mal_rab_status;

    result = send_event(CN_EVENT_RAB_STATUS, CN_SUCCESS, sizeof(rab_status), &rab_status);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_RAB_STATUS!");
    }

exit:
    return;
#endif
}


void handle_event_rat_name(void *data_p)
{
#ifndef USE_MAL_NET
    (void) data_p;
    CN_LOG_I("MAL NET disabled");
#else
    mal_net_rat_name *mal_rat_name_p = (mal_net_rat_name *)data_p;
    cn_rat_name_t rat_name = CN_RAT_NAME_UNKNOWN;
    cn_sint32_t result = 0;

    REQUIRE_VALID_EVENT_DATA(data_p);

    rat_name = *mal_rat_name_p;
    CN_LOG_D("rat name: %d", rat_name);

    result = send_event(CN_EVENT_RAT_NAME, CN_SUCCESS, sizeof(rat_name), &rat_name);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_RAT_NAME!");
    }

exit:
    return;
#endif /* USE_MAL_NET */
}

#ifdef USE_MAL_FTD
static void issue_empage_urc(int page, int seqnr)
{
    cn_empage_info_t *empage_p = NULL;
    int result;

    empage_p = calloc(1, sizeof(*empage_p));

    if (!empage_p) {
        CN_LOG_E("calloc failed for empage_p !");
        goto exit;
    }

    empage_p->page  = (int32_t)page;
    empage_p->seqnr = (int32_t)seqnr;

    result = send_event(CN_EVENT_EMPAGE_INFO, CN_SUCCESS, sizeof(*empage_p), empage_p);

    if (-1 == result) {
        CN_LOG_E("error sending CN_EVENT_EMPAGE !");
    }

    free(empage_p);

exit:
    return;
}
#endif /* USE_MAL_FTD */


#ifdef USE_MAL_FTD
static bool write_empage_measurement_to_file(int page, int seqnr, int no_of_params, view_details_t *view_details_p)
{
    FILE   *fp;
    size_t  count;
    char   *str_p = NULL;
    int     i;
    int     length;
    bool    rc = false;

    fp = fopen(EM_MEASUREMENT_FILE, "w+");

    if (fp == NULL) {
        CN_LOG_E("error storing measurement for page %d, seqnr %d , cant open file !", page, seqnr);
        return false;
    }

    str_p = calloc(1, EM_MEASUREMENT_MAX_LENGTH);

    if (!str_p) {
        CN_LOG_E("calloc failed for str_p");
        goto close_and_exit;
    }

    /* Set a lock for the file */
    flockfile(fp);

    /* Write header */
    sprintf(str_p, "%d,%d,%d\n", page, seqnr, no_of_params);
    length = strlen(str_p);

    count = fwrite(str_p, 1, length, fp);

    if (count != (size_t)length) {
        CN_LOG_E("error writing measurement header for page %d, seqnr %d !", page, seqnr);
        goto unlock_close_and_exit;
    }

#ifndef HAVE_ANDROID_OS
    printf("\n\n[*] --- FT MEASUREMENT number %.05d for page %.02d (%.02d parameters) --- [*]\n", seqnr, page, no_of_params);
#endif

    /* Write each parameter measurement (name/value) */
    for (i = 0; i < no_of_params; i++) {
        memset(str_p, 0, EM_MEASUREMENT_MAX_LENGTH);

        if (strncmp(view_details_p->table_to_print[i].str_value , FTD_MEASUREMENT_NOT_SUPPORTED, strlen(FTD_MEASUREMENT_NOT_SUPPORTED)) == 0) {
            sprintf(str_p, "\"%s\",1,\"%s\"\n", view_details_p->table_to_print[i].name_p, "");
#ifndef HAVE_ANDROID_OS
            printf("%.02d : \"%s\",1,\"%s\"\n", i + 1, view_details_p->table_to_print[i].name_p, "");
#endif
        } else if (strncmp(view_details_p->table_to_print[i].str_value , FTD_MEASUREMENT_NOT_AVAILABLE, strlen(FTD_MEASUREMENT_NOT_AVAILABLE)) == 0) {
            sprintf(str_p, "\"%s\",2,\"%s\"\n", view_details_p->table_to_print[i].name_p, "");
#ifndef HAVE_ANDROID_OS
            printf("%.02d : \"%s\",2,\"%s\"\n", i + 1, view_details_p->table_to_print[i].name_p, "");
#endif
        } else if (strncmp(view_details_p->table_to_print[i].str_value , FTD_MEASUREMENT_REQ_FAILED, strlen(FTD_MEASUREMENT_REQ_FAILED)) == 0) {
            sprintf(str_p, "\"%s\",3,\"%s\"\n", view_details_p->table_to_print[i].name_p, "");
#ifndef HAVE_ANDROID_OS
            printf("%.02d : \"%s\",3,\"%s\"\n", i + 1, view_details_p->table_to_print[i].name_p, "");
#endif
        } else if (strncmp(view_details_p->table_to_print[i].str_value , FTD_MEASUREMENT_REQ_TIMEOUT, strlen(FTD_MEASUREMENT_REQ_TIMEOUT)) == 0) {
            sprintf(str_p, "\"%s\",4,\"%s\"\n", view_details_p->table_to_print[i].name_p, "");
#ifndef HAVE_ANDROID_OS
            printf("%.02d : \"%s\",4,\"%s\"\n", i + 1, view_details_p->table_to_print[i].name_p, "");
#endif
        } else {
            sprintf(str_p, "\"%s\",0,\"%s\"\n", view_details_p->table_to_print[i].name_p, view_details_p->table_to_print[i].str_value);
#ifndef HAVE_ANDROID_OS
            printf("%.02d : \"%s\",0,\"%s\"\n", i + 1, view_details_p->table_to_print[i].name_p, view_details_p->table_to_print[i].str_value);
#endif
        }

        length = strlen(str_p);

        count = fwrite(str_p, 1, length, fp);

        if (count != (size_t)length) {
            CN_LOG_E("error writing measurement entry for parameter %s (status %d , value %s)  !",
                     view_details_p->table_to_print[i].name_p, 0, view_details_p->table_to_print[i].str_value);
            goto unlock_close_and_exit;
        }
    }

    rc = true;

unlock_close_and_exit:
#ifndef HAVE_ANDROID_OS
    printf("[*] --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- [*]\n\n");
#endif

    funlockfile(fp);
close_and_exit:
    (void)fclose(fp);
    free(str_p);

    return rc;

}
#endif /* USE_MAL_FTD */

void handle_event_empage_measurement(void *data_p)
{
#ifndef USE_MAL_FTD
    (void) data_p;
    CN_LOG_E("MAL FTD disabled");
#else
    view_details_t *view_details_p = NULL;
    int page, seqnr, no_of_params;

    if (get_empage_activation_in_progress()) {
        /* Issue special URC to indicate success */
        issue_empage_urc(EM_URC_ACTIVATE_INDICATOR, EM_URC_RESULT_SUCCESS);
        set_empage_activation_in_progress(false);
    }

    REQUIRE_VALID_EVENT_DATA(data_p);

    if (!get_empage_write_tofile()) {
        goto exit;
    }

    view_details_p = (view_details_t *)data_p;
    page           = get_empage_activepage();
    no_of_params   = view_details_p->size_of_table;
    seqnr          = get_empage_seqnr();

    if (write_empage_measurement_to_file(page, seqnr, no_of_params, view_details_p)) {
        if (get_empage_urc_issue()) {
            /* Special treatment for one-shot mode */
#ifndef ENABLE_MODULE_TEST
            /* Shutdown timer supervision and reinitialize states */
            if (cn_timer_stop(empage_timer_expired) < 0) {
                CN_LOG_E("empage supervision timer can't be stopped");
            }

#endif
            issue_empage_urc(page, seqnr);
            set_empage_urc_issue(false);
            set_empage_write_tofile(false);
        }

        set_empage_seqnr(seqnr + 1);
    }

exit:
    return;
#endif /* USE_MAL_FTD */
}

void handle_event_empage_error(void *data_p)
{
#ifndef USE_MAL_FTD
    (void) data_p;
    CN_LOG_E("MAL FTD disabled");
#else
    mal_ftd_error_type error_code = (mal_ftd_error_type)data_p;

    if (MAL_FTD_FAIL == error_code) {
        /* Issue a special URC to indicate error */
        issue_empage_urc(EM_URC_ERROR_INDICATOR, EM_URC_ERROR_EVENT);
    }

    return;
#endif /* USE_MAL_FTD */
}

void handle_event_empage_activation(void *data_p)
{
#ifndef USE_MAL_FTD
    (void) data_p;
    CN_LOG_E("MAL FTD disabled");
#else
    mal_ftd_error_type error_code = (mal_ftd_error_type)data_p;

    /*
     * NOTE : After a activation request (periodic mode) there is either of
     * SUCCESS : MAL_FTD_MEASUREMENT_IND
     * FAILURE : MAL_FTD_ACT_IND with an error code (MAL_FTD_FAIL)
     */
    if (MAL_FTD_FAIL == error_code) {
        /* Issue a special URC to indicate error */
        issue_empage_urc(EM_URC_ACTIVATE_INDICATOR, EM_URC_RESULT_FAILURE);
    } else {
        CN_LOG_E("Unexpected error code (%d).", error_code);
    }

    set_empage_activation_in_progress(false);

    return;
#endif /* USE_MAL_FTD */
}

void handle_event_empage_deactivation(void *data_p)
{
#ifndef USE_MAL_FTD
    (void) data_p;
    CN_LOG_E("MAL FTD disabled");
#else
    mal_ftd_error_type error_code = (mal_ftd_error_type)data_p;

    /*
     * NOTE : After a deactivation request (periodic mode) there is either of
     * SUCCESS : MAL_FTD_ACT_IND with an error code (MAL_FTD_SUCCESS)
     * FAILURE : MAL_FTD_ACT_IND with an error code (MAL_FTD_FAIL)
     */
    if (MAL_FTD_SUCCESS == error_code) {
        /* Issue special URC to indicate success */
        issue_empage_urc(EM_URC_DEACTIVATE_INDICATOR, EM_URC_RESULT_SUCCESS);
    } else {
        /* Issue a special URC to indicate error */
        issue_empage_urc(EM_URC_DEACTIVATE_INDICATOR, EM_URC_RESULT_FAILURE);
    }

    set_empage_deactivation_in_progress(false);

    return;
#endif /* USE_MAL_FTD */
}
