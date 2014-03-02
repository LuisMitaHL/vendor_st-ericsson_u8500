/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <string.h>
#include "exe_internal.h"
#include "exe_start.h"
#include "exe_sms_client.h"
#include "exe_sms_requests.h"

#include "r_sms.h"
#include "t_sms.h"
#include "r_cbs.h"
#include "r_sms_cb_session.h"

struct smsclient_s {
    int fd_request;
    int fd_event;
    bool subscribed;
};


/********************************************************************
 * Private prototypes
 ********************************************************************
 */
static void smsclient_handle_event_sms_short_message_received(void *ReceivedSignal_p);
static void smsclient_handle_event_sms_status_report_received(void *ReceivedSignal_p);
static bool smsclient_selector_callback_event(int fd, void *data_p);
static bool smsclient_selector_callback_request(int fd, void *data_p);

/********************************************************************
 * Private methods
 ********************************************************************
 */
static exe_sms_class_t smsclient_convert_category_to_class(SMS_Category_t category)
{
    exe_sms_class_t sms_class;

    switch (category) {
    case SMS_CATEGORY_CLASS_ZERO:
        sms_class = EXE_SMS_CATEGORY_CLASS_ZERO;
        break;
    case SMS_CATEGORY_STANDARD:
        sms_class = EXE_SMS_CATEGORY_CLASS_ONE;
        break;
    case SMS_CATEGORY_SIM_SPECIFIC:
    case SMS_CATEGORY_SIM_SPECIFIC_TO_BE_ERASED:
        sms_class = EXE_SMS_CATEGORY_CLASS_TWO;
        break;
    case SMS_CATEGORY_TE_SPECIFIC:
        sms_class = EXE_SMS_CATEGORY_CLASS_THREE;
        break;
    default:
        sms_class = EXE_SMS_CATEGORY_UNKNOWN;
        break;
    }

    return sms_class;
}

static uint8_t smsclient_get_coding_group(uint8_t pdu_dcs_byte)
{
    return (pdu_dcs_byte & EXE_SMS_CODING_GROUP_MASK);
}

static uint8_t smsclient_get_data_coding_scheme(const uint8_t *pdu_p)
{
    uint8_t adress_offset;
    uint8_t address_length = pdu_p[1];

    if (address_length % 2) {
        adress_offset = address_length / 2 + 1;
    } else {
        adress_offset = address_length / 2;
    }

    /* skip first octet + address length + address type + address offset + protocol identifier */
    pdu_p += 4 + adress_offset;

    return *pdu_p;
}

static void smsclient_handle_event_sms_short_message_received(void *received_signal_p)
{
    SMS_ShortMessageReceived_t *sms_received_p = NULL;
    SMS_EventStatus_t eventStatus;
    uint8_t *pdu_p = NULL;
    uint8_t index = 0;
    exe_sms_mt_t sms_mt;
    uint8_t dcs = 0;

    sms_received_p = calloc(1, sizeof(SMS_ShortMessageReceived_t));

    if (NULL == sms_received_p) {
        ATC_LOG_E("sms_received_p allocation failed!");
        goto exit;
    }

    /* Unpack event */
    eventStatus = Event_SMS_ShortMessageReceived(received_signal_p, sms_received_p);

    if (SMS_EVENT_OK != eventStatus) {
        ATC_LOG_E("Event_SMS_ShortMessageReceived failed!");
        ATC_LOG_E("SMS_EventStatus: %d", eventStatus);
        goto exit;
    }

    /* Allocate PDU buffer */
    pdu_p = calloc(1, sizeof(SMS_SMSC_Address_TPDU_t));

    if (pdu_p == NULL) {
        ATC_LOG_E("pdu_p allocation failed!");
        goto exit;
    }

    index = smsutil_sms_to_pdu(&(sms_received_p->SMSC_Address_TPDU), pdu_p);

    dcs = smsclient_get_data_coding_scheme(sms_received_p->SMSC_Address_TPDU.TPDU.Data);
    sms_mt.class = smsclient_convert_category_to_class(sms_received_p->Category);
    sms_mt.group = smsclient_get_coding_group(dcs);
    sms_mt.slot.storage = (msg_storage_t)sms_received_p->Slot.Storage;
    sms_mt.slot.position = (exe_msg_position_t)sms_received_p->Slot.Position;
    sms_mt.total_length = index;
    sms_mt.tpdu_length = sms_received_p->SMSC_Address_TPDU.TPDU.Length;
    sms_mt.pdu_p = pdu_p;

    exe_event(EXE_UNSOLICITED_CMT, &sms_mt);

    if (0 != sms_received_p->Slot.Position) {
        exe_event(EXE_UNSOLICITED_CMTI, &sms_mt);
    }

exit:
    free(sms_received_p);
    free(pdu_p);
}

