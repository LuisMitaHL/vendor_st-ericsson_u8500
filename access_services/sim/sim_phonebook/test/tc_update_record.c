/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: READ Record testcase file
 *
 * Author: Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simpb.h"
#include "simpbd_common.h"
#include "sim_stub/sim_response_data.h"
#include "simpbd_p.h"
#include "test.h"
#include "tc.h"

/* callback parameters */
extern ste_simpb_callback_t ste_simpb_callback_data;

static char update_input_string[]="Kalle\0";


simpb_test_state_type_t test_update_record(int state, ste_simpb_t **ste_simpb_pp)
{
    ste_simpb_status_t status;
    ste_sim_closure_t closure;
    int stub_fd = 0;
    int fd;
    void *data_p;
    int size;
    uintptr_t client_ct = 144;
    uintptr_t simpbd_ct = 0;
    ste_simpb_t *ste_simpb_p = *ste_simpb_pp;

    /* read PBR record 1. sim_pbr_data */

    { /* CLIENT ACTION */
        ste_simpb_file_info_t file_info;
        memset(&file_info, 0, sizeof(ste_simpb_file_info_t));
        file_info.slice = 1;
        file_info.record_number = 1;
        file_info.instance = 0;
        file_info.length = 10;
        file_info.p2 = 4;

        TESTSIMPB(ste_simpb_update(ste_simpb_p, client_ct, &file_info, update_input_string, strlen(update_input_string)));
    }

    { /* SIMd ACTION */
        simpbd_test_ste_uicc_sim_file_update_record_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_update_sim_file_record_response_t *sim_data_p;
        int size = 0;
        int size_data = 0;

        response_p = (simpbd_test_ste_uicc_sim_file_update_record_resp_t *)calloc(sizeof(simpbd_test_ste_uicc_sim_file_update_record_resp_t), sizeof(uint8_t));

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_UPDATE_RECORD, &data_p, &size));
        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        simpbd_ct = req.client_tag;

        if (!strncmp(update_input_string, req.data, strlen(update_input_string))) {
            printf("[%d] [%s] [%s] Something is WRONG! UPDATE RECORD -> strncmp(update_input_string, req.data, strlen(update_input_string) != 0\n",
                    getpid(), __FILE__, __FUNCTION__);
        }

        /* need to send the response back */

        sim_data_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        sim_data_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        sim_data_p->status_word.sw1 = 0x90;
        sim_data_p->status_word.sw2 = 0x00;
        size_data = sizeof(ste_uicc_update_sim_file_record_response_t);

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
        response_p->size = size_data;
        memcpy(response_p->data, (void *)sim_data_p, size_data);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD;
        response_p->client_tag = simpbd_ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_UPDATE_RECORD, response_p, size));

        free(response_p);
    }

    {
        /* CLIENT ACTION Data returning*/

        ste_simpb_cb_update_data_t *update_cb_data_p = NULL;
        int data_size = 0;
        int i = 0;
        int fd = ste_simpb_fd_get(ste_simpb_p);

        waitForEvent(1000, fd);
        TESTSIMPB(ste_simbp_receive(ste_simpb_p));

        update_cb_data_p = (ste_simpb_cb_update_data_t *)(ste_simpb_callback_data.cb_data_p);

        if (0 == update_cb_data_p->sw1) { /* try once more if data came but was empty */
            TESTSIMPB(ste_simbp_receive(ste_simpb_p));
        }

        update_cb_data_p = (ste_simpb_cb_update_data_t *)(ste_simpb_callback_data.cb_data_p);

        printf("[%d] [%s] [%s] sw1=0x%02X\n", getpid(), __FILE__, __FUNCTION__, update_cb_data_p->sw1);
        printf("[%d] [%s] [%s] sw2=0x%02X\n", getpid(), __FILE__, __FUNCTION__, update_cb_data_p->sw2);

        TESTSIMPB(!(0x90 == update_cb_data_p->sw1)); /* note the reverse logical notation. Comparison is to match STE_SIMPB_SUCCESS */
        TESTSIMPB(!(0x00 == update_cb_data_p->sw2));
    }

    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}
