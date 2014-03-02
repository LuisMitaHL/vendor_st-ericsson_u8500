/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r_sms.h"
#include "r_cbs.h"
#include "t_cbs.h"

#include "exe_internal.h"
#include "exe_request_record_p.h"
#include "exe_sms_client.h"
#include "exe_sms_utils.h"
#include "exe_sms_requests.h"
#include "exe_sms_requests_cbs.h"

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include "exe_extended_sms_utils.h"
#endif

/* static const fields */

/*
 * Hard-coded ACK delivery report.
 * Data part description:
 *
 *    ____ Message Type Indicator
 *   |      ____ Parameter Indicator 00000000 indicates no optional fields present
 *   |     |
 * {0x00, 0x00}
 */
static const SMS_TPDU_t DeliverReportTPDU_ACK = {
    0x02,        /* Length */
    {0x00, 0x00} /* Data   */
};

/*
 * Hard-coded NACK delivery report.
 * Data part description:
 *
 *      ____ Message Type Indicator
 *     |      ____ Failure Cause
 *     |     |      ____ Parameter Indicator 00000000 indicates no optional fields present
 *     |     |     |
 *   {0x00, 0xFF, 0x00}
 */
static const SMS_TPDU_t DeliverReportTPDU_NACK = {
    0x03,              /* Length */
    {0x00, 0xFF, 0x00} /* Data   */
};

/*
 * Convert pdu to bin and sends a sms
 */
exe_request_result_t request_sms_send(exe_request_record_t *record_p)
{
    exe_t                       *exe_p       = record_p->exe_p;
    smsclient_t                 *smsclient_p = NULL;
    exe_request_result_t        result    = EXE_SUCCESS;
    exe_cmgs_t                  *req_data_p = (exe_cmgs_t *)record_p->request_data_p;
    unsigned char               *pdu_data = NULL;
    uint8_t                     sm_reference = 0;
    SMS_RequestControlBlock_t  *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_SMSC_Address_TPDU_t     tpdu;

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("request_sms_send: smsclient_p is NULL");
        goto error;
    }

    if (NULL == req_data_p) {
        ATC_LOG_E("request_sms_send: missing data");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state)  {
        unsigned char          sc_address_length = 0;
        bool expect_more = false;
        pdu_data = req_data_p->pdu_data_p;

        /* Get the length of SCA - service center address. */
        sc_address_length = (unsigned char)pdu_data[0];

        if (sc_address_length > req_data_p->length) {
            goto error;
        }

        memset(&tpdu, 0, sizeof(SMS_SMSC_Address_TPDU_t));

        /* Copy the SCA if any */
        if (sc_address_length > 0) {
            memmove(&tpdu.ServiceCenterAddress, pdu_data, sc_address_length + 1);
        }

        /* Copy the TP data. */
        memmove(tpdu.TPDU.Data, pdu_data + sc_address_length + 1, req_data_p->length);
        tpdu.TPDU.Length = (req_data_p->length);

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                               (void *)request_record_get_client_tag(record_p));

        request_status = Request_SMS_ShortMessageSend(request_ctrl_block_p,
                         &tpdu,
                         NULL, /* Slot */
                         expect_more,
                         &sm_reference,
                         &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            print_sms_error
            ("request_sms_send, Request_SMS_ShortMessageSend failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        exe_cmgs_t response_data;
        request_status = Response_SMS_ShortMessageSend(record_p->response_data_p, &sm_reference, &sms_error);
        memset(&response_data, 0, sizeof(response_data));
        response_data.mr = sm_reference;

        if (request_status != SMS_REQUEST_FAILED_PARAMETER && request_status != SMS_REQUEST_FAILED_UNPACKING) {
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
            response_data.error = mo_sms_error_to_cms(sms_error);
#else
            response_data.error = SMS_ErrorToCMS(sms_error);
#endif
        }

        if (request_status == SMS_REQUEST_OK && sms_error == SMS_ERROR_INTERN_NO_ERROR) {
            result = EXE_SUCCESS;
        } else {
            result = EXE_FAILURE;
            print_sms_error("request_sms_send, Response_SMS_ShortMessageSend failed --> ",
                            sms_error);
        }

        exe_request_complete(record_p, result, &response_data);
        return result;
    }

error:
    return EXE_FAILURE;
}

exe_request_result_t request_sms_write(exe_request_record_t *record_p)
{
    exe_t                       *exe_p = record_p->exe_p;
    smsclient_t                 *smsclient_p = NULL;
    exe_request_result_t        result = EXE_SUCCESS;
    exe_cmgw_t                  *req_data_p = (exe_cmgw_t *)record_p->request_data_p;
    unsigned char               *pdu_data = NULL;
    SMS_Slot_t                  slot;
    SMS_RequestControlBlock_t   *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_SMSC_Address_TPDU_t     tpdu;
    SMS_Status_t                sms_status = SMS_STATUS_UNSENT;

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("request_sms_write: smsclient_p is NULL");
        goto error;
    }

    if (NULL == req_data_p) {
        ATC_LOG_E("request_sms_write: missing data");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state)  {
        unsigned char   sc_address_length = 0;
        bool            expect_more = false;

        pdu_data = req_data_p->pdu_data_p;

        /* Get the length of SCA - service center address. */
        sc_address_length = (unsigned char)pdu_data[0];

        if (sc_address_length > req_data_p->length) {
            ATC_LOG_E("request_sms_write: address length is wrong");
            goto error;
        }

        memset(&tpdu, 0, sizeof(SMS_SMSC_Address_TPDU_t));

        /* Copy the SCA if any */
        if (sc_address_length > 0) {
            memmove(&tpdu.ServiceCenterAddress, pdu_data, sc_address_length + 1);
        }

        memmove(tpdu.TPDU.Data, pdu_data + sc_address_length + 1, req_data_p->length);
        tpdu.TPDU.Length = (req_data_p->length);
        slot.Position = (SMS_Position_t)req_data_p->slot.position;
        slot.Storage = (SMS_Storage_t)req_data_p->slot.storage;

        switch (req_data_p->stat) {
        case 0:
            sms_status = SMS_STATUS_UNREAD;
            break;
        case 1:
            sms_status = SMS_STATUS_READ;
            break;
        case 2:
            sms_status = SMS_STATUS_UNSENT;
            break;
        case 3:
            sms_status = SMS_STATUS_SENT;
            break;
        case 16:
            /* No correct status yet */
            /* fall through*/
        default:
            goto error;
        }

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                               (void *)request_record_get_client_tag(record_p));

        request_status = Request_SMS_ShortMessageWrite(request_ctrl_block_p,
                         sms_status,
                         &tpdu,
                         &slot,
                         &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            print_sms_error
            ("request_sms_send, Request_SMS_ShortMessageSend failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        exe_cmgw_t response_data;
        request_status = Response_SMS_ShortMessageWrite(record_p->response_data_p, &slot, &sms_error);
        memset(&response_data, 0, sizeof(response_data));
        response_data.slot.position = (exe_msg_position_t)slot.Position;

        if (SMS_REQUEST_FAILED_PARAMETER != request_status  && SMS_REQUEST_FAILED_UNPACKING != request_status) {
            response_data.error = SMS_ErrorToCMS(sms_error);
        }

        if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_NO_ERROR == sms_error) {
            result = EXE_SUCCESS;
        } else {
            result = EXE_FAILURE;
            print_sms_error("request_sms_send, Response_SMS_ShortMessageSend failed --> ",
                            sms_error);
        }

        exe_request_complete(record_p, result, &response_data);
        return result;
    }

error:
    return EXE_FAILURE;
}

/* Maps against 3ggp 27005-900 4.6. */
typedef enum {
    SMS_ACK_RESULT_TEXT = 0,
    SMS_ACK_RESULT_SUCCESS = 1,
    SMS_ACK_RESULT_FAILURE = 2
} sms_ack_result;


