/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/sockios.h>

#include "mpl_config.h" /* libmpl */

#include "atc_log.h"

#include "exe_internal.h"
#include "exe_pscc_client.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc_util.h"
#include "exe_pscc_requests_modify_data_call.h"
#include "exe_request_record_p.h"



/********************************************************************
 * Public methods
 ********************************************************************
 */

/****************************************************************************************************************************************************
 * REQUEST_MODIFY: Scenario 0 (successful case - the context(s) are modified.)
 *
 * Scenario description:
 * ---------------------
 * 1)  Read QoS parameters from iadb and Send pscc modify request
 * 2)  Receive pscc modify response
 * 3)  Receive pscc modified event
 * NOTE: Step 1-2 shared with scenario 1
 *
 * REQUEST_MODIFY: Scenario 1 (unsuccessful case - the context(s) are not modified.)
 * Scenario description:
 * ---------------------
 * 1)  Read QoS parameters from iadb and Send pscc modify request
 * 2)  Receive pscc modify response
 * 3)  Receive pscc modify failed event
 * NOTE: Step 1-2 shared with scenario 0
 *
 *****************************************************************************************************************************************************/

exe_pscc_result_t pscc_request_modify_data_call_scenario_0_1_step_1_send_pscc_modify_request(exe_request_record_t *record_p)
{
    exe_pscc_result_t         result        = EXE_PSCC_FAILURE;
    pscc_msg_t               *request_p     = NULL;
    psccclient_record_data_t *client_data_p = NULL;
    int                       cid           = 0;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("request_p is NULL");
        goto exit;
    }

    request_p->req_create.id = pscc_modify;

    client_data_p = (psccclient_record_data_t *) record_p->client_data_p;

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data_p is NULL");
        goto exit;
    }

    if (NULL == client_data_p->cgcmod_p) {
        ATC_LOG_E("client_data_p->cgcmod_p is NULL");
        goto exit;
    }

    if (NULL == client_data_p->cgcmod_p->cid_p) {
        ATC_LOG_E("client_data_p->cgcmod_p->cid_p is NULL");
        goto exit;
    }

    cid = client_data_p->cgcmod_p->cid_p->cid;

    int use_97_qos = false;

    if (!psccclient_iadb_get(cid, psccclient_iadb_field_use_rel_97_qos, &use_97_qos)) {
        ATC_LOG_E("QoS version preference undefined!");
        goto exit;
    }

    if (true == use_97_qos) {
        if (!add_2G_qos_parameters_to_pscc_message(cid, request_p)) {
            ATC_LOG_E("Error in parameter conversion!");
            goto exit;
        }
    } else {
        if (!add_qos_parameters_to_pscc_message(cid, request_p)) {
            ATC_LOG_E("Error in parameter conversion!");
            goto exit;
        }
    }

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto exit;
    }


    result = EXE_PSCC_SUCCESS;

exit:
    mpl_msg_free((mpl_msg_t *) request_p);
    return result;
}


