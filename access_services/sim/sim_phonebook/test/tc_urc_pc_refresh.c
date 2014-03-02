/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description     : simpb test sim urc pc refresh ind
 *
 * Author          : Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *
 */

#include <test.h>
#include <tc.h>
#include <simpb.h>
#include <simpbd_common.h>
#include "sim_stub/sim_response_data.h"
#include <simpbd_p.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* callback parameters */
extern ste_simpb_callback_t ste_simpb_callback_data;

#define SIMPBD_PBR_UINT8_USIM_GLOBAL_PATH {0x3f,0x00,0x7F,0x10,0x5F,0x3A,0x4F,0x30}

simpb_test_state_type_t test_unsolicted_events_CAT_PC_REFRESH_INDs(int state, ste_simpb_t **ste_simpb_pp)
{
    void *data_p;
    int size;
    uintptr_t client_ct = 0; /* Unsolicited event */
    uintptr_t ct = 144; /* Unsolicited event */
    uintptr_t simpbd_ct = 0;
    ste_simpb_t *ste_simpb_p = *ste_simpb_pp;

    /* STE_CAT_CAUSE_PC_REFRESH_FILE_IND */

    /* read PBR record 1. sim_pbr_data */

    /* We have no initial CLIENT ACTION here as this is an unsolicited event from SIMd */

    { /* SIMd ACTION */
        ste_cat_pc_refresh_file_ind_t event_pc_refresh_file_ind;
        simpbd_test_ste_sim_read_resp_t *event_p;
        int size = 0;
        int size_data = 0;
        uint8_t path_data[] = SIMPBD_PBR_UINT8_USIM_GLOBAL_PATH;

        /* need to send event */

        size_data = sizeof(ste_cat_pc_refresh_file_ind_t);
        memcpy(&event_pc_refresh_file_ind.path.path, path_data, 8);
        event_pc_refresh_file_ind.path.pathlen = 8;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        event_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        event_p->size = size_data;
        memcpy(event_p->data, (void *)&event_pc_refresh_file_ind, size_data);

        event_p->retval = UICC_REQUEST_STATUS_OK;
        event_p->cause = STE_CAT_CAUSE_PC_REFRESH_FILE_IND;
        event_p->client_tag = simpbd_ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_SIM_URC, event_p, size));

        free(event_p);
    }

    TESTCHECK(test_handle_pbr_caching(0, &ste_simpb_p));

    /* STE_CAT_CAUSE_PC_REFRESH_IND */

    { /* SIMd ACTION */
        ste_cat_pc_refresh_ind_t event_pc_refresh_ind;
        simpbd_test_ste_sim_read_resp_t *event_p;
        int size = 0;
        int size_data = 0;
        uint8_t path_data[] = SIMPBD_PBR_UINT8_USIM_GLOBAL_PATH;

        /* need to send event */

        size_data = sizeof(ste_cat_pc_refresh_ind_t);
        event_pc_refresh_ind.type = STE_SIM_PC_REFRESH_UICC_RESET; /* SIMPBD will trigger on any value */
        event_pc_refresh_ind.simd_tag = 0;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        event_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        event_p->size = size_data;
        memcpy(event_p->data, (void *)&event_pc_refresh_ind, size_data);

        event_p->retval = UICC_REQUEST_STATUS_OK;
        event_p->cause = STE_CAT_CAUSE_PC_REFRESH_IND;
        event_p->client_tag = simpbd_ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_SIM_URC, event_p, size));

        free(event_p);
    }

    TESTCHECK(test_handle_pbr_caching(0, &ste_simpb_p));


    /* No CLIENT ACTION as this URC from SIM is handled by SIMPBD in the background */


    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}


/* utility to handle the chaching of PBR */