static void smsclient_handle_event_sms_status_report_received(void *received_signal_p)
{
    SMS_SMSC_Address_TPDU_t *cds_tpdu_p = NULL;
    SMS_Slot_t slot;
    SMS_EventStatus_t event_status;
    uint8_t *pdu_p = NULL;
    uint8_t index = 0;
    exe_sms_status_report_t cds;

    cds_tpdu_p = calloc(1, sizeof(SMS_SMSC_Address_TPDU_t));

    if (NULL == cds_tpdu_p) {
        ATC_LOG_E("smsclient_handle_event_sms_status_report_received: malloc cds_tpdu_p failed!");
        goto exit;
    }

    /* Unpack event */
    event_status = Event_SMS_StatusReportReceived(received_signal_p, &slot, cds_tpdu_p);

    if (SMS_EVENT_OK != event_status) {
        ATC_LOG_E("Event_SMS_StatusReportReceived failed!");
        ATC_LOG_E("SMS_EventStatus: %d", event_status);
        goto exit;
    }

    /* Allocate PDU buffer */
    pdu_p = calloc(1, sizeof(SMS_SMSC_Address_TPDU_t));

    if (NULL == pdu_p) {
        ATC_LOG_E("malloc pdu_p failed!");
        goto exit;
    }

    index = smsutil_sms_to_pdu(cds_tpdu_p, pdu_p);

    cds.slot.storage = slot.Storage;
    cds.slot.position = slot.Position;
    cds.total_length = index;
    cds.tpdu_length = cds_tpdu_p->TPDU.Length;
    cds.pdu_p = pdu_p;
    exe_event(EXE_UNSOLICITED_CDS, &cds);

    if (0 != slot.Position) {
        exe_event(EXE_UNSOLICITED_CDSI, &cds);
    }

exit:
    free(cds_tpdu_p);
    free(pdu_p);
}

static void smsclient_handle_event_cbs_cell_broadcast_message_received(union SMS_SIGNAL *received_signal_p)
{
    SMS_EventStatus_t event_status;
    CB_Pdu_t *Pdu_p = NULL;
    uint16_t i;

    Pdu_p = (CB_Pdu_t *) calloc(1, sizeof(CB_Pdu_t));

    if (NULL == Pdu_p) {
        ATC_LOG_E("Pdu_p allocation failed!");
        goto exit;
    }

    event_status = Event_CBS_CellBroadcastMessageReceived(received_signal_p, NULL, Pdu_p);

    if (SMS_EVENT_OK != event_status) {
        ATC_LOG_E("Event_CBS_CellBroadcastMessageReceived failed! SMS_EventStatus: %d", event_status);
        goto exit;
    }

    exe_event(EXE_UNSOLICITED_CBM, (exe_cbm_t *) Pdu_p);

exit:
    free(Pdu_p);
}

static void smsclient_handle_event_sms_storage_status_changed(void *received_signal_p)
{
    SMS_StorageStatus_t *storage_status_p = NULL;
    SMS_EventStatus_t event_status;
    exe_ciev_storage_status_t ciev_storage_status;

    storage_status_p = calloc(1, sizeof(SMS_StorageStatus_t));

    if (NULL == storage_status_p) {
        ATC_LOG_E("smsclient_handle_event_sms_storage_status_changed: malloc storage_status_p failed!");
        goto exit;
    }

    /* Unpack event */
    event_status = Event_SMS_StorageStatusChanged(received_signal_p, storage_status_p);

    if (SMS_EVENT_OK != event_status) {
        ATC_LOG_E("smsclient_handle_event_sms_storage_status_changed: Event_SMS_StorageStatusChanged failed!");
        ATC_LOG_E("SMS_EventStatus: %d", event_status);
        goto exit;
    }

    ciev_storage_status.storage_full_SIM = storage_status_p->StorageFullSIM;

    exe_event(EXE_UNSOLICITED_CIEV_STORAGE, &ciev_storage_status);

exit:
    free(storage_status_p);
}

