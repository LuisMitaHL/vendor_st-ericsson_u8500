/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cn_rat_control.h"
#include "cn_log.h"
#include "cn_macros.h"

#include "sim.h"
#include "sim_client.h"

#include <stdlib.h>

/* request states */
#define REQUEST_STATE_GET_SIM_RAT_RESPONSE           1005
#define REQUEST_STATE_SET_MODEM_RAT_RESPONSE         1006
#define REQUEST_STATE_SET_MODEM_RF_ON_RESPONSE       1007

request_status_t handle_request_update_rat_from_sim(void *data_p, request_record_t *record_p);

int update_rat_from_sim()
{
#ifdef ENABLE_RAT_BALANCING
    request_record_t *record_p = NULL;
    request_status_t status = 0;

    record_p = request_record_create(CN_REQUEST_SET_MANDATED_NETWORK_TYPE_FROM_SIM, 0, 0);

    if (!record_p) {
        CN_LOG_E("Not possible to create request record!");

        return -1;
    }

    record_p->state = REQUEST_STATE_REQUEST;
    record_p->request_handler_p = handle_request_update_rat_from_sim;
    status = handle_request_update_rat_from_sim(NULL, record_p);

    if (REQUEST_STATUS_PENDING != status) {
        CN_LOG_E("Unexpected status %d!", status);
        request_record_free(record_p);
        return -1;
    }

#endif

    return 0;
}


request_status_t handle_request_update_rat_from_sim(
    void *data_p,
    request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;

    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        uicc_request_status_t uicc_status = UICC_REQUEST_STATUS_FAILED;
        CN_LOG_D("ste_uicc_sim_read_preferred_RAT_setting -> request");

        record_p->request_data_p = data_p;

        uicc_status = ste_uicc_sim_read_preferred_RAT_setting(sim_client_get_handle(),
                      (uintptr_t)request_record_get_modem_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != uicc_status) {
            CN_LOG_E("ste_uicc_sim_read_preferred_RAT_setting failed! status=%d", uicc_status);
            goto error;
        }

        record_p->state = REQUEST_STATE_GET_SIM_RAT_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_GET_SIM_RAT_RESPONSE: {
        cn_request_set_preferred_network_type_t *data_p = NULL;
        ste_uicc_sim_read_preferred_RAT_setting_response_t *response_data_p =
            (ste_uicc_sim_read_preferred_RAT_setting_response_t *)record_p->response_data_p;
        CN_LOG_D("ste_uicc_sim_read_preferred_RAT_setting <- response");

        if (NULL == response_data_p) {
            CN_LOG_E("ste_uicc_sim_read_preferred_RAT_setting response NULL");
            goto error;
        }

        if (STE_UICC_STATUS_CODE_OK != response_data_p->uicc_status_code) {
            CN_LOG_E("ste_uicc_sim_read_preferred_RAT_setting response status %d details %d", response_data_p->uicc_status_code, response_data_p->uicc_status_code_fail_details);
            goto error;
        }

        data_p = calloc(1, sizeof(cn_request_set_preferred_network_type_t));

        if (!data_p) {
            CN_LOG_E("calloc failed for data_p");
            goto error;
        }

        switch (response_data_p->RAT) {
        case STE_UICC_SIM_RAT_SETTING_DUAL_MODE:
            data_p->type = CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE;
            break;
        case STE_UICC_SIM_RAT_SETTING_2G_ONLY:
            data_p->type = CN_NETWORK_TYPE_GSM_ONLY;
            break;
        case STE_UICC_SIM_RAT_SETTING_3G_ONLY:
            data_p->type = CN_NETWORK_TYPE_WCDMA_ONLY;
            break;
        case STE_UICC_SIM_RAT_SETTING_NO_PREFERENCES:
            CN_LOG_D("SIM has no preferred RAT setting");
            free(data_p);
            request_record_free(record_p);

            return REQUEST_STATUS_DONE;
        case STE_UICC_SIM_RAT_SETTING_ENABLE_ALL_SUPPORTED_RATS:
        default:
            data_p->type = CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE;
            break;
        }

        CN_LOG_D("Setting RAT mode from SIM = 0x%x", data_p->type);
        CN_LOG_D("handle_request_set_preferred_network_type -> request");

        record_p->state = REQUEST_STATE_REQUEST;
        record_p->request_handler_p = handle_request_set_preferred_network_type;
        status = handle_request_set_preferred_network_type((void *)data_p, record_p);

        free(data_p);

        return status;
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        goto error;
    }
    }

