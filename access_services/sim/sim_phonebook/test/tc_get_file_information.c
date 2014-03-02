/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
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


simpb_test_state_type_t test_get_file_info(int state, ste_simpb_t **ste_simpb_pp)
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
        file_info.phonebook_type = STE_SIMPB_PHONEBOOK_TYPE_GLOBAL;
        file_info.file_type = STE_SIMPB_FILE_TYPE_PBR;
        file_info.slice = 1;
        file_info.record_number = 1;
        file_info.instance = 0;
        file_info.length = 0;
        file_info.p2 = 4;

        TESTSIMPB(ste_simpb_file_information_get(ste_simpb_p, client_ct, &file_info));
    }

    { /* SIMd ACTION */
        simpbd_test_ste_uicc_sim_file_get_information_req_t req;
        simpbd_test_ste_sim_read_resp_t *response_p;
        ste_uicc_get_file_information_response_t *rdata_p = NULL;
        int size = 0;
        int size_data = 0;

        TESTCHECK(getStubEvent(1000, SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, &data_p, &size));

        memcpy(&req, data_p, sizeof(req));
        free(data_p);
        simpbd_ct = req.client_tag;


        size_data = sizeof(ste_uicc_get_file_information_response_t) + sim_fcp_data.length;
        rdata_p = (ste_uicc_get_file_information_response_t *)calloc(size_data, sizeof(uint8_t));
        rdata_p->uicc_status_code = STE_UICC_STATUS_CODE_OK;
        rdata_p->uicc_status_code_fail_details = STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS;
        rdata_p->status_word.sw1 = sim_fcp_data.sw1;
        rdata_p->status_word.sw2 = sim_fcp_data.sw2;;
        rdata_p->fcp = 0;
        memcpy((uint8_t *)(&rdata_p->length + 1), sim_fcp_data.data, sim_fcp_data.length);
        rdata_p->length = sim_fcp_data.length;

        {
            int i=0;
            uint8_t* datap = (uint8_t *)(&rdata_p->length + 1);
            printf("[%d] [%s] [%s] TEST -> FCP_DATA:\n[%d] [%s] [%s] ",
                    getpid(), __FILE__, __FUNCTION__,getpid(), __FILE__, __FUNCTION__);
            for (i=0; i < sim_fcp_data.length; i++) {
                printf("%02X", (uint8_t)datap[i]);
                if (!((i+1)%8)) {
                    printf("\n[%d] [%s] [%s] ", getpid(), __FILE__, __FUNCTION__);
                }
            }
            printf("\n");
        }

        size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
        response_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));

        response_p->size = size_data;
        memcpy(&(response_p->data), (void *)rdata_p, size_data);
        free(rdata_p);

        response_p->retval = UICC_REQUEST_STATUS_OK;
        response_p->cause = STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
        response_p->client_tag = simpbd_ct;

        TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_UICC_SIM_FILE_INFORMATION_GET, response_p, size));

        free(response_p);
    }

    {
        /* CLIENT ACTION Data returning*/

        ste_simpb_cb_file_information_data_t *file_info_cb_data_p = NULL;
        uint8_t *fcp_data_p = NULL;
        int data_size = 0;
        int i = 0;
        int fd = ste_simpb_fd_get(ste_simpb_p);

        waitForEvent(10000, fd);
        TESTSIMPB(ste_simbp_receive(ste_simpb_p));
        file_info_cb_data_p = (ste_simpb_cb_file_information_data_t *)(ste_simpb_callback_data.cb_data_p);
        fcp_data_p = (uint8_t *)(file_info_cb_data_p->data_p);
        data_size = file_info_cb_data_p->data_size;

        if (data_size == 0) { /* having this extra read out due to some spurious socket behavior in test environment. */
            TESTSIMPB_PRINT("client read 0 bytes, retry");
            TESTSIMPB(ste_simbp_receive(ste_simpb_p));
        }

        file_info_cb_data_p = (ste_simpb_cb_file_information_data_t *)(ste_simpb_callback_data.cb_data_p);
        fcp_data_p = (uint8_t *)(file_info_cb_data_p->data_p);
        data_size = file_info_cb_data_p->data_size;

        printf("[%d] [%s] [%s] TESTSTUB: fd = %d \n", getpid(), __FILE__, __FUNCTION__, fd);
        printf("[%d] [%s] [%s] TESTSTUB: data_size = %d\n", getpid(), __FILE__, __FUNCTION__, data_size);
        printf("[%d] [%s] [%s] TESTSTUB: read_data_p = 0x%08x\n", getpid(), __FILE__, __FUNCTION__, fcp_data_p);

        printf("[%d] [%s] [%s] TEST -> FCP_DATA:\n[%d] [%s] [%s] ",
                getpid(), __FILE__, __FUNCTION__,getpid(), __FILE__, __FUNCTION__);
        for (i=0; i<data_size; i++) {
            printf("%02X", (uint8_t)fcp_data_p[i]);
            if (!((i+1)%8)) {
                printf("\n[%d] [%s] [%s] ", getpid(), __FILE__, __FUNCTION__);
            }
        }
        printf("\n");


        TESTSIMPB(memcmp(fcp_data_p, sim_fcp_data.data ,sim_fcp_data.length)); /* memcmp return 0 if OK == STE_SIMPB_SUCCESS used by MACRO*/
    }

    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}