exe_request_result_t request_select_message_service_set(exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    exe_request_result_t result = EXE_SUCCESS;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;

    if (EXE_STATE_REQUEST == record_p->state) {
        exe_t *exe_p = record_p->exe_p;
        exe_csms_t *req_data = (exe_csms_t *) record_p->request_data_p;
        smsclient_t *smsclient_p = NULL;
        SMS_NetworkAcknowledge_t sms_ack = SMS_NETWORK_ACKNOWLEDGE_NORMAL;

        if (NULL == req_data) {
            ATC_LOG_E("request_select_message_service_set: req_data is NULL");
            goto error;
        }

        /* at parser ensures that req_data->n will never have any illegal values */
        sms_ack = (SMS_NetworkAcknowledge_t)req_data->n;

        smsclient_p = exe_get_smsclient(exe_p);

        if (NULL == smsclient_p) {
            ATC_LOG_E("request_select_message_service_set: smsclient_p is NULL!");
            goto error;
        }

        request_ctrl_block_p =
            smsclient_alloc_requestctrlblock(smsclient_p,
                                             (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_select_message_service_set: request_ctrl_block_p is null");
            goto error;
        }

        request_status = R_Req_SMS_DeliverReportControlSet(request_ctrl_block_p,
                         sms_ack, &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            print_sms_error("request_select_message_service_set, R_Req_SMS_DeliverReportControlSet failed --> ", sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;

        return EXE_PENDING;
    } else {
        request_status = R_Resp_SMS_DeliverReportControlSet(record_p->response_data_p, &sms_error);

        if (request_status == SMS_REQUEST_OK && sms_error == SMS_ERROR_INTERN_NO_ERROR) {
            result = EXE_SUCCESS;
        } else {
            result = EXE_FAILURE;
        }

        exe_request_complete(record_p, result, NULL);
        return result;
    }

error:
    return EXE_FAILURE;
}

exe_request_result_t request_select_message_service_get(exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    exe_request_result_t result = EXE_SUCCESS;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;

    if (EXE_STATE_REQUEST == record_p->state) {
        exe_t *exe_p = record_p->exe_p;
        smsclient_t *smsclient_p = NULL;
        SMS_NetworkAcknowledge_t sms_ack = SMS_NETWORK_ACKNOWLEDGE_NORMAL;

        smsclient_p = exe_get_smsclient(exe_p);

        if (NULL == smsclient_p) {
            ATC_LOG_E("request_select_message_service_get: smsclient_p is NULL!");
            goto error;
        }

        request_ctrl_block_p =
            smsclient_alloc_requestctrlblock(smsclient_p,
                                             (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_select_message_service_get: ctrl_block is null");
            goto error;
        }

        request_status = R_Req_SMS_DeliverReportControlGet(request_ctrl_block_p,
                         &sms_ack, &sms_error);
        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            print_sms_error("request_select_message_service_get, R_Req_SMS_DeliverReportControlGet failed --> ", sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;

        return EXE_PENDING;
    } else {
        SMS_NetworkAcknowledge_t read_value = SMS_NETWORK_ACKNOWLEDGE_NORMAL;
        request_status = R_Resp_SMS_DeliverReportControlGet(record_p->response_data_p, &read_value, &sms_error);

        if (request_status == SMS_REQUEST_OK && sms_error == SMS_ERROR_INTERN_NO_ERROR) {
            exe_csms_t *client_data_p = (exe_csms_t *) record_p->client_data_p;
            client_data_p = (exe_csms_t *)calloc(1, sizeof(exe_csms_t));

            if (NULL == client_data_p) {
                ATC_LOG_E("request_select_message_service_get: client_data is NULL. Failed to allocate memory!!!");
                goto error;
            }

            client_data_p->n = read_value;
            exe_request_complete(record_p, result, client_data_p);
            free(client_data_p);
            result = EXE_SUCCESS;
        } else {
            result = EXE_FAILURE;
            exe_request_complete(record_p, result, NULL);
        }

        return result;
    }

error:
    return EXE_FAILURE;
}

exe_request_result_t request_sms_send_ack(exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    exe_request_result_t result = EXE_SUCCESS;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_TPDU_t tpdu;
    unsigned char *pdu_data = NULL;

    if (EXE_STATE_REQUEST == record_p->state) {
        exe_t *exe_p = record_p->exe_p;
        smsclient_t *smsclient_p = NULL;
        exe_cnma_t *req_data = (exe_cnma_t *) record_p->request_data_p;
        SMS_RP_ErrorCause_t rp_error_cause = 0;

        if (NULL == req_data) {
            ATC_LOG_E("request_sms_send_ack: req_data is NULL");
            goto error;
        }

        smsclient_p = exe_get_smsclient(exe_p);

        if (NULL == smsclient_p) {
            ATC_LOG_E("request_sms_send_ack: smsclient_p is NULL!");
            goto error;
        }

        pdu_data = req_data->pdu_data_p;
        memset(&tpdu, 0, sizeof(SMS_TPDU_t));

        if (SMS_ACK_RESULT_FAILURE == req_data->n) {
            rp_error_cause = SMS_ERROR_RP_TEMPORARY_FAILURE;
            tpdu = (SMS_TPDU_t) DeliverReportTPDU_NACK;
        } else  if (SMS_ACK_RESULT_SUCCESS == req_data->n || SMS_ACK_RESULT_TEXT == req_data->n) {
            rp_error_cause = 0;
            tpdu = (SMS_TPDU_t) DeliverReportTPDU_ACK;
        } else {
            ATC_LOG_E("request_sms_send_ack: unsupported argument");
            goto error;
        }

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                               (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_sms_send_ack: ctrl_block is null");
            goto error;
        }

        request_status = Request_SMS_DeliverReportSend(request_ctrl_block_p,
                         rp_error_cause, &tpdu,
                         &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            print_sms_error
            ("request_sms_send_ack, Request_SMS_DeliverReportSend failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;

        return EXE_PENDING;
    } else {
        request_status = Response_SMS_DeliverReportSend(record_p->response_data_p, &sms_error);

        if (request_status == SMS_REQUEST_OK && sms_error == SMS_ERROR_INTERN_NO_ERROR) {
            result = EXE_SUCCESS;
        } else {
            result = EXE_FAILURE;
        }

        exe_request_complete(record_p, result, NULL);
        return result;
    }

    return EXE_SUCCESS;
error:
    return EXE_FAILURE;
}

exe_request_result_t request_message_event_subscribe(exe_request_record_t *record_p)
{
    exe_t *exe_p = NULL;
    smsclient_t *smsclient_p = NULL;
    exe_message_subscribe_t *message_subscribe_p = NULL;
    exe_message_subscribe_t *client_data_p = NULL;
    exe_request_result_t result = EXE_FAILURE;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    CBS_Error_t cbs_error = CBS_ERROR_NONE;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;

    exe_p = record_p->exe_p;

    if (NULL == exe_p) {
        ATC_LOG_E("request_message_event_subscribe: exe_p is NULL");
        result = EXE_FAILURE;
        goto exit;
    }

    smsclient_p = exe_get_smsclient(exe_p);
    client_data_p = (exe_message_subscribe_t *)record_p->client_data_p;

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        client_data_p = calloc(1, sizeof(exe_message_subscribe_t));

        if (NULL == client_data_p) {
            ATC_LOG_E("request_message_event_subscribe: client_data is NULL. Failed to allocate memory!!!");
            goto exit;
        }

        record_p->client_data_p = client_data_p;
        message_subscribe_p = (exe_message_subscribe_t *)record_p->request_data_p;
        client_data_p->sms = message_subscribe_p->sms;
        client_data_p->cbs = message_subscribe_p->cbs;

        if (!smsclient_get_sms_subscription(smsclient_p) && client_data_p->sms) {
            result = smsclient_sms_event_subscribe(smsclient_p, record_p);

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_SMS_EVENT_SUBSCRIBE_RESPONSE;
            }

            goto exit;
        } else if (client_data_p->cbs) {
            result = exe_cbs_activate_subscription(smsclient_p, record_p);

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_CBS_ACTIVATE_RESPONSE;
            }

            goto exit;
        } else if (!client_data_p->cbs) {

            result = exe_cbs_deactivate_subcription(smsclient_p, record_p);

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_CBS_DEACTIVATE_RESPONSE;
            }

            goto exit;
        }

        break;
    }
    case EXE_STATE_MSG_SMS_EVENT_SUBSCRIBE_RESPONSE: {

        request_status = Response_SMS_EventsSubscribe(record_p->response_data_p, &sms_error);

        if (SMS_REQUEST_OK != request_status ||
                SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_message_event_subscribe, Response_SMS_EventsSubscribe failed --> ",
                            sms_error);
            client_data_p->sms = false;
            client_data_p->cbs = false;

            result = EXE_FAILURE;
            goto exit;
        }

        smsclient_set_sms_subscription(smsclient_p, true);

        if (client_data_p->cbs) {

            result = exe_cbs_activate_subscription(smsclient_p, record_p);

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_CBS_ACTIVATE_RESPONSE;
            }

            goto exit;
        }

        break;
    }
    case EXE_STATE_MSG_CBS_ACTIVATE_RESPONSE: {
        request_status = Response_CBS_Activate(record_p->response_data_p, &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {
            print_cbs_error("request_message_event_subscribe, Response_CBS_Subscribe failed --> ",
                            cbs_error);
            client_data_p->cbs = false;
            result = EXE_FAILURE;
            goto exit;
        }

        break;
    }
    case EXE_STATE_MSG_CBS_DEACTIVATE_RESPONSE: {
        request_status = Response_CBS_Deactivate(record_p->response_data_p, &cbs_error);

        if (SMS_REQUEST_OK != request_status ||
                CBS_ERROR_NONE != cbs_error) {
            print_cbs_error("request_message_event_subscribe, Response_CBS_Deactivate failed --> ",
                            cbs_error);
            client_data_p->cbs = false;
            result = EXE_FAILURE;
            goto exit;
        }

        if (smsclient_get_sms_subscription(smsclient_p) && !client_data_p->sms) {
            result = smsclient_sms_event_unsubscribe(smsclient_p, record_p);

            if (EXE_PENDING == result) {
                record_p->state = EXE_STATE_MSG_SMS_EVENT_UNSUBSCRIBE_RESPONSE;
            }

            goto exit;
        }

        break;
    }
    case EXE_STATE_MSG_SMS_EVENT_UNSUBSCRIBE_RESPONSE: {
        request_status = Response_SMS_EventsUnsubscribe(record_p->response_data_p, &sms_error);

        if (SMS_REQUEST_OK != request_status ||
                SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_message_event_subscribe, Response_SMS_EventsUnsubscribe failed --> ",
                            sms_error);
            client_data_p->sms = false;
            client_data_p->cbs = false;

            result = EXE_FAILURE;
            goto exit;
        }

        smsclient_set_sms_subscription(smsclient_p, false);
        break;
    }
    default:
        goto exit;
    }

    result = EXE_SUCCESS;
exit:

    if (EXE_STATE_REQUEST != record_p->state &&
            EXE_PENDING != result) {
        exe_request_complete(record_p, result, client_data_p);
        free(client_data_p);
    }

    return result;
}

