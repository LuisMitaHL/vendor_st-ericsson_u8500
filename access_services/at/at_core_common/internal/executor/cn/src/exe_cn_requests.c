/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "atc_log.h"
#include "predial.h"

#include "exe.h"
#include "exe_cn_requests.h"
#include "exe_cn_utils.h"
#include "exe_internal.h"
#include "exe_request_record.h"
#include "exe_request_record_p.h"
#include "exe_requests.h"

#include "cn_client.h"


static exe_request_result_t request_reg_status_event_config(exe_request_record_t *record_p);
static exe_request_result_t util_request_neighbour_cells_info_read(exe_request_record_t *record_p, cn_neighbour_rat_type_t rat_type);
static exe_request_result_t util_request_serving_cells_info_read(exe_request_record_t *record_p, cn_neighbour_rat_type_t rat_type);

/* Add client specific requester states here. */
#define EXE_STATE_MCE_RF_ON_REQUEST                 10
#define EXE_STATE_GSS_NETWORK_AUTO_REQUEST          11
#define EXE_STATE_MCE_STATUS_REQUEST                12
#define EXE_STATE_GSS_NETWORK_TYPE_REQUEST          13
#define EXE_STATE_CALL_HOLD_ANSWER_WAITING          15
#define EXE_STATE_CALL_LIST_REQUEST                 19
#define EXE_STATE_CALL_LIST_RESPONSE                20
#define EXE_STATE_NETWORK_SEARCH                    21
#define EXE_STATE_NETWORK_SEARCH_ABORT              22
#define EXE_STATE_NETWORK_SEARCH_TERMINATE          23
#define EXE_STATE_USSD_ABORT_RESPONSE               24
#define EXE_STATE_NET_QUERY                         25

#define EXE_FLAG_ID_IMEISV 5602    /* PP flag id for the IMEISV value */


/********************************************************************
 * Misc
 ********************************************************************
 */

#define GSM_BAND_INFO_NOT_AVAILABLE 2

typedef enum {
    CALL_WAITING_MODE_DISABLED     = 0,
    CALL_WAITING_MODE_ENABLED      = 1,
    CALL_WAITING_MODE_QUERY_STATUS = 2,
    CALL_WAITING_MODE_UNDEFINED    = 255
} call_waiting_mode;

/* Track state of dtmf_start / dtmf_stop */
static int state_dtmf_start         = 0;

static exe_request_result_t util_request_neighbour_cells_info_read(exe_request_record_t *record_p, cn_neighbour_rat_type_t rat_type)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I(" -> request");
        switch(rat_type){
            case CN_NEIGHBOUR_RAT_TYPE_2G:
                result = cn_request_get_neighbour_cells_complete_info(cn_client_get_context(), rat_type, request_record_get_client_tag(record_p));
                break;
            case CN_NEIGHBOUR_RAT_TYPE_3G:
                result = cn_request_get_neighbour_cells_extd_info(cn_client_get_context(), rat_type, request_record_get_client_tag(record_p));
                break;
            default:
                ATC_LOG_E("unknown RAT!");
                goto error;
        }

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_get_neighbour_cells_info failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        exe_2g_3g_neigh_cell_response_t response;
        cn_neighbour_cells_info_t *neighbour_cells_info_p = NULL;
        int i = 0;
        int res = 0;

        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_get_neighbour_cells_info failed!");
            goto error_cb;
        }

        if (!record_p->response_data_p) {
            ATC_LOG_E("record_p->response_data_p is NULL!");
            goto error_cb;
        }

        memset(&response, 0, sizeof(exe_2g_3g_neigh_cell_response_t));
        neighbour_cells_info_p = (cn_neighbour_cells_info_t *)record_p->response_data_p;

        res = convert_cn_neighbouring_cell_info_to_exe_info(&response, neighbour_cells_info_p);

        if (-1 == res) {
            ATC_LOG_E("util_convert_cn_neighbour_cells_info_to_exe_info failed!");
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &response);
        free(response.neigh_cell_info_p);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}


static exe_request_result_t util_request_serving_cells_info_read(exe_request_record_t *record_p, cn_neighbour_rat_type_t rat_type)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I(" -> request");

        result = cn_request_get_neighbour_cells_extd_info(cn_client_get_context(), rat_type, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        exe_2g_3g_neigh_cell_response_t response;
        cn_neighbour_cells_info_t *neighbour_ecid_info_p = NULL;
        int res = 0;

        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_get_neighbour_cells_extd_info failed!");
            goto error_cb;
        }

        if (!record_p->response_data_p) {
            ATC_LOG_E("record_p->response_data_p is NULL!");
            goto error_cb;
        }

        memset(&response, 0, sizeof(exe_2g_3g_neigh_cell_response_t));
        neighbour_ecid_info_p = (cn_neighbour_cells_info_t *)record_p->response_data_p;

        res = convert_cn_neighbouring_cell_info_to_exe_info(&response, neighbour_ecid_info_p);

        if (-1 == res) {
            ATC_LOG_E("util_convert_cn_neighbour_ecid_info_to_serving_cell_exe_info failed!");
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &response);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_neighbour_2g_cellinfo(exe_request_record_t *record_p)
{
    return util_request_neighbour_cells_info_read(record_p, CN_NEIGHBOUR_RAT_TYPE_2G);
}

exe_request_result_t request_neighbour_3g_cellinfo(exe_request_record_t *record_p)
{
    return util_request_neighbour_cells_info_read(record_p, CN_NEIGHBOUR_RAT_TYPE_3G);
}

exe_request_result_t request_serving_2g_cellinfo(exe_request_record_t *record_p)
{
    return util_request_serving_cells_info_read(record_p, CN_NEIGHBOUR_RAT_TYPE_2G);
}

exe_request_result_t request_serving_3g_cellinfo(exe_request_record_t *record_p)
{
    return util_request_serving_cells_info_read(record_p, CN_NEIGHBOUR_RAT_TYPE_3G);
}

/**
 * request_radio_on
 *
 * Turns on radio.
 * Preferred network is set to modem default or last set value.
 */
