/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <string.h>

#include "atc_log.h"
#include "exe_internal.h"
#include "exe.h"
#include "exe_pscc_client.h"
#include "exe_pscc_iadb.h"
#include "exe_pscc_requests.h"
#include "exe_pscc_requests_data_call_list.h"
#include "exe_pscc_util.h"
#include "exe_request_record.h"
#include "exe_request_record_p.h"


/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static bool check_for_cid_match(exe_cgpaddr_cid_t *cgpaddr_cid_p, int cid_to_match);
static bool psccclient_create_connection_id_list(exe_request_record_t *record_p, int length);
static bool psccclient_add_connection_id_to_list(exe_request_record_t *record_p, int index, int connection_id);
static int *psccclient_get_connection_id_list(exe_request_record_t *record_p, int *length_p);
static int  psccclient_get_connection_id_on_index(exe_request_record_t *record_p, int i);
static bool psccclient_add_cgdcont_item_to_list(exe_request_record_t *record_p, int cid, int pdp_type,
        char *apn_p, char *pdp_addr_p, char *ipv6_pdp_addr_p, int d_comp, int h_comp);
static bool psccclient_add_cgact_read_item_to_list(exe_request_record_t *record_p, int cid, int state);
static bool psccclient_add_qos_item_to_response_list(exe_request_record_t *record_p, exe_pdp_qos_t *qos_p);
static bool psccclient_add_pdp_address_item_to_list(exe_request_record_t *record_p, int cid, char *pdp_address_p, char *ipv6_pdp_address_p);

static bool psccclient_add_auth_item_to_list(exe_request_record_t *record_p, int cid, exe_ppp_auth_prot_t auth_prot);

/********************************************************************
 * Private methods
 ********************************************************************
 */

static bool psccclient_add_auth_item_to_list(exe_request_record_t *record_p, int cid, exe_ppp_auth_prot_t auth_prot)
{
    exe_auth_list_response_t *new_item_p = NULL;
    exe_auth_list_response_t *auth_response_p = NULL;

    auth_response_p = (exe_auth_list_response_t *) record_p->response_data_p;
    new_item_p = (exe_auth_list_response_t *) calloc(1, sizeof(exe_auth_list_response_t));

    if (NULL == new_item_p) {
        ATC_LOG_E("Could not allocate memory.");
        return false;
    }

    new_item_p->auth_prot = auth_prot;
    new_item_p->cid = cid;
    new_item_p->next_p = NULL;

    if (NULL == auth_response_p) {
        record_p->response_data_p = new_item_p;
    } else {
        while (NULL != auth_response_p->next_p) {
            auth_response_p = auth_response_p->next_p;
        }

        auth_response_p->next_p = new_item_p;
    }

    return true;
}

static bool check_for_cid_match(exe_cgpaddr_cid_t *cgpaddr_cid_p, int cid_to_match)
{
    bool result = false;

    while (NULL != cgpaddr_cid_p) {
        if (cgpaddr_cid_p->cid == cid_to_match) {
            result = true;
            break;
        }

        cgpaddr_cid_p = cgpaddr_cid_p->next_p;
    }

    return result;
}


static bool psccclient_create_connection_id_list(exe_request_record_t *record_p, int length)
{
    bool result = false;
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data is NULL. Failed.");
        goto exit;
    }

    /* free old connection id vector if present */
    if (NULL != client_data_p->connection_ids_data_list_p) {
        free(client_data_p->connection_ids_data_list_p);
    }

    client_data_p->connection_ids_data_list_p = (int *)malloc(sizeof(int) * length);

    if (NULL == client_data_p->connection_ids_data_list_p) {
        ATC_LOG_E("client_data_p->connection_ids_data_list_p is NULL. Failed to allocate memory!!");
        goto exit;
    }

    memset(client_data_p->connection_ids_data_list_p, 0 , sizeof(int) * length);
    client_data_p->connection_ids_length = length;

    result = true;

exit:
    return result;
}

static bool psccclient_add_connection_id_to_list(exe_request_record_t *record_p, int index, int connection_id)
{
    bool result = false;

    /*client_data_p is supposedly tested in psccclient_create_connection_id_list*/
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (index >= client_data_p->connection_ids_length) {
        goto exit;
    }

    client_data_p->connection_ids_data_list_p[index] = connection_id;

    result = true;

exit:
    return result;
}


static int *psccclient_get_connection_id_list(exe_request_record_t *record_p, int *length_p)
{
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);
    int *list_p = NULL;

    if (NULL == length_p) {
        ATC_LOG_E("length_p is NULL. Failed.");
        goto exit;
    }

    *length_p = 0;

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data is NULL. Failed.");
        goto exit;
    }

    if (NULL != client_data_p->connection_ids_data_list_p && client_data_p->connection_ids_length > 0) {

        list_p = (int *)malloc(sizeof(int) * client_data_p->connection_ids_length);

        if (NULL == list_p) {
            ATC_LOG_E("malloc failed.");
            goto exit;
        }

        *length_p = client_data_p->connection_ids_length;
        memcpy(list_p, client_data_p->connection_ids_data_list_p, *length_p * sizeof(int));
        /* Clean up and reset*/
        free(client_data_p->connection_ids_data_list_p);
        client_data_p->connection_ids_data_list_p = NULL;
        client_data_p->connection_ids_length = *length_p;
    }

exit:
    return list_p;
}

