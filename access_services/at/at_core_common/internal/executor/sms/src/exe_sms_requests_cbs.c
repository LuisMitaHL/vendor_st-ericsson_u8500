/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>

#include "r_cbs.h"
#include "t_cbs.h"

#include "exe_internal.h"
#include "exe_request_record_p.h"
#include "exe_sms_client.h"
#include "exe_sms_requests.h"

typedef struct {
    exe_cscb_data_t new_cscb_data;
    exe_cscb_data_t prev_cscb_data;
} exe_cscb_client_data_t;

exe_request_result_t exe_cbs_subscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p, exe_cscb_data_t *cscb_data_p)
{
    SMS_RequestControlBlock_t  *request_ctrl_block_p  = NULL;
    SMS_RequestStatus_t         request_status        = SMS_REQUEST_OK;
    CBS_Error_t                 cbs_error             = CBS_ERROR_NONE;
    CBS_Subscription_t         *subscription_p        = NULL;

    if (NULL == smsclient_p) {
        ATC_LOG_E("exe_cbs_subscribe: smsclient_p is NULL");
        goto error;
    }

    if (NULL == cscb_data_p) {
        ATC_LOG_E("exe_cbs_subscribe: cscb_data_p is NULL");
        goto error;
    }

    if (NULL == cscb_data_p->subscription_p) {
        ATC_LOG_E("exe_cbs_subscribe: subscription_p is NULL");
        goto error;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("exe_cbs_subscribe: smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    subscription_p = (CBS_Subscription_t *)(cscb_data_p->subscription_p);

    request_status = Request_CBS_Subscribe(
                         request_ctrl_block_p,
                         0, /* don't subscribe to all */
                         cscb_data_p->num_of_subs,
                         subscription_p,
                         &cbs_error);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (SMS_REQUEST_PENDING != request_status ||
            CBS_ERROR_NONE != cbs_error) {
        print_cbs_error("exe_cbs_subscribe: Request_CBS_Subscribe failed --> ", cbs_error);
        goto error;
    }

    return EXE_PENDING;
error:
    return EXE_FAILURE;
}

exe_request_result_t exe_cbs_unsubscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p, exe_cscb_data_t *cscb_data_p)
{
    SMS_RequestControlBlock_t  *request_ctrl_block_p    = NULL;
    SMS_RequestStatus_t         request_status          = SMS_REQUEST_OK;
    CBS_Error_t                 cbs_error               = CBS_ERROR_NONE;

    if (NULL == smsclient_p) {
        ATC_LOG_E("exe_cbs_unsubscribe: smsclient_p is NULL!");
        goto error;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("exe_cbs_unsubscribe: smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    request_status = Request_CBS_Unsubscribe(
                         request_ctrl_block_p,
                         0, /* don't unsubscribe to all */
                         cscb_data_p->num_of_subs,
                         (CBS_Subscription_t *)cscb_data_p->subscription_p,
                         &cbs_error);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (SMS_REQUEST_PENDING != request_status ||
            CBS_ERROR_NONE != cbs_error) {
        print_cbs_error("exe_cbs_unsubscribe: Request_CBS_Unsubscribe failed --> ", cbs_error);
        goto error;
    }

    return EXE_PENDING;
error:
    return EXE_FAILURE;

}

exe_request_result_t exe_cbs_activate_subscription(smsclient_t *smsclient_p, exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t  *request_ctrl_block_p  = NULL;
    SMS_RequestStatus_t         request_status        = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error             = SMS_ERROR_INTERN_NO_ERROR;
    CBS_Error_t                 cbs_error             = CBS_ERROR_NONE;

    if (NULL == smsclient_p) {
        ATC_LOG_E("exe_cbs_activate_subscription: smsclient_p is NULL");
        goto error;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("exe_cbs_activate_subscription: smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    request_status = Request_CBS_Activate(request_ctrl_block_p, &cbs_error);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (SMS_REQUEST_PENDING != request_status ||
            CBS_ERROR_NONE != cbs_error) {
        print_cbs_error("exe_cbs_activate_subscription: Request_CBS_Activate failed --> ", cbs_error);
        goto error;
    }

    return EXE_PENDING;
error:
    return EXE_FAILURE;
}

exe_request_result_t exe_cbs_deactivate_subcription(smsclient_t *smsclient_p, exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t  *request_ctrl_block_p  = NULL;
    SMS_RequestStatus_t         request_status        = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error             = SMS_ERROR_INTERN_NO_ERROR;
    CBS_Error_t                 cbs_error             = CBS_ERROR_NONE;

    if (NULL == smsclient_p) {
        ATC_LOG_E("exe_cbs_deactivate_subcription: smsclient_p is NULL");
        goto error;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("exe_cbs_deactivate_subcription: smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    request_status = Request_CBS_Deactivate(request_ctrl_block_p, &cbs_error);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (SMS_REQUEST_PENDING != request_status ||
            CBS_ERROR_NONE != cbs_error) {
        print_cbs_error("exe_cbs_deactivate_subcription: Request_CBS_Deactivate failed --> ", cbs_error);
        goto error;
    }

    return EXE_PENDING;
error:
    return EXE_FAILURE;
}

exe_request_result_t exe_cbs_get_number_of_subcriptions(smsclient_t *smsclient_p, exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t  *request_ctrl_block_p  = NULL;
    SMS_RequestStatus_t         request_status        = SMS_REQUEST_OK;
    CBS_Error_t                 cbs_error             = CBS_ERROR_NONE;
    CBS_Subscription_t         *subscription_p        = NULL;

    if (NULL == smsclient_p) {
        ATC_LOG_E("exe_cbs_get_number_of_subcriptions: smsclient_p is NULL");
        goto error;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("exe_cbs_get_number_of_subcriptions: smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    request_status = Request_CBS_GetNumberOfSubscriptions(
                         request_ctrl_block_p,
                         0, /* this client subscriptions */
                         NULL,
                         &cbs_error);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (SMS_REQUEST_PENDING != request_status ||
            CBS_ERROR_NONE != cbs_error) {
        print_cbs_error("exe_cbs_get_number_of_subcriptions: Request_CBS_GetNumberOfSubscriptions failed --> ", cbs_error);
        goto error;
    }

    return EXE_PENDING;
error:
    return EXE_FAILURE;
}

exe_request_result_t exe_cbs_get_subscriptions(smsclient_t *smsclient_p,
        exe_request_record_t *record_p,
        uint8_t number_of_subscriptions)
{
    SMS_RequestControlBlock_t  *request_ctrl_block_p  = NULL;
    SMS_RequestStatus_t         request_status        = SMS_REQUEST_OK;
    CBS_Error_t                 cbs_error             = CBS_ERROR_NONE;
    CBS_Subscription_t         *subscription_p        = NULL;

    if (NULL == smsclient_p) {
        ATC_LOG_E("exe_cbs_get_number_of_subcriptions: smsclient_p is NULL");
        goto error;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("exe_cbs_get_number_of_subcriptions: smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    request_status = Request_CBS_GetSubscriptions(
                         request_ctrl_block_p,
                         0, /* this client subscriptions */
                         &number_of_subscriptions,
                         NULL, /* Subscriptions_p not needed for request in NO_WAIT_MODE */
                         &cbs_error);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (SMS_REQUEST_PENDING != request_status ||
            CBS_ERROR_NONE != cbs_error) {
        print_cbs_error("exe_cbs_get_number_of_subcriptions: Request_CBS_GetSubscriptions failed --> ", cbs_error);
        goto error;
    }

    return EXE_PENDING;
error:
    return EXE_FAILURE;
}

exe_request_result_t request_cbs_set_subscription(exe_request_record_t *record_p)
{
    exe_request_result_t result = EXE_FAILURE;
    exe_cscb_data_t *cscb_data_p = NULL;
    CBS_Error_t cbs_error = CBS_ERROR_NONE;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;
    smsclient_t *smsclient_p = exe_get_smsclient(record_p->exe_p);
    exe_cscb_client_data_t *client_data_p = record_p->client_data_p;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_cbs_set_subscription, incorrect in data");
            goto exit;
        }

        cscb_data_p = ((exe_cscb_data_t *) record_p->request_data_p);

        client_data_p = calloc(1, sizeof(exe_cscb_client_data_t));

        if (NULL == client_data_p) {
            ATC_LOG_E("request_cbs_set_subscription: client_data allocation failed");
            goto exit;
        }

        record_p->client_data_p = client_data_p;

        client_data_p->new_cscb_data.subscription_p = (exe_cscb_subscription_t *)
                calloc(cscb_data_p->num_of_subs, sizeof(exe_cscb_subscription_t));

        if (NULL == client_data_p->new_cscb_data.subscription_p) {
            ATC_LOG_E("request_cbs_set_subscription: subscription_p allocation failed");
            goto exit;
        }

        client_data_p->new_cscb_data.num_of_subs = cscb_data_p->num_of_subs;
        memcpy(client_data_p->new_cscb_data.subscription_p, cscb_data_p->subscription_p,
               cscb_data_p->num_of_subs * sizeof(exe_cscb_subscription_t));

        result = exe_cbs_get_number_of_subcriptions(smsclient_p, record_p);

        if (EXE_PENDING == result) {
            record_p->state = EXE_STATE_MSG_CBS_NUMBER_OF_SUBSRIPTIONS_RESPONSE;
        }

        break;
    }
    case EXE_STATE_MSG_CBS_NUMBER_OF_SUBSRIPTIONS_RESPONSE: {
        uint8_t number_of_subscriptions = 0;
        request_status = Response_CBS_GetNumberOfSubscriptions(record_p->response_data_p,
                         &number_of_subscriptions, &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {

            if (CBS_ERROR_INVALID_SUBSCRIPTION == cbs_error) {
                /* CBS_ERROR_INVALID_SUBSCRIPTION - no subscripptions exists */
                number_of_subscriptions = 0;
            } else {
                print_cbs_error("request_cbs_set_subscription, Response_CBS_GetNumberOfSubscriptions failed --> ",
                                cbs_error);

                goto exit;
            }
        }

        if (0 == number_of_subscriptions) {
            if (0 == client_data_p->new_cscb_data.num_of_subs) {
                result = EXE_SUCCESS;
                goto exit;
            }

            result = exe_cbs_subscribe(smsclient_p, record_p, &(client_data_p->new_cscb_data));

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_CBS_SUBSCRIBE_RESPONSE;
            }

        } else {
            client_data_p->prev_cscb_data.num_of_subs = number_of_subscriptions;
            result = exe_cbs_get_subscriptions(smsclient_p, record_p, number_of_subscriptions);

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_CBS_GET_SUBSRIPTIONS_RESPONSE;
            }
        }

        break;
    }
    case EXE_STATE_MSG_CBS_GET_SUBSRIPTIONS_RESPONSE: {
        client_data_p->prev_cscb_data.subscription_p =
            calloc(client_data_p->prev_cscb_data.num_of_subs, sizeof(exe_cscb_subscription_t));

        if (NULL == client_data_p->prev_cscb_data.subscription_p) {
            ATC_LOG_E("request_cbs_set_subscription: subscription_p allocation failed");
            goto exit;
        }

        request_status = Response_CBS_GetSubscriptions(
                             record_p->response_data_p,
                             &(client_data_p->prev_cscb_data.num_of_subs),
                             (CBS_Subscription_t *)client_data_p->prev_cscb_data.subscription_p,
                             &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {
            print_cbs_error("request_cbs_set_subscription, Response_CBS_GetSubscriptions failed --> ",
                            cbs_error);
            goto exit;
        }

        result = exe_cbs_unsubscribe(smsclient_p, record_p, &(client_data_p->prev_cscb_data));

        if (EXE_PENDING == result) {
            record_p->state = EXE_STATE_MSG_CBS_UNSUBSCRIBE_RESPONSE;
        }

        break;
    }
    case EXE_STATE_MSG_CBS_UNSUBSCRIBE_RESPONSE: {

        request_status = Response_CBS_Unsubscribe(record_p->response_data_p, &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {
            print_cbs_error("request_cbs_set_subscription, Response_CBS_Unsubscribe failed --> ",
                            cbs_error);
            goto exit;
        }

        if (0 == client_data_p->new_cscb_data.num_of_subs) {
            result = EXE_SUCCESS;
            goto exit;
        }

        result = exe_cbs_subscribe(smsclient_p, record_p, &(client_data_p->new_cscb_data));

        if (EXE_PENDING == result) {
            record_p->state = EXE_STATE_MSG_CBS_SUBSCRIBE_RESPONSE;
        }

        break;
    }
    case EXE_STATE_MSG_CBS_SUBSCRIBE_RESPONSE: {
        request_status = Response_CBS_Subscribe(record_p->response_data_p, &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {
            print_cbs_error("request_cbs_set_subscription, Response_CBS_Subscribe failed --> ",
                            cbs_error);
            goto exit;
        }

        result = EXE_SUCCESS;
        break;
    }
    default:
        goto exit;
    }

exit:

    if (EXE_STATE_REQUEST != record_p->state &&
            EXE_PENDING != result) {
        exe_request_complete(record_p, result, NULL);

        if (NULL != client_data_p) {
            free(client_data_p->new_cscb_data.subscription_p);
            free(client_data_p->prev_cscb_data.subscription_p);
            free(client_data_p);
        }
    }

    return result;
}

exe_request_result_t request_cbs_get_subscription(exe_request_record_t *record_p)
{
    exe_request_result_t result = EXE_FAILURE;
    exe_t *exe_p = NULL;
    smsclient_t *smsclient_p = NULL;
    CBS_Error_t cbs_error = CBS_ERROR_NONE;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;
    exe_cscb_data_t *client_data_p = record_p->client_data_p;

    exe_p = record_p->exe_p;

    if (NULL == exe_p) {
        ATC_LOG_E("request_cbs_get_subscription: exe_p is NULL");
        goto exit;
    }

    smsclient_p = exe_get_smsclient(exe_p);

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {

        result = exe_cbs_get_number_of_subcriptions(smsclient_p, record_p);

        if (EXE_PENDING == result) {
            record_p->state = EXE_STATE_MSG_CBS_NUMBER_OF_SUBSRIPTIONS_RESPONSE;
        }

        break;
    }
    case EXE_STATE_MSG_CBS_NUMBER_OF_SUBSRIPTIONS_RESPONSE: {
        uint8_t number_of_subscriptions = 0;
        request_status = Response_CBS_GetNumberOfSubscriptions(record_p->response_data_p,
                         &number_of_subscriptions, &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {

            if (CBS_ERROR_INVALID_SUBSCRIPTION == cbs_error) {
                /* CBS_ERROR_INVALID_SUBSCRIPTION - no subscripptions exists */
                number_of_subscriptions = 0;
            } else {
                print_cbs_error("request_cbs_get_subscription, Response_CBS_GetNumberOfSubscriptions failed --> ",
                                cbs_error);

                goto exit;
            }
        }

        if (number_of_subscriptions > 0) {

            client_data_p = calloc(1, sizeof(exe_cscb_data_t));

            if (NULL == client_data_p) {
                ATC_LOG_E("request_cbs_get_subscription: client_data allocation failed");
                goto exit;
            }

            record_p->client_data_p = client_data_p;

            client_data_p->subscription_p = calloc(number_of_subscriptions, sizeof(exe_cscb_subscription_t));

            if (NULL == client_data_p->subscription_p) {
                ATC_LOG_E("request_cbs_get_subscription: subscription_p allocation failed");
                goto exit;
            }

            client_data_p->num_of_subs = number_of_subscriptions;
            result = exe_cbs_get_subscriptions(smsclient_p, record_p, number_of_subscriptions);

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_CBS_GET_SUBSRIPTIONS_RESPONSE;
            }
        } else {
            result = EXE_SUCCESS;
        }

        break;
    }
    case EXE_STATE_MSG_CBS_GET_SUBSRIPTIONS_RESPONSE: {
        request_status = Response_CBS_GetSubscriptions(
                             record_p->response_data_p,
                             &(client_data_p->num_of_subs),
                             (CBS_Subscription_t *)client_data_p->subscription_p,
                             &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {
            print_cbs_error("request_cbs_set_subscription, Response_CBS_GetSubscriptions failed --> ",
                            cbs_error);
            goto exit;
        }

        result = EXE_SUCCESS;
        break;
    }
    default:
        goto exit;
    }

exit:

    if (EXE_STATE_REQUEST != record_p->state &&
            EXE_PENDING != result) {
        exe_request_complete(record_p, result, client_data_p);

        if (NULL != client_data_p) {
            free(client_data_p->subscription_p);
            free(client_data_p);
        }
    }

    return result;
}