exe_request_result_t request_sms_memory_capacity(exe_request_record_t *record_p)
{
    exe_t *exe_p = NULL;
    smsclient_t *smsclient_p = NULL;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    exe_request_result_t result = EXE_FAILURE;

    exe_p = record_p->exe_p;

    if (NULL == exe_p) {
        ATC_LOG_E("request_sms_memory_capacity: exe_p is NULL");
        goto error;
    }

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("request_sms_memory_capacity: smsclient_p is NULL");
        goto error;
    }

    if ((EXE_STATE_REQUEST == record_p->state)) { /* REQUEST PART */
        exe_esmsfull_t *request_data_p = (exe_esmsfull_t *)record_p->request_data_p;

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_sms_memory_capacity: request_data_p NULL");
            goto error;
        }

        if (0 == request_data_p->full) {
            request_ctrl_block_p =
                smsclient_alloc_requestctrlblock(smsclient_p,
                                                 (void *)request_record_get_client_tag(record_p));

            if (NULL == request_ctrl_block_p) {
                ATC_LOG_E("request_sms_memory_capacity: request_ctrl_block_p NULL");
                goto error;
            }

            request_status = Request_SMS_MemCapacityAvailableSend(request_ctrl_block_p, &sms_error);
            smsclient_free_requestctrlblock(&request_ctrl_block_p);

            if (SMS_REQUEST_PENDING != request_status) {
                print_sms_error(
                    "request_sms_memory_capacity, Request_SMS_MemCapacityAvailableSend failed --> ",
                    sms_error);
                goto error;
            }

            record_p->state = EXE_STATE_RESPONSE;
            result = EXE_PENDING;
        } else {
            request_ctrl_block_p =
                smsclient_alloc_requestctrlblock(smsclient_p,
                                                 (void *)request_record_get_client_tag(record_p));

            request_status = Request_SMS_MemCapacityFullReport(request_ctrl_block_p, &sms_error);
            smsclient_free_requestctrlblock(&request_ctrl_block_p);

            if (SMS_REQUEST_PENDING != request_status) {
                print_sms_error(
                    "request_sms_memory_capacity, Request_SMS_MemCapacityFullReport failed --> ",
                    sms_error);
                goto error;
            }

            result = EXE_SUCCESS;
        }

    } else {
        request_status = Response_SMS_MemCapacityAvailableSend(record_p->response_data_p, &sms_error);

        if (request_status == SMS_REQUEST_OK && (sms_error == SMS_ERROR_INTERN_NO_ERROR
                    || sms_error == SMS_ERROR_INTERN_NO_ERROR_NO_ACTION)) {
            result = EXE_SUCCESS;
        } else {
            result = EXE_FAILURE;
            print_sms_error("request_sms_memory_capacity, Response_SMS_MemCapacityAvailableSend failed --> ",
                            sms_error);
        }

        exe_request_complete(record_p, result, NULL);
    }

error:
    return result;
}

