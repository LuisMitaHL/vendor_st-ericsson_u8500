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

simpb_test_state_type_t test_unsolicted_events_SIM_STATUS_STATE_CHANGED_IND(int state, ste_simpb_t **ste_simpb_pp)
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
         sim_status_t event_status_ind;
         simpbd_test_ste_sim_read_resp_t *event_p;
         int size = 0;
         int size_data = 0;

         /* need to send event */

         size_data = sizeof(sim_status_t);
         event_status_ind.reason = SIM_REASON_DISCONNECTED_CARD;  /* should result in STE_SIMPB_SIM_STATE_SIM_ABSENT for client */

         size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
         event_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
         event_p->size = size_data;
         memcpy(event_p->data, (void *)&event_status_ind, size_data);

         event_p->retval = UICC_REQUEST_STATUS_OK;
         event_p->cause = STE_UICC_CAUSE_SIM_STATUS;
         event_p->client_tag = simpbd_ct;

         TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_SIM_URC, event_p, size));

         free(event_p);
     }

     {
         /* CLIENT ACTION to receive an unsolicited event from SIMPBD regarding the status change */

         ste_simpb_cb_sim_state_t *sim_state_p = NULL;
         int data_size = 0;
         int i = 0;
         int fd = ste_simpb_fd_get(ste_simpb_p);

         waitForEvent(1000, fd);
         TESTSIMPB(ste_simbp_receive(ste_simpb_p));

         sim_state_p = (ste_simpb_cb_sim_state_t *)(ste_simpb_callback_data.cb_data_p);

         printf("[%d] [%s] [%s] unsolicited event received = %d\n",
                 getpid(), __FILE__, __FUNCTION__, sim_state_p->value);

         TESTSIMPB(!(STE_SIMPB_SIM_STATE_SIM_ABSENT == sim_state_p->value)); /* Test ok if argument is == 0 */
     }


     /* STE_CAT_CAUSE_PC_REFRESH_IND */

     { /* SIMd ACTION */
         ste_uicc_sim_state_changed_t event_state_changed_ind;
         simpbd_test_ste_sim_read_resp_t *event_p;
         int size = 0;
         int size_data = 0;

         /* need to send event */

         size_data = sizeof(ste_uicc_sim_state_changed_t);
         event_state_changed_ind.state = SIM_STATE_REJECTED_CARD_SIM_LOCK; /* no mapping to similar event in SIMPBD, map to STE_SIMPB_SIM_STATE_UNKNOWN */

         size = size_data + sizeof(simpbd_test_ste_sim_read_resp_t);
         event_p = (simpbd_test_ste_sim_read_resp_t *)calloc(size, sizeof(uint8_t));
         event_p->size = size_data;
         memcpy(event_p->data, (void *)&event_state_changed_ind, size_data);

         event_p->retval = UICC_REQUEST_STATUS_OK;
         event_p->cause = STE_UICC_CAUSE_SIM_STATE_CHANGED;
         event_p->client_tag = simpbd_ct;

         TESTCHECK(sendStubResponse(SIMPBD_TEST_STE_SIM_URC, event_p, size));

         free(event_p);
     }

     {
         /* CLIENT ACTION to receive an unsolicited event from SIMPBD regarding the status change */

         ste_simpb_cb_sim_state_t *sim_state_p = NULL;
         int data_size = 0;
         int i = 0;
         int fd = ste_simpb_fd_get(ste_simpb_p);

         waitForEvent(1000, fd);
         TESTSIMPB(ste_simbp_receive(ste_simpb_p));

         sim_state_p = (ste_simpb_cb_sim_state_t *)(ste_simpb_callback_data.cb_data_p);

         printf("[%d] [%s] [%s] unsolicited event received = %d\n",
                 getpid(), __FILE__, __FUNCTION__, sim_state_p->value);

         TESTSIMPB(!(STE_SIMPB_SIM_STATE_UNKNOWN == sim_state_p->value)); /* Test ok if argument is == 0 */
     }
     /* No CLIENT ACTION as this URC from SIM is handled by SIMPBD in the background */


     return SIMPB_TEST_SUCCESS;

 error:

     return SIMPB_TEST_FAIL;
}