static bool smsclient_selector_callback_event(int fd, void *data_p)
{
    smsclient_t     *smsclient_p = (smsclient_t *) data_p;
    uint32_t        primitive = 0;
    SMS_ClientTag_t client_tag;
    void            *signal_p = NULL;

    if (NULL == smsclient_p) {
        ATC_LOG_I("smsclient_selector_callback_event: smsclient is NULL!");
        goto exit;
    }

    signal_p = Util_SMS_SignalReceiveOnSocket(smsclient_p->fd_event, &primitive, &client_tag);

    if (NULL != signal_p) {
        switch (primitive) {
        case EVENT_CBS_CELLBROADCASTMESSAGERECEIVED:
            smsclient_handle_event_cbs_cell_broadcast_message_received(signal_p);
            break;
        case EVENT_SMS_SHORTMESSAGERECEIVED:
            smsclient_handle_event_sms_short_message_received(signal_p);
            break;
        case EVENT_SMS_STATUSREPORTRECEIVED:
            smsclient_handle_event_sms_status_report_received(signal_p);
            break;
        case EVENT_SMS_STORAGESTATUSCHANGED:
            smsclient_handle_event_sms_storage_status_changed(signal_p);
            break;
        default:
            ATC_LOG_E("smsclient_selector_callback_event: got an illegal request");
            break;
        }

        Util_SMS_SignalFree(signal_p);
    } else {
        ATC_LOG_E("********************************************************");
        ATC_LOG_E("***** at_core lost contact with sms deamon         *****");
        ATC_LOG_E("********************************************************");

        /* Do nothing. Let the request callback handle the disconnect */

        /* Returning false will remove the fd from the select-loop */
        return false;
    }

exit:
    return true;
}

static bool smsclient_selector_callback_request(int fd, void *data_p)
{
    smsclient_t                    *smsclient_p         = (smsclient_t *) data_p;
    uint32_t        primitive = 0;
    SMS_ClientTag_t client_tag = 0;
    void            *signal_p = NULL;
    exe_request_t request = NULL;
    exe_request_record_t *request_record_p = NULL;
    int request_id = 0;

    if (NULL == smsclient_p) {
        ATC_LOG_I("smsclient_selector_callback_request: smsclient is NULL!");
        goto exit;
    }

    signal_p = Util_SMS_SignalReceiveOnSocket(smsclient_p->fd_request, &primitive, &client_tag);

    if (signal_p) {
        ATC_LOG_I("smsclient_selector_callback_request: clientTag: %d", client_tag);
        request_id = client_tag;
        request_record_p = request_record_get_record_from_client_tag(request_id);

        if (NULL != request_record_p) {
            request = request_record_get_request(request_record_p);

            if (request != NULL) {
                request_record_set_response_data(request_record_p, (void *)signal_p);
                /* Call the handler function with the actual queue record */
                request(request_record_p);
            } else {
                ATC_LOG_E("smsclient_selector_callback_request: Client tag data corrupt");
            }
        } else {
            ATC_LOG_E("smsclient_selector_callback_request: request record is NULL");
        }

        Util_SMS_SignalFree(signal_p);
    } else {
        ATC_LOG_E("********************************************************");
        ATC_LOG_E("***** at_core lost contact with sms deamon         *****");
        ATC_LOG_E("********************************************************");

        /* Connection with SMS server lost. Shut down this end and clean-up*/
        smsclient_close_session(smsclient_p);

        /* Returning false will remove the fd from the select-loop */
        return false;
    }

exit:
    return true;
}

/********************************************************************
 * Public methods
 ********************************************************************
 */

void smsclient_set_sms_subscription(smsclient_t *sms_client_p, bool sms_subscribed)
{
    if (NULL != sms_client_p) {
        sms_client_p->subscribed = sms_subscribed;
    }
}

bool smsclient_get_sms_subscription(smsclient_t *sms_client_p)
{
    if (NULL != sms_client_p) {
        return sms_client_p->subscribed;
    } else {
        return false;
    }
}