exe_request_result_t request_sms_memory_capacity_get(exe_request_record_t *record_p)
{
    exe_esmsfull_read_t esmsfull_read_res;
    exe_t *exe_p = NULL;
    smsclient_t *smsclient_p = NULL;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    exe_request_result_t result = EXE_FAILURE;

    exe_p = record_p->exe_p;

    if (NULL == exe_p) {
        ATC_LOG_E("exe_p is NULL");
        goto error;
    }

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("smsclient_p is NULL");
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {
        ATC_LOG_D("-> request");

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                                   (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_ctrl_block_p NULL");
            goto error;
        }

        request_status = Request_SMS_MemCapacityGetState(request_ctrl_block_p, &(esmsfull_read_res.memfull), &sms_error);
        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status) {
            print_sms_error(
                "request_sms_memory_capacity_get, Request_SMS_MemCapacityAvailableSend failed --> ",
                sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;
        return EXE_PENDING;
    } else {
        ATC_LOG_D("<- response");

        request_status = Response_SMS_MemCapacityGetState(record_p->response_data_p, &(esmsfull_read_res.memfull), &sms_error);

        if (request_status == SMS_REQUEST_OK && (sms_error == SMS_ERROR_INTERN_NO_ERROR
                    || sms_error == SMS_ERROR_INTERN_NO_ERROR_NO_ACTION)) {
            result = EXE_SUCCESS;
        } else {
            result = EXE_FAILURE;
            print_sms_error("request_sms_memory_capacity, Response_SMS_MemCapacityAvailableSend failed --> ",
                            sms_error);
        }
        exe_request_complete(record_p, result, &esmsfull_read_res);
    }

error:
    return result;
}

static void get_storage_info_from_slot(SMS_SlotInformation_t slot_information, exe_msg_storage_info_t *storage_info_p)
{
    storage_info_p->total_space =
        slot_information.ReadCount
        + slot_information.UnreadCount
        + slot_information.SentCount
        + slot_information.UnsentCount
        + slot_information.FreeCount;

    storage_info_p->used_space =
        slot_information.ReadCount
        + slot_information.UnreadCount
        + slot_information.SentCount
        + slot_information.UnsentCount;
}


exe_request_result_t request_sms_storage_info(exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    exe_request_result_t result = EXE_SUCCESS;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    exe_t *exe_p = NULL;
    smsclient_t *smsclient_p = NULL;
    SMS_SlotInformation_t slot_information;
    SMS_StorageStatus_t storage_status;
    exe_cpms_msg_storage_area_t *req_data_p = NULL;
    exe_cpms_msg_storage_area_t *client_data_p = NULL;

    exe_p = record_p->exe_p;
    req_data_p = (exe_cpms_msg_storage_area_t *)(record_p->request_data_p);

    if (NULL == req_data_p) {
        ATC_LOG_E("request_sms_storage_info: req_data_p is NULL");
        goto error;
    }

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("request_sms_storage_info: smsclient_p is NULL!");
        goto error;
    }

    request_ctrl_block_p =
        smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("request_sms_storage_info: ctrl_block is null");
        goto error;
    }

    client_data_p = (exe_cpms_msg_storage_area_t *)record_p->client_data_p;

    if (NULL == client_data_p &&
            EXE_STATE_REQUEST != record_p->state) {
        ATC_LOG_E("request_sms_storage_info: client_data is NULL");
        goto error;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        if (NULL != client_data_p) {
            ATC_LOG_I("request_sms_storage_info: client_data is not NULL on entry. Freeing old data.");
            free(client_data_p);
        }

        client_data_p = (exe_cpms_msg_storage_area_t *)calloc(1, sizeof(exe_cpms_msg_storage_area_t));

        if (NULL == client_data_p) {
            ATC_LOG_E("request_sms_storage_info: client_data is NULL. Failed to allocate memory!!!");
            goto error;
        }

        record_p->client_data_p = client_data_p;

        client_data_p->mem1 = req_data_p->mem1;
        client_data_p->mem2 = req_data_p->mem2;
        client_data_p->mem3 = req_data_p->mem3;

        request_status =
            Request_SMS_StorageCapacityGet(request_ctrl_block_p,
                                           client_data_p->mem1.memory,
                                           &slot_information,
                                           &storage_status,
                                           &sms_error);

        if (SMS_REQUEST_PENDING != request_status) {
            print_sms_error
            ("request_sms_storage_info, Request_SMS_StorageCapacityGet failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_MSG_STORAGE_MEM1_RESPONSE;
        goto pending;
    }
    case EXE_STATE_MSG_STORAGE_MEM1_RESPONSE: {
        request_status =
            Response_SMS_StorageCapacityGet(record_p->response_data_p,
                                            &slot_information,
                                            &storage_status,
                                            &sms_error);

        if (request_status != SMS_REQUEST_OK || sms_error != SMS_ERROR_INTERN_NO_ERROR) {
            print_sms_error("request_sms_storage_info, Response_SMS_StorageCapacityGet failed --> ",
                            sms_error);
            goto error;
        }

        get_storage_info_from_slot(slot_information, &(client_data_p->mem1));

        request_status =
            Request_SMS_StorageCapacityGet(request_ctrl_block_p,
                                           client_data_p->mem2.memory,
                                           &slot_information,
                                           &storage_status,
                                           &sms_error);

        if (SMS_REQUEST_PENDING != request_status) {
            print_sms_error
            ("request_sms_storage_info, Request_SMS_StorageCapacityGet failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_MSG_STORAGE_MEM2_RESPONSE;
        goto pending;
    }
    case EXE_STATE_MSG_STORAGE_MEM2_RESPONSE: {
        request_status =
            Response_SMS_StorageCapacityGet(record_p->response_data_p,
                                            &slot_information,
                                            &storage_status,
                                            &sms_error);

        if (SMS_REQUEST_OK != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_sms_storage_info, Response_SMS_ShortMessageSend failed --> ",
                            sms_error);
            goto error;
        }

        get_storage_info_from_slot(slot_information, &(client_data_p->mem2));

        request_status =
            Request_SMS_StorageCapacityGet(request_ctrl_block_p,
                                           client_data_p->mem3.memory,
                                           &slot_information,
                                           &storage_status,
                                           &sms_error);

        if (SMS_REQUEST_PENDING != request_status) {
            print_sms_error
            ("request_sms_storage_info, Request_SMS_StorageCapacityGet failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_MSG_STORAGE_MEM3_RESPONSE;
        goto pending;
    }
    case EXE_STATE_MSG_STORAGE_MEM3_RESPONSE: {
        request_status =
            Response_SMS_StorageCapacityGet(record_p->response_data_p,
                                            &slot_information,
                                            &storage_status,
                                            &sms_error);

        if (request_status != SMS_REQUEST_OK || sms_error != SMS_ERROR_INTERN_NO_ERROR) {
            print_sms_error("request_sms_storage_info, Response_SMS_ShortMessageSend failed --> ",
                            sms_error);
            goto error;
        }

        get_storage_info_from_slot(slot_information, &(client_data_p->mem3));
    }
    default:
        break;
    }

    smsclient_free_requestctrlblock(&request_ctrl_block_p);
    exe_request_complete(record_p, result, client_data_p);
    free(client_data_p);
    return EXE_SUCCESS;

pending:
    smsclient_free_requestctrlblock(&request_ctrl_block_p);
    return EXE_PENDING;
error:
    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, client_data_p);
    }

    free(client_data_p);
    return EXE_FAILURE;
}

exe_request_result_t request_sms_delete(exe_request_record_t *record_p)
{
    smsclient_t *smsclient_p = NULL;
    SMS_Error_t sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_Slot_t slot;
    SMS_RequestStatus_t request_status = SMS_REQUEST_OK;
    exe_cmgd_response_t response_data;
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    exe_request_result_t result = EXE_FAILURE;

    if ((EXE_STATE_REQUEST == record_p->state)) {
        exe_cmgd_del_t *request_data_p = (exe_cmgd_del_t *)record_p->request_data_p;

        if (NULL == request_data_p) {
            ATC_LOG_E("request_sms_delete: request_data_p NULL");
            goto exit;
        }

        smsclient_p = exe_get_smsclient(record_p->exe_p);

        if (NULL == smsclient_p) {
            ATC_LOG_E("request_sms_delete: smsclient_p is NULL");
            goto exit;
        }

        slot.Position = request_data_p->index;
        slot.Storage = request_data_p->storage;

        request_ctrl_block_p =
            smsclient_alloc_requestctrlblock(smsclient_p,
                                             (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_sms_delete: request_ctrl_block_p NULL");
            goto exit;
        }

        request_status = Request_SMS_ShortMessageDelete(request_ctrl_block_p, &slot, &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status) {
            print_sms_error(
                "request_sms_delete, Request_SMS_ShortMessageDelete failed --> ",
                sms_error);
            goto exit;
        }

        record_p->state = EXE_STATE_RESPONSE;
        result = EXE_PENDING;

    } else {
        request_status = Response_SMS_ShortMessageDelete(record_p->response_data_p, &sms_error);
        response_data.error = EXE_CMS_UNKNOWN_ERROR;

        if (SMS_REQUEST_FAILED_PARAMETER != request_status && SMS_REQUEST_FAILED_UNPACKING != request_status) {
            response_data.error = SMS_ErrorToCMS(sms_error);
        }

        if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_NO_ERROR == sms_error) {
            result = EXE_SUCCESS;
        } else {
            print_sms_error("request_sms_delete, Response_SMS_ShortMessageDelete failed --> ",
                            sms_error);
        }

        exe_request_complete(record_p, result, &response_data);
    }

exit:
    return result;
}


exe_request_result_t request_sms_delete_all(exe_request_record_t *record_p)
{
    exe_t                  *exe_p = NULL;
    smsclient_t            *smsclient_p = NULL;
    SMS_Error_t             sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_Slot_t              slot = {0, 0};
    exe_cmgd_response_t     response_data;
    SMS_SearchInfo_t        search_info;
    SMS_RequestStatus_t          request_status = SMS_REQUEST_OK;
    SMS_RequestControlBlock_t   *request_ctrl_block_p = NULL;
    exe_request_result_t         result = EXE_FAILURE;

    smsclient_p = exe_get_smsclient(record_p->exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("request_sms_delete_all: smsclient_p is NULL");
        goto end;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST: {
        exe_cmgd_del_all_t *cmgd_del_all_p = NULL;
        ATC_LOG_I("request_sms_delete_all -> request");

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("request_sms_delete_all: request_data_p NULL");
            goto end;
        }

        find_sms_init((exe_msg_search_info_t *)&search_info,
                      ((exe_cmgd_del_all_t *)record_p->request_data_p)->status,
                      ((exe_cmgd_del_all_t *)record_p->request_data_p)->storage);
        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                               (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_sms_delete_all: request_ctrl_block_p NULL");
            goto end;
        }

        request_status = Request_SMS_ShortMessageFind(request_ctrl_block_p, &search_info, &slot, &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status) {
            print_sms_error(
                "request_sms_delete_all, Request_SMS_ShortMessageFind failed --> ",
                sms_error);
            goto end;
        }

        /* We need to save status and storage for deleting all messages,
         * as we cannot trust parser allocated memory we allocate
         * a new area and sets the request data pointer to it.
         */
        cmgd_del_all_p = malloc(sizeof(exe_cmgd_del_all_t));

        if (NULL == cmgd_del_all_p) {
            ATC_LOG_E("request_sms_delete_all: could not allocate memory");
            goto end;
        }

        memcpy(cmgd_del_all_p, record_p->request_data_p, sizeof(exe_cmgd_del_all_t));
        record_p->request_data_p = cmgd_del_all_p;

        record_p->state = EXE_STATE_MSG_DELETE;
        result = EXE_PENDING;
        break;
    }
    case EXE_STATE_MSG_DELETE: {
        request_status = Response_SMS_ShortMessageFind(record_p->response_data_p, &slot, &sms_error);
        response_data.error = EXE_CMS_UNKNOWN_ERROR;

        if (SMS_REQUEST_FAILED_PARAMETER != request_status &&  SMS_REQUEST_FAILED_UNPACKING != request_status) {
            response_data.error = SMS_ErrorToCMS(sms_error);
        }

        if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_NO_ERROR == sms_error) {
            request_ctrl_block_p =
                smsclient_alloc_requestctrlblock(smsclient_p,
                                                 (void *)request_record_get_client_tag(record_p));

            if (NULL == request_ctrl_block_p) {
                ATC_LOG_E("request_sms_delete_all: request_ctrl_block_p NULL");
                response_data.error = EXE_CMS_UNKNOWN_ERROR;
                goto end;
            }

            request_status = Request_SMS_ShortMessageDelete(request_ctrl_block_p, &slot, &sms_error);

            smsclient_free_requestctrlblock(&request_ctrl_block_p);

            if (request_status != SMS_REQUEST_PENDING) {
                response_data.error = SMS_ErrorToCMS(sms_error);
                print_sms_error(
                    "request_sms_delete_all, Request_SMS_ShortMessageDelete failed --> ",
                    sms_error);
                goto end;
            }

            record_p->state = EXE_STATE_MSG_DELETE_RESPONSE;
            result = EXE_PENDING;
            goto end;
        }

        if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_MESSAGE_NOT_FOUND == sms_error) {
            result = EXE_SUCCESS;
            goto end;
        }

        ATC_LOG_E("Response_SMS_ShortMessageFind: response fail");
        break;
    }
    case EXE_STATE_MSG_DELETE_RESPONSE: {
        request_status = Response_SMS_ShortMessageDelete(record_p->response_data_p, &sms_error);
        response_data.error = EXE_CMS_UNKNOWN_ERROR;

        if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_NO_ERROR == sms_error) {
            find_sms_init((exe_msg_search_info_t *)&search_info,
                          ((exe_cmgd_del_all_t *) record_p->request_data_p)->status,
                          ((exe_cmgd_del_all_t *) record_p->request_data_p)->storage);
            request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                                   (void *)request_record_get_client_tag(record_p));

            if (NULL == request_ctrl_block_p) {
                ATC_LOG_E("request_sms_delete_all: request_ctrl_block_p NULL");
                goto end;
            }

            request_status = Request_SMS_ShortMessageFind(request_ctrl_block_p, &search_info, &slot, &sms_error);

            smsclient_free_requestctrlblock(&request_ctrl_block_p);

            if (SMS_REQUEST_PENDING != request_status) {
                response_data.error = SMS_ErrorToCMS(sms_error);
                print_sms_error(
                    "request_sms_delete_all, Request_SMS_ShortMessageFind failed --> ",
                    sms_error);
                goto end;
            }

            record_p->state = EXE_STATE_MSG_DELETE;
            result = EXE_PENDING;
            goto end;
        }

        if (SMS_REQUEST_FAILED_PARAMETER != request_status && SMS_REQUEST_FAILED_UNPACKING != request_status) {
            response_data.error = SMS_ErrorToCMS(sms_error);
        }

        print_sms_error("request_sms_delete, Response_SMS_ShortMessageDelete failed --> ",
                        sms_error);
        break;
    }
    default:
        break;
    }

end:

    if (EXE_PENDING != result && EXE_STATE_REQUEST != record_p->state) {
        free(record_p->request_data_p);
        exe_request_complete(record_p, result, &response_data);
    }

    return result;
}