error:
    return status;
}


request_status_t cn_rat_control_handle_request(
    void *data_p,
    request_record_t *record_p)
{
    request_status_t status = REQUEST_STATUS_ERROR;


    REQUIRE_VALID_RECORD(record_p);

    switch (record_p->state) {
    case REQUEST_STATE_REQUEST: {
        uicc_request_status_t status;
        cn_request_set_preferred_network_type_t *network_type_p = NULL;
        CN_LOG_D("ste_uicc_sim_read_preferred_RAT_setting -> request");

        network_type_p = calloc(1, sizeof(cn_request_set_preferred_network_type_t));

        if (!network_type_p) {
            CN_LOG_E("calloc failed for network_type_p");
            goto error;
        }

        /* Save requested network type. */
        network_type_p->type = ((cn_request_set_preferred_network_type_t *) data_p)->type;
        record_p->request_data_p = network_type_p;

        status = ste_uicc_sim_read_preferred_RAT_setting(sim_client_get_handle(),
                 (uintptr_t)request_record_get_modem_tag(record_p));

        if (UICC_REQUEST_STATUS_OK != status) {
            CN_LOG_E("ste_uicc_sim_read_preferred_RAT_setting failed! status=%d", status);
            send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        record_p->state = REQUEST_STATE_GET_SIM_RAT_RESPONSE;

        return REQUEST_STATUS_PENDING;
    }
    case REQUEST_STATE_GET_SIM_RAT_RESPONSE: {
        cn_request_set_preferred_network_type_t *request_data_p =
            (cn_request_set_preferred_network_type_t *)record_p->request_data_p;
        ste_uicc_sim_read_preferred_RAT_setting_response_t *response_data_p =
            (ste_uicc_sim_read_preferred_RAT_setting_response_t *)record_p->response_data_p;
        CN_LOG_D("ste_uicc_sim_read_preferred_RAT_setting <- response");

        if (NULL == response_data_p) {
            CN_LOG_E("ste_uicc_sim_read_preferred_RAT_setting response NULL");
            send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (STE_UICC_STATUS_CODE_OK != response_data_p->uicc_status_code) {
            CN_LOG_E("ste_uicc_sim_read_preferred_RAT_setting response status %d details %d", response_data_p->uicc_status_code, response_data_p->uicc_status_code_fail_details);
            send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
            goto error;
        }

        if (STE_UICC_SIM_RAT_SETTING_NO_PREFERENCES == response_data_p->RAT ||
                STE_UICC_SIM_RAT_SETTING_ENABLE_ALL_SUPPORTED_RATS == response_data_p->RAT ||
                (STE_UICC_SIM_RAT_SETTING_DUAL_MODE == response_data_p->RAT && CN_NETWORK_TYPE_GSM_WCDMA_AUTO_MODE == request_data_p->type) ||
                (STE_UICC_SIM_RAT_SETTING_2G_ONLY == response_data_p->RAT && CN_NETWORK_TYPE_GSM_ONLY == request_data_p->type) ||
                (STE_UICC_SIM_RAT_SETTING_3G_ONLY == response_data_p->RAT && CN_NETWORK_TYPE_WCDMA_ONLY == request_data_p->type)) {
            /* SIM have no preferred RAT set rat requested by client, or the requested RAT matches those that are allowed. */
            CN_LOG_D("handle_request_set_preferred_network_type -> request");

            record_p->state = REQUEST_STATE_REQUEST;
            record_p->request_handler_p = handle_request_set_preferred_network_type;
            status = handle_request_set_preferred_network_type((void *)request_data_p, record_p);

            free(record_p->request_data_p);

            return status;
        } else {
            CN_LOG_E("mal_gss_set_preferred_network_type failed, not allowed by SIM");
            send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_REQUEST_SIM_RAT_REJECTED, record_p->client_tag, 0, NULL);
            goto error;
        }
    }
    default: {
        CN_LOG_E("record_p->state has an unknown value!");
        send_response(record_p->client_id, CN_RESPONSE_SET_PREFERRED_NETWORK_TYPE, CN_FAILURE, record_p->client_tag, 0, NULL);
        goto error;
    }
    }

error:

    if (NULL != record_p->request_data_p) {
        free(record_p->request_data_p);
    }

    return status;
}