exe_request_result_t request_radio_on(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_rf_on(cn_client_get_context() , request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: request_radio_on request failed, result=%d!", __FUNCTION__, result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


/**
 * request_radio_on
 *
 * Turns off radio.
 */
exe_request_result_t request_radio_off(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_rf_off(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

/**
 * request_preferred_network
 *
 * Sets the preferred network.
 */
exe_request_result_t request_preferred_network(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        exe_cfun_request_t *request_p = NULL;

        EXE_CHECK_GOTO_ERROR(NULL != record_p->request_data_p);
        request_p = (exe_cfun_request_t *)record_p->request_data_p;

        result = cn_request_set_preferred_network_type(cn_client_get_context(), request_p->network, request_record_get_client_tag(record_p));

        if (CN_REQUEST_NOT_SUPPORTED  == result) {
            return EXE_NOT_SUPPORTED;
        }

        EXE_CHECK_GOTO_ERROR(CN_SUCCESS == result);

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


/**
 * request_radio_status
 *
 * Query radio status. Used by AT+CFUN?.
 */
exe_request_result_t request_radio_status(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_rf_status(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            return EXE_FAILURE;
        }

        record_p->state = EXE_STATE_MCE_STATUS_REQUEST;
        return EXE_PENDING;
    } else if (EXE_STATE_MCE_STATUS_REQUEST == record_p->state) {
        cn_rf_status_t rf_status;
        exe_cfun_response_t response;
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("%s: no response data!", __FUNCTION__);
            exe_request_complete(record_p, EXE_FAILURE, &response);
            return EXE_FAILURE;
        }

        rf_status = *((cn_rf_status_t *)record_p->response_data_p);

        switch (rf_status) {
        case CN_RF_OFF:
            response.value = 4;
            exe_request_complete(record_p, EXE_SUCCESS, &response);
            return EXE_SUCCESS;

        case CN_RF_ON:
            result = cn_request_get_preferred_network_type(cn_client_get_context(), request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                exe_request_complete(record_p, EXE_FAILURE, &response);
                return EXE_FAILURE;
            }

            record_p->state = EXE_STATE_GSS_NETWORK_TYPE_REQUEST;
            return EXE_PENDING;

        case CN_RF_TRANSITION:
            exe_request_complete(record_p, EXE_FAILURE, &response);
            return EXE_FAILURE;

        default:
            ATC_LOG_E("%s: invalid rf_status=%d!", __FUNCTION__, rf_status);
            exe_request_complete(record_p, EXE_FAILURE, &response);
            return EXE_FAILURE;
        }
    } else if (EXE_STATE_GSS_NETWORK_TYPE_REQUEST == record_p->state) {
        cn_network_type_t network_type;
        exe_cfun_response_t response;
        ATC_LOG_I("request_radio_status <- response preferred network");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL == record_p->response_data_p) {
            exe_request_complete(record_p, EXE_FAILURE, &response);
            return EXE_FAILURE;
        }

        network_type = *((cn_network_type_t *)record_p->response_data_p);

        switch (network_type) {
        case CN_NETWORK_TYPE_GSM_WCDMA_PREFERRED_WCDMA:
            response.value = 7;
            break;
        case CN_NETWORK_TYPE_GSM_ONLY:
            response.value = 5;
            break;
        case CN_NETWORK_TYPE_WCDMA_ONLY:
            response.value = 6;
            break;
        default:
            response.value = -1;
            break;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_creg_read(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("-> request");
        result = cn_request_registration_state_normal(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("CN_SUCCESS != result");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        cn_registration_info_t *reg_info_p;
        exe_creg_reg_info_t reg_info = { EXE_NET_REG_STAT_UNKNOWN, 0, 0, EXE_BASIC_ACT_UNKNOWN, 0};
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("response data NULL");
            goto error;
        }

        reg_info_p = (cn_registration_info_t *)record_p->response_data_p;

        if (!map_registration_state_from_cn_to_at(reg_info_p->reg_status,
                &reg_info.stat)) {
            ATC_LOG_E("%s: registration mapping failed!", __FUNCTION__);
            goto error;
        }

        reg_info.lac = reg_info_p->lac;
        reg_info.cid = reg_info_p->cid;
        reg_info.cs_attached = reg_info_p->cs_attached;

        if (EXE_NET_REG_STAT_REGISTERED_HOME == reg_info.stat ||
                EXE_NET_REG_STAT_REGISTERED_ROAMING == reg_info.stat) {

            if (!map_rat_to_basic_act(reg_info_p->rat, &reg_info.act)) {
                ATC_LOG_E("%s: radio technology mapping failed", __FUNCTION__);
                goto error;
            }
        }

        exe_request_complete(record_p, EXE_SUCCESS, &reg_info);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state=%d!", record_p->state);
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

exe_request_result_t request_creg_set(exe_request_record_t *record_p)
{
    return request_reg_status_event_config(record_p);
}


exe_request_result_t request_ereg_read(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_registration_state_normal(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("CN_SUCCESS != result");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        cn_registration_info_t *reg_info_p;
        exe_ereg_reg_info_t reg_info = { EXE_NET_REG_STAT_UNKNOWN, 0, 0, EXE_ACT_GSM, 0 };
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("response data NULL");
            goto error;
        }

        reg_info_p = (cn_registration_info_t *)record_p->response_data_p;

        if (!map_registration_state_from_cn_to_at(reg_info_p->reg_status,
                &reg_info.stat)) {
            ATC_LOG_E("%s: registration mapping failed!", __FUNCTION__);
            goto error;
        }

        reg_info.lac = reg_info_p->lac;
        reg_info.cid = reg_info_p->cid;
        reg_info.cs_attached = reg_info_p->cs_attached;

        if (EXE_NET_REG_STAT_REGISTERED_HOME == reg_info.stat ||
                EXE_NET_REG_STAT_REGISTERED_ROAMING == reg_info.stat) {

            if (!map_rat_to_basic_act(reg_info_p->rat, &reg_info.act)) {
                ATC_LOG_E("%s: radio technology mapping failed", __FUNCTION__);
                goto error;
            }
        }

        exe_request_complete(record_p, EXE_SUCCESS, &reg_info);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_ereg_set(exe_request_record_t *record_p)
{
    return request_reg_status_event_config(record_p);
}

exe_request_result_t request_modem_reset(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("-> request");

        result = cn_request_reset_modem_with_dump(cn_client_get_context(), CN_CPU_L23, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_reset_modem_with_dump failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_reset_modem_with_dump returns %d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
error:
    return EXE_FAILURE;
}

exe_request_result_t request_gprs_registration_state_read(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_registration_state_gprs(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("CN_SUCCESS != result");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        cn_registration_info_t *reg_info_p;
        exe_cgreg_reg_info_t cgreg_res = { 0, 0, 0, 0 };
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("response data NULL");
            goto error;
        }

        reg_info_p = (cn_registration_info_t *)record_p->response_data_p;

        if (!map_gprs_registration_state_from_cn_to_at(reg_info_p->reg_status,
                reg_info_p->gprs_attached, &cgreg_res.stat)) {
            ATC_LOG_E("registration mapping failed");
            goto error;
        }

        if (EXE_NET_REG_STAT_REGISTERED_HOME == cgreg_res.stat ||
                EXE_NET_REG_STAT_REGISTERED_ROAMING == cgreg_res.stat) {
            cgreg_res.lac = reg_info_p->lac; /* Location area code set */
            cgreg_res.ci = reg_info_p->cid;  /* Cell id */

            if (!map_rat_to_act(reg_info_p->rat, &cgreg_res.act)) {
                ATC_LOG_E("rat mapping failed");
                goto error;
            }
        }


        exe_request_complete(record_p, EXE_SUCCESS, &cgreg_res);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_gprs_registration_state_set(exe_request_record_t *record_p)
{
    return request_reg_status_event_config(record_p);
}


exe_request_result_t request_network_automatic_register(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_automatic_network_registration(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_FAILURE == record_p->client_result_code) {
            exe_request_complete(record_p, EXE_FAILURE, NULL);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:
    return EXE_FAILURE;
}


exe_request_result_t request_network_manual_register(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request data NULL");
            goto error;
        }

        result = cn_request_manual_network_registration(cn_client_get_context(),
                 (cn_network_registration_req_data_t *)record_p->request_data_p,
                 request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_network_manual_register_auto_fallback(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_user_status_t user_status;

        ATC_LOG_I("-> request");

        if (!record_p->request_data_p) {
            ATC_LOG_E("no request data!");
            goto error;
        }

        result = cn_request_manual_network_registration_with_automatic_fallback(cn_client_get_context(),
                 (cn_network_registration_req_data_t *)record_p->request_data_p,
                 request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_manual_network_registration_with_automatic_fallback() failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            if (CN_SUCCESS_AUTOMATIC_FALLBACK != record_p->client_result_code) {
                ATC_LOG_E("failed! record_p->client_result_code = %d", record_p->client_result_code);
                goto error;
            } else {
                ATC_LOG_W("Manual registration to selected network failed, but fallback to automatic selection succeeded!");
            }
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_network_deregister(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);

        result = cn_request_network_deregister(cn_client_get_context(),
                                               request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_network_deregister request request failed (%d)", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_network_get_settings(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);

        result = cn_request_net_query_mode(cn_client_get_context(),
                                           request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_net_query_mode request failed");
            goto error;
        }

        record_p->client_data_p = malloc(sizeof(uint8_t));

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("could not allocate memory");
            goto error;
        }

        /* Need to save the format data to use when we get the response */
        *((uint8_t *)record_p->client_data_p) = (uint8_t)((exe_cops_request_data_t *)record_p->request_data_p)->format;
        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        cn_registration_info_t *reg_info_p = NULL;
        exe_cops_response_data_t response;
        int n;
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("null data received");
            goto error;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("response data from service is NULL");
            free(record_p->client_data_p);
            goto error;
        }

        (void)memset(&response, 0, sizeof(response));
        response.format = *((uint8_t *)record_p->client_data_p);
        free(record_p->client_data_p);
        reg_info_p = (cn_registration_info_t *)record_p->response_data_p;

        switch (reg_info_p->reg_status) {
            /* Both values has the same meaning here, Intentional fallthrough */
        case CN_REGISTERED_TO_HOME_NW:      /* Registered to home network */
        case CN_REGISTERED_ROAMING: {       /* Registered in roaming */
            /* operator is selected */
            response.operator_selected = true;
            break;
        }
        /* Intentional fallthrough */
        case CN_NOT_REG_NOT_SEARCHING:
        case CN_NOT_REG_SEARCHING_OP:
        case CN_REGISTRATION_DENIED:
        case CN_REGISTRATION_UNKNOWN:

        default:
            /* operator is not selected */
            response.operator_selected = false;
        }

        if (response.operator_selected) {
            switch (response.format) {
            case EXE_COPS_FORMAT_LONG: /* copy long alpha numeric */

                n = snprintf((char *) &response.operator,
                             EXE_COPS_OPERATOR_MAX_LONG_LEN + 1,
                             "%s",
                             reg_info_p->long_operator_name);

                if (n < 0) {
                    ATC_LOG_E("invalid format in request data");
                    goto error;
                } else if (n >= EXE_COPS_OPERATOR_MAX_LONG_LEN + 1) {
                    ATC_LOG_E("Warning! truncated operator string");
                } else {
                    ;
                }

                response.operator[EXE_COPS_OPERATOR_MAX_LONG_LEN] = '\0';
                break;

            case EXE_COPS_FORMAT_SHORT: /* copy short alpha numeric */

                n = snprintf((char *) &response.operator,
                             EXE_COPS_OPERATOR_MAX_SHORT_LEN + 1,
                             "%s",
                             reg_info_p->short_operator_name);

                if (n < 0) {
                    ATC_LOG_E("invalid format in request data");
                    goto error;
                } else if (n >= EXE_COPS_OPERATOR_MAX_SHORT_LEN + 1) {
                    ATC_LOG_E("Warning! truncated operator string");
                } else {
                    ;
                }

                response.operator[EXE_COPS_OPERATOR_MAX_SHORT_LEN] = '\0';
                break;

            case EXE_COPS_FORMAT_NUMERIC: /* copy code */

                n = snprintf((char *) &response.operator,
                             EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH + 1,
                             "%s",
                             reg_info_p->mcc_mnc);

                if (n < 0) {
                    ATC_LOG_E("invalid format in request data");
                    goto error;
                } else if (n >= EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH + 1) {
                    ATC_LOG_E("Warning! truncated operator string");
                }

                response.operator[EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH] = '\0';
                break;

            default:
                ATC_LOG_E("invalid format in request data");
                goto error;
            }

            if (!map_rat_to_act(reg_info_p->rat, &response.act)) {
                ATC_LOG_E("%s: radio technology mapping failed", __FUNCTION__);
                goto error;
            }
        }

        response.mode = reg_info_p->search_mode;

        exe_request_complete(record_p, EXE_SUCCESS, &response);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_network_search(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_cops_response_data_t *response_p = NULL;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("-> request current operator");

        result = cn_request_net_query_mode(cn_client_get_context(),
                                           request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("Current operator request failed");
            goto error;
        }

        record_p->state = EXE_STATE_NET_QUERY;
        return EXE_PENDING;
    }
    case EXE_STATE_NET_QUERY: {
        ATC_LOG_I("-> request manual search");

        result = cn_request_manual_network_search(cn_client_get_context(),
                 request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("Manual search request failed");
            goto error;
        }

        if (record_p->response_data_p) {
            record_p->request_data_p =
                calloc(1, sizeof(cn_registration_info_t));

            if (!record_p->request_data_p) {
                ATC_LOG_E("Memory allocation failure!");
                goto error;
            }

            memmove(record_p->request_data_p, record_p->response_data_p,
                    sizeof(cn_registration_info_t));
        }

        record_p->state = EXE_STATE_NETWORK_SEARCH;
        return EXE_PENDING;
    }
    case EXE_STATE_NETWORK_SEARCH: {
        cn_manual_network_search_data_t *network_search_data_p =
            (cn_manual_network_search_data_t *)record_p->response_data_p;
        exe_cops_network_list_t network_info;
        uint32_t i;
        ATC_LOG_I("<- response");

        if (record_p->abort == true) {
            ATC_LOG_I("-> abort request");

            result = cn_request_interrupt_network_search(cn_client_get_context(), request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_I("request_network_interrupt_search, call to mal_net_interrupt_search failed.");
                return EXE_FAILURE;
            } else {
                record_p->state = EXE_STATE_NETWORK_SEARCH_ABORT;
                return EXE_PENDING;
            }
        }

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("failed! record_p->client_result_code = %d", record_p->client_result_code);

            if (CN_REQUEST_INTERRUPTED == record_p->client_result_code) {
                response_p = (exe_cops_response_data_t *) calloc(1, sizeof(*response_p));

                if (response_p) {
                    response_p->error_code = CMEE_NOT_FOUND;
                }
            }
            else if (CN_REQUEST_CAUSE_NET_NOT_FOUND == record_p->client_result_code) {
                response_p = (exe_cops_response_data_t *) calloc(1, sizeof(*response_p));

                if (response_p) {
                    response_p->error_code = CMEE_NO_NETWORK_SERVICE;
                }
            }

            goto error;
        }

        if (NULL == network_search_data_p) {
            ATC_LOG_E("null data received");
            goto error;
        }

        /* Allocate memory for translation of network list from the modem */
        network_info.num_of_networks = network_search_data_p->num_of_networks;

        /* Prepare sorting of the list */
        int nr_home = 0;
        int nr_preferred = 0;
        int nr_forbidden = 0;
        int nr_other = 0;
        int additional = 0;

        for (i = 0; i < network_info.num_of_networks; i++) {
            int dual_rat = 0;

            if (network_search_data_p->cn_network_info[i].band_info != GSM_BAND_INFO_NOT_AVAILABLE && network_search_data_p->cn_network_info[i].umts_available == 1) {
                additional++;
                dual_rat = 1;
            }

            switch ((cn_uint8_t)network_search_data_p->cn_network_info[i].network_type) {
            case 0: /* Home PLMN */
                nr_home += 1 + dual_rat;
                break;
            case 1: /* Preferred PLMN */
                nr_preferred +=  1 + dual_rat;
                break;
            case 2: /* Forbidden PLMN */
                nr_forbidden +=  1 + dual_rat;
                break;
            case 3: /* Other PLMN */
                nr_other += 1 + dual_rat;
                break;
            default:
                /* Do nothing */
                break;
            }

        }

        network_info.completelist_p =
            (exe_cops_operator_info_t *)calloc(network_info.num_of_networks + additional,
                                               sizeof(exe_cops_operator_info_t));

        if (NULL == network_info.completelist_p) {
            ATC_LOG_E("could not allocate memory");
            goto error;
        }

        int index = 0;
        int index_home = 0;
        int index_preferred = nr_home;
        int index_forbidden = nr_home + nr_preferred;
        int index_other = nr_home + nr_preferred + nr_forbidden;
        cn_uint8_t is_current_umts = 0;
        cn_registration_info_t *reg_info_p =
            (cn_registration_info_t *)record_p->request_data_p;

        if (reg_info_p) {
            switch (reg_info_p->rat) {
            case CN_RAT_TYPE_UMTS:
                /*Fall trough */
            case CN_RAT_TYPE_HSDPA:
                /*Fall trough */
            case CN_RAT_TYPE_HSUPA:
                /*Fall trough */
            case CN_RAT_TYPE_HSPA:
                is_current_umts = 1;
                break;
            default:
                is_current_umts = 0;
                break;
            }

            free(reg_info_p);
            record_p->request_data_p = NULL;
        }

        for (i = 0; i < network_info.num_of_networks; i++) {
            int act = EXE_ACT_LAST;
            int n = 0;

            switch ((cn_uint8_t)network_search_data_p->cn_network_info[i].network_type) {
            case 0: /* Home PLMN */
                index = index_home;
                index_home++;
                break;
            case 1: /* Preferred PLMN */
                index = index_preferred;
                index_preferred++;
                break;
            case 2: /* Forbidden PLMN */
                index = index_forbidden;
                index_forbidden++;
                break;
            case 3: /* Other PLMN */
                index = index_other;
                index_other++;
                break;
            default:
                /* Do nothing */
                break;
            }

            /* Same format used, copy only and ensure NULL termination */
            (void)strncpy(network_info.completelist_p[index].numeric,
                          (const char *)network_search_data_p->cn_network_info[i].mcc_mnc,
                          sizeof(network_info.completelist_p[i].numeric) - 1);

            (void)strncpy(network_info.completelist_p[index].long_alphanumeric,
                          (const char *)network_search_data_p->cn_network_info[i].long_op_name,
                          sizeof(network_info.completelist_p[i].long_alphanumeric) - 1);

            (void)strncpy(network_info.completelist_p[index].short_alphanumeric,
                          (const char *)network_search_data_p->cn_network_info[i].short_op_name,
                          sizeof(network_info.completelist_p[i].short_alphanumeric) - 1);

            /* Translation to format according to AT spec. needed */
            switch (network_search_data_p->cn_network_info[i].network_status) {
            case 0:
                strncpy(network_info.completelist_p[index].stat, "0\0", 2);
                break;
            case 1:
                strncpy(network_info.completelist_p[index].stat, "1\0", 2);
                break;
            case 2: {
                ATC_LOG_D("current_rat: %X, umts_available: %X",
                          is_current_umts,
                          network_search_data_p->cn_network_info[i].umts_available);

                if (network_search_data_p->cn_network_info[i].umts_available != is_current_umts) {
                    /* We are on current cell but uses GSM not UMTS */
                    strncpy(network_info.completelist_p[index].stat, "1\0", 2);
                } else {
                    strncpy(network_info.completelist_p[index].stat, "2\0", 2);
                }

                break;
            }
            case 3:
                strncpy(network_info.completelist_p[index].stat, "3\0", 2);
                break;
            default:
                ATC_LOG_E("invalid <stat> %d", network_search_data_p->cn_network_info[i].network_status);
                free(network_info.completelist_p);
                goto error;
                break;
            }

            switch (network_search_data_p->cn_network_info[i].umts_available) {
            case 0:
                act = EXE_ACT_GSM;
                break;
            case 1:
                act = EXE_ACT_UTRAN;
                break;
            default:
                ATC_LOG_E("invalid <umts_available> %d", network_search_data_p->cn_network_info[i].umts_available);
                free(network_info.completelist_p);
                goto error;
                break;
            }

            n = snprintf(network_info.completelist_p[index].act, EXE_COPS_OPERATOR_MAX_ACT_LEN + 1, "%d", act);

            if (n < 0) {
                ATC_LOG_E("Could not write to operator field");
                free(network_info.completelist_p);
                goto error;
            } else if (EXE_COPS_OPERATOR_MAX_ACT_LEN + 1 <= n) {
                ATC_LOG_E("<AcT> string exceeded maximum length");
                free(network_info.completelist_p);
                goto error;
            }

            if (network_search_data_p->cn_network_info[i].band_info != GSM_BAND_INFO_NOT_AVAILABLE && network_search_data_p->cn_network_info[i].umts_available == 1) {

                switch ((cn_uint8_t)network_search_data_p->cn_network_info[i].network_type) {
                case 0: /* Home PLMN */
                    index = index_home;
                    index_home++;
                    break;
                case 1: /* Preferred PLMN */
                    index = index_preferred;
                    index_preferred++;
                    break;
                case 2: /* Forbidden PLMN */
                    index = index_forbidden;
                    index_forbidden++;
                    break;
                case 3: /* Other PLMN */
                    index = index_other;
                    index_other++;
                    break;
                default:
                    /* Do nothing */
                    break;
                }

                memmove(&(network_info.completelist_p[index]), &(network_info.completelist_p[index-1]), sizeof(exe_cops_operator_info_t));
                act = EXE_ACT_GSM;

                if (network_search_data_p->cn_network_info[i].network_status == 2) {
                    ATC_LOG_D("is_current_umts: %X", is_current_umts);

                    if (is_current_umts == 1) {
                        /* We are on current cell but uses UMTS not GSM */
                        strncpy(network_info.completelist_p[index].stat, "1\0", 2);
                    } else {
                        strncpy(network_info.completelist_p[index].stat, "2\0", 2);
                    }
                }

                n = snprintf(network_info.completelist_p[index].act, EXE_COPS_OPERATOR_MAX_ACT_LEN + 1, "%d", act);

                if (n < 0) {
                    ATC_LOG_E("request_network_get_available_operators(): Could not write to operator field");
                    free(network_info.completelist_p);
                    goto error;
                } else if (EXE_COPS_OPERATOR_MAX_ACT_LEN + 1 <= n) {
                    ATC_LOG_E("request_network_get_available_operators(): <AcT> string exceeded maximum length");
                    free(network_info.completelist_p);
                    goto error;
                }
            }
        }

        network_info.num_of_networks += additional;
        exe_request_complete(record_p, EXE_SUCCESS, &network_info);
        free(network_info.completelist_p);
        return EXE_SUCCESS;
    }

    case EXE_STATE_NETWORK_SEARCH_ABORT:
        ATC_LOG_I("<- response search failed");
        /* This should be the response to cn_request_manual_network_search (failure). */
        record_p->state = EXE_STATE_NETWORK_SEARCH_TERMINATE;
        return EXE_PENDING;

    case EXE_STATE_NETWORK_SEARCH_TERMINATE:
        ATC_LOG_I("<- response abort succeeded");
        /* This should be the response to cn_request_interrupt_network_search. */
        request_record_destroy(record_p);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, response_p);

        free(response_p);
    }

    if (record_p->request_data_p) {
        free(record_p->request_data_p);
        record_p->request_data_p = NULL;
    }

    return EXE_FAILURE;
}


/********************************************************************
 * Call handling
 ********************************************************************
 */

/**
 * Converts cn context for call info to clcc struct.
 */
static void exe_cn_context_to_clcc(cn_call_context_t *call_context_p, exe_clcc_call_info_t *call_info_p)
{
    call_info_p->id         = call_context_p->call_id;
    call_info_p->direction  = (0 == call_context_p->is_MT) ? 0 : 1;
    call_info_p->state      = call_context_p->call_state;
    call_info_p->multipart  = (0 == call_context_p->is_multiparty) ? 0 : 1;
    call_info_p->type       = call_context_p->address_type;

    if (!map_cn_call_mode_to_clcc_mode(call_context_p->mode, (exe_clcc_mode_t *)&call_info_p->mode)) {
        ATC_LOG_E("Failed to map cn call mode (0x%X) to clcc mode!", call_context_p->mode);
    }

    if (!map_cause_no_cli_to_cli_validity(call_context_p->cause_no_cli,
                                          call_context_p->number,
                                          &call_info_p->cli_validity)) {
        ATC_LOG_E("Failed to map 'Cause of no CLI' (0x%X) to 'CLI validity'!", call_context_p->cause_no_cli);
    }

    (void)strncpy(call_info_p->number, (const char *)call_context_p->number,
                  sizeof(call_info_p->number) - 1);
    call_info_p->number[sizeof(call_info_p->number)-1] = '\0';
}


/**
 * request_get_current_calls
 *
 * Returns info about all current calls.
 */
exe_request_result_t request_get_current_calls(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    cn_call_list_t *call_list_p;

    switch (record_p->state) {
    case EXE_STATE_REQUEST:
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_current_call_list(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS == result) {
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        break;

    case EXE_STATE_RESPONSE:
        call_list_p = (cn_call_list_t *)record_p->response_data_p;
        exe_clcc_response_t clcc_resp;
        int i;
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (call_list_p) {
            clcc_resp.number_of_calls = (call_list_p->nr_of_calls > EXE_MAX_NUMBER_CALLS)
                                        ? EXE_MAX_NUMBER_CALLS : call_list_p->nr_of_calls;

            for (i = 0; i < clcc_resp.number_of_calls; i++) {
                exe_cn_context_to_clcc(&call_list_p->call_context[i], &clcc_resp.call_info[i]);
            }

            exe_request_complete(record_p, EXE_SUCCESS, &clcc_resp);
            return EXE_SUCCESS;
        } else {
            ATC_LOG_E("response data from service is NULL");
        }

        break;

    default:
        break;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


/*
 * Finds number of calls and call-id based for current calls with a call-state matching supplied filter.
 *
 *  call_list_p     - Pointer to current call-list
 *  filter          - Filter with call-states to search for
 *  index           - One (1) based index of id to return in call_id
 *  call_id         - Optional pointer to id of call, 0 if none found
 *
 * Returns number of calls found matching call-state filter criteria, 0 if none found.
 * Returns -1 if error.
 */
static int get_call_status(cn_call_list_t *call_list_p, cn_call_state_filter_t filter, int index, cn_uint8_t *call_id_p)
{
    int nr_of_calls = 0;
    int calls_matching = 0;
    cn_uint8_t call_id = 0;
    int i;

    if (!call_list_p) {
        goto error;
    }

    nr_of_calls = call_list_p->nr_of_calls;

    for (i = 0; i < nr_of_calls; i++) {
        switch (call_list_p->call_context[i].call_state) {
        case CN_CALL_STATE_ACTIVE:

            if (CN_CALL_STATE_FILTER_ACTIVE & filter) {
                calls_matching++;

                if (calls_matching == index) {
                    call_id = call_list_p->call_context[i].call_id;
                }
            }

            break;

        case CN_CALL_STATE_HOLDING:

            if (CN_CALL_STATE_FILTER_HOLDING & filter) {
                calls_matching++;

                if (calls_matching == index) {
                    call_id = call_list_p->call_context[i].call_id;
                }
            }

            break;

        case CN_CALL_STATE_DIALING:

            if (CN_CALL_STATE_FILTER_DIALLING & filter) {
                calls_matching++;

                if (calls_matching == index) {
                    call_id = call_list_p->call_context[i].call_id;
                }
            }

            break;

        case CN_CALL_STATE_ALERTING:

            if (CN_CALL_STATE_FILTER_ALERTING & filter) {
                calls_matching++;

                if (calls_matching == index) {
                    call_id = call_list_p->call_context[i].call_id;
                }
            }

            break;

        case CN_CALL_STATE_INCOMING:

            if (CN_CALL_STATE_FILTER_INCOMING & filter) {
                calls_matching++;

                if (calls_matching == index) {
                    call_id = call_list_p->call_context[i].call_id;
                }
            }

            break;

        case CN_CALL_STATE_WAITING:

            if (CN_CALL_STATE_FILTER_WAITING & filter) {
                calls_matching++;

                if (calls_matching == index) {
                    call_id = call_list_p->call_context[i].call_id;
                }
            }

            break;

        case CN_CALL_STATE_IDLE:

            if (CN_CALL_STATE_FILTER_IDLING & filter) {
                calls_matching++;

                if (calls_matching == index) {
                    call_id = call_list_p->call_context[i].call_id;
                }
            }

            break;

        default:
            break;
        }
    }

    if (call_id_p) {
        *call_id_p = call_id;
    }

    return calls_matching;

error:
    return -1;
}


exe_request_result_t request_call_dial_up(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_atd_data_t *atd_data_p;
    exe_atd_response_t response;

    response.error_code = CMEE_OK;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        cn_dial_t dial;
        exe_atd_data_t *atd_data_p = (exe_atd_data_t *)record_p->request_data_p;

        if (!atd_data_p) {
            ATC_LOG_I("%s: request_data_p is NULL!", __FUNCTION__);
            goto error;
        }

        if (!atd_data_p->phone_number_p) {
            ATC_LOG_I("%s: phone_number_p is NULL!", __FUNCTION__);
            goto error;
        }

        memset(&dial, 0, sizeof(dial));
        (void)strncpy(dial.phone_number, (const char *)atd_data_p->phone_number_p, sizeof(dial.phone_number));
        dial.clir = atd_data_p->clir;
        dial.sat_initiated = false;

        if (atd_data_p->cuus1_data_p && atd_data_p->cuus1_data_p->data) {
            memmove(dial.user_to_user, atd_data_p->cuus1_data_p->data, atd_data_p->cuus1_data_p->length);
            dial.user_to_user_len = atd_data_p->cuus1_data_p->length;
        }

        result = cn_request_dial(cn_client_get_context(), &dial, request_record_get_client_tag(record_p));

        if (CN_SUCCESS == result) {
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        break;
    }

    case EXE_STATE_RESPONSE:
        ATC_LOG_I("%s -> response", __FUNCTION__);

        switch (record_p->client_result_code) {
        case CN_SUCCESS:
            response.error_code = CMEE_OK;
            exe_request_complete(record_p, EXE_SUCCESS, &response);
            return EXE_SUCCESS;
        case CN_REQUEST_CC_SERVICE_MODIFIED_SS:
            response.error_code = CMEE_MODIFIED_TO_SS;
            break;
        case CN_REQUEST_CC_SERVICE_MODIFIED_USSD:
            response.error_code = CMEE_MODIFIED_TO_USSD;
            break;
        case CN_REQUEST_FDN_BLOCKED:
            response.error_code = CMEE_FDN_BLOCKED;
            break;
        default:
            response.error_code = CMEE_OPERATION_NOT_ALLOWED;
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
            break;
        }

        break;

    default:
        break;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &response);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_call_hold(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_chld_t *exe_chld_p = (exe_chld_t *)record_p->request_data_p;
        uint8_t client_data[2];

        if (!exe_chld_p) {
            ATC_LOG_E("%s: request data from executor is NULL!", __FUNCTION__);
            break;
        }

        /* Copy executor parameter */
        client_data[0] = (uint8_t)exe_chld_p->n;
        client_data[1] = 0; /* To be used for call-id when needed */

        record_p->client_data_p = malloc(sizeof(client_data));

        if (!record_p->client_data_p) {
            ATC_LOG_E("%s: Memory allocation failure!", __FUNCTION__);
            break;
        }

        memcpy(record_p->client_data_p, client_data, sizeof(client_data));

        switch (client_data[0]) {
        case 0: {
            /* =0 Releases all held calls or sets User Determined User Busy (UDUB) for a waiting call. */
            /* NOTE: "waiting" appears to mean not just waiting, but incoming calls as well */
            ATC_LOG_I("%s -> request hang-up", __FUNCTION__);

            result = cn_request_hangup(cn_client_get_context(),
                                       (CN_CALL_STATE_FILTER_HOLDING  |
                                        CN_CALL_STATE_FILTER_INCOMING |
                                        CN_CALL_STATE_FILTER_WAITING),
                                       0, /* call-id not used when filter is used */
                                       request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_hangup holding/waiting, failed (result:%d)!", __FUNCTION__, result);
                break;
            }

            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        case 1: {
            /* =1  A: Releases all active calls (if any exist), and B: accepts the other (held or waiting) call. */
            /* =1 Part A */
            ATC_LOG_I("%s -> request hang-up active call(s)", __FUNCTION__);
            result = cn_request_hangup(cn_client_get_context(),
                                       CN_CALL_STATE_FILTER_ACTIVE,
                                       0, /* call-id not used when filter is used */
                                       request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_hangup active call(s), failed (result:%d)!", __FUNCTION__, result);
                break;
            }

            /* Next step is to get call-list, and then part B: answer waiting, or resume held call */
            record_p->state = EXE_STATE_CALL_LIST_REQUEST;
            return EXE_PENDING;
        }

        case 2:
        case 3:
        case 4: {
            /* Need a current call-list for commands 2, 3, 4 */
            ATC_LOG_I("%s -> request call_list", __FUNCTION__);
            result = cn_request_current_call_list(cn_client_get_context(), request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_current_call_list failed (result:%d)!", __FUNCTION__, result);
                break;
            }

            record_p->state = EXE_STATE_CALL_LIST_RESPONSE;
            return EXE_PENDING;
        }

        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18: {
            /* =1X Releases a specific call X. */
            ATC_LOG_I("%s -> request hang-up call-id=%d", __FUNCTION__, (client_data[0] % 10));
            result = cn_request_hangup(cn_client_get_context(),
                                       CN_CALL_STATE_FILTER_NONE,
                                       (client_data[0] % 10),   /* X is the remainder of n / 10. */
                                       request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_hangup call-id=%d, failed (result:%d)!", __FUNCTION__, (client_data[0] % 10), result);
                break;
            }

            /* Last request has been made */
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28: {
            /* =2X Places all active calls on hold except call X with which communication shall be supported. */
            ATC_LOG_I("%s -> request conference call split", __FUNCTION__);

            result = cn_request_conference_call_split(cn_client_get_context(),
                     (client_data[0] % 10),  /* X is the remainder of n / 10. */
                     request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_conference_call_split call_id=%d, failed (result:%d)!",
                          __FUNCTION__, (client_data[0] % 10), result);
                break;
            }

            /* Last request has been made */
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        default:
            ATC_LOG_E("%s: unexpected n=%d!", __FUNCTION__, client_data[0]);
            break;
        } /* switch (exe_chld_p->n) */

        break;
    }

    case EXE_STATE_CALL_LIST_REQUEST: {
        /* Finish handling of previous request */
        ATC_LOG_I("%s <- response hang-up active call(s)", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: cn_request_hangup failed (result:%d)!", __FUNCTION__, result);
            break;
        }

        /* Next request */
        /* Need a current call-list for remaining commands */
        ATC_LOG_I("%s -> request call_list", __FUNCTION__);
        result = cn_request_current_call_list(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_current_call_list failed (result:%d)!", __FUNCTION__, result);
            break;
        }

        record_p->state = EXE_STATE_CALL_LIST_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_CALL_LIST_RESPONSE: {
        cn_call_list_t *call_list_p = (cn_call_list_t *)record_p->response_data_p;
        uint8_t client_data[2];
        cn_uint8_t call_id;
        int nr_of_calls;
        ATC_LOG_I("%s <- response call_list", __FUNCTION__);

        if (!call_list_p) {
            ATC_LOG_E("%s: response data from service is NULL!", __FUNCTION__);
            break;
        }

        if (!record_p->client_data_p) {
            ATC_LOG_E("%s: client data is NULL!", __FUNCTION__);
            break;
        }

        memcpy(client_data, record_p->client_data_p, sizeof(client_data));

        switch (client_data[0]) {
        case 1: {
            /* =1  A: Releases all active calls (if any exist), and B: accepts the other (held or waiting) call. */
            /* =1  Part B */
            /* Waiting calls have priority over held */
            /* NOTE: "waiting" appears to mean not just waiting, but incoming calls as well */
            nr_of_calls = get_call_status(call_list_p,
                                          (CN_CALL_STATE_FILTER_INCOMING | CN_CALL_STATE_FILTER_WAITING),
                                          1,
                                          &call_id);

            if (call_id) {
                ATC_LOG_I("%s -> request answer waiting call", __FUNCTION__);
                result = cn_request_answer_call(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

                if (CN_SUCCESS != result) {
                    ATC_LOG_E("%s: cn_request_answer_call call_id=%d, failed (result:%d)!", __FUNCTION__, call_id, result);
                    break;
                }

            } else {
                nr_of_calls = get_call_status(call_list_p, CN_CALL_STATE_FILTER_HOLDING, 1, &call_id);

                if (call_id) {
                    ATC_LOG_I("%s -> request resume held call", __FUNCTION__);
                    result = cn_request_resume_call(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

                    if (CN_SUCCESS != result) {
                        ATC_LOG_E("%s: cn_request_resume_call call_id=%d, failed (result:%d)!", __FUNCTION__, call_id, result);
                        break;
                    }

                } else {
                    ATC_LOG_E("%s: but there is no waiting or held call!", __FUNCTION__);
                    break;
                }
            }

            /* Last request has been made */
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        case 2: {
            /* =2 Places all active call (if any exist) on hold and accept the other (held or waiting) call. */
            /* NOTE: no held or waiting calls need to exist */
            cn_uint8_t waiting_call = 0;
            cn_uint8_t holding_call = 0;
            cn_sint32_t nr_of_waiting = get_call_status(call_list_p, CN_CALL_STATE_FILTER_WAITING, 1, &waiting_call);
            cn_sint32_t nr_of_holding;

            if (!nr_of_waiting) {
                nr_of_holding = get_call_status(call_list_p, CN_CALL_STATE_FILTER_HOLDING, 1, &holding_call);
            } else {
                nr_of_holding = 0;
                holding_call = 0;
            }

            nr_of_calls = get_call_status(call_list_p, CN_CALL_STATE_FILTER_ACTIVE, 1, &call_id);

            if (waiting_call) {
                if (call_id) {
                    ATC_LOG_I("%s -> request hold call", __FUNCTION__);
                    result = cn_request_hold_call(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

                    if (CN_SUCCESS != result) {
                        ATC_LOG_E("%s: cn_request_hold_call call_id=%d, failed (result:%d)!", __FUNCTION__, call_id, result);
                        break;
                    }

                    /* Save the waiting call-id to answer */
                    client_data[1] = waiting_call;
                    memcpy(record_p->client_data_p, client_data, sizeof(client_data));

                    /* Next request answers waiting call */
                    record_p->state = EXE_STATE_CALL_HOLD_ANSWER_WAITING;

                } else {
                    ATC_LOG_I("%s -> request answer call", __FUNCTION__);
                    result = cn_request_answer_call(cn_client_get_context(), waiting_call, request_record_get_client_tag(record_p));

                    if (CN_SUCCESS != result) {
                        ATC_LOG_E("%s: cn_request_answer_call call_id=%d, failed (result:%d)!", __FUNCTION__, waiting_call, result);
                        break;
                    }

                    /* Last request has been made */
                    record_p->state = EXE_STATE_RESPONSE;
                }

            } else if (holding_call) {
                if (call_id) {
                    ATC_LOG_I("%s -> request swap calls", __FUNCTION__);
                    result = cn_request_swap_calls(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

                } else {
                    ATC_LOG_I("%s -> request resume call", __FUNCTION__);
                    result = cn_request_resume_call(cn_client_get_context(), holding_call, request_record_get_client_tag(record_p));
                }

                if (CN_SUCCESS != result) {
                    ATC_LOG_E("%s: cn_request_resume/swap_call(s) call_id=%d, failed (result:%d)!", __FUNCTION__, holding_call, result);
                    break;
                }

                /* Last request has been made */
                record_p->state = EXE_STATE_RESPONSE;

            } else if (call_id) {
                ATC_LOG_I("%s -> request hold call", __FUNCTION__);
                result = cn_request_hold_call(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

                if (CN_SUCCESS != result) {
                    ATC_LOG_E("%s: cn_request_hold_call call_id=%d, failed (result:%d)!", __FUNCTION__, call_id, result);
                    break;
                }

                /* Last request has been made */
                record_p->state = EXE_STATE_RESPONSE;

            } else {
                ATC_LOG_E("%s: but there is no active, waiting or held call!", __FUNCTION__);
                break;
            }

            return EXE_PENDING;
        }

        case 3: {
            /* =3 Adds a held call to the conversation */
            nr_of_calls = get_call_status(call_list_p, CN_CALL_STATE_FILTER_HOLDING, 1, &call_id);

            if (call_id) {
                ATC_LOG_I("%s -> request conference call", __FUNCTION__);
                result = cn_request_conference_call(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

                if (CN_SUCCESS != result) {
                    ATC_LOG_E("%s: cn_request_conference_call call_id=%d, failed (result:%d)!", __FUNCTION__, call_id, result);
                    break;
                }

            } else {
                ATC_LOG_E("%s: but there is no held call!", __FUNCTION__);
                break;
            }

            /* Last request has been made */
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        case 4: {
            /* =4 Connects the two calls and disconnects the subscriber from both calls. */
            /* Need 2 calls, one active/alerting and the other in hold state. */
            nr_of_calls = get_call_status(call_list_p,
                                          (CN_CALL_STATE_FILTER_ACTIVE | CN_CALL_STATE_FILTER_ALERTING),
                                          1,
                                          &call_id);

            if (1 != nr_of_calls) {
                ATC_LOG_E("%s: need one (1) active alerting call, got %d!", __FUNCTION__, nr_of_calls);
                break;
            }

            nr_of_calls = get_call_status(call_list_p,
                                          CN_CALL_STATE_FILTER_HOLDING,
                                          1,
                                          &call_id);

            if (1 != nr_of_calls) {
                ATC_LOG_E("%s: need one (1) held call, got %d!", __FUNCTION__, nr_of_calls);
                break;
            }

            ATC_LOG_I("%s -> request explicit call transfer", __FUNCTION__);
            result = cn_request_explicit_call_transfer(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_explicit_call_transfer call_id=%d, failed (result:%d)!", __FUNCTION__, call_id, result);
                break;
            }

            /* Last request has been made */
            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        default:
            ATC_LOG_E("%s: unexpected n=%d!", __FUNCTION__, client_data[0]);
            break;
        } /* switch (exe_chld_p->n) */

        break;
    }

    case EXE_STATE_CALL_HOLD_ANSWER_WAITING: {
        uint8_t client_data[2];
        ATC_LOG_I("%s <- response hold call", __FUNCTION__);

        if (!record_p->client_data_p) {
            ATC_LOG_E("%s: client data is NULL!", __FUNCTION__);
            break;
        }

        memcpy(client_data, record_p->client_data_p, sizeof(client_data));

        if (0 == client_data[1]) {
            ATC_LOG_E("%s: invalid call-id!", __FUNCTION__);
            break;
        }

        ATC_LOG_I("%s -> request answer call", __FUNCTION__);
        result = cn_request_answer_call(cn_client_get_context(), client_data[1], request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_answer_call call_id=%d, failed (result:%d)!", __FUNCTION__, client_data[1], result);
            break;
        }

        /* Last request has been made */
        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_RESPONSE:
        ATC_LOG_I("%s <- response final", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: cn_request_.... failed (result:%d)!", __FUNCTION__, result);
            break;
        }

        free(record_p->client_data_p);
        record_p->client_data_p = NULL;

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;

    default:
        ATC_LOG_E("%s: unknown state!", __FUNCTION__);
        break;
    }

error:

    if (record_p->client_data_p) {
        free(record_p->client_data_p);
        record_p->client_data_p = NULL;
    }

    if (EXE_STATE_REQUEST != record_p->state) {
        if (CN_FAILURE == record_p->client_result_code) {
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
        }

        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_answer(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST:
        ATC_LOG_I("%s -> request call_list", __FUNCTION__);
        result = cn_request_current_call_list(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_current_call_list error=%d!", __FUNCTION__, result);
            break;
        }

        record_p->state = EXE_STATE_CALL_LIST_RESPONSE;
        return EXE_PENDING;

    case EXE_STATE_CALL_LIST_RESPONSE: {
        cn_call_list_t *call_list_p = (cn_call_list_t *)record_p->response_data_p;
        cn_uint8_t call_id;
        int nr_of_calls;
        ATC_LOG_I("%s <- response call_list", __FUNCTION__);

        if (!call_list_p) {
            ATC_LOG_E("%s: response data from service is NULL", __FUNCTION__);
            break;
        }

        /* Waiting calls have priority over incoming */
        nr_of_calls = get_call_status(call_list_p, CN_CALL_STATE_FILTER_WAITING, 1, &call_id);

        if (!nr_of_calls) {
            nr_of_calls = get_call_status(call_list_p, CN_CALL_STATE_FILTER_INCOMING, 1, &call_id);

            if (!nr_of_calls) {
                ATC_LOG_E("%s: but there is no waiting or incoming call!", __FUNCTION__);
                break;
            }
        }

        ATC_LOG_I("%s -> request answer call", __FUNCTION__);
        result = cn_request_answer_call(cn_client_get_context(), call_id, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_answer_call call_id=%d, failed (result:%d)!", __FUNCTION__, call_id, result);
            break;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("%s <- response answer call", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
            break;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

    default:
        ATC_LOG_E("%s: unknown state!", __FUNCTION__);
        break;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_hangup(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        /* Hang-up all calls with first found state */
        cn_call_state_filter_t filter = CN_CALL_STATE_FILTER_ACTIVE |
                                        CN_CALL_STATE_FILTER_DIALLING |
                                        CN_CALL_STATE_FILTER_ALERTING |
                                        CN_CALL_STATE_FILTER_INCOMING;
        ATC_LOG_I("%s -> request", __FUNCTION__);

        result = cn_request_hangup(cn_client_get_context(), filter, 0 /* not used */, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_hangup, filter=0x%X error=%d!", __FUNCTION__, filter, result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

    default:
        ATC_LOG_E("%s: unknown state=%d!", __FUNCTION__, record_p->state);
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    ATC_LOG_E("%s: fail", __FUNCTION__);
    return EXE_FAILURE;
}


exe_request_result_t request_hangup_all(exe_request_record_t *record_p)
{
    cn_error_code_t result = EXE_FAILURE;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_call_state_filter_t filter = CN_CALL_STATE_FILTER_ALL;
        ATC_LOG_I("%s -> request", __FUNCTION__);

        result = cn_request_hangup(cn_client_get_context(), filter, 0 /* not used */, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_hangup filter=0x%X, failed (result:%d)!", __FUNCTION__, filter, result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_RESPONSE:
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;

    default:
        ATC_LOG_E("%s: unknown state=%d!", __FUNCTION__, record_p->state);
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    ATC_LOG_E("%s: fail", __FUNCTION__);
    return EXE_FAILURE;
}


exe_request_result_t request_ussd_send(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_error_response_t status = { .error = CMEE_UNKNOWN };

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request data NULL");
            goto error;
        } else {
            cn_ussd_info_t  ss_ussd_data;
            exe_cusd_request_data_t *cusd_request_p = (exe_cusd_request_data_t *)(record_p->request_data_p);
            size_t ussd_buf_len = sizeof(ss_ussd_data.ussd_string);

            memset(&ss_ussd_data, 0, sizeof(ss_ussd_data));

            ss_ussd_data.length       = (int32_t)cusd_request_p->ussd_length;
            ss_ussd_data.dcs          = (uint32_t) cusd_request_p->dcs;

            memmove(ss_ussd_data.ussd_string, cusd_request_p->str_p,
                    ss_ussd_data.length < ussd_buf_len ? ss_ussd_data.length : ussd_buf_len);

            ATC_LOG_I("request_ussd_send -> mal_ss_request_ussd ");
            result = cn_request_ussd(cn_client_get_context(),
                                     &ss_ussd_data,
                                     request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("cn_request_ussd failed!");
                goto error;
            }

            ATC_LOG_I("request_ussd_send -> OK ");
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_RESPONSE: {
        cn_response_ussd_t *response_p = (cn_response_ussd_t *)record_p->response_data_p;

        if (true == record_p->abort) {
            ATC_LOG_I("-> abort request");

            result = cn_request_ussd_abort(cn_client_get_context(), request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("request to abort USSD failed!");
                return EXE_FAILURE;
            } else {
                record_p->state = EXE_STATE_USSD_ABORT_RESPONSE;
                return EXE_PENDING;
            }
        }

        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);

            if (record_p->client_result_code == CN_REQUEST_FDN_BLOCKED) {
                status.error = CMEE_FDN_BLOCKED;
            } else {
                status.error = CMEE_UNKNOWN;
            }

            goto error;
        }

        if (response_p != NULL && response_p->ss_error.cn_ss_error_code_type != CN_SS_ERROR_CODE_TYPE_NONE) {
            ATC_LOG_E("%s: failed! SS error code", __FUNCTION__);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

    case EXE_STATE_USSD_ABORT_RESPONSE:
        ATC_LOG_I("<- abort response");
        request_record_destroy(record_p);
        return EXE_SUCCESS;

    default:
        ATC_LOG_E("unknown state=%d!", record_p->state);
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &status);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_ussd_cancel(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);

        result = cn_request_ussd_abort(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_ussd_abort failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_signal_strength(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_request_result_t exe_result = EXE_SUCCESS;
    int rx_rssi = 0;
    int signal_strength = 0;
    exe_csq_data_t st_data;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_rssi_value(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_I("request_signal_strength: cn_request_rssi_value failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        exe_result = EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (!record_p->response_data_p) {
            ATC_LOG_I("request_signal_strength: cn_response_rssi_value returned bad pointer!");
            goto error;
        }

        rx_rssi = *((cn_rssi_t *)(record_p->response_data_p));
        /* Convert CN RSSI value to AT RSSI value
         *
         * Target value range: 0..31
         * Expected value range from executor: 51..113 (-51dBm to -113dBm).
         *
         * Other values will be translated to the minimum/maximum value.
         *
         * Low signal strength (-113dBm) should be indicated with a zero integer. Therefore the scale is reversed.
         * Since the input value range is 64 we can divide by two to get the desired translation.
         */

        /* RSSI defined */
        if (rx_rssi > 0) {
            /* Filter out bad sensor values */
            if (rx_rssi > 113) {
                rx_rssi = 113;
            } else if (rx_rssi < 51) {
                rx_rssi = 51;
            }

            /* Convert to GSM 07.07 scale (0..31) */
            st_data.rssi = (113 - rx_rssi) >> 1;
        }
        /* RSSI unknown or not detectable */
        else {
            st_data.rssi = 99;
        }

        st_data.ber = 99; /* TODO: Not implemented */
        exe_request_complete(record_p, EXE_SUCCESS, &st_data);
    }

    return exe_result;

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_clip_read(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    int m;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_clip_status(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_clip_status failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL != record_p->response_data_p) {
            m = *((cn_clip_status_t *)record_p->response_data_p);
        } else {
            ATC_LOG_E("response data is NULL");
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &m);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_clir_read(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_clir_read_t clir_read_res;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_clir_status(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_clir_status failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        cn_clir_t *response_p = NULL;
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_FAILURE == record_p->client_result_code) {
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
            goto error;
        }

        if (NULL != record_p->response_data_p) {
            /* Todo: We should cast to the cn type and not the executer one
             * but I can't find it in the cn code so for now assume they match
             */
            response_p = (cn_clir_t *)record_p->response_data_p;
            clir_read_res.n = response_p->setting;
            clir_read_res.m = response_p->status;
        } else {
            ATC_LOG_E("response data is NULL");
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &clir_read_res);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_clir_set(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        int n;
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (record_p->request_data_p == NULL) {
            ATC_LOG_E("request_clir_set, no in data");
            goto error;
        }

        n = ((exe_clir_t *)record_p->request_data_p)->n;
        result = cn_request_set_clir(cn_client_get_context(), n, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_set_clir failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_FAILURE == record_p->client_result_code) {
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
            goto error;
        } else {
            exe_request_complete(record_p, EXE_SUCCESS, NULL);
            return EXE_SUCCESS;
        }
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

exe_request_result_t request_modem_sleep(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        int n;
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (record_p->request_data_p == NULL) {
            ATC_LOG_E("request_modem_sleep, no in data");
            goto error;
        }

        n = ((exe_modem_sleep_t *)record_p->request_data_p)->n;
        result = cn_request_sleep_test_mode(cn_client_get_context(), n, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_modem_sleep failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_sleep_test_mode returns %d", record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;

    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

void exe_ccfc_free_query_response_data(exe_ccfc_data_t **ccfc_data_pp)
{
    exe_ccfc_data_t *next_p = NULL;
    exe_ccfc_data_t *free_p = NULL;

    next_p = *ccfc_data_pp;

    while (NULL != next_p) {
        free(next_p->number_p);
        free_p = next_p;
        next_p = next_p->next_p;
        free(free_p);
    }

    *ccfc_data_pp = NULL;
}


bool exe_ccfc_create_query_response_data(exe_ccfc_head_t *ccfc_head_p, cn_call_forward_info_list_t *ccfc_query_response_p, int requested_classes)
{
    exe_ccfc_data_t *ccfc_prev_p                 = NULL;
    exe_ccfc_data_t *ccfc_curr_p                 = NULL;
    exe_ccfc_data_t *ccfc_first_p                = NULL;
    int              number_of_call_forward_info = ccfc_query_response_p->num;
    int              i                           = 0;
    int              j                           = 0;
    int              classcounter                = 1;
    /* request_call_forwarding only perceivable client of this function,
     * no need to check ccfc_head_p, as it will be != NULL */

    /* This is only to satisfy Coverity, ccfc_prev_p must be != NULL */
    ccfc_prev_p = ccfc_head_p->data_p = ccfc_first_p;

    ATC_LOG_I("exe_ccfc_create_query_response_data: Number of call fwd entries from MAL:%d", number_of_call_forward_info);

    /* sanity checking num to avoid runaway memory allocation and limit risk of segmentation fault if bad value */
    if (EXE_CCFC_MAX_NUMBER_OF_QUERY_INFOS < number_of_call_forward_info) {
        ATC_LOG_E("exe_ccfc_create_query_response_data: Corrupt number of ccfc query entries received. %d entries", number_of_call_forward_info);
        return false;
    }

    /* take care of the case if there are no registered numbers */
    if (0 == number_of_call_forward_info) {
        /* No data returned from the network, set all requested classes to disabled. */
        int response_mask = requested_classes;

        if (EXE_CLASSX_DATA & response_mask) {
            response_mask |= EXE_CLASSX_DATA_CIRCUIT_SYNC | EXE_CLASSX_DATA_CIRCUIT_ASYNC | EXE_CLASSX_PACKET_ACCESS | EXE_CLASSX_PAD_ACCESS;
            response_mask &= ~(EXE_CLASSX_DATA);
        }

        for (i = 1; i <= 8; i++) {
            if (classcounter & response_mask) {
                ccfc_curr_p = (exe_ccfc_data_t *)calloc(1, sizeof(exe_ccfc_data_t));
                ccfc_curr_p->class = classcounter;
                ccfc_curr_p->status = EXE_CCFC_STATUS_DISABLED;

                if (NULL == ccfc_head_p->data_p) {
                    ccfc_head_p->data_p = ccfc_curr_p;
                } else {
                    ccfc_prev_p->next_p = ccfc_curr_p;
                }

                ccfc_prev_p = ccfc_curr_p;
            }

            classcounter = classcounter << 1;
        }

        return true;
    }


    /* Loop through all received number_of_call_forward_info. */
    for (i = 0; i < number_of_call_forward_info; i++) {
        classcounter = 1;

        /* Each call_fwd_info may describe an aggregated class, if so it has to be split into
         * the individual components. */
        for (j = 1; j <= 8; j++) {
            if (classcounter & ccfc_query_response_p->call_fwd_info[i].service_class) {
                ccfc_curr_p = (exe_ccfc_data_t *)calloc(1, sizeof(exe_ccfc_data_t));
                /* calloc ensures that last next_p in list is NULL and that (0 == call_fwd_info[i].number[0]) */

                if (NULL == ccfc_curr_p) {
                    ATC_LOG_E("exe_ccfc_create_query_response_data: allocate ccfc_curr_p failed");
                    return false;
                }

                ccfc_curr_p->mode   = EXE_CCFC_MODE_QUERY_STATUS;
                ccfc_curr_p->class  = classcounter;
                ccfc_curr_p->status = (exe_ccfc_status_t)ccfc_query_response_p->call_fwd_info[i].status;
                ccfc_curr_p->reason = (exe_ccfc_reason_t)ccfc_query_response_p->call_fwd_info[i].reason;
                ccfc_curr_p->number_p = (char *)calloc(1, strlen(ccfc_query_response_p->call_fwd_info[i].number) + 1);

                if (NULL == ccfc_curr_p->number_p) {
                    free(ccfc_curr_p);
                    ATC_LOG_E("exe_ccfc_create_query_response_data: allocate ccfc_curr_p->number_p failed");
                    return false;
                }

                strcpy(ccfc_curr_p->number_p, ccfc_query_response_p->call_fwd_info[i].number);

                if (EXE_CCFC_REASON_NO_REPLY == ccfc_curr_p->reason) {
                    ccfc_curr_p->time = (int)ccfc_query_response_p->call_fwd_info[i].time_seconds;
                }

                ccfc_curr_p->type = (int)ccfc_query_response_p->call_fwd_info[i].toa;

                if (NULL == ccfc_head_p->data_p) {
                    ccfc_head_p->data_p = ccfc_curr_p;
                } else {
                    ccfc_prev_p->next_p = ccfc_curr_p;
                }

                ccfc_prev_p = ccfc_curr_p;

            }

            classcounter = classcounter << 1;
        }
    }


    return true;
}


exe_request_result_t request_call_forwarding(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_ccfc_head_t ccfc_head = { .error_code = CMEE_UNKNOWN };

    if (EXE_STATE_REQUEST == record_p->state) {
        if (NULL != record_p->request_data_p) {
            cn_call_forward_info_t    call_forward_info;
            exe_ccfc_data_t          *ccfc_request_p = (exe_ccfc_data_t *)(record_p->request_data_p);
            int                       time           = ccfc_request_p->time;
            int                       type           = ccfc_request_p->type;
            exe_ccfc_mode_t           mode           = ccfc_request_p->mode;
            exe_ccfc_reason_t         reason         = ccfc_request_p->reason;

            record_p->client_data_p = calloc(1, sizeof(exe_ccfc_data_t));

            if (NULL == record_p->client_data_p) {
                ATC_LOG_E("request_call_forwarding, could not allocate record_p->client_data_p");
                goto error;
            }

            /*Need to remember the mode and requested classes when handling the response part*/
            ((exe_ccfc_data_t *)record_p->client_data_p)->mode = ccfc_request_p->mode;
            ((exe_ccfc_data_t *)record_p->client_data_p)->class = ccfc_request_p->class;

            /* As the MODEM requires a parameter in the closed set of [5..30] s in step of 5 s we transform the
             * time value to fit this more coarse granularity in time. The AT command spec stipulates interval
             * to be [1..30] s in steps of 1 s. */
            time = ((time + 4) / 5) * 5;

            call_forward_info.status        = (int32_t)ccfc_request_p->mode; /* Yes, status is the correct MAL parameter here */
            call_forward_info.reason        = (int32_t)ccfc_request_p->reason;
            call_forward_info.service_class = (int32_t)ccfc_request_p->class;

            /* 128-255 is allowed for type according to AT command spec. The coding of this parameter is explained
             * in 3gpp TS 124.008 v7 (10.5.4.7). Note that some settings are not allowed in CCFC, why currently we treat
             * deviations as an error.
             */
            if (EXE_CCFC_MODE_REGISTRATION == mode && EXE_CCFC_NUM_TYPE_PLUS != type &&
                    EXE_CCFC_NUM_TYPE_NO_PLUS != type  && EXE_CCFC_NUM_TYPE_NATIONAL != type) {
                ATC_LOG_E("request_call_forwarding: phone numbering plan (type) not as per spec = %d. Should be 129, 145 or 161.", type);
                goto error;
            }

            call_forward_info.toa          = (int32_t)type;
            strncpy(call_forward_info.number, ccfc_request_p->number_p, sizeof(call_forward_info.number));
            call_forward_info.time_seconds = (int32_t)time;

            ATC_LOG_I("%s -> request", __FUNCTION__);

            if (EXE_CCFC_MODE_QUERY_STATUS == ccfc_request_p->mode) {
                result = cn_request_query_call_forward(cn_client_get_context(), &call_forward_info, request_record_get_client_tag(record_p));
            } else {
                result = cn_request_set_call_forward(cn_client_get_context(), &call_forward_info, request_record_get_client_tag(record_p));
            }

            if (CN_SUCCESS != result) {
                ATC_LOG_E("cn_request_query/set_call_forward failed (result:%d)", result);
                goto error;
            }

            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);
        ccfc_head.data_p = NULL;

        switch (record_p->client_result_code) {
        case CN_FAILURE:
            ccfc_head.error_code = CMEE_UNKNOWN;
            goto error;
        case CN_REQUEST_NOT_SUPPORTED:
            ccfc_head.error_code = CMEE_OPERATION_NOT_SUPPORTED;
            goto error;
        case CN_REQUEST_FDN_BLOCKED:
            ccfc_head.error_code = CMEE_FDN_BLOCKED;
            goto error;
        case CN_SUCCESS: {
            exe_request_result_t result   = EXE_SUCCESS;
            exe_ccfc_data_t *client_ccfc_data_p = (exe_ccfc_data_t *)record_p->client_data_p;

            ccfc_head.error_code = CMEE_OK;

            if (NULL != record_p->response_data_p) { /* Not supported or failure should result with result_data_p == NULL */
                if (EXE_CCFC_MODE_QUERY_STATUS == client_ccfc_data_p->mode) { /* This is a query response */
                    if (!exe_ccfc_create_query_response_data(&ccfc_head, ((cn_call_forward_info_list_t *)record_p->response_data_p), client_ccfc_data_p->class)) {
                        /* if error here nothing need to be freed, see exe_ccfc_create_query_response_data()*/
                        /* Error print done in  exe_ccfc_create_query_response_data()*/
                        ccfc_head.error_code = CMEE_INVALID_INDEX;
                        goto error;
                    }
                }

                /* If we have done SET of registration, erasure etc, no response is to be sent to higher layers, except to indicate success. */
            }

            if (NULL != record_p->client_data_p) {
                free(record_p->client_data_p);
                record_p->client_data_p = NULL;
            }

            exe_request_complete(record_p, result, &ccfc_head);

            if (NULL != ccfc_head.data_p) {
                exe_ccfc_free_query_response_data(&ccfc_head.data_p);
            }

            return result;
        }
        break;
        default:
            break;
        }
    }

error:

    if (NULL != record_p->client_data_p) {
        free(record_p->client_data_p);
        record_p->client_data_p = NULL;
    }

    if (EXE_STATE_REQUEST != record_p->state) { /* same error clause used for both request and response */
        exe_request_complete(record_p, EXE_FAILURE, &ccfc_head);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_modem_property_set(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    int n = 0;
    cn_modem_property_t modem_prop_struct = {CN_MODEM_PROPERTY_TYPE_CTM, {0}};

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_modem_property_set, no in data");
            goto error;
        }

        n = ((exe_etty_t *)record_p->request_data_p)->n;

        if (n == 0) {
            modem_prop_struct.value.ctm = CN_MODEM_PROP_VALUE_CTM_NOT_SUPPORTED;
        } else {
            modem_prop_struct.value.ctm = CN_MODEM_PROP_VALUE_CTM_SUPPORTED;
        }

        result = cn_request_set_modem_property(cn_client_get_context(), modem_prop_struct, request_record_get_client_tag(record_p));
        /*
         * TODO: This is now a structure, so we should be passing a pointer and not the structure itself.
         *
         */

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_set_modem_property failed (result:%d)", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_FAILURE == record_p->client_result_code) {
            exe_cn_register_fail_cause((cn_exit_cause_t *)record_p->response_data_p, __FUNCTION__);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_set_supp_svc_notification(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST:
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (record_p->request_data_p) {
            cn_cssn_setting_t cssn_setting;
            int mode = ((exe_cssn_request_data_t *)record_p->request_data_p)->mode;

            cssn_setting = (0 == mode)
                           ? CN_SUPPL_SVC_NOTIFY_DISABLE
                           : CN_SUPPL_SVC_NOTIFY_ENABLE;

            result = cn_request_set_cssn(cn_client_get_context(), cssn_setting, request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("cn_request_set_cssn failed (result:%d)", result);
                goto error;
            }

            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        }

        break;

    case EXE_STATE_RESPONSE:
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: cn request failed (result:%d)", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;

    default:
        break;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_dtmf_send(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_vts_t *dtmf_p = NULL;
        exe_dtmf_data_t *dtmf_data_p = NULL;

        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (record_p->request_data_p == NULL) {
            ATC_LOG_E("request_dtmf_send, no in data");
            goto error;
        }

        dtmf_data_p = ((exe_dtmf_data_t *)record_p->request_data_p);

        if (NULL == dtmf_data_p->dtmf_p) {
            ATC_LOG_E("request_dtmf_send: received NULL string");
            goto error;
        }

        /* We need to support both single character DTMF strings with a huge
         * duration and multi-character DTMF strings with a short duration.
         * If the former use the dtmf_start & dtmf_stop API, if the latter
         * use the dtmf_send API. This should provide a good balance.
         *
         * The divisor will be duration = 10000000 - then use dtmf_start/stop.
         *
         * This means that a combination of multi-character strings and
         * long duration will have the duration limited to 10 (1 sec)
         * because of a limitation in lower layers.
         */
        if (1 < dtmf_data_p->length || 10000000 != exe_get_dtmf_duration(record_p->exe_p)) {
            /* AT+VTD sets the duration time to n, where n means n/10 seconds.
             * Example: <n> = 2 means DTMF tone duration = 200 ms.
             * Thus the multiplication with 100 below.
             * Furthermore, 0 is not 0 ms but the modem default value. */
            result = cn_request_dtmf_send(cn_client_get_context(),
                                          dtmf_data_p->dtmf_p,
                                          dtmf_data_p->length,
                                          CN_DTMF_STRING_TYPE_ASCII,
                                          exe_get_dtmf_duration(record_p->exe_p) * 100,
                                          0,
                                          request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_dtmf_send failed!", __FUNCTION__);
                goto error;
            }
        } else {
            result = cn_request_dtmf_start(cn_client_get_context(),
                                           *dtmf_data_p->dtmf_p,
                                           request_record_get_client_tag(record_p));

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_dtmf_start failed!", __FUNCTION__);
                goto error;
            }

            /* Remember the state so we shut it off later */
            state_dtmf_start = 1;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("%s: unknown state!", __FUNCTION__);
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_set_dtmf_duration(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_vtd_t dtmf_duration = 0;

        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (!record_p->request_data_p) {
            ATC_LOG_E("%s: request_data_p contains no data", __FUNCTION__);
            goto error;
        }

        dtmf_duration = *((exe_vtd_t *)record_p->request_data_p);

        /* dtmf_duration is a global variable */
        exe_set_dtmf_duration(record_p->exe_p, dtmf_duration);

        /* Stop DTMF if started if the value assigned to VTD
         * is anything other than 10000000 (eternity).
         */
        if (10000000 != dtmf_duration && state_dtmf_start) {
            result = cn_request_dtmf_stop(cn_client_get_context(),
                                          request_record_get_client_tag(record_p));

            /* Clear the state regardless */
            state_dtmf_start = 0;

            if (CN_SUCCESS != result) {
                ATC_LOG_E("%s: cn_request_dtmf_stop failed!", __FUNCTION__);
                goto error;
            }

            record_p->state = EXE_STATE_RESPONSE;
            return EXE_PENDING;
        } else {
            /* We are done */
            return EXE_SUCCESS;
        }

    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("%s: unknown state!", __FUNCTION__);
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_get_dtmf_duration(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_vtd_t *dtmf_duration_res_p = record_p->request_data_p;
    ATC_LOG_I("%s -> request", __FUNCTION__);

    if (NULL == dtmf_duration_res_p) {
        ATC_LOG_E("request_get_dtmf_duration, dtmf_duration_res_p is NULL");
        goto error;
    }

    *dtmf_duration_res_p = exe_get_dtmf_duration(record_p->exe_p);

    record_p->response_data_p = (void *)dtmf_duration_res_p;
    return EXE_SUCCESS;
error:
    return EXE_FAILURE;
}


exe_request_result_t request_set_call_waiting(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    cn_call_waiting_t call_waiting;
    exe_error_response_t status = { .error = CMEE_UNKNOWN };

    if (EXE_STATE_REQUEST == record_p->state) {
        exe_call_waiting_request_t *request_ccwa_data_p = NULL;
        unsigned int data[2];

        ATC_LOG_I("-> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request data NULL");
            goto error;
        }

        request_ccwa_data_p = (exe_call_waiting_request_t *)record_p->request_data_p;

        call_waiting.setting = request_ccwa_data_p->mode;
        call_waiting.service_class = request_ccwa_data_p->requested_classes;

        result = cn_request_set_call_waiting(cn_client_get_context(), &call_waiting, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("failed!");

            if (result == CN_REQUEST_FDN_BLOCKED) {
                status.error = CMEE_FDN_BLOCKED;
            } else {
                status.error = CMEE_UNKNOWN;
            }

            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            if (record_p->client_result_code == CN_REQUEST_FDN_BLOCKED) {
                status.error = CMEE_FDN_BLOCKED;
            } else {
                status.error = CMEE_UNKNOWN;
            }

            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &status);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_query_call_waiting_status(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_error_response_t status = { .error = CMEE_UNKNOWN };

    if (EXE_STATE_REQUEST == record_p->state) {
        exe_call_waiting_request_t *request_ccwa_data_p = NULL;

        ATC_LOG_I("-> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request data NULL");
            goto error;
        }

        request_ccwa_data_p = (exe_call_waiting_request_t *)record_p->request_data_p;

        result = cn_request_get_call_waiting(cn_client_get_context(), request_ccwa_data_p->requested_classes, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("failed!");
            goto error;
        }

        record_p->client_data_p = calloc(1, sizeof(exe_call_waiting_request_t));

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("could not allocate record_p->client_data_p");
            goto error;
        }

        /* The requested classes are needed in the parser */
        ((exe_call_waiting_request_t *)record_p->client_data_p)->mode  = request_ccwa_data_p->mode;
        ((exe_call_waiting_request_t *)record_p->client_data_p)->requested_classes = request_ccwa_data_p->requested_classes;

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        cn_call_waiting_t *response_p = NULL;
        exe_call_waiting_response_t handler_response;
        exe_call_waiting_request_t *client_call_waiting_data_p = NULL;

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("failed! record_p->client_result_code = %d", record_p->client_result_code);

            if (record_p->client_result_code == CN_REQUEST_FDN_BLOCKED) {
                status.error = CMEE_FDN_BLOCKED;
            } else {
                status.error = CMEE_UNKNOWN;
            }

            goto error;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("record_p->response_data_p is NULL");
            status.error = CMEE_UNKNOWN;
            goto error;
        }

        response_p = (cn_call_waiting_t *) record_p->response_data_p;

        memset(&handler_response, 0, sizeof(handler_response));

        handler_response.enabled = response_p->setting;

        if (CN_CALL_WAITING_SERVICE_ENABLED == handler_response.enabled) {
            handler_response.classes_status = response_p->service_class;
        }

        if (NULL == record_p->client_data_p) {
            ATC_LOG_E("record_p->client_data_p is NULL");
            status.error = CMEE_UNKNOWN;
            goto error;
        }

        client_call_waiting_data_p = (exe_call_waiting_request_t *)record_p->client_data_p;
        handler_response.requested_classes = client_call_waiting_data_p->requested_classes;
        handler_response.mode = client_call_waiting_data_p->mode;

        free(record_p->client_data_p);
        record_p->client_data_p = NULL;

        ATC_LOG_I("<- response");
        exe_request_complete(record_p, EXE_SUCCESS, &handler_response);
        return EXE_SUCCESS;
    }

error:
    free(record_p->client_data_p);
    record_p->client_data_p = NULL;

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &status);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_change_barring_password(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_request_result_t exe_result = EXE_FAILURE;
    exe_change_pin_t handler_response = { .error_code = CMEE_UNKNOWN };

    if (EXE_STATE_REQUEST == record_p->state) {
        exe_change_pin_t *exe_change_pin_p = NULL;
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("%s: request data NULL", __FUNCTION__);
            goto error;
        }

        exe_change_pin_p = (exe_change_pin_t *)record_p->request_data_p;

        result = cn_request_change_barring_password(cn_client_get_context(),
                 exe_change_pin_p->facility_p,
                 exe_change_pin_p->old_passwd_p,
                 exe_change_pin_p->new_passwd_p,
                 request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_change_barring_password failed (result:%d)", result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s -> response", __FUNCTION__);
        memset(&handler_response, 0, sizeof(exe_change_pin_t));
        handler_response.error_code = CMEE_OK;

        if (CN_SUCCESS != record_p->client_result_code) {
            if (CN_REQUEST_NOT_ALLOWED == record_p->client_result_code) {
                handler_response.error_code = CMEE_OPERATION_NOT_ALLOWED;
            } else if (CN_REQUEST_INCORRECT_PASSWORD == record_p->client_result_code) {
                handler_response.error_code = CMEE_INCORRECT_PASSWORD;
            } else if (CN_REQUEST_FDN_BLOCKED == record_p->client_result_code) {
                handler_response.error_code = CMEE_FDN_BLOCKED;
            } else {
                ATC_LOG_E("cn_request_change_barring_password returned unexpected result code! (result:%d)", result);
                handler_response.error_code = CMEE_UNKNOWN;
            }

            goto error;
        }

        handler_response.error_code = CMEE_OK;

        exe_request_complete(record_p, EXE_SUCCESS, &handler_response);
        exe_result = EXE_SUCCESS;
    }

    return exe_result;

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &handler_response); /* handler_response carries CMEE error code */
    }

    return EXE_FAILURE;
}


exe_request_result_t request_set_query_barring(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    exe_request_result_t exe_result = EXE_FAILURE;
    exe_clck_t handler_response = { .status = CMEE_UNKNOWN };

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_clck_t *exe_clck_p = NULL;
        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_set_query_barring, request data NULL");
            goto error;
        }

        exe_clck_p = (exe_clck_t *)record_p->request_data_p;

        if (EXE_CLCK_MODE_QUERY_STATUS == exe_clck_p->mode) {
            result = cn_request_query_call_barring(cn_client_get_context(),
                                                   exe_clck_p->facility_p,
                                                   exe_clck_p->class,
                                                   request_record_get_client_tag(record_p));
        } else {
            cn_call_barring_t call_barring;

            call_barring.setting = (EXE_CLCK_MODE_UNLOCK == exe_clck_p->mode) ? CN_CALL_BARRING_DEACTIVATION : CN_CALL_BARRING_ACTIVATION;
            call_barring.service_class = exe_clck_p->class;

            result = cn_request_set_call_barring(cn_client_get_context(),
                                                 exe_clck_p->facility_p,
                                                 &call_barring,
                                                 exe_clck_p->passwd_p,
                                                 request_record_get_client_tag(record_p));
        }

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_query/set_call_barring failed (result:%d)", result);
            goto error;
        }

        record_p->client_data_p = (void *)exe_clck_p->mode;  /* so the response part can be sure of mode */
        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }

    case EXE_STATE_RESPONSE: {
        exe_clck_mode_t mode = (exe_clck_mode_t)(record_p->client_data_p);

        if (true == record_p->abort) {
            ATC_LOG_I("-> aborting");
            /* NOTE: there is no support in MAL for aborting this type of request */
            return EXE_SUCCESS;
        }

        ATC_LOG_I("%s -> response", __FUNCTION__);

        if (CN_SUCCESS == record_p->client_result_code) {
            handler_response.status = CMEE_OK;

            /* If there is response data the we have a QUERY response, otherwise it is ACTIVATE/DEACTIVATE responses*/
            if (NULL != record_p->response_data_p) {
                cn_call_barring_t *call_barring_p = (cn_call_barring_t *)(record_p->response_data_p);

                handler_response.class = (unsigned char)call_barring_p->service_class;
                handler_response.lock_status = (exe_clck_status_t)call_barring_p->setting;
                exe_request_complete(record_p, EXE_SUCCESS, &handler_response);
                exe_result = EXE_SUCCESS;

            } else if (EXE_CLCK_MODE_QUERY_STATUS == mode) {
                /* we have no data from MAL but it was a QUERY, so something went wrong */
                goto error;
            } else {
                exe_request_complete(record_p, EXE_SUCCESS, NULL);
                exe_result = EXE_SUCCESS;
            }
        } else {
            if (CN_REQUEST_NOT_ALLOWED == record_p->client_result_code) {
                handler_response.status = CMEE_OPERATION_NOT_ALLOWED;
            } else if (CN_REQUEST_INCORRECT_PASSWORD == record_p->client_result_code) {
                handler_response.status = CMEE_INCORRECT_PASSWORD;
            } else if (CN_REQUEST_FDN_BLOCKED == record_p->client_result_code) {
                handler_response.status = CMEE_FDN_BLOCKED;
            } else {
                ATC_LOG_E("cn_request_change_barring_password returned unexpected result code! (result:%d)", result);
                handler_response.status = CMEE_UNKNOWN;
            }

            goto error;
        }

        return exe_result;
    }

    default:
        ATC_LOG_E("unknown state=%d!", record_p->state);
        goto error;
    }


error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &handler_response); /* handler_response carries CMEE error code */
    }

    return EXE_FAILURE;
}

exe_request_result_t request_cnap_read(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    int m;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_cnap_status(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_cnap_status failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        if (NULL != record_p->response_data_p) {
            m = *((cn_cnap_status_t *)record_p->response_data_p);
        } else {
            ATC_LOG_E("response data is NULL");
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &m);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_colr_do(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;
    int m;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);
        result = cn_request_colr_status(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_colr_status failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (NULL != record_p->response_data_p) {
            m = *((cn_colr_status_t *) record_p->response_data_p);
        } else {
            ATC_LOG_E("response data is NULL");
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &m);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


exe_request_result_t request_signal_info_reporting_set(exe_request_record_t *record_p)
{
    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_error_code_t result = CN_FAILURE;
        exe_cmer_data_t *data_p = NULL;
        cn_rssi_mode_t mode;

        ATC_LOG_I("-> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request data is NULL");
            goto error;
        }

        data_p = (exe_cmer_data_t *) record_p->request_data_p;
        mode = (data_p->ind == 1) ? CN_RSSI_EVENT_REPORTING_ENABLE : CN_RSSI_EVENT_REPORTING_DISABLE;

        result = cn_request_set_signal_info_reporting(cn_client_get_context(), mode, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("request failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("unsuccessful! result:%d", record_p->client_result_code);
            goto error;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}

exe_request_result_t request_user_activity_set(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("%s: record_p is NULL", __FUNCTION__);
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_user_status_t user_status;

        ATC_LOG_I("%s -> request", __FUNCTION__);

        if (!record_p->request_data_p) {
            ATC_LOG_E("%s: no request data!", __FUNCTION__);
            goto error;
        }

        user_status = *((unsigned char *)(record_p->request_data_p));

        result = cn_request_set_user_activity_status(cn_client_get_context(), user_status, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_set_user_activity_status failed!", __FUNCTION__);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: cn_request_set_user_activity_status failed!", __FUNCTION__);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("%s: unknown state!", __FUNCTION__);
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}

static exe_request_result_t request_reg_status_event_config(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_reg_status_trigger_level_t trigger_level;

        ATC_LOG_I("-> request");

        if (!record_p->request_data_p) {
            ATC_LOG_E("no request data!");
            goto error;
        }

        trigger_level = (cn_reg_status_trigger_level_t) * ((int *)(record_p->request_data_p));

        result = cn_request_reg_status_event_config(cn_client_get_context(), trigger_level, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_reg_status_event_config failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_response_reg_status_event_config returns %d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_rat_name_ind_reporting(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_ecrat_set_t *ecrat_p = NULL;
        cn_event_reporting_type_t type;
        cn_bool_t enable_reporting;

        ATC_LOG_I("-> request");

        if (!record_p->request_data_p) {
            ATC_LOG_E("no request data!");
            goto error;
        }

        ecrat_p = (exe_ecrat_set_t *)(record_p->request_data_p);

        type = CN_EVENT_REPORTING_TYPE_RAT_NAME;
        enable_reporting = ecrat_p->n;

        result = cn_request_set_event_reporting(cn_client_get_context(), type, enable_reporting, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_set_event_reporting failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_set_event_reporting returns %d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_nitz_name_ind_reporting(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_ennir_set_t *ennir_p = NULL;
        cn_event_reporting_type_t type;
        cn_bool_t enable_reporting;

        ATC_LOG_I("-> request");

        if (!record_p->request_data_p) {
            ATC_LOG_E("no request data!");
            goto error;
        }

        ennir_p = (exe_ennir_set_t *)(record_p->request_data_p);

        type = CN_EVENT_REPORTING_TYPE_NETWORK_INFO;
        enable_reporting = ennir_p->n;

        result = cn_request_set_event_reporting(cn_client_get_context(), type, enable_reporting, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_set_event_reporting failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_set_event_reporting returns %d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);

error:
    return EXE_FAILURE;
}



exe_request_result_t request_rat_name(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("-> request");

        result = cn_request_rat_name(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_set_event_reporting failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        cn_rat_name_t *rat_name_p = (cn_rat_name_t *)record_p->response_data_p;
        exe_ecrat_read_t ecrat;

        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_set_event_reporting returns %d", record_p->client_result_code);
            goto error_cb;
        }

        if (!record_p->response_data_p) {
            ATC_LOG_E("record_p->response_data_p is NULL!");
            goto error_cb;
        }

        ecrat.rat = (exe_rat_name_t) * rat_name_p;

        exe_request_complete(record_p, EXE_SUCCESS, &ecrat);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_als_set(exe_request_record_t *record_p)
{
    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_modem_property_t property;
        cn_uint8_t value = 0;
        cn_error_code_t result = CN_FAILURE;

        ATC_LOG_I("-> request");

        if (record_p->request_data_p == NULL) {
            ATC_LOG_E("request data is NULL");
            goto error;
        }

        property.type = CN_MODEM_PROPERTY_TYPE_ALS;

        if (EXE_LINE_TWO == ((exe_line_request_response_data_t *)record_p->request_data_p)->line) {
            property.value.als.current_line = CN_ALS_LINE_TWO;
        } else {
            property.value.als.current_line = CN_ALS_LINE_NORMAL;
        }

        result = cn_request_set_modem_property(cn_client_get_context(),
                                               property, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("request failed");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("unsuccessful result:%d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}


exe_request_result_t request_als_read(exe_request_record_t *record_p)
{
    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_error_code_t result = CN_FAILURE;

        ATC_LOG_I("-> request");

        result = cn_request_get_modem_property(cn_client_get_context(),
                                               CN_MODEM_PROPERTY_TYPE_ALS, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("request failed");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        exe_line_request_response_data_t response;
        cn_modem_property_t *property_p = NULL;

        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("unsuccessful result:%d", record_p->client_result_code);
            goto error_cb;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("response data is NULL");
            goto error_cb;
        }

        property_p = (cn_modem_property_t *) record_p->response_data_p;

        if (CN_MODEM_PROPERTY_TYPE_ALS != property_p->type) {
            ATC_LOG_E("invalid response type");
            goto error_cb;
        }

        if (CN_ALS_LINE_TWO == property_p->value.als.current_line) {
            response.current_line = EXE_LINE_TWO;
        } else {
            response.current_line = EXE_LINE_NORMAL;
        }

        if (CN_SUPPORTED_ALS_LINE1_AND_LINE2 == property_p->value.als.supported_lines) {
            response.supported_lines = EXE_LINE_SUPPORT;
        } else {
            response.supported_lines = EXE_LINE_NO_SUPPORT;
        }

        exe_request_complete(record_p, EXE_SUCCESS, &response);

        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}


exe_request_result_t request_modem_shutdown(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        ATC_LOG_I("-> request");

        result = cn_request_modem_power_off(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_modem_power_off failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_request_modem_power_off returns %d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}


exe_request_result_t request_send_tx_back_off_event(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_epwrred_t *epwrred_p = (exe_epwrred_t *)record_p->request_data_p;
        cn_tx_back_off_event_t event;

        if (!epwrred_p) {
            ATC_LOG_E("record_p->request_data_p is NULL!");
            goto error;
        }

        ATC_LOG_I("-> request");

        event = (cn_tx_back_off_event_t)epwrred_p->event;
        result = cn_request_send_tx_back_off_event(cn_client_get_context(), event, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("cn_request_send_tx_back_off_event failed!");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("cn_response_send_tx_back_off_event returns %d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default:
        ATC_LOG_E("unknown state! (%d)", record_p->state);
        goto error_cb;
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}

exe_request_result_t request_set_empage(exe_request_record_t *record_p)
{

    if (!record_p) {
        ATC_LOG_E("record_p is NULL!");
        goto error;
    }

#ifdef ENABLE_FTD

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_error_code_t      result         = CN_FAILURE;
        exe_empage_t        *request_data_p = NULL;
        cn_empage_t          empage;

        ATC_LOG_I("-> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request data is NULL");
            goto error;
        }

        request_data_p = (exe_empage_t *) record_p->request_data_p;
        empage.mode    = request_data_p->mode;
        empage.page    = request_data_p->page;
        empage.timer   = request_data_p->timer;

        result = cn_request_set_empage(cn_client_get_context(), &empage, request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("request failed");
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("<- response");

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("unsuccessful result:%d", record_p->client_result_code);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, NULL);
        return EXE_SUCCESS;
    }
    default: {
        ATC_LOG_E("unknown state!");
        goto error_cb;
    }
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;
#endif
error:
    return EXE_FAILURE;
}


/**
 * request_read_imeisv
 * Reads out the PP flag value that pepresents IMEISV.
 *
 */
exe_request_result_t request_read_imeisv(exe_request_record_t *record_p)
{
    cn_error_code_t result = 0;

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_I("%s -> request", __FUNCTION__);

        result = cn_request_get_pp_flags(cn_client_get_context(), false, EXE_FLAG_ID_IMEISV,
                                            request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: cn_request_get_pp_flags request failed, result=%d!", __FUNCTION__, result);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        cn_pp_flag_list_t *flag_list_p = (cn_pp_flag_list_t *)record_p->response_data_p;
        exe_esvn_response_t response;
        cn_pp_flag_t *flag_p = NULL;

        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: failed! record_p->client_result_code = %d", __FUNCTION__, record_p->client_result_code);
            goto error;
        }

        flag_p = (cn_pp_flag_t *)&flag_list_p->info[0];
        EXE_CHECK_GOTO_ERROR(flag_p->pp_feature == EXE_FLAG_ID_IMEISV);
        EXE_CHECK_GOTO_ERROR(flag_p->pp_value < 100);

        response.imeisv = (uint8_t)(flag_p->pp_value);

        exe_request_complete(record_p, EXE_SUCCESS, &response);
        return EXE_SUCCESS;
    }

error:
    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }
    return EXE_FAILURE;
}

exe_request_result_t request_radiover_read(exe_request_record_t *record_p)
{
    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        cn_error_code_t result = CN_FAILURE;

        ATC_LOG_I("%s -> request", __FUNCTION__);

        result = cn_request_baseband_version(cn_client_get_context(), request_record_get_client_tag(record_p));

        if (CN_SUCCESS != result) {
            ATC_LOG_E("%s: request failed", __FUNCTION__);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    }
    case EXE_STATE_RESPONSE: {
        ATC_LOG_I("%s <- response", __FUNCTION__);

        if (CN_SUCCESS != record_p->client_result_code) {
            ATC_LOG_E("%s: unsuccessful result:%d", __FUNCTION__, record_p->client_result_code);
            goto error_cb;
        }

        if (NULL == record_p->response_data_p) {
            ATC_LOG_E("%s: response data is NULL", __FUNCTION__);
            goto error_cb;
        }

        exe_request_complete(record_p, EXE_SUCCESS, record_p->response_data_p);

        return EXE_SUCCESS;
    }
    default: {
        ATC_LOG_E("%s: unknown state!", __FUNCTION__);
        goto error_cb;
    }
    }

error_cb:
    exe_request_complete(record_p, EXE_FAILURE, NULL);
    return EXE_FAILURE;

error:
    return EXE_FAILURE;
}