exe_request_result_t request_sms_relay_link_control_set(exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t   *request_ctrl_block_p   = NULL;
    exe_request_result_t        result                  = EXE_SUCCESS;
    SMS_RequestStatus_t         request_status          = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error               = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RelayControl_t          relay_link_control      = SMS_RELAY_CONTROL_DISABLED;

    if (EXE_STATE_REQUEST == record_p->state) {
        smsclient_t         *smsclient_p    = NULL;
        exe_cmms_t          *req_data_p       = (exe_cmms_t *) record_p->request_data_p;
        SMS_RP_ErrorCause_t rp_error_cause  = 0;

        if (NULL == req_data_p) {
            ATC_LOG_E("request_sms_relay_link_control_set: req_data is NULL");
            goto error;
        }

        smsclient_p = exe_get_smsclient(record_p->exe_p);

        if (NULL == smsclient_p) {
            ATC_LOG_E("request_sms_relay_link_control_set: sc_p is NULL!");
            goto error;
        }

        request_ctrl_block_p =
            smsclient_alloc_requestctrlblock(smsclient_p,
                                             (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_sms_relay_link_control_set: ctrl_block is null");
            goto error;
        }

        relay_link_control = (SMS_RelayControl_t) req_data_p->n;

        request_status = R_Req_SMS_RelayLinkControlSet(request_ctrl_block_p, relay_link_control, &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            print_sms_error
            ("request_sms_relay_link_control_set, R_Req_SMS_RelayLinkControlSet failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;

        return EXE_PENDING;
    } else {
        request_status = R_Resp_SMS_RelayLinkControlSet(record_p->response_data_p, &sms_error);

        if (request_status == SMS_REQUEST_OK && sms_error == SMS_ERROR_INTERN_NO_ERROR) {
            result = EXE_SUCCESS;
        } else {
            print_sms_error
            ("request_sms_relay_link_control_set, R_Resp_SMS_RelayLinkControlSet failed --> ",
             sms_error);
            result = EXE_FAILURE;
        }

        exe_request_complete(record_p, result, NULL);
        return result;
    }

error:
    return EXE_FAILURE;
}

exe_request_result_t request_sms_relay_link_control_get(exe_request_record_t *record_p)
{

    SMS_RequestControlBlock_t   *request_ctrl_block_p   = NULL;
    exe_request_result_t        result                  = EXE_SUCCESS;
    SMS_RequestStatus_t         request_status          = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error               = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RelayControl_t          relay_link_control      = SMS_RELAY_CONTROL_DISABLED;

    if (EXE_STATE_REQUEST == record_p->state) {
        smsclient_t         *smsclient_p    = NULL;
        SMS_RP_ErrorCause_t rp_error_cause  = 0;

        smsclient_p = exe_get_smsclient(record_p->exe_p);

        if (NULL == smsclient_p) {
            ATC_LOG_E("request_sms_relay_link_control_get: sc_p is NULL!");
            goto error;
        }

        request_ctrl_block_p =
            smsclient_alloc_requestctrlblock(smsclient_p,
                                             (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("request_sms_relay_link_control_get: ctrl_block is null");
            goto error;
        }

        request_status = R_Req_SMS_RelayLinkControlGet(request_ctrl_block_p, &relay_link_control, &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            print_sms_error
            ("request_sms_relay_link_control_get, R_Req_SMS_RelayLinkControlGet failed --> ",
             sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;

        return EXE_PENDING;
    } else {

        exe_cmms_t res_data;
        request_status = R_Resp_SMS_RelayLinkControlGet(record_p->response_data_p, &relay_link_control, &sms_error);

        if (request_status == SMS_REQUEST_OK && sms_error == SMS_ERROR_INTERN_NO_ERROR) {
            result = EXE_SUCCESS;
        } else {
            print_sms_error
            ("request_sms_relay_link_control_get, R_Resp_SMS_RelayLinkControlGet failed --> ",
             sms_error);
            result = EXE_FAILURE;
        }

        res_data.n = relay_link_control;

        exe_request_complete(record_p, result, &res_data);
        return result;
    }

    return EXE_SUCCESS;
error:
    return EXE_FAILURE;
}

exe_request_result_t request_sms_read(exe_request_record_t *record_p)
{
    exe_t                       *exe_p = record_p->exe_p;
    smsclient_t                 *smsclient_p = NULL;
    exe_cmgr_t                  *req_data_p = NULL;
    SMS_Slot_t                  slot = {0, 0};
    SMS_RequestControlBlock_t   *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_SMSC_Address_TPDU_t     tpdu;
    SMS_Status_t                sms_status = SMS_STATUS_UNKNOWN;
    uint8_t                     index = 0;
    exe_cmgr_response_t         response_data;
    uint8_t                     *pdu_p = NULL;

    smsclient_p = exe_get_smsclient(exe_p);

    memset(&response_data, 0x00, sizeof(response_data));

    if (NULL == smsclient_p) {
        ATC_LOG_E("request_sms_read: smsclient_p is NULL");
        goto error;
    }

    switch (record_p->state) {

    case EXE_STATE_REQUEST: {

        /* store the request data since we need it also in the next iteration */
        req_data_p = (exe_cmgr_t *)malloc(sizeof(exe_cmgr_t));

        if (NULL == req_data_p) {
            ATC_LOG_E("Out of memory");
            response_data.error = EXE_CMS_MEMORY_FAILURE;
            goto error;
        }

        memcpy(req_data_p, (exe_cmgr_t *)record_p->request_data_p, sizeof(exe_cmgr_t));
        record_p->request_data_p = req_data_p;

        slot.Position = req_data_p->index;
        slot.Storage =  req_data_p->storage;

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                               (void *)request_record_get_client_tag(record_p));

        request_status = Request_SMS_ShortMessageRead(request_ctrl_block_p,
                         &slot,
                         NULL,
                         NULL,
                         &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_sms_read, Request_SMS_ShortMessageRead failed --> ",
                            sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_MSG_READ_SMS;
        goto pending;
    }

    case EXE_STATE_MSG_READ_SMS: {

        req_data_p = (exe_cmgr_t *)record_p->request_data_p;

        memset(&tpdu, 0x00, sizeof(SMS_SMSC_Address_TPDU_t));

        request_status = Response_SMS_ShortMessageRead(record_p->response_data_p,
                         &sms_status, &tpdu, &sms_error);

        if (SMS_REQUEST_FAILED_PARAMETER != request_status) {
            response_data.error = SMS_ErrorToCMS(sms_error);
        }

        if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_NO_ERROR == sms_error) {

            switch (sms_status) {
            case SMS_STATUS_UNREAD:
                response_data.status = EXE_SMS_STATUS_UNREAD;
                break;
            case SMS_STATUS_READ:
                response_data.status = EXE_SMS_STATUS_READ;
                break;
            case SMS_STATUS_UNSENT:
                response_data.status = EXE_SMS_STATUS_UNSENT;
                break;
            case SMS_STATUS_SENT:
                response_data.status = EXE_SMS_STATUS_SENT;
                break;
                /* Wrong status */
            default:
                response_data.error = EXE_CMS_UNKNOWN_ERROR;
                goto error;
            }

            pdu_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

            if (NULL == pdu_p) {
                ATC_LOG_E("Out of memory");
                response_data.error = EXE_CMS_MEMORY_FAILURE;
                goto error;
            }

            index = smsutil_sms_to_pdu(&tpdu, pdu_p);

            response_data.pdu_data_p = pdu_p;
            response_data.tpdu_length = tpdu.TPDU.Length;
            response_data.total_length = index;

            /* if msg is 'unread', change status to 'read' */
            if (sms_status == SMS_STATUS_UNREAD) {
                slot.Position = req_data_p->index;
                slot.Storage =  req_data_p->storage;

                request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                                       (void *)request_record_get_client_tag(record_p));

                request_status = Request_SMS_ShortMessageStatusSet(request_ctrl_block_p,
                                 &slot,
                                 SMS_STATUS_READ,
                                 &sms_error);

                smsclient_free_requestctrlblock(&request_ctrl_block_p);

                /* store the response data */
                record_p->client_data_p = malloc(sizeof(exe_cmgr_response_t));

                if (NULL == record_p->client_data_p) {
                    ATC_LOG_E("Out of memory");
                    response_data.error = EXE_CMS_MEMORY_FAILURE;
                    goto error;
                }

                memcpy(record_p->client_data_p, &response_data, sizeof(exe_cmgr_response_t));

                record_p->state = EXE_STATE_MSG_SET_STATUS;
                goto pending;
            }

        } else {
            print_sms_error("request_sms_read, Response_SMS_ShortMessageRead failed --> ",
                            sms_error);
            goto error;
        }

        /* we are done */
        break;
    }

    case EXE_STATE_MSG_SET_STATUS: {

        req_data_p = (exe_cmgr_t *)record_p->request_data_p;

        /* get the stored response data */
        memcpy(&response_data, record_p->client_data_p, sizeof(exe_cmgr_response_t));
        free(record_p->client_data_p);

        request_status = Response_SMS_ShortMessageStatusSet(record_p->response_data_p,
                         &sms_error);

        if (SMS_REQUEST_OK != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            response_data.error = SMS_ErrorToCMS(sms_error);
            print_sms_error("request_sms_read, Response_SMS_ShortMessageStatusSet failed --> ",
                            sms_error);
            goto error;
        }

        /* we are done */
        break;
    }

    default:
        ATC_LOG_E("%s: state %d unknown", __FUNCTION__, record_p->state);
        goto error;

    }

    /* free the request data */
    free(req_data_p);
    exe_request_complete(record_p, EXE_SUCCESS, &response_data);
    free(response_data.pdu_data_p);

    return EXE_SUCCESS;

pending:
    return EXE_PENDING;

error:

    if (NULL != req_data_p) {
        free(req_data_p);
    }

    if (NULL != response_data.pdu_data_p) {
        free(response_data.pdu_data_p);
    }

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &response_data);
    }

    return EXE_FAILURE;
}

exe_request_result_t request_cgsms_set(exe_request_record_t *record_p)
{
    exe_t                       *exe_p = record_p->exe_p;
    smsclient_t                 *smsclient_p = NULL;
    exe_cgsms_request_t         *req_data_p = NULL;
    SMS_RequestControlBlock_t   *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_MO_Route_t              sms_route = SMS_MO_ROUTE_CS_PS;

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("%s: smsclient_p is NULL", __FUNCTION__);
        goto error;
    }

    if (EXE_STATE_REQUEST == record_p->state) {

        if (NULL == record_p->request_data_p) {
            ATC_LOG_E("%s: record_p->request_data_p is NULL", __FUNCTION__);
            goto error;
        }

        req_data_p = (exe_cgsms_request_t *)record_p->request_data_p;

        sms_route = (SMS_MO_Route_t)(req_data_p->service);

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                               (void *)request_record_get_client_tag(record_p));

        request_status = R_Req_SMS_MO_RouteSet(request_ctrl_block_p, sms_route, &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_cgsms_set, Request_SMS_ShortMessageRead failed --> ", sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;

        return EXE_PENDING;
    } else {

        request_status = R_Resp_SMS_MO_RouteSet(record_p->response_data_p, &sms_error);

        if (SMS_REQUEST_OK != request_status) {
            print_sms_error("request_cgsms_set, R_Resp_SMS_MO_RouteSet failed --> ", sms_error);
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

exe_request_result_t request_cgsms_read(exe_request_record_t *record_p)
{
    exe_t                       *exe_p = record_p->exe_p;
    smsclient_t                 *smsclient_p = NULL;
    exe_cgsms_request_t         *req_data_p = NULL;
    SMS_RequestControlBlock_t   *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_MO_Route_t              sms_route = SMS_MO_ROUTE_PS;
    exe_cgsms_response_t        response_data;

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("%s: smsclient_p is NULL", __FUNCTION__);
        goto error;
    }

    memset(&response_data, 0x00, sizeof(response_data));

    if (EXE_STATE_REQUEST == record_p->state) {

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                               (void *)request_record_get_client_tag(record_p));

        request_status = R_Req_SMS_MO_RouteGet(request_ctrl_block_p, &sms_route, &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_cgsms_read, R_Req_SMS_MO_RouteGet failed --> ", sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_RESPONSE;

        return EXE_PENDING;
    } else {

        request_status = R_Resp_SMS_MO_RouteGet(record_p->response_data_p, &sms_route, &sms_error);

        if (SMS_REQUEST_OK != request_status) {
            print_sms_error("request_cgsms_read, R_Resp_SMS_MO_RouteGet failed --> ", sms_error);
            goto error;
        }

        response_data.service = (exe_cgsms_service_t)sms_route;

        exe_request_complete(record_p, EXE_SUCCESS, &response_data);
        return EXE_SUCCESS;
    }

error:

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, NULL);
    }

    return EXE_FAILURE;
}


/* This is a local function used by request_send_sms_from_preferred_storage */
static void exe_sms_change_recipient(SMS_SMSC_Address_TPDU_t *tpdu_p, char *da_p, uint8_t toda)
{
    uint8_t da_length = strlen(da_p);
    uint8_t i = 0;
    uint8_t k = 0;
    uint8_t bcd_buffer_size = 0;
    uint8_t *bcd_buffer_p = NULL;
    uint8_t *changed_sms_tpdu_p = NULL;
    uint8_t new_msg_len = 0;
    uint8_t copy_msg_length = 0;
    uint8_t new_number_length = 0;

    bcd_buffer_size = (da_length + 1) / 2;
    bcd_buffer_p = (uint8_t *)malloc(bcd_buffer_size * sizeof(uint8_t));

    if (NULL == bcd_buffer_p) {
        return;
    }

    memset(bcd_buffer_p, 0xff, bcd_buffer_size);

    /* This is a local function for request_cmss_set.
     * Both da_p and tpdu_p are known to be =! NULL
     *
     * example TPDU.Data = -01-00-0D-91-44-52-89-01-31-16-01-FF-00-00-05-CB-30-9B-5D
     */

    /* convert string to bcd data */
    for (i = 0; i < da_length; i++) {
        uint8_t c = (uint8_t)da_p[i];

        if (0x30 > c || 0x39 < c) {
            continue;
        }

        /* numbers in odd position in lower nibble,
         * numbers in even position in upper nibble
         * k index the bcd buffer, i index the phone number string.
         * new_number_length is the size of the new number string,
         * non numericals removed (e.g + character);
         */
        if (0 != (new_number_length + 1) % 2) {
            bcd_buffer_p[k] &= (c - 0x30) | 0xf0;
        } else {
            bcd_buffer_p[k] &= ((c - 0x30) << 4) | 0x0f;
            k++;
        }

        new_number_length++;
    }

    /* creating a copy of original sms with the new recipient number */
    copy_msg_length = tpdu_p->TPDU.Length - (tpdu_p->TPDU.Data[2] + 1) / 2 - 4;
    new_msg_len = copy_msg_length + (new_number_length + 1) / 2 + 4;

    /* Truncate message if new recipient number is longer than old and
     * message has MAX SIZE
     */
    if (SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH < new_msg_len) {
        new_msg_len = SMS_SHORT_MESSAGE_TPDU_MAX_LENGTH;
    }

    changed_sms_tpdu_p = (uint8_t *)calloc(new_msg_len, sizeof(uint8_t));

    if (NULL != changed_sms_tpdu_p) {
        memcpy(changed_sms_tpdu_p, tpdu_p->TPDU.Data, 2);
        changed_sms_tpdu_p[2] = new_number_length;
        changed_sms_tpdu_p[3] = toda;
        memcpy(&(changed_sms_tpdu_p[4]), bcd_buffer_p, bcd_buffer_size);
        memcpy(&(changed_sms_tpdu_p[bcd_buffer_size+4]), &(tpdu_p->TPDU.Data[11]), copy_msg_length);

        /* copying the changed SMS to its destination buffer */
        memcpy(tpdu_p->TPDU.Data, changed_sms_tpdu_p, new_msg_len);
    }

    free(changed_sms_tpdu_p);
    free(bcd_buffer_p);
}

exe_request_result_t request_send_sms_from_preferred_storage(exe_request_record_t *record_p)
{
    exe_t                       *exe_p = record_p->exe_p;
    smsclient_t                 *smsclient_p = NULL;
    SMS_RequestControlBlock_t   *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_Slot_t                  slot = {0, 0};
    SMS_Status_t                sms_status;
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;
    exe_cmss_request_t          *req_data_p = NULL;
    uint16_t                    index = 0;
    msg_storage_t               storage = MSG_STORAGE_UNKNOWN;
    exe_request_result_t        result = EXE_SUCCESS;
    exe_cmss_response_t         response_data;

    memset(&response_data, 0x00, sizeof(response_data));
    response_data.error = EXE_CMS_UNKNOWN_ERROR;
    req_data_p = (exe_cmss_request_t *)record_p->request_data_p;

    /* Check if an ongoing execution shall be aborted. */
    if (record_p->abort && record_p->state != EXE_STATE_REQUEST) {
        if (NULL != record_p->request_data_p) {
            req_data_p = (exe_cmss_request_t *)record_p->request_data_p;
            free(req_data_p->da_p);
            free(req_data_p);
        }

        return EXE_SUCCESS;
    }

    smsclient_p = exe_get_smsclient(exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("%s: smsclient_p is NULL", __FUNCTION__);
        goto error;
    }

    if (NULL == req_data_p) {
        ATC_LOG_E("%s: record_p->request_data_p is NULL", __FUNCTION__);
        response_data.error = EXE_CMS_UNKNOWN_ERROR;
        goto error;
    }

    slot.Storage = req_data_p->storage;
    slot.Position = req_data_p->index;

    switch (record_p->state) {

    case EXE_STATE_REQUEST: {
        ATC_LOG_I("%s: EXE_STATE_REQUEST", __FUNCTION__);

        /* Copy the request data as we need it for several call iterations
         * The parser will free its data and we will loose it otherwise.*/
        req_data_p = (exe_cmss_request_t *)malloc(sizeof(exe_cmss_request_t));
        EXE_CHECK_GOTO_ERROR(NULL != req_data_p);

        memcpy(req_data_p, (exe_cmss_request_t *)record_p->request_data_p, sizeof(exe_cmss_request_t));

        if (NULL != req_data_p->da_p) {
            char *da_p = ((exe_cmss_request_t *)record_p->request_data_p)->da_p;
            req_data_p->da_p = (char *)calloc(strlen(da_p) + 1, sizeof(char));
            EXE_CHECK_GOTO_ERROR(NULL != req_data_p->da_p);
            strcpy(req_data_p->da_p, da_p);
        }

        record_p->request_data_p = req_data_p;

        if (NULL == req_data_p->da_p) {

            request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));
            request_status = Request_SMS_ShortMessageSend(request_ctrl_block_p, NULL, &slot, 0, NULL, &sms_error);
            smsclient_free_requestctrlblock(&request_ctrl_block_p);

            if (SMS_REQUEST_PENDING != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
                print_sms_error("request_cmss_set, Request_SMS_ShortMessageSend failed --> ", sms_error);
                response_data.error = SMS_ErrorToCMS(sms_error);
                goto error;
            }

            record_p->state = EXE_STATE_MSG_SEND_SMS;

        } else {
            ATC_LOG_I("%s: EXE_STATE_REQUEST. Request_SMS_ShortMessageRead", __FUNCTION__);
            request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));
            request_status =  Request_SMS_ShortMessageRead(request_ctrl_block_p, &slot, NULL, NULL, &sms_error);
            smsclient_free_requestctrlblock(&request_ctrl_block_p);

            record_p->state = EXE_STATE_MSG_READ_SMS;
        }

        result = EXE_PENDING;

        break;
    }

    case EXE_STATE_MSG_READ_SMS: {
        SMS_SMSC_Address_TPDU_t tpdu;

        ATC_LOG_I("%s: EXE_STATE_CMSS_READ_SMS entered", __FUNCTION__);

        memset(&tpdu, 0, sizeof(SMS_SMSC_Address_TPDU_t));

        request_status = Response_SMS_ShortMessageRead(record_p->response_data_p, &sms_status, &tpdu, &sms_error);

        if (SMS_REQUEST_OK != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_cmss_set, Response_SMS_ShortMessageRead failed --> ", sms_error);
            response_data.error = SMS_ErrorToCMS(sms_error);
            goto error;
        }

        if (0 == tpdu.TPDU.Length) {
            response_data.error = EXE_CMS_INVALID_MESSAGE_UNSPECIFIED;
            goto error;
        }

        exe_sms_change_recipient(&tpdu, req_data_p->da_p, req_data_p->toda);

        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));
        request_status =  Request_SMS_ShortMessageWrite(request_ctrl_block_p, sms_status, &tpdu, &slot, &sms_error);
        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_cmss_set, Request_SMS_ShortMessageWrite failed --> ", sms_error);
            response_data.error = SMS_ErrorToCMS(sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_MSG_WRITE_SMS;;

        break;
    }

    case EXE_STATE_MSG_WRITE_SMS: {
        ATC_LOG_I("%s: EXE_STATE_CMSS_WRITE_SMS entered", __FUNCTION__);

        request_status = Response_SMS_ShortMessageWrite(record_p->response_data_p, &slot, &sms_error);

        if (SMS_REQUEST_OK != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_cmss_set, Response_SMS_ShortMessageWrite failed --> ", sms_error);
            response_data.error = SMS_ErrorToCMS(sms_error);
            goto error;
        }

        /* Make call to actually send the message */
        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));
        request_status = Request_SMS_ShortMessageSend(request_ctrl_block_p, NULL, &slot, 0, NULL, &sms_error);
        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (SMS_REQUEST_PENDING != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
            print_sms_error("request_cmss_set, Request_SMS_ShortMessageSend failed --> ", sms_error);
            response_data.error = SMS_ErrorToCMS(sms_error);
            goto error;
        }

        record_p->state = EXE_STATE_MSG_SEND_SMS;

        break;
    }

    case EXE_STATE_MSG_SEND_SMS: {
        uint8_t mr = 0;

        request_status = Response_SMS_ShortMessageSend(record_p->response_data_p, &mr, &sms_error);

        if (SMS_REQUEST_OK != request_status) {
            response_data.error = SMS_ErrorToCMS(sms_error);
            goto error;
        }

        response_data.mr = mr;

        exe_request_complete(record_p, EXE_SUCCESS, &response_data);

        free(req_data_p->da_p);
        free(req_data_p);

        break;
    }

    default:
        ATC_LOG_E("%s: state %d unknown", __FUNCTION__, record_p->state);
        goto error;
        break;

    }

    return result;