static int  psccclient_get_connection_id_on_index(exe_request_record_t *record_p, int index)
{
    int result = 0;
    psccclient_record_data_t *client_data_p = (psccclient_record_data_t *)request_record_get_client_data(record_p);

    if (NULL == client_data_p) {
        ATC_LOG_E("client_data is NULL. Failed.");
        goto exit;
    }

    if (index < client_data_p->connection_ids_length) {
        result = client_data_p->connection_ids_data_list_p[index];
    }

exit:
    return result;
}

static bool psccclient_add_cgdcont_item_to_list(exe_request_record_t *record_p, int cid, int pdp_type,
        char *apn_p, char *pdp_addr_p, char* ipv6_pdp_addr_p, int d_comp, int h_comp)
{
    exe_cgdcont_response_t *response_data_p = (exe_cgdcont_response_t *)record_p->response_data_p;
    exe_cgdcont_response_t *new_item_p = NULL;

    new_item_p = (exe_cgdcont_response_t *)calloc(1, sizeof(exe_cgdcont_response_t));

    if (NULL == new_item_p) {
        ATC_LOG_E("new_item_p is NULL. Failed to allocate memory!!!");
        goto error;
    }

    new_item_p->cgdcont_p = (exe_cgdcont_t *)calloc(1, sizeof(exe_cgdcont_t));

    if (NULL == new_item_p->cgdcont_p) {
        ATC_LOG_E("response_data_p->cgdcont_p is NULL. Failed to allocate memory!!!");
        goto error;
    }

    if (NULL != apn_p) {
        new_item_p->cgdcont_p->apn_p = malloc(strlen(apn_p) + 1);

        if (NULL == new_item_p->cgdcont_p->apn_p) {
            ATC_LOG_E("new_item_p->cgdcont_p->apn_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strcpy(new_item_p->cgdcont_p->apn_p, apn_p);
        }
    }

    if (NULL != pdp_addr_p) {
        new_item_p->cgdcont_p->pdp_addr_p = malloc(strlen(pdp_addr_p) + 1);

        if (NULL == new_item_p->cgdcont_p->pdp_addr_p) {
            ATC_LOG_E("new_item_p->cgdcont_p->pdp_addr_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strcpy(new_item_p->cgdcont_p->pdp_addr_p, pdp_addr_p);
        }
    }

    if (NULL != ipv6_pdp_addr_p) {
        new_item_p->cgdcont_p->ipv6_pdp_addr_p = malloc(strlen(ipv6_pdp_addr_p) + 1);

        if (NULL == new_item_p->cgdcont_p->ipv6_pdp_addr_p) {
            ATC_LOG_E("new_item_p->cgdcont_p->ipv6_pdp_addr_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strcpy(new_item_p->cgdcont_p->ipv6_pdp_addr_p, ipv6_pdp_addr_p);
        }
    }

    new_item_p->cgdcont_p->cid        = cid;
    new_item_p->cgdcont_p->pdp_type   = pdp_type;
    new_item_p->cgdcont_p->d_comp     = (d_comp == 0) ? 0 : 1; /* Values from pscc are true/false (-1/0) but AT should return 1/0 */
    new_item_p->cgdcont_p->h_comp     = (h_comp == 0) ? 0 : 1;

    new_item_p->next_p                = NULL;

    if (NULL == record_p->response_data_p) {
        record_p->response_data_p = new_item_p;
    } else {
        while (NULL != response_data_p->next_p) {
            response_data_p = response_data_p->next_p;
        }

        response_data_p->next_p = new_item_p;
    }

    return true;

error:

    if (NULL != new_item_p) {
        if (NULL != new_item_p->cgdcont_p) {
            if (NULL != new_item_p->cgdcont_p->apn_p) {
                free(new_item_p->cgdcont_p->apn_p);
            }

            if (NULL != new_item_p->cgdcont_p->pdp_addr_p) {
                free(new_item_p->cgdcont_p->pdp_addr_p);
            }

            if (NULL != new_item_p->cgdcont_p->ipv6_pdp_addr_p) {
                free(new_item_p->cgdcont_p->ipv6_pdp_addr_p);
            }

            free(new_item_p->cgdcont_p);
        }

        free(new_item_p);
    }

    return false;
}

static bool psccclient_add_cgact_read_item_to_list(exe_request_record_t *record_p, int cid, int state)
{
    exe_cgact_read_response_t *response_data_p = NULL;
    exe_cgact_read_response_t *new_item_p;
    bool result = false;

    new_item_p = (exe_cgact_read_response_t *)malloc(sizeof(exe_cgact_read_response_t));

    if (NULL == new_item_p) {
        ATC_LOG_E("new_item_p is NULL. Failed to allocate memory!!!");
        goto exit;
    }

    response_data_p = (exe_cgact_read_response_t *)record_p->response_data_p;

    if (NULL == record_p->response_data_p) {
        record_p->response_data_p = new_item_p;
    } else {
        while (NULL != response_data_p->next_p) {
            response_data_p = response_data_p->next_p;
        }

        response_data_p->next_p = new_item_p;
    }

    new_item_p->cid     = cid;
    new_item_p->state   = state;
    new_item_p->next_p  = NULL;

    result = true;

exit:
    return result;
}

/**
 * psccclient_add_qos_item_to_response_list
 *
 *  Description:
 *  Related to +CGEQNEG (read)
 *  Copy qos parameters to response data.
 */
static bool psccclient_add_qos_item_to_response_list(exe_request_record_t *record_p, exe_pdp_qos_t *qos_p)
{
    exe_pdp_qos_response_t *response_data_p =
        (exe_pdp_qos_response_t *) record_p->response_data_p;
    exe_pdp_qos_response_t *new_item_p = NULL;

    if (NULL == qos_p) {
        ATC_LOG_E("Could not add item since qos_p is NULL");
        return false;
    }

    new_item_p = (exe_pdp_qos_response_t *) calloc(1,
                 sizeof(exe_pdp_qos_response_t));

    if (NULL == new_item_p) {
        ATC_LOG_E("new_item_p is NULL. Failed to allocate memory!!!");
        goto error;
    }

    new_item_p->pdp_qos.cid = qos_p->cid;
    new_item_p->pdp_qos.traffic_class = qos_p->traffic_class;
    new_item_p->pdp_qos.max_bitrate_ul = qos_p->max_bitrate_ul;
    new_item_p->pdp_qos.max_bitrate_dl = qos_p->max_bitrate_dl;
    new_item_p->pdp_qos.guaranteed_bitrate_ul = qos_p->guaranteed_bitrate_ul;
    new_item_p->pdp_qos.guaranteed_bitrate_dl = qos_p->guaranteed_bitrate_dl;
    new_item_p->pdp_qos.delivery_order = qos_p->delivery_order;
    new_item_p->pdp_qos.max_sdu_size = qos_p->max_sdu_size;

    if (NULL != qos_p->sdu_error_ratio_p) {
        new_item_p->pdp_qos.sdu_error_ratio_p = (char *) calloc(1, 3 + 1);

        if (NULL == new_item_p->pdp_qos.sdu_error_ratio_p) {
            ATC_LOG_E("new_item_p->sdu_error_ratio_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strncpy(new_item_p->pdp_qos.sdu_error_ratio_p,
                    qos_p->sdu_error_ratio_p, 3);
        }
    }

    if (NULL != qos_p->residual_ber_p) {
        new_item_p->pdp_qos.residual_ber_p = (char *) calloc(1, 3 + 1);

        if (NULL == new_item_p->pdp_qos.residual_ber_p) {
            ATC_LOG_E("new_item_p->residual_ber_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strncpy(new_item_p->pdp_qos.residual_ber_p, qos_p->residual_ber_p,
                    3);
        }
    }

    new_item_p->pdp_qos.delivery_err_sdu = qos_p->delivery_err_sdu;
    new_item_p->pdp_qos.transfer_delay = qos_p->transfer_delay;
    new_item_p->pdp_qos.traffic_handling_priority = qos_p->traffic_handling_priority;
    new_item_p->pdp_qos.source_statistics_descriptor = qos_p->source_statistics_descriptor;
    new_item_p->pdp_qos.signalling_indication  = qos_p->signalling_indication;

    if (NULL == record_p->response_data_p) {
        record_p->response_data_p = new_item_p;
    } else {
        while (NULL != response_data_p->next_p) {
            response_data_p = response_data_p->next_p;
        }

        response_data_p->next_p = new_item_p;
    }

    return true;

error:

    if (NULL != new_item_p) {
        if (NULL != new_item_p->pdp_qos.sdu_error_ratio_p) {
            free(new_item_p->pdp_qos.sdu_error_ratio_p);
        }

        if (NULL != new_item_p->pdp_qos.residual_ber_p) {
            free(new_item_p->pdp_qos.residual_ber_p);
        }

        free(new_item_p);
    }

    return false;
}

static bool psccclient_add_pdp_address_item_to_list(exe_request_record_t *record_p, int cid, char *pdp_address_p, char *ipv6_pdp_address_p)
{
    ATC_LOG_I("Adding cid: %i ipv4 address: %s and ipv6 address: %s", cid, pdp_address_p, ipv6_pdp_address_p);
    exe_cgpaddr_response_t *response_data_p = NULL;
    exe_cgpaddr_response_t *new_item_p = NULL;

    response_data_p = (exe_cgpaddr_response_t *) record_p->response_data_p;
    new_item_p = (exe_cgpaddr_response_t *) malloc(sizeof(exe_cgpaddr_response_t));

    if (NULL == new_item_p) {
        ATC_LOG_E("Could not allocate memory.");
        goto error;
    }

    new_item_p->pdp_address_p = NULL;
    new_item_p->ipv6_pdp_address_p = NULL;
    new_item_p->next_p = NULL;
    new_item_p->cid = cid;

    if (NULL != pdp_address_p) {
        new_item_p->pdp_address_p = malloc((strlen(pdp_address_p) + 1) * sizeof(char));

        if (NULL == new_item_p->pdp_address_p) {
            ATC_LOG_E("new_item_p->pdp_addr_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strcpy(new_item_p->pdp_address_p, pdp_address_p);
        }
    }

    if (NULL != ipv6_pdp_address_p) {
        new_item_p->ipv6_pdp_address_p = malloc((strlen(ipv6_pdp_address_p) + 1) * sizeof(char));

        if (NULL == new_item_p->ipv6_pdp_address_p) {
            ATC_LOG_E("new_item_p->ipv6_pdp_address_p is NULL. Failed to allocate memory!!!");
            goto error;
        } else {
            strcpy(new_item_p->ipv6_pdp_address_p, ipv6_pdp_address_p);
        }
    }

    if (NULL == record_p->response_data_p) {
        record_p->response_data_p = new_item_p;
    } else {
        while (NULL != response_data_p->next_p) {
            response_data_p = response_data_p->next_p;
        }

        response_data_p->next_p = new_item_p;
    }

    return true;
error:

    if (NULL != new_item_p) {
        if (NULL != new_item_p->pdp_address_p) {
            free(new_item_p->pdp_address_p);
        }
        if (NULL != new_item_p->ipv6_pdp_address_p) {
            free(new_item_p->ipv6_pdp_address_p);
        }
        free(new_item_p);
    }

    return false;
}


/********************************************************************
 * Public methods
 ********************************************************************
 */


/****************************************************************************************************************************************************
 * UNSOL_DATA_CALL_LIST_CHANGED (the event is not sent to the RIL, it is triggered when handling other requests/event)
 *
 * Scenario description:
 * ---------------------
 * 1)  Send    pscc_list_all_connections request
 * 2)  Receive pscc_list_all_connections response
 * 3)  Send    pscc_get request (connection information)
 * 4)  Receive pscc_get response  <-- Recurse step 3-4 until all connections in the list have been processed
 * 5)  ->  not APPLICABLE ANYMORE -> Send    RIL_UNSOL_DATA_CALL_LIST_CHANGED
 *
 ****************************************************************************************************************************************************/

/****************************************************************************************************************************************************
 * REQUEST_DATA_CALL_LIST
 *
 * Scenario description:
 * ---------------------
 * 1)  Send    pscc_list_all_connections request
 * 2)  Receive pscc_list_all_connections response
 * 3)  Send    pscc_get request (connection information)
 * 4)  Receive pscc_get response  <-- Recurse step 3-4 until all connections in the list have been processed
 * 5)  ->  not APPLICABLE ANYMORE -> Send    RIL_RESPONSE_DATA_CALL_LIST
 *
 ****************************************************************************************************************************************************/

/****************************************************************************************************************************************************
 * REQUEST_MODIFY (successful case - the context(s) are modified.)
 *
 * For step 1-4, REQUEST_DATA_CALL_LIST scenario is used.
 *
 * Scenario description:
 * ---------------------
 * 1)  Send    pscc_list_all_connections request
 * 2)  Receive pscc_list_all_connections response
 * 3)  Send    pscc_get request (connection information)
 * 4)  Receive pscc_get response  <-- Recurse step 3-4 until all connections in the list have been processed
 * 5)  Send    pscc modify request
 * 6)  Receive pscc modify response
 * 7)  Receive pscc modified event
 *
 * REQUEST_MODIFY: Scenario 1 (unsuccessful case - the context(s) are not modified.)
 *
 * For step 1-4, REQUEST_DATA_CALL_LIST scenario is used.
 *
 * Scenario description:
 * ---------------------
 * 1)  Send    pscc_list_all_connections request
 * 2)  Receive pscc_list_all_connections response
 * 3)  Send    pscc_get request (connection information)
 * 4)  Receive pscc_get response  <-- Recurse step 3-4 until all connections in the list have been processed
 * 5)  Send    pscc modify request
 * 6)  Receive pscc modify response
 * 7)  Receive pscc modify failed event
 *
 ****************************************************************************************************************************************************/



/********************************************************************************************************************************************************************
 * HANDLER FUNCTIONS COMMON FOR BOTH SCENARIOS                                                                                                                      *
 ********************************************************************************************************************************************************************/


/* 2)  Receive pscc_list_all_connections response
 * 3)  Send    pscc_get request (connection information)         mpl_msg_resp_t* response_p
 */
exe_pscc_result_t pscc_data_call_list_step_2_3_receive_pscc_list_all_connections_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t    result               = EXE_PSCC_FAILURE;
    exe_pscc_action_t    action               = EXE_PSCC_ACTION_UNDEFINED;
    pscc_msg_t          *request_p            = NULL;
    mpl_param_element_t *param_elem_connid_p  = NULL;
    mpl_msg_resp_t      *response_p           = NULL;
    int                  length               = 0;
    int                  i                    = 0;
    int                  connection_id        = 0;

    response_p = psccclient_get_pscc_response_p(record_p);

    if (NULL == response_p) {
        ATC_LOG_E("response is NULL");
        goto exit;
    }

    /* Take care of error scenario */
    if (response_p->result != pscc_result_ok) {
        ATC_LOG_E("response failed: %d", response_p->result);
        goto exit;
    }

    action = psccclient_get_action(record_p);

    /* Determine size of list, allocate and initialize memory for the response */
    length = mpl_list_len(response_p->param_list_p);
    /* Check how many id we actual got */
    int found_records = 0;

    for (i = 0; i < length; i++) {
        if (0 == i) {
            param_elem_connid_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);
        } else {
            param_elem_connid_p = mpl_param_list_find_next(pscc_paramid_connid, param_elem_connid_p);
        }

        if (param_elem_connid_p != NULL) {
            found_records++;
        }
    }

    length = found_records;

    /* Empty list, return response to the client and skip the rest of the scenario */
    if (length <= 0) {
        int cids_length = 0;
        int *cids_p = psccclient_iadb_list_cids_without_connection_ids(NULL, 0, &cids_length);

        if (NULL != cids_p) {
            for (i = 0; i < cids_length; i++) {
                switch (action) {
                case EXE_PSCC_ACTION_DATA_LIST_CGDCONT: {
                    int   cid            = cids_p[i];
                    char *apn_p          = NULL;
                    int   pdp_type       = 0;
                    char *pdp_address_p  = NULL;
                    char *ipv6_pdp_address_p  = NULL;
                    int   hcmp           = 0;
                    int   dcmp           = 0;

                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_pdp_type, &pdp_type);
                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_apn_p, &apn_p);
                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_pdp_address_p, &pdp_address_p);
                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_ipv6_pdp_address_p, &ipv6_pdp_address_p);
                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_h_comp, &hcmp);
                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_d_comp, &dcmp);

                    (void)psccclient_add_cgdcont_item_to_list(record_p, cids_p[i], pdp_type, apn_p, pdp_address_p, ipv6_pdp_address_p,dcmp, hcmp);
                    break;
                }
                case EXE_PSCC_ACTION_DATA_LIST_CGACT: {
                    psccclient_add_cgact_read_item_to_list(record_p, cids_p[i], 0 /* 0 for disconnected */);
                    break;
                }

                case EXE_PSCC_ACTION_DATA_LIST_PDP_ADDRESS: {
                    char *pdp_address_p = NULL;
                    char *ipv6_pdp_address_p = NULL;

                    psccclient_record_data_t *client_data_p =
                        (psccclient_record_data_t *) request_record_get_client_data(record_p);
                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_pdp_address_p, &pdp_address_p);
                    psccclient_iadb_get(cids_p[i], psccclient_iadb_field_ipv6_pdp_address_p, &ipv6_pdp_address_p);
                    if (NULL == client_data_p->cgpaddr_p) {
                        /* If cgpaddr_p is null, we should list all cids */
                        (void) psccclient_add_pdp_address_item_to_list(record_p, cids_p[i], pdp_address_p, ipv6_pdp_address_p);
                    } else {
                        /* Else, we need to match against provided cids */
                        if (check_for_cid_match(client_data_p->cgpaddr_p, cids_p[i])) {
                            (void) psccclient_add_pdp_address_item_to_list(record_p, cids_p[i], pdp_address_p, ipv6_pdp_address_p);
                        }
                    }

                    break;
                }
                case EXE_PSCC_ACTION_DATA_LIST_AUTH: {
                    pscc_auth_method_t auth_method = pscc_auth_method_none; /* Default value, No Authentication */
                    psccclient_user_get(cids_p[i], psccclient_user_field_auth_prot, (int *)&auth_method);

                    if (!psccclient_add_auth_item_to_list(record_p,
                                                          cids_p[i],
                                                          auth_mode_pscc2generic(auth_method))) {
                        goto exit;
                    }

                    break;
                }
                default:
                    break;
                }
            }

            free(cids_p);
        }

        result = EXE_PSCC_SUCCESS_TERMINATE;
        goto exit;
    }

    if (!psccclient_create_connection_id_list(record_p, length)) {
        ATC_LOG_E("Failed psccclient_create_connection_id_list.");
        result = EXE_PSCC_FAILURE;
        goto exit_on_empty;
    }

    for (i = 0; i < length; i++) {
        if (0 == i) {
            param_elem_connid_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);
        } else {
            param_elem_connid_p = mpl_param_list_find_next(pscc_paramid_connid, param_elem_connid_p);
        }

        if (param_elem_connid_p != NULL) {
            if (!psccclient_add_connection_id_to_list(record_p, i, *((int *)param_elem_connid_p->value_p))) {
                ATC_LOG_E("Failed to add a connection id in the list (index=%d)", i);
                result = EXE_PSCC_FAILURE;
                goto exit;
            }
        }
    }

    /* Send pscc_get request for first connection (and then continue in the following response handler for the rest) */
    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("failed to allocate memory for pscc_request_p");
        result = EXE_PSCC_FAILURE;
        goto exit;
    }

    request_p->req_get.id = pscc_get;

    connection_id = psccclient_get_connection_id_on_index(record_p, 0);

    /* TODO connection id is set in send_request, this should not be needed. */
    (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_connid, &connection_id);

    switch (action) {

    case EXE_PSCC_ACTION_DATA_LIST_CGDCONT: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_pdp_type, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_apn, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ip_address, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ipv6_address, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_hcmp, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_dcmp, NULL);
        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_CGACT: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_connection_status, NULL);
        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_NEG_QOS: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_traffic_class, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_max_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_max_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_max_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_max_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_guaranteed_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_guaranteed_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_delivery_order, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_max_sdu_size, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_residual_ber, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_sdu_error_ratio, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_delivery_erroneous_sdu, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_transfer_delay, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_traffic_handling_priority, NULL);
        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_PDP_ADDRESS: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ip_address, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ipv6_address, NULL);
        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_AUTH: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_auth_method, NULL);
        break;
    }
    default:
        break;
    }

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        result = EXE_PSCC_FAILURE;
        goto exit;
    }

    result = EXE_PSCC_SUCCESS;