void *smsclient_open_session()
{
    SMS_RequestStatus_t         request_status          = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error               = SMS_ERROR_INTERN_NO_ERROR;
    smsclient_t *smsclient_p = calloc(1, sizeof(*smsclient_p));

    ATC_LOG_D("called");

    request_status = Request_SMS_CB_SessionCreate(&smsclient_p->fd_request, &smsclient_p->fd_event, &sms_error);

    if (request_status != SMS_REQUEST_OK) {
        ATC_LOG_E("Request_SMS_CB_SessionCreate failed --> %s", str_sms_error(sms_error));
        goto error;
    }

    /* Set service status */
    exe_start_set_status(EXE_START_SMS, EXE_START_CONNECTED_READY);

    /* Register file descriptors to monitor */
    selector_register_callback_for_fd(smsclient_p->fd_request, smsclient_selector_callback_request, (void *) smsclient_p);
    selector_register_callback_for_fd(smsclient_p->fd_event, smsclient_selector_callback_event, (void *) smsclient_p);
    ATC_LOG_I("SUCCEDED FD request = %d, event = %d",
              smsclient_p->fd_request, smsclient_p->fd_event);

    return smsclient_p;

error:
    free(smsclient_p);
    return NULL;
}

void smsclient_close_session(void *service_p)
{
    smsclient_t *smsclient_p = (smsclient_t *)service_p;
    SMS_RequestControlBlock_t  *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t         request_status       = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error            = SMS_ERROR_INTERN_NO_ERROR;
    exe_request_result_t        request_result       = EXE_FAILURE;

    ATC_LOG_D("called");

    if (NULL == smsclient_p) {
        ATC_LOG_E("smsclient_p is NULL");
        return;
    }

    request_result = smsclient_sms_event_unsubscribe(smsclient_p, NULL);

    if (EXE_SUCCESS != request_result) {
        ATC_LOG_E("smsclient_sms_event_unsubscribe failed");
        return;
    }

    smsclient_set_sms_subscription(smsclient_p, false);

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, smsclient_close_session);

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("smsclient_alloc_requestctrlblock failed");
        return;
    }

    request_ctrl_block_p->WaitMode = SMS_WAIT_MODE;
    request_status = Request_SMS_CB_SessionDestroy(request_ctrl_block_p, smsclient_p->fd_event, &sms_error);

    /*
     * NOTE: The request will return with the request_status set to
     * SMS_REQUEST_FAILED_PARAMETER if the socket to the server is
     * disconnected.
     */
    if (SMS_REQUEST_OK != request_status &&
            SMS_REQUEST_FAILED_PARAMETER != request_status) {
        ATC_LOG_E("Request_SMS_CB_SessionDestroy failed --> %s", str_sms_error(sms_error));
    }

    selector_deregister_callback_for_fd(smsclient_p->fd_request);
    selector_deregister_callback_for_fd(smsclient_p->fd_event);

    smsclient_free_requestctrlblock(&request_ctrl_block_p);
    free(smsclient_p);
    smsclient_p = NULL;

    /* Set service status */
    exe_start_set_status(EXE_START_SMS, EXE_START_NOT_CONNECTED);
}

exe_request_result_t smsclient_sms_event_subscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t  *request_ctrl_block_p    = NULL;
    SMS_RequestStatus_t         request_status          = SMS_REQUEST_OK;
    SMS_Error_t                 sms_error               = SMS_ERROR_INTERN_NO_ERROR;

    ATC_LOG_D("called");

    if (NULL == smsclient_p) {
        ATC_LOG_E("smsclient_p is NULL");
        goto error;
    }

    request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    request_status = Request_SMS_EventsSubscribe(request_ctrl_block_p, &sms_error);
    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    if (request_status != SMS_REQUEST_PENDING) {
        ATC_LOG_E("Request_SMS_EventsSubscribe failed --> %s", str_sms_error(sms_error));
        goto error;
    }

    return EXE_PENDING;
error:
    return EXE_FAILURE;
}