error:

    if (NULL != req_data_p) {
        free(req_data_p->da_p);
        free(req_data_p);
    }

    if (EXE_STATE_REQUEST != record_p->state) {
        exe_request_complete(record_p, EXE_FAILURE, &response_data);
    }

    return EXE_FAILURE;
}


/* This is a local function used by request_sms_list */
static exe_request_result_t exe_sms_list_find_message(exe_request_record_t *record_p,
        smsclient_t *smsclient_p,
        exe_cmgl_request_t *exe_cmgl_request_p,
        exe_cmgl_response_t *exe_cmgl_response_p)
{
    exe_request_result_t        result = EXE_FAILURE;
    SMS_SearchInfo_t            search_info;
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_RequestControlBlock_t  *request_ctrl_block_p = NULL;
    SMS_Slot_t                  slot = {SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID};
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;

    ATC_LOG_I("exe_sms_list_find_message");

    if (!find_sms_set(&search_info, exe_cmgl_request_p->status, exe_cmgl_request_p->storage, exe_cmgl_request_p->position, exe_cmgl_request_p->browse)) {
        ATC_LOG_E("exe_sms_list_find_message: invalid search params");
        exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;
        goto end;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p,
                           (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("exe_sms_list_find_message: request_ctrl_block_p NULL");
        goto end;
    }

    request_status = Request_SMS_ShortMessageFind(request_ctrl_block_p, &search_info, &slot, &sms_error);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (SMS_REQUEST_PENDING != request_status) {
        print_sms_error("exe_sms_list_find_message, Request_SMS_ShortMessageFind failed --> ", sms_error);
        goto end;
    }

    result = EXE_PENDING;

end:
    return result;
}

/* This is a local function used by request_sms_list */
static exe_request_result_t exe_sms_list_state_request(exe_request_record_t *record_p,
        smsclient_t *smsclient_p,
        exe_cmgl_request_t *exe_cmgl_request_p,
        exe_cmgl_response_t *exe_cmgl_response_p)
{
    exe_request_result_t result = EXE_FAILURE;

    ATC_LOG_I("exe_sms_list_state_request");

    /* Initial find searches for "first" match. */
    exe_cmgl_request_p->browse = MSG_FIRST;

    if (NULL == exe_cmgl_response_p) {
        exe_cmgl_response_p = malloc(sizeof(exe_cmgl_response_t));

        if (NULL == exe_cmgl_response_p) {
            ATC_LOG_E("exe_sms_list_state_request: response_data_p NULL");
            goto end;
        } else {
            exe_cmgl_response_p->num_pdus = 0;
            exe_cmgl_response_p->pdu_data_p = NULL;
            exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;
            request_record_set_client_data(record_p, exe_cmgl_response_p);
        }
    }

    if (exe_sms_list_find_message(record_p, smsclient_p, exe_cmgl_request_p, exe_cmgl_response_p) == EXE_FAILURE) {
        ATC_LOG_E("exe_sms_list_state_request: find failed");
        goto end;
    }

    /* subsequent finds search for "next" match. */
    exe_cmgl_request_p->browse = MSG_NEXT;

    {
        /* We need to save information for listing messages,
         * as we cannot trust parser allocated memory we allocate
         * a new area and set the request data pointer to it.
         * However we only want to do this on the first time that
         * this function is called.
         * (The first time this function is called, record_p->request_data_p
         * points to memory on the stack.)
         */
        exe_cmgl_request_t *exe_cmgl_request_new_p = malloc(sizeof(exe_cmgl_request_t));

        if (NULL == exe_cmgl_request_new_p) {
            ATC_LOG_E("exe_sms_list_state_request: could not allocate memory");
            exe_cmgl_response_p->error = EXE_CMS_MEMORY_FAILURE;
            goto end;
        }

        memcpy(exe_cmgl_request_new_p, record_p->request_data_p, sizeof(exe_cmgl_request_t));
        record_p->request_data_p = exe_cmgl_request_new_p;
    }

    record_p->state = EXE_STATE_MSG_READ_SMS;
    result = EXE_PENDING;

end:
    return result;
}

/* This is a local function used by request_sms_list */
static exe_request_result_t exe_sms_list_state_read(exe_request_record_t *record_p,
        smsclient_t *smsclient_p,
        exe_cmgl_request_t *exe_cmgl_request_p,
        exe_cmgl_response_t *exe_cmgl_response_p)
{
    exe_request_result_t        result = EXE_FAILURE;
    SMS_Error_t                 sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_Slot_t                  slot = {SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID};
    SMS_RequestStatus_t         request_status = SMS_REQUEST_OK;
    SMS_RequestControlBlock_t  *request_ctrl_block_p = NULL;

    ATC_LOG_I("exe_sms_list_state_read");

    request_status = Response_SMS_ShortMessageFind(record_p->response_data_p, &slot, &sms_error);
    exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;

    if (SMS_REQUEST_FAILED_PARAMETER != request_status && SMS_REQUEST_FAILED_UNPACKING != request_status) {
        exe_cmgl_response_p->error = SMS_ErrorToCMS(sms_error);
    }

    if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_NO_ERROR == sms_error) {
        request_ctrl_block_p =
            smsclient_alloc_requestctrlblock(smsclient_p,
                                             (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("exe_sms_list_state_read: request_ctrl_block_p NULL");
            exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;
            goto end;
        }

        /* We need to store position returned and use it as the starting point for the next Find. */
        exe_cmgl_request_p->position = (exe_msg_position_t) slot.Position;

        request_status = Request_SMS_ShortMessageRead(request_ctrl_block_p,
                         &slot,
                         NULL,
                         NULL,
                         &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            exe_cmgl_response_p->error = SMS_ErrorToCMS(sms_error);
            print_sms_error("exe_sms_list_state_read, Request_SMS_ShortMessageRead failed --> ", sms_error);
            goto end;
        }

        record_p->state = EXE_STATE_MSG_SET_STATUS;
        result = EXE_PENDING;
        goto end;
    } else {
        if (SMS_REQUEST_OK == request_status && SMS_ERROR_INTERN_MESSAGE_NOT_FOUND == sms_error) {
            if (exe_cmgl_response_p->num_pdus > 0) {
                exe_cmgl_response_p->error = SMS_ErrorToCMS(SMS_ERROR_INTERN_NO_ERROR);
                result = EXE_SUCCESS;
            }
        } else {
            print_sms_error("exe_sms_list_state_read, Request_SMS_ShortMessageFind failed --> ", sms_error);
        }

        goto end;
    }

    ATC_LOG_E("Response_SMS_ShortMessageFind: response fail");

end:
    return result;
}

/* This is a local function used by request_sms_list */
static exe_request_result_t exe_sms_list_state_set_status(exe_request_record_t *record_p,
        smsclient_t *smsclient_p,
        exe_cmgl_request_t *exe_cmgl_request_p,
        exe_cmgl_response_t *exe_cmgl_response_p)
{
    exe_request_result_t            result = EXE_FAILURE;
    SMS_Error_t                     sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RequestStatus_t             request_status = SMS_REQUEST_OK;
    SMS_RequestControlBlock_t      *request_ctrl_block_p = NULL;
    exe_cmgl_response_pdu_data_t   *response_pdu_p = NULL;
    SMS_Slot_t                      slot = {SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID};
    uint8_t                        *pdu_p = NULL;
    uint8_t                         index = 0;
    SMS_Status_t                    sms_status = SMS_STATUS_UNKNOWN;
    SMS_SMSC_Address_TPDU_t         tpdu;

    ATC_LOG_I("exe_sms_list_state_set_status");

    memset(&tpdu, 0, sizeof(SMS_SMSC_Address_TPDU_t));

    request_status = Response_SMS_ShortMessageRead(record_p->response_data_p, &sms_status, &tpdu, &sms_error);
    exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;

    if (SMS_REQUEST_FAILED_PARAMETER != request_status && SMS_REQUEST_FAILED_UNPACKING != request_status) {
        exe_cmgl_response_p->error = SMS_ErrorToCMS(sms_error);
    }

    if (SMS_REQUEST_OK != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
        print_sms_error("exe_sms_list_state_set_status, Response_SMS_ShortMessageRead failed --> ", sms_error);
        goto end;
    }

    if (0 == tpdu.TPDU.Length) {
        exe_cmgl_response_p->error = EXE_CMS_INVALID_MESSAGE_UNSPECIFIED;
        goto end;
    }

    response_pdu_p = malloc(sizeof(exe_cmgl_response_pdu_data_t));

    if (NULL == response_pdu_p) {
        ATC_LOG_E("Out of memory");
        exe_cmgl_response_p->error = EXE_CMS_MEMORY_FAILURE;
        goto end;
    }

    switch (sms_status) {
    case SMS_STATUS_UNREAD:
        response_pdu_p->status = EXE_SMS_STATUS_UNREAD;
        break;
    case SMS_STATUS_READ:
        response_pdu_p->status = EXE_SMS_STATUS_READ;
        break;
    case SMS_STATUS_UNSENT:
        response_pdu_p->status = EXE_SMS_STATUS_UNSENT;
        break;
    case SMS_STATUS_SENT:
        response_pdu_p->status = EXE_SMS_STATUS_SENT;
        break;
    default:
        exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;
        goto end;
    }

    response_pdu_p->next_p = NULL;
    response_pdu_p->position = exe_cmgl_request_p->position;

    pdu_p = malloc(sizeof(SMS_SMSC_Address_TPDU_t));

    if (NULL == pdu_p) {
        ATC_LOG_E("Out of memory");
        exe_cmgl_response_p->error = EXE_CMS_MEMORY_FAILURE;
        goto end;
    }

    index = smsutil_sms_to_pdu(&tpdu, pdu_p);

    response_pdu_p->pdu_p = pdu_p;
    response_pdu_p->total_length = index;
    response_pdu_p->tpdu_length = tpdu.TPDU.Length;

    /* Add to response pdu element to end of linked list. */
    {
        exe_cmgl_response_pdu_data_t *curr_p = exe_cmgl_response_p->pdu_data_p;

        if (curr_p != NULL) {
            while (curr_p->next_p != NULL) {
                curr_p = curr_p->next_p;
            }

            curr_p->next_p = response_pdu_p;
        } else {
            exe_cmgl_response_p->pdu_data_p = response_pdu_p;
        }

        exe_cmgl_response_p->num_pdus++;
        response_pdu_p = NULL; /* Set to NULL to prevent freeing of memory in block after end: identifier. */
    }

    if (sms_status == SMS_STATUS_UNREAD) {
        /* Change status of unread messages to read. */
        if (!find_sms_slot_set(&slot, exe_cmgl_request_p->storage, exe_cmgl_request_p->position)) {
            exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;
            ATC_LOG_E("exe_sms_list_state_set_status: invalid slot params");
            goto end;
        }

        request_ctrl_block_p =
            smsclient_alloc_requestctrlblock(smsclient_p,
                                             (void *)request_record_get_client_tag(record_p));

        if (NULL == request_ctrl_block_p) {
            ATC_LOG_E("exe_sms_list_state_set_status: request_ctrl_block_p NULL");
            exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;
            goto end;
        }

        request_status = Request_SMS_ShortMessageStatusSet(request_ctrl_block_p,
                         &slot,
                         SMS_STATUS_READ,
                         &sms_error);

        smsclient_free_requestctrlblock(&request_ctrl_block_p);

        if (request_status != SMS_REQUEST_PENDING) {
            exe_cmgl_response_p->error = SMS_ErrorToCMS(sms_error);
            print_sms_error("exe_sms_list_state_set_status, Request_SMS_ShortMessageDelete failed --> ", sms_error);
            goto end;
        }

        record_p->state = EXE_STATE_MSG_SET_STATUS_RESPONSE;
        result = EXE_PENDING;
    } else {
        /* No need to change status of message so find next message to add to list. */
        result = exe_sms_list_find_message(record_p, smsclient_p, exe_cmgl_request_p, exe_cmgl_response_p);
        record_p->state = EXE_STATE_MSG_READ_SMS;
    }

end:
    free(response_pdu_p);
    return result;
}

/* This is a local function used by request_sms_list */
static exe_request_result_t exe_sms_list_state_set_status_response(exe_request_record_t *record_p, smsclient_t *smsclient_p, exe_cmgl_request_t *exe_cmgl_request_p, exe_cmgl_response_t *exe_cmgl_response_p)
{
    exe_request_result_t result = EXE_FAILURE;
    SMS_Error_t          sms_error = SMS_ERROR_INTERN_NO_ERROR;
    SMS_RequestStatus_t  request_status = SMS_REQUEST_OK;

    ATC_LOG_I("exe_sms_list_state_set_status_response");

    request_status = Response_SMS_ShortMessageStatusSet(record_p->response_data_p, &sms_error);
    exe_cmgl_response_p->error = EXE_CMS_UNKNOWN_ERROR;

    if (SMS_REQUEST_FAILED_PARAMETER != request_status && SMS_REQUEST_FAILED_UNPACKING != request_status) {
        exe_cmgl_response_p->error = SMS_ErrorToCMS(sms_error);
    }

    if (SMS_REQUEST_OK != request_status || SMS_ERROR_INTERN_NO_ERROR != sms_error) {
        print_sms_error("exe_sms_list_state_set_status_response, Response_SMS_ShortMessageStatusSet failed --> ", sms_error);
    } else {
        /* Find next message to add to list. */
        result = exe_sms_list_find_message(record_p, smsclient_p, exe_cmgl_request_p, exe_cmgl_response_p);
        record_p->state = EXE_STATE_MSG_READ_SMS;
    }

    return result;
}

exe_request_result_t request_sms_list(exe_request_record_t *record_p)
{
    smsclient_t            *smsclient_p = NULL;
    exe_cmgl_request_t     *exe_cmgl_request_p = record_p->request_data_p;
    exe_cmgl_response_t    *exe_cmgl_response_p = NULL;
    exe_request_result_t    result = EXE_FAILURE;

    smsclient_p = exe_get_smsclient(record_p->exe_p);

    if (NULL == smsclient_p) {
        ATC_LOG_E("request_sms_list: smsclient_p is NULL");
        goto end;
    }

    if (NULL == exe_cmgl_request_p) {
        ATC_LOG_E("request_sms_list: request_data_p NULL");
        goto end;
    }

    exe_cmgl_response_p = request_record_get_client_data(record_p);

    if (NULL == exe_cmgl_response_p && EXE_STATE_REQUEST != record_p->state) {
        ATC_LOG_E("request_sms_list: exe_cmgl_response_p NULL");
        goto end;
    }

    switch (record_p->state) {
    case EXE_STATE_REQUEST:
        result = exe_sms_list_state_request(record_p, smsclient_p, exe_cmgl_request_p, exe_cmgl_response_p);
        break;
    case EXE_STATE_MSG_READ_SMS:
        result = exe_sms_list_state_read(record_p, smsclient_p, exe_cmgl_request_p, exe_cmgl_response_p);
        break;
    case EXE_STATE_MSG_SET_STATUS:
        result = exe_sms_list_state_set_status(record_p, smsclient_p, exe_cmgl_request_p, exe_cmgl_response_p);
        break;
    case EXE_STATE_MSG_SET_STATUS_RESPONSE:
        result = exe_sms_list_state_set_status_response(record_p, smsclient_p, exe_cmgl_request_p, exe_cmgl_response_p);
        break;
    default:
        ATC_LOG_E("request_sms_list: Invalid state %d", record_p->state);
        break;
    }

end:

    if (EXE_PENDING != result) {
        exe_cmgl_response_t response_data;

        if (NULL == exe_cmgl_response_p) {
            /* We have been unable to malloc the response data structure. */
            response_data.num_pdus = 0;
            response_data.pdu_data_p = NULL;
            response_data.error = EXE_CMS_MEMORY_FAILURE;
        } else if (EXE_SUCCESS == result) {
            /* Success result. */
            response_data = *exe_cmgl_response_p;
        } else {
            /* Something has not completed successfully. */
            response_data.num_pdus = 0;
            response_data.pdu_data_p = NULL;
            response_data.error = exe_cmgl_response_p->error;
        }

        exe_request_complete(record_p, result, &response_data);

        /* Free memory allocated while handling CMGL. */
        free(exe_cmgl_request_p);

        if (NULL != exe_cmgl_response_p) {
            exe_cmgl_response_pdu_data_t *pdu_data_p = exe_cmgl_response_p->pdu_data_p;

            while (NULL != pdu_data_p) {
                exe_cmgl_response_pdu_data_t *next_p = pdu_data_p->next_p;
                free(pdu_data_p->pdu_p);
                free(pdu_data_p);
                pdu_data_p = next_p;
            }

            free(exe_cmgl_response_p);
        }
    }

    return result;
}