simpb_test_state_type_t test_handle_pbr_caching(int state, ste_simpb_t **ste_simpb_pp)
{
    void *data_p;
    int size;
    uintptr_t client_ct = 0; /* Unsolicited event */
    uintptr_t ct = 144; /* Unsolicited event */
    uintptr_t simpbd_ct = 0;
    ste_simpb_t *ste_simpb_p = *ste_simpb_pp;

/* Role: SIM server serving SIMPBs requests*/
/* wait for stub socket to send us the get card status request from pb */
    {
        simpbd_test_ste_uicc_sim_card_status_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_sim_card_status_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_CARD_STATUS, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_sim_card_status_response_t);
        rdata_p = (ste_uicc_sim_card_status_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->card_type = SIM_CARD_TYPE_UICC;
        rdata_p->status = STE_UICC_STATUS_CODE_OK;
        rdata_p->num_apps = 1;
        rdata_p->card_state = STE_UICC_CARD_STATE_READY;
        rdata_p->upin_state = STE_UICC_PIN_STATE_ENABLED_VERIFIED;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_CARD_STATUS;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_CARD_STATUS, response_p, size));
        free(rdata_p);
        free(response_p);
    }

    /* wait for stub socket to send us the get app info request from pb */
    {
        simpbd_test_ste_uicc_get_app_info_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_sim_app_info_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_GET_APP_INFO, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_sim_app_info_response_t);
        rdata_p = (ste_uicc_sim_app_info_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->app_type = SIM_APP_USIM;
        rdata_p->status = STE_UICC_STATUS_CODE_OK;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_APP_INFO;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_GET_APP_INFO, response_p, size));
        free(response_p);
    }

    /* wait for stub socket to send us the get ste_uicc_sim_get_file_information request from pb  GLOBAL phonebook */
    {
        simpbd_test_ste_uicc_sim_file_get_information_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_get_file_information_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_get_file_information_response_t) + sim_fcp_data.length;
        rdata_p = (ste_uicc_get_file_information_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        rdata_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        rdata_p->status_word.sw1 = sim_fcp_data.sw1;
        rdata_p->status_word.sw2 = sim_fcp_data.sw2;;
        rdata_p->fcp = 0;
        memcpy((uint8_t *)(&(rdata_p->length) + 1), sim_file_information_data[0].data, sim_file_information_data[0].length);
        rdata_p->length = sim_file_information_data[0].length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, response_p, size));
        free(response_p);
    }

    /* wait for stub socket to send us the get ste_uicc_sim_get_file_information request from pb LOCAL phonebook */
    {
        simpbd_test_ste_uicc_sim_file_get_information_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_get_file_information_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;

        size_data = sizeof(ste_uicc_get_file_information_response_t) + sim_fcp_data.length;
        rdata_p = (ste_uicc_get_file_information_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        rdata_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        rdata_p->status_word.sw1 = sim_fcp_data.sw1;
        rdata_p->status_word.sw2 = sim_fcp_data.sw2;;
        rdata_p->fcp = 0;
        memcpy((uint8_t *)(&(rdata_p->length) + 1), sim_file_information_data[0].data, sim_file_information_data[0].length);
        rdata_p->length = sim_file_information_data[0].length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, response_p, size));
        free(response_p);
    }

    /* wait for stub socket to send us the get pbr info request from pb STE_SIMPB_REQUEST_ID_CACHE_PBR*/
    {
        simpbd_test_ste_uicc_sim_file_read_record_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p = NULL;
        ste_uicc_sim_file_read_record_response_t *data_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, &data_p, &size))

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;
        /* need to send the response back */

        size_data = sizeof(ste_uicc_sim_file_read_record_response_t) + sim_pbr_data.length;
        data_p = (ste_uicc_sim_file_read_record_response_t *)calloc(size_data, sizeof(uint8_t));
        data_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        data_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        data_p->status_word.sw1 = sim_pbr_data.sw1;
        data_p->status_word.sw2 = sim_pbr_data.sw2;
        data_p->data = 0;  /* 0 address as data is piggy-backed in packet */
        memcpy((uint8_t *)(&(data_p->length) + 1), sim_pbr_data.data, sim_pbr_data.length);
        data_p->length = sim_pbr_data.length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        response_p->size = size_data;
        memcpy(response_p->data, (void *)data_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, response_p, size));
    }


    /* wait for stub socket to send us the get pbr info request from pb STE_SIMPB_REQUEST_ID_CACHE_PBR*/
    {
        simpbd_test_ste_uicc_sim_file_read_record_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p = NULL;
        ste_uicc_sim_file_read_record_response_t *data_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, &data_p, &size))

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;
        /* need to send the response back */

        size_data = sizeof(ste_uicc_sim_file_read_record_response_t) + sim_pbr_data.length;
        data_p = (ste_uicc_sim_file_read_record_response_t *)calloc(size_data, sizeof(uint8_t));
        data_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        data_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        data_p->status_word.sw1 = sim_pbr_data.sw1;
        data_p->status_word.sw2 = sim_pbr_data.sw2;
        data_p->data = 0;  /* 0 address as data is piggy-backed in packet */
        memcpy((uint8_t *)(&(data_p->length) + 1), sim_pbr_data.data, sim_pbr_data.length);
        data_p->length = sim_pbr_data.length;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        response_p->size = size_data;
        memcpy(response_p->data, (void *)data_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, response_p, size));
    }

    /* wait for stub socket to send us the get pbr info request from pb STE_SIMPB_REQUEST_ID_CACHE_PBR*/
    {
        simpbd_test_ste_uicc_sim_file_read_record_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p = NULL;
        ste_uicc_sim_file_read_record_response_t *data_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, &data_p, &size))

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        ct = req.client_tag;
        /* need to send the response back */

        size_data = sizeof(ste_uicc_sim_file_read_record_response_t) + sim_pbr_data.length;
        data_p = (ste_uicc_sim_file_read_record_response_t *)calloc(size_data, sizeof(uint8_t));
        data_p->uicc_status_code = STE_UICC_STATUS_CODE_FAIL;
        data_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        data_p->status_word.sw1 = 0x63;
        data_p->status_word.sw2 = 0x40;
        data_p->data = 0;  /* 0 address as data is piggy-backed in packet */
        data_p->length = 0;

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        response_p->size = size_data;
        memcpy(response_p->data, (void *)data_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
        response_p->client_tag = ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_READ_RECORD, response_p, size));
    }

    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}

