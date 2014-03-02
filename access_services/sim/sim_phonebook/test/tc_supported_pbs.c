/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description     : simpb test of get supported phonebooks, select/get phonebook
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

simpb_test_state_type_t test_get_supported_pbs(int state, ste_simpb_t **ste_simpb_pp)
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

        TESTSIMPB(ste_simpb_supported_phonebooks_get(ste_simpb_p, client_ct));
    }

    /* No SIMd ACTION  as this is intrinsic to SIMPBD */

    {
        /* CLIENT ACTION Data returning*/

        ste_simpb_cb_supported_phonebooks_data_t *pbs_cb_data_p = NULL;
        int data_size = 0;
        int i = 0;
        int fd = ste_simpb_fd_get(ste_simpb_p);

        waitForEvent(1000, fd);
        TESTSIMPB(ste_simbp_receive(ste_simpb_p));

        pbs_cb_data_p = (ste_simpb_cb_read_data_t *)(ste_simpb_callback_data.cb_data_p);

        printf("[%d] [%s] [%s] supported phonebooks = %d\n",
                getpid(), __FILE__, __FUNCTION__, pbs_cb_data_p->supported_phonebooks);

        TESTSIMPB(!(STE_SIMPB_ISO_NORMAL_COMPLETION == pbs_cb_data_p->sim_iso_error)); /* Test ok if argument is == 0 */

    }

    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}

simpb_test_state_type_t test_set_pb(int state, ste_simpb_t **ste_simpb_pp)
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

        TESTSIMPB(ste_simpb_phonebook_select(ste_simpb_p, client_ct, STE_SIMPB_PHONEBOOK_TYPE_GLOBAL));
    }


    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}

simpb_test_state_type_t test_get_pb(int state, ste_simpb_t **ste_simpb_pp)
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

        TESTSIMPB(ste_simpb_phonebook_selected_get(ste_simpb_p, client_ct));
    }

    /* No SIMd ACTION  as this is intrinsic to SIMPBD */

    {
        /* CLIENT ACTION Data returning*/

        ste_simpb_cb_selected_phonebook_data_t *pbs_cb_data_p = NULL;
        int data_size = 0;
        int i = 0;
        int fd = ste_simpb_fd_get(ste_simpb_p);

        waitForEvent(1000, fd);
        TESTSIMPB(ste_simbp_receive(ste_simpb_p));

        pbs_cb_data_p = (ste_simpb_cb_selected_phonebook_data_t *)(ste_simpb_callback_data.cb_data_p);

        printf("[%d] [%s] [%s] selected phonebooks = %d\n",
                getpid(), __FILE__, __FUNCTION__, pbs_cb_data_p->selected_phonebook);


        TESTSIMPB(!(STE_SIMPB_ISO_NORMAL_COMPLETION == pbs_cb_data_p->sim_iso_error)); /* Test ok if argument is == 0 */
    }

    return SIMPB_TEST_SUCCESS;

error:

    return SIMPB_TEST_FAIL;
}