exe_request_result_t smsclient_sms_event_unsubscribe(smsclient_t *smsclient_p, exe_request_record_t *record_p)
{
    SMS_RequestControlBlock_t *request_ctrl_block_p = NULL;
    SMS_RequestStatus_t        request_status       = SMS_REQUEST_OK;
    SMS_Error_t                sms_error            = SMS_ERROR_INTERN_NO_ERROR;
    exe_request_result_t       result               = EXE_FAILURE;

    ATC_LOG_D("called");

    if (NULL == smsclient_p) {
        ATC_LOG_I("smsclient_p is NULL!");
        goto error;
    }

    if (NULL != record_p) {
        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, (void *)request_record_get_client_tag(record_p));
        result = EXE_PENDING;
    } else {
        request_ctrl_block_p = smsclient_alloc_requestctrlblock(smsclient_p, smsclient_sms_event_unsubscribe);
        result = EXE_SUCCESS;
    }

    if (NULL == request_ctrl_block_p) {
        ATC_LOG_E("smsclient_alloc_requestctrlblock failed");
        goto error;
    }

    if (NULL == record_p) {
        request_ctrl_block_p->WaitMode = SMS_WAIT_MODE;
    }

    request_status = Request_SMS_EventsUnsubscribe(request_ctrl_block_p, &sms_error);
    smsclient_free_requestctrlblock(&request_ctrl_block_p);

    /*
     * NOTE: The request will return with the request_status set to
     * SMS_REQUEST_FAILED_PARAMETER if the socket to the server is
     * disconnected.
     */
    if (SMS_REQUEST_OK != request_status &&
            SMS_REQUEST_FAILED_PARAMETER != request_status) {
        ATC_LOG_E("Request_SMS_EventsUnsubscribe failed --> %s", str_sms_error(sms_error));
        goto error;
    }

    return result;
error:
    return EXE_FAILURE;
}

SMS_RequestControlBlock_t *smsclient_alloc_requestctrlblock(smsclient_t *smsclient_p, void *data_p)
{
    SMS_RequestControlBlock_t *request_ctrl_block_p = malloc(sizeof(SMS_RequestControlBlock_t));

    if (NULL != request_ctrl_block_p) {
        request_ctrl_block_p->WaitMode  = SMS_NO_WAIT_MODE;
        /* Cast the pointer to a ClientTag (uint32_t) */
        request_ctrl_block_p->ClientTag = (SMS_ClientTag_t) ((uintptr_t)data_p);
        request_ctrl_block_p->Socket    = smsclient_p->fd_request;
    }

    return request_ctrl_block_p;
}

void smsclient_free_requestctrlblock(SMS_RequestControlBlock_t **requestCtrlBlock_pp)
{
    if (*requestCtrlBlock_pp != NULL) {
        free(*requestCtrlBlock_pp);
        *requestCtrlBlock_pp = NULL;
    }
}

void find_sms_init(exe_msg_search_info_t *const search_info_p, exe_msg_status_t sms_status, msg_storage_t storage)
{
    /* Initiate search info for first search */
    search_info_p->status        = sms_status;
    search_info_p->browse_option = MSG_FIRST;
    search_info_p->slot.position = 1;   /* First search; Start at position 0 */
    search_info_p->slot.storage  = storage;
}

bool find_sms_slot_set(SMS_Slot_t *const slot_p,  msg_storage_t storage, exe_msg_position_t position)
{
    bool result = true;

    slot_p->Position = (SMS_Position_t) position;

    switch (storage) {
    case MSG_STORAGE_ME:
        slot_p->Storage = SMS_STORAGE_ME;
        break;
    case MSG_STORAGE_SM:
        slot_p->Storage = SMS_STORAGE_SM;
        break;
    case MSG_STORAGE_MT:
        slot_p->Storage = SMS_STORAGE_MT;
        break;
    case MSG_STORAGE_UNKNOWN:
    default:
        result = false;
        break;
    }

    return result;
}

bool find_sms_set(SMS_SearchInfo_t *const search_info_p,
                  exe_cmgl_status_t status,
                  msg_storage_t storage,
                  exe_msg_position_t position,
                  exe_msg_browse_option_t browse)
{
    bool result = true;

    search_info_p->BrowseOption = (SMS_BrowseOption_t) browse;

    switch (status) {
    case EXE_CMGL_STATUS_REC_UNREAD:
        search_info_p->Status = SMS_STATUS_UNREAD;
        break;
    case EXE_CMGL_STATUS_REC_READ:
        search_info_p->Status = SMS_STATUS_READ;
        break;
    case EXE_CMGL_STATUS_STO_UNSENT:
        search_info_p->Status = SMS_STATUS_UNSENT;
        break;
    case EXE_CMGL_STATUS_STO_SENT:
        search_info_p->Status = SMS_STATUS_SENT;
        break;
    case EXE_CMGL_STATUS_ALL:
        search_info_p->Status = SMS_STATUS_ANY_STATUS;
        break;
    default:
        result = false;
        break;
    }

    if (result) {
        result = find_sms_slot_set(&search_info_p->Slot, storage, position);
    }

    return result;
}