exit:
    mpl_msg_free((mpl_msg_t *)request_p);

exit_on_empty:
    return result;
}

/* 4)  Receive pscc_get response  <-- Recurse step 3-4 until all connections in the list have been processed
 * 5)  Send   UNSOL_DATA_CALL_LIST_CHANGED
 */
exe_pscc_result_t pscc_data_call_list_step_4_5_receive_pscc_get_response(exe_request_record_t *record_p)
{
    exe_pscc_result_t     result         = EXE_PSCC_FAILURE;
    exe_pscc_action_t     action         = EXE_PSCC_ACTION_UNDEFINED;
    pscc_msg_t           *request_p      = NULL;
    mpl_msg_resp_t       *response_p     = NULL;
    mpl_param_element_t  *param_elem_p   = NULL;
    mpl_param_element_t  *param_elem_extended_p   = NULL;

    action = psccclient_get_action(record_p);
    response_p = psccclient_get_pscc_response_p(record_p);
    EXE_CHECK_GOTO_ERROR(NULL != response_p);

    /* Take care of error scenario */
    if (response_p->result != pscc_result_ok) {
        ATC_LOG_E("response failed: %d", response_p->result);
        goto error;
    }

    switch (action) {
    case EXE_PSCC_ACTION_DATA_LIST_CGDCONT: {

        int   cid            = 0;
        char *apn_p          = NULL;
        int   pdp_type       = 0;
        char *pdp_address_p  = NULL;
        char *ipv6_pdp_address_p  = NULL;
        int   hcmp           = 0;
        int   dcmp           = 0;

        /* Connection id  -> cid */
        param_elem_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            if (!psccclient_iadb_get_cid_on_conn_id(*((int *)param_elem_p->value_p), &cid)) {
                ATC_LOG_I("No matching CID for connid (%d) found.", *((int *)param_elem_p->value_p));
                goto exit;
            }
        }

        /* PDP type */
        param_elem_p = mpl_param_list_find(pscc_paramid_pdp_type, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            pdp_type = *((int *)param_elem_p->value_p);
        }

        /* APN */
        param_elem_p = mpl_param_list_find(pscc_paramid_apn, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            apn_p = (char *)param_elem_p->value_p;
        }

        /* IP Address [ PDP_ADDRESS ] */
        param_elem_p = mpl_param_list_find(pscc_paramid_own_ip_address, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            pdp_address_p = (char *)param_elem_p->value_p;
        }

        /* IPv6 Address [ PDP_ADDRESS ] */
        param_elem_p = mpl_param_list_find(pscc_paramid_own_ipv6_address, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            ipv6_pdp_address_p = (char *)param_elem_p->value_p;
        }

        /* HCMP */
        param_elem_p = mpl_param_list_find(pscc_paramid_hcmp, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            hcmp = *((int *)param_elem_p->value_p);
        }

        /* DCMP */
        param_elem_p = mpl_param_list_find(pscc_paramid_dcmp, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            dcmp = *((int *)param_elem_p->value_p);
        }

        (void)psccclient_add_cgdcont_item_to_list(record_p, cid, pdp_type, apn_p, pdp_address_p, ipv6_pdp_address_p, dcmp, hcmp);

        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_CGACT: {

        int   cid            = 0;
        int   state          = 0;

        /* Connection id */
        param_elem_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            if (!psccclient_iadb_get_cid_on_conn_id(*((int *)param_elem_p->value_p), &cid)) {
                ATC_LOG_I("No matching CID for connid (%d) found.", *((int *)param_elem_p->value_p));
                goto exit;
            }
        }

        /* Connection status */
        param_elem_p = mpl_param_list_find(pscc_paramid_connection_status, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            if (pscc_connection_status_connected == *((pscc_connection_status_t *) param_elem_p->value_p)) {
                state = 1;
            }
        }

        (void)psccclient_add_cgact_read_item_to_list(record_p, cid, state);

        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_PDP_ADDRESS: {
        int cid = 0;
        char *pdp_address_p = NULL;
        char *ipv6_pdp_address_p = NULL;

        /* Connection id */
        param_elem_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            (void) psccclient_iadb_get_cid_on_conn_id(*((int *) param_elem_p->value_p), &cid);
        }

        /* PDP Address */
        param_elem_p = mpl_param_list_find(pscc_paramid_own_ip_address, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            pdp_address_p = (char *) param_elem_p->value_p;
        }

        /* IPv6 PDP Address */
        param_elem_p = mpl_param_list_find(pscc_paramid_own_ipv6_address, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            ipv6_pdp_address_p = (char *) param_elem_p->value_p;
        }

        psccclient_record_data_t *client_data_p =
            (psccclient_record_data_t *) request_record_get_client_data(record_p);

        if (NULL == client_data_p->cgpaddr_p) {
            /* If cgpaddr_p is null, we should list all cids */
            (void) psccclient_add_pdp_address_item_to_list(record_p, cid,
                    pdp_address_p, ipv6_pdp_address_p);
        } else {
            /* Else, we need to match against provided cids */
            if (check_for_cid_match(client_data_p->cgpaddr_p, cid)) {
                (void) psccclient_add_pdp_address_item_to_list(record_p, cid,
                        pdp_address_p, ipv6_pdp_address_p);
            }
        }

        break;
    }

    case EXE_PSCC_ACTION_DATA_LIST_NEG_QOS: {
        exe_pdp_qos_t  *qos_neg_p = calloc(1, sizeof(exe_pdp_qos_t));

        if (NULL == qos_neg_p) {
            ATC_LOG_E("qos_neg_p is NULL. Failed to allocate memory!!!");
            goto error;
        }

        ATC_LOG_I("Read negotiated qos parameters.");

        /* Connection id  -> cid */
        param_elem_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            if (!psccclient_iadb_get_cid_on_conn_id(*((int *)param_elem_p->value_p), &qos_neg_p->cid)) {
                ATC_LOG_E("psccclient_iadb_get_cid_on_conn_id failed");
                free(qos_neg_p);
                goto error;
            }
        }

        /* Note that an enum seem to be packed as a uint8_t in mpl, which means that the parameters cannot be typecasted
         * into enums when read (as enums are considered ints and thus four bytes of data would be read instead of the
         * packed single byte).*/
        /* Traffic_class */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_traffic_class, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->traffic_class = convert_PSCC_to_AT_traffic_class(*((int *)param_elem_p->value_p));
        }

        /* Max bit rate uplink */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_max_bitrate_uplink, response_p->param_list_p);
        param_elem_extended_p = mpl_param_list_find(pscc_paramid_neg_qos_extended_max_bitrate_uplink, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p && NULL != param_elem_extended_p && NULL != param_elem_extended_p->value_p) {
            qos_neg_p->max_bitrate_ul = convert_PSCC_to_AT_bitrate(*((uint8_t *)param_elem_p->value_p), *((uint8_t *)param_elem_extended_p->value_p));
        }

        /* Max bit rate downlink */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_max_bitrate_downlink, response_p->param_list_p);
        param_elem_extended_p = mpl_param_list_find(pscc_paramid_neg_qos_extended_max_bitrate_downlink, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p && NULL != param_elem_extended_p && NULL != param_elem_extended_p->value_p) {
            qos_neg_p->max_bitrate_dl = convert_PSCC_to_AT_bitrate(*((uint8_t *)param_elem_p->value_p), *((uint8_t *)param_elem_extended_p->value_p));
        }

        /* Guaranteed bit rate uplink */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_guaranteed_bitrate_uplink, response_p->param_list_p);
        param_elem_extended_p = mpl_param_list_find(pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p && NULL != param_elem_extended_p && NULL != param_elem_extended_p->value_p) {
            qos_neg_p->guaranteed_bitrate_ul = convert_PSCC_to_AT_bitrate(*((uint8_t *)param_elem_p->value_p), *((uint8_t *)param_elem_extended_p->value_p));
        }

        /* Guaranteed bit rate downlink */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_guaranteed_bitrate_downlink, response_p->param_list_p);
        param_elem_extended_p = mpl_param_list_find(pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p && NULL != param_elem_extended_p && NULL != param_elem_extended_p->value_p) {
            qos_neg_p->guaranteed_bitrate_dl = convert_PSCC_to_AT_bitrate(*((uint8_t *)param_elem_p->value_p), *((uint8_t *)param_elem_extended_p->value_p));
        }

        /* delivery order */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_delivery_order, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->delivery_order = convert_PSCC_to_AT_SDU_delivery_order(*((uint8_t *)param_elem_p->value_p));
        }

        /* Max SDU size */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_max_sdu_size, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->max_sdu_size = convert_PSCC_to_AT_max_SDU_size(*((uint8_t *)param_elem_p->value_p));
        }

        /* SDU Error ratio */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_sdu_error_ratio, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->sdu_error_ratio_p = calloc(1, 3 + 1);
            convert_PSCC_to_AT_SDU_error_ratio(*((uint8_t *)param_elem_p->value_p), qos_neg_p->sdu_error_ratio_p);
        }

        /* Residual bit error rate */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_residual_ber, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->residual_ber_p =  calloc(1, 3 + 1);
            convert_PSCC_to_AT_residual_BER(*(uint8_t *)param_elem_p->value_p, qos_neg_p->residual_ber_p);
        }

        /* Delivery of erroneous SDU */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_delivery_erroneous_sdu, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->delivery_err_sdu = convert_PSCC_to_AT_delivery_of_erroneous_SDU(*((uint8_t *)param_elem_p->value_p));
        }

        /* Transfer Delay */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_transfer_delay, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->transfer_delay = convert_PSCC_to_AT_transfer_delay(*((uint8_t *)param_elem_p->value_p));
        }

        /* Traffic Handling Priority */
        param_elem_p = mpl_param_list_find(pscc_paramid_neg_qos_traffic_handling_priority, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            qos_neg_p->traffic_handling_priority = convert_PSCC_to_AT_traffic_handling_priority(*((uint8_t *)param_elem_p->value_p));
        }

        psccclient_record_data_t *client_data_p =
            (psccclient_record_data_t *) request_record_get_client_data(record_p);

        if (check_for_cid_match(client_data_p->cgpaddr_p, qos_neg_p->cid)) {
            (void)psccclient_add_qos_item_to_response_list(record_p, qos_neg_p);
        }


        if (NULL != qos_neg_p->sdu_error_ratio_p) {
            free(qos_neg_p->sdu_error_ratio_p);
        }

        if (NULL != qos_neg_p->residual_ber_p) {
            free(qos_neg_p->residual_ber_p);
        }

        free(qos_neg_p);
        break;
    }

    case EXE_PSCC_ACTION_DATA_LIST_AUTH: {
        int cid = 0;
        pscc_auth_method_t auth_method = pscc_auth_method_none; /* Default value, No Authentication */

        /* Connection id */
        param_elem_p = mpl_param_list_find(pscc_paramid_connid, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            (void) psccclient_iadb_get_cid_on_conn_id(*((int *) param_elem_p->value_p), &cid);
        }

        /* Auth method */
        param_elem_p = mpl_param_list_find(pscc_paramid_auth_method, response_p->param_list_p);

        if (NULL != param_elem_p && NULL != param_elem_p->value_p) {
            auth_method = *((int *) param_elem_p->value_p);
        }

        if (!psccclient_add_auth_item_to_list(record_p, cid, auth_mode_pscc2generic(auth_method))) {
            goto error;
        }

        break;
    }
    default:
        ATC_LOG_E("Unknown action type. Aborting.");
        goto error;
        break;
    }

exit:
    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_SUCCESS;

error:
    mpl_msg_free((mpl_msg_t *)request_p);

    return result;
}


exe_pscc_result_t exe_pscc_list_all_connections_add_from_iadb(exe_request_record_t *record_p)
{
    /* The list so far includes only items known by PSCC. Complement with items only in iadb. */
    exe_pscc_action_t     action         = EXE_PSCC_ACTION_UNDEFINED;
    int cids_length = 0;
    int i;
    int *conn_id_list_p = NULL;
    int conn_id_list_length = 0;
    conn_id_list_p = psccclient_get_connection_id_list(record_p, &conn_id_list_length);
    int *cids_p = psccclient_iadb_list_cids_without_connection_ids(conn_id_list_p, conn_id_list_length, &cids_length);


    action = psccclient_get_action(record_p);

    if (NULL != cids_p) {
        for (i = 0; i < cids_length; i++) {
            switch (action) {
            case EXE_PSCC_ACTION_DATA_LIST_CGDCONT: {
                int   cid            = cids_p[i];
                char *apn_p          = NULL;
                int   pdp_type       = 0;
                char *pdp_address_p  = NULL;
                char *ipv6_pdp_address_p  = NULL;
                int   hcmp           = 0;
                int   dcmp           = 0;

                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_pdp_type, &pdp_type);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_apn_p, &apn_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_pdp_address_p, &pdp_address_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_ipv6_pdp_address_p, &ipv6_pdp_address_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_h_comp, &hcmp);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_d_comp, &dcmp);

                (void)psccclient_add_cgdcont_item_to_list(record_p, cids_p[i], pdp_type, apn_p, pdp_address_p, ipv6_pdp_address_p, dcmp, hcmp);
                break;
            }

            case EXE_PSCC_ACTION_DATA_LIST_CGACT:
                psccclient_add_cgact_read_item_to_list(record_p, cids_p[i], 0 /* 0 for disconnected */);
                break;

            case EXE_PSCC_ACTION_DATA_LIST_PDP_ADDRESS: {
                char *pdp_address_p = NULL;
                char *ipv6_pdp_address_p = NULL;

                psccclient_record_data_t *client_data_p = NULL;
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_pdp_address_p,
                                    &pdp_address_p);
                psccclient_iadb_get(cids_p[i], psccclient_iadb_field_ipv6_pdp_address_p,
                                    &ipv6_pdp_address_p);

                client_data_p = (psccclient_record_data_t *) request_record_get_client_data(record_p);

                if (NULL == client_data_p->cgpaddr_p) {
                    /* If cgpaddr_p is null, we should list all cids */
                    (void) psccclient_add_pdp_address_item_to_list(record_p,
                            cids_p[i], pdp_address_p, ipv6_pdp_address_p);
                } else {
                    /* Else, we need to match against provided cids */
                    if (check_for_cid_match(client_data_p->cgpaddr_p, cids_p[i])) {
                        (void) psccclient_add_pdp_address_item_to_list(
                            record_p, cids_p[i], pdp_address_p, ipv6_pdp_address_p);
                    }
                }

                break;
            }
            case EXE_PSCC_ACTION_DATA_LIST_AUTH: {
                pscc_auth_method_t auth_prot = pscc_auth_method_none; /* Default value, No Authentication */
                psccclient_user_get(cids_p[i], psccclient_user_field_auth_prot, (int *)&auth_prot);

                if (!psccclient_add_auth_item_to_list(record_p,
                                                      cids_p[i],
                                                      auth_mode_pscc2generic(auth_prot))) {
                    goto error;
                }

                break;
            }
            default:
                break;
            }

        }

        free(cids_p);
    }


    if (NULL != conn_id_list_p) {
        free(conn_id_list_p);
    }

    return EXE_PSCC_SUCCESS;

error:

    if (NULL != conn_id_list_p) {
        free(conn_id_list_p);
    }

    return EXE_PSCC_FAILURE;
}

exe_pscc_result_t exe_pscc_list_all_connections_get(exe_request_record_t *record_p, int current_index)
{
    int                   connection_id  = 0;
    exe_pscc_action_t     action         = EXE_PSCC_ACTION_UNDEFINED;
    pscc_msg_t           *request_p      = NULL;

    request_p = mpl_req_msg_alloc(PSCC_PARAM_SET_ID);

    if (NULL == request_p) {
        ATC_LOG_E("failed to allocate memory for pscc_request_p");
        goto error;
    }

    request_p->req_get.id  = pscc_get;
    connection_id = psccclient_get_connection_id_on_index(record_p, current_index);
    action = psccclient_get_action(record_p);

    /* TODO connection id is set in send_request, this should not be needed. */
    (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_connid, &connection_id);

    /* TODO break out these settings, should be the very same as in pscc_data_call_list_step_2_3_receive_pscc_list_all_connections_response. */
    switch (action) {
    case EXE_PSCC_ACTION_DATA_LIST_CGDCONT: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_pdp_type, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_apn, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ip_address, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ipv6_address, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_hcmp, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_dcmp, NULL);
        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_CGACT: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_connection_status, NULL);
        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_NEG_QOS: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_traffic_class, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_max_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_max_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_max_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_max_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_guaranteed_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_guaranteed_bitrate_uplink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_guaranteed_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_extended_guaranteed_bitrate_downlink, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_delivery_order, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_max_sdu_size, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_residual_ber, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_sdu_error_ratio, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_delivery_erroneous_sdu, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_transfer_delay, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_neg_qos_traffic_handling_priority, NULL);
        break;
    }

    case EXE_PSCC_ACTION_DATA_LIST_PDP_ADDRESS : {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ip_address, NULL);
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_own_ipv6_address, NULL);
        break;
    }
    case EXE_PSCC_ACTION_DATA_LIST_AUTH: {
        (void) mpl_add_param_to_list(&request_p->req_get.param_list_p, pscc_paramid_auth_method, NULL);
        break;
    }

    default:
        ATC_LOG_E("Wrong action in record. No pscc request sent!");
        goto error;
        break;
    }

    if (!psccclient_send_request(record_p, request_p)) {
        ATC_LOG_E("psccclient_send_request failed!");
        goto error;
    }

    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_SUCCESS;

error:
    mpl_msg_free((mpl_msg_t *)request_p);
    return EXE_PSCC_FAILURE;
}
