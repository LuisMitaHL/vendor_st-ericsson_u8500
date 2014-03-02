/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <string.h>

#include "t_sms.h"
#include "r_sms.h"
#include "t_cbs.h"
#include "r_cbs.h"
#include "t_sms_cb_session.h"
#include "whitebox.h"
#include "wb_test.h"

/*
 * CMGL Tests require support for changing status of stored PDUs. Therefore the
 * stub must be more complicated for that test case.
 */
typedef struct {
    SMS_Slot_t Slot;
    SMS_Status_t Status;
    SMS_SMSC_Address_TPDU_t TPDU;
} sms_stub_cmgl_t;

#define SMS_STUB_CMGL_NUM_TPDUS 5
static sms_stub_cmgl_t *sms_stub_cmgl_data_p = NULL;
static SMS_SearchInfo_t sms_stub_cmgl_search_info = {SMS_STATUS_UNKNOWN, {SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID}, SMS_BROWSE_OPTION_FIRST};
static SMS_Slot_t sms_stub_cmgl_slot_write = {SMS_STORAGE_MT, SMS_STORAGE_POSITION_INVALID};
static SMS_Position_t sms_stub_cmgl_position_read = SMS_STORAGE_POSITION_INVALID;
bool first_time = true;
static SMS_MO_Route_t sms_route = SMS_MO_ROUTE_PS;


static void sms_stub_cmgl_data_init(void)
{
    if (sms_stub_cmgl_data_p == NULL) {
        sms_stub_cmgl_data_p = malloc(sizeof(sms_stub_cmgl_t) * SMS_STUB_CMGL_NUM_TPDUS);

        if (sms_stub_cmgl_data_p != NULL) {
            uint32_t Index;

            for (Index = 0; Index < SMS_STUB_CMGL_NUM_TPDUS; Index++) {
                sms_stub_cmgl_data_p[Index].Slot.Storage = SMS_STORAGE_MT;
                sms_stub_cmgl_data_p[Index].Slot.Position = SMS_STORAGE_POSITION_INVALID;
                sms_stub_cmgl_data_p[Index].Status = SMS_STATUS_UNKNOWN;
                memset(&sms_stub_cmgl_data_p[Index].TPDU, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));
                sms_stub_cmgl_data_p[Index].TPDU.ServiceCenterAddress.Length = 0;
                sms_stub_cmgl_data_p[Index].TPDU.TPDU.Length = 0;
            }
        }
    }
}

static void sms_stub_cmgl_data_destroy_check(void)
{
    if (sms_stub_cmgl_data_p != NULL) {
        uint32_t Index;

        for (Index = 0; Index < SMS_STUB_CMGL_NUM_TPDUS; Index++) {
            if (sms_stub_cmgl_data_p[Index].Status != SMS_STATUS_UNKNOWN ||
                    sms_stub_cmgl_data_p[Index].TPDU.ServiceCenterAddress.Length != 0 ||
                    sms_stub_cmgl_data_p[Index].TPDU.TPDU.Length != 0 ||
                    sms_stub_cmgl_data_p[Index].Slot.Storage != SMS_STORAGE_MT ||
                    sms_stub_cmgl_data_p[Index].Slot.Position != SMS_STORAGE_POSITION_INVALID) {
                /* Non-empty data found. Do not destroy data structure. */
                return;
            }
        }

        free(sms_stub_cmgl_data_p);
        sms_stub_cmgl_data_p = NULL;
    }
}

static void sms_stub_cmgl_data_delete(const SMS_Position_t Position)
{
    uint32_t Index;
    sms_stub_cmgl_data_init();

    if (Position == SMS_STORAGE_POSITION_INVALID || Position > SMS_STUB_CMGL_NUM_TPDUS) {
        /* Invalid position */
        return;
    } else {
        /* Convert from 1-based to 0-based. */
        Index = Position - 1;
    }

    if (sms_stub_cmgl_data_p != NULL) {
        sms_stub_cmgl_data_p[Index].Slot.Storage = SMS_STORAGE_MT;
        sms_stub_cmgl_data_p[Index].Slot.Position = SMS_STORAGE_POSITION_INVALID;
        sms_stub_cmgl_data_p[Index].Status = SMS_STATUS_UNKNOWN;
        memset(&sms_stub_cmgl_data_p[Index].TPDU, 0xFF, sizeof(SMS_SMSC_Address_TPDU_t));
        sms_stub_cmgl_data_p[Index].TPDU.ServiceCenterAddress.Length = 0;
        sms_stub_cmgl_data_p[Index].TPDU.TPDU.Length = 0;
    }

    sms_stub_cmgl_data_destroy_check();
}

static void sms_stub_cmgl_data_write(SMS_Slot_t *const Slot_p, const SMS_Status_t Status, const SMS_SMSC_Address_TPDU_t *const TPDU_p)
{
    uint32_t Index = UINT32_MAX;
    sms_stub_cmgl_data_init();

    if (Slot_p->Position == SMS_STORAGE_POSITION_INVALID || Slot_p->Position > SMS_STUB_CMGL_NUM_TPDUS) {
        uint32_t n;

        /* Invalid position supplied. Find unused space. */
        for (n = 0; n < SMS_STUB_CMGL_NUM_TPDUS; n++) {
            if (sms_stub_cmgl_data_p[n].Slot.Position == SMS_STORAGE_POSITION_INVALID) {
                Slot_p->Position = n + 1; /* Convert 0-based to 1-based. */
                Index = n;
                break;
            }
        }
    } else {
        /* Convert from 1-based to 0-based. */
        Index = Slot_p->Position - 1;
    }

    if (sms_stub_cmgl_data_p != NULL && Index != UINT32_MAX) {
        sms_stub_cmgl_data_p[Index].Slot = *Slot_p;
        sms_stub_cmgl_data_p[Index].Status = Status;
        memcpy(&sms_stub_cmgl_data_p[Index].TPDU, TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t));
    }
}

static void sms_stub_cmgl_data_read(const SMS_Position_t Position, SMS_Status_t *const Status_p, SMS_SMSC_Address_TPDU_t *const TPDU_p)
{
    uint32_t Index;
    sms_stub_cmgl_data_init();

    if (Position == SMS_STORAGE_POSITION_INVALID || Position > SMS_STUB_CMGL_NUM_TPDUS) {
        /* Invalid position */
        return;
    } else {
        /* Convert from 1-based to 0-based. */
        Index = Position - 1;
    }

    if (sms_stub_cmgl_data_p != NULL && TPDU_p != NULL && Status_p != NULL) {
        *Status_p = sms_stub_cmgl_data_p[Index].Status;
        memcpy(TPDU_p, &sms_stub_cmgl_data_p[Index].TPDU, sizeof(SMS_SMSC_Address_TPDU_t));
        /* Adjust the SMSC Address length to subtract the length of the TypeOfAddress byte */
        TPDU_p->ServiceCenterAddress.Length--;
    }
}

static bool sms_stub_cmgl_data_find(const SMS_SearchInfo_t *const SearchInfo_p, SMS_Slot_t *const Slot_p)
{
    uint32_t Index;
    bool MatchFound = false;

    if (sms_stub_cmgl_data_p != NULL && SearchInfo_p != NULL && Slot_p != NULL) {
        if (SearchInfo_p->BrowseOption == SMS_BROWSE_OPTION_FIRST) {
            /* Start search from start of buffer. */
            Index = 0;
        } else {
            /* Start search from slot after supplied. */
            Index = SearchInfo_p->Slot.Position; /* Position will be 1-based. */
        }

        for (; Index < SMS_STUB_CMGL_NUM_TPDUS; Index++) {
            if ((sms_stub_cmgl_data_p[Index].Status == SearchInfo_p->Status ||
                    SMS_STATUS_ANY_STATUS == SearchInfo_p->Status) &&
                    SMS_STATUS_UNKNOWN != sms_stub_cmgl_data_p[Index].Status) {
                *Slot_p = sms_stub_cmgl_data_p[Index].Slot;
                MatchFound = true;
                break;
            }
        }
    }

    return MatchFound;
}

static void sms_stub_cmgl_data_status_update(const SMS_Position_t Position, const SMS_Status_t Status)
{
    uint32_t Index;

    if (Position == SMS_STORAGE_POSITION_INVALID || Position > SMS_STUB_CMGL_NUM_TPDUS) {
        /* Invalid position */
        return;
    } else {
        /* Convert from 1-based to 0-based. */
        Index = Position - 1;
    }

    if (sms_stub_cmgl_data_p != NULL) {
        sms_stub_cmgl_data_p[Index].Status = Status;
    }
}

SMS_RequestStatus_t Request_SMS_CB_SessionCreate(
    int         *const  RequestResponseSocket_p,
    int         *const  EventSocket_p,
    SMS_Error_t *const  Error_p)
{
    *RequestResponseSocket_p = FD_SMS_REQUEST;
    *EventSocket_p = FD_SMS_EVENT;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_CB_SessionDestroy(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const int                               EventSocket,
    SMS_Error_t                     *const  Error_p)
{
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_EventsSubscribe(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    SMS_Error_t                     *const  Error_p)
{

    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_SMS_EventsSubscribe(
    const void                    *const  SigStruct_p,
    SMS_Error_t                   *const  Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_EventsUnsubscribe(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    SMS_Error_t                     *const  Error_p)
{
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Response_SMS_EventsUnsubscribe(
    const void                    *const  SigStruct_p,
    SMS_Error_t                   *const  Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_CBS_Subscribe(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const uint8_t                           SubscribeAll_CBMI,
    const uint8_t                           NumberOfSubscriptions,
    const CBS_Subscription_t        *const  Subscriptions_p,
    CBS_Error_t                     *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *Error_p = CBS_ERROR_NONE;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_CBS_Subscribe(
    const void        *const  SigStruct_p,
    CBS_Error_t       *const  Error_p)
{
    *Error_p = CBS_ERROR_NONE;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_CBS_Unsubscribe(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const uint8_t                           UnsubscribeAll_CBMI,
    const uint8_t                           NumberOfSubscriptions,
    const CBS_Subscription_t        *const  Subscriptions_p,
    CBS_Error_t               *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_CBS_Unsubscribe(
    const void        *const  SigStruct_p,
    CBS_Error_t *const  Error_p)
{
    *Error_p = CBS_ERROR_NONE;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_CBS_Activate(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    CBS_Error_t               *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_CBS_Activate(
    const void        *const  SigStruct_p,
    CBS_Error_t       *const  Error_p)
{
    *Error_p = CBS_ERROR_NONE;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_CBS_Deactivate(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    CBS_Error_t               *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_CBS_Deactivate(
    const void        *const  SigStruct_p,
    CBS_Error_t       *const  Error_p)
{
    *Error_p = CBS_ERROR_NONE;
    return SMS_REQUEST_OK;
}
SMS_RequestStatus_t Request_CBS_GetNumberOfSubscriptions(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const uint8_t                           AllSubscriptions,
    uint8_t                   *const  NumberOfSubscriptions_p,
    CBS_Error_t               *const  Error_p)

{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}


SMS_RequestStatus_t Response_CBS_GetNumberOfSubscriptions(
    const void                   *const  SigStruct_p,
    uint8_t                *const  NumberOfSubscriptions_p,
    CBS_Error_t            *const  Error_p)
{
    *NumberOfSubscriptions_p = 1;
    *Error_p = CBS_ERROR_NONE;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_CBS_GetSubscriptions(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const uint8_t                           AllSubscriptions,
    uint8_t                   *const  NumberOfSubscriptions_p,
    CBS_Subscription_t        *const  Subscriptions_p,
    CBS_Error_t               *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_CBS_GetSubscriptions(
    const void                   *const  SigStruct_p,
    uint8_t                *const  NumberOfSubscriptions_p,
    CBS_Subscription_t     *const  Subscriptions_p,
    CBS_Error_t            *const  Error_p)
{
    *NumberOfSubscriptions_p = 1;
    Subscriptions_p->FirstDataCodingScheme = 1;
    Subscriptions_p->LastDataCodingScheme = 10;
    Subscriptions_p->FirstMessageIdentifier = 40;
    Subscriptions_p->LastMessageIdentifier = 50;

    *Error_p = CBS_ERROR_NONE;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_ShortMessageSend(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t   *const  SMSC_Address_TPDU_p,
    const SMS_Slot_t                *const  Slot_p,
    const uint8_t                           MoreToSend,
    uint8_t                         *const  SM_Reference_p,
    SMS_Error_t                     *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);

    if (CURRENT_TEST_CASE == wb_testcase_cmms_cmgs && !MoreToSend && first_time) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    if (CURRENT_TEST_CASE == wb_testcase_cmms_cmgs && first_time) {
        first_time = false;
    } else if (CURRENT_TEST_CASE == wb_testcase_cmms_cmgs && !first_time) {
        first_time = true;
    }

    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Request_SMS_ShortMessageWrite(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const SMS_Status_t                      Status,
    const SMS_SMSC_Address_TPDU_t   *const  SMSC_Address_TPDU_p,
    SMS_Slot_t                *const  Slot_p,
    SMS_Error_t               *const  Error_p)
{
    if (wb_testcase_cmgl == CURRENT_TEST_CASE) {
        sms_stub_cmgl_data_write(Slot_p, Status, SMSC_Address_TPDU_p);
        sms_stub_cmgl_slot_write = *Slot_p;
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    }

    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}


SMS_RequestStatus_t Request_SMS_MemCapacityAvailableSend(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    SMS_Error_t                     *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Request_SMS_StorageCapacityGet(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const SMS_Storage_t                     Storage,
    SMS_SlotInformation_t           *const  SlotInformation_p,
    SMS_StorageStatus_t             *const  StorageStatus_p,
    SMS_Error_t                     *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_SMS_StorageCapacityGet(
    const void                *const  SigStruct_p,
    SMS_SlotInformation_t     *const  SlotInformation_p,
    SMS_StorageStatus_t       *const  StorageStatus_p,
    SMS_Error_t               *const  Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    SlotInformation_p->FreeCount = 5;
    SlotInformation_p->ReadCount = 1;
    SlotInformation_p->SentCount = 2;
    SlotInformation_p->UnreadCount = 3;
    SlotInformation_p->UnsentCount = 4;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_ShortMessageRead(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const SMS_Slot_t                *const  Slot_p,
    SMS_Status_t                    *const  Status_p,
    SMS_SMSC_Address_TPDU_t         *const  SMSC_Address_TPDU_p,
    SMS_Error_t                     *const  Error_p)

{
    if (wb_testcase_cmgl == CURRENT_TEST_CASE) {
        sms_stub_cmgl_position_read = Slot_p->Position;
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    }

    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_SMS_ShortMessageRead(
    const void              *const  SigStruct_p,
    SMS_Status_t            *const  Status_p,
    SMS_SMSC_Address_TPDU_t *const  SMSC_Address_TPDU_p,
    SMS_Error_t             *const  Error_p)
{
    uint8_t address_value[] = { 0x35, 0x85, 0x08, 0x77, 0x10, 0x10 };

    if (wb_testcase_cmss_set_new_recipient == CURRENT_TEST_CASE) {
        SMS_SMSC_Address_TPDU_t tpdu;
        SMS_Error_t error = SMS_ERROR_INTERN_NO_ERROR;
        SMS_Status_t status = SMS_STATUS_UNREAD;
        uint8_t pdu_data[] = {0x01, 0x00, 0x0d, 0x91, 0x44, 0x52, 0x89, 0x01, 0x51,
                              0x11, 0xf9, 0x00, 0x00, 0x05, 0xcb, 0x30, 0x9b, 0x5d, 0x06
                             };

        memset(&tpdu, 0x00, sizeof(SMS_SMSC_Address_TPDU_t));
        memcpy(&tpdu.TPDU.Data, pdu_data, 19);
        tpdu.TPDU.Length = 19;
        tpdu.ServiceCenterAddress.TypeOfAddress = 91;
        tpdu.ServiceCenterAddress.Length = sizeof(address_value);
        memcpy(&tpdu.ServiceCenterAddress.AddressValue, &address_value, tpdu.ServiceCenterAddress.Length);

        memcpy(SMSC_Address_TPDU_p, &tpdu, sizeof(SMS_SMSC_Address_TPDU_t));
        *Error_p = error;
        *Status_p = status;
        return SMS_REQUEST_OK;
    } else if (wb_testcase_cmgl == CURRENT_TEST_CASE) {
        sms_stub_cmgl_data_read(sms_stub_cmgl_position_read, Status_p, SMSC_Address_TPDU_p);
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        return SMS_REQUEST_OK;
    } else {
        uint8_t address_value[] = { 0x35, 0x85, 0x08, 0x77, 0x10, 0x10 };
        uint8_t sms_value[] = { 0x41, 0x42, 0x43 }; /* reads "ABC" */

        if (NULL != SMSC_Address_TPDU_p && NULL != Error_p) {

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
            *Status_p = SMS_STATUS_UNREAD;

            SMSC_Address_TPDU_p->ServiceCenterAddress.Length
            = sizeof(address_value);
            SMSC_Address_TPDU_p->ServiceCenterAddress.TypeOfAddress = 0x91;
            memmove(SMSC_Address_TPDU_p->ServiceCenterAddress.AddressValue,
                    address_value, sizeof(address_value));

            SMSC_Address_TPDU_p->TPDU.Length = sizeof(sms_value);
            memmove(SMSC_Address_TPDU_p->TPDU.Data,
                    sms_value, sizeof(sms_value));
            return SMS_REQUEST_OK;
        }
    }
}

SMS_EventStatus_t Event_SMS_ShortMessageReceived(
    const void *const SigStruct_p,
    SMS_ShortMessageReceived_t *const ShortMessageData_p)
{
    uint8_t address_value[] = { 0x35, 0x85, 0x08, 0x77, 0x10, 0x10 };
    uint8_t sms_value[] = { 0x41, 0x42, 0x43 }; /* reads "ABC" */

    if (NULL != ShortMessageData_p) {
        ShortMessageData_p->Category = SMS_CATEGORY_STANDARD;
        ShortMessageData_p->IsReplaceType = 0;
        ShortMessageData_p->Slot.Position = 2;
        ShortMessageData_p->Slot.Storage = SMS_STORAGE_ME; /* store on flash */
        /* SMSC adress */
        ShortMessageData_p->SMSC_Address_TPDU.ServiceCenterAddress.Length
        = sizeof(address_value);
        ShortMessageData_p->SMSC_Address_TPDU.ServiceCenterAddress.TypeOfAddress
        = 0x91;
        memmove(
            ShortMessageData_p->SMSC_Address_TPDU.ServiceCenterAddress.AddressValue,
            address_value, sizeof(address_value));
        ShortMessageData_p->SMSC_Address_TPDU.TPDU.Length
        = sizeof(sms_value);
        memmove(ShortMessageData_p->SMSC_Address_TPDU.TPDU.Data,
                sms_value, sizeof(sms_value));
        return SMS_EVENT_OK;
    } else {
        return SMS_EVENT_FAILED_UNPACKING;
    }
}

SMS_EventStatus_t Event_SMS_StatusReportReceived(
    const void              *const  SigStruct_p,
    SMS_Slot_t              *const  Slot_p,
    SMS_SMSC_Address_TPDU_t *const  StatusReport_p)
{
    uint8_t address_value[] = { 0x35, 0x85, 0x08, 0x77, 0x10, 0x10 };
    uint8_t sms_value[] = { 0x41, 0x42, 0x43 }; /* reads "ABC" */
    SMS_Storage_t storage = SMS_STORAGE_ME;
    SMS_Position_t position = 1;

    if ((NULL != StatusReport_p) && (NULL != Slot_p)) {
        /* SMSC adress */
        StatusReport_p->ServiceCenterAddress.Length
        = sizeof(address_value);
        StatusReport_p->ServiceCenterAddress.TypeOfAddress
        = 0x91;
        memmove(StatusReport_p->ServiceCenterAddress.AddressValue,
                address_value, sizeof(address_value));
        StatusReport_p->TPDU.Length
        = sizeof(sms_value);
        memmove(StatusReport_p->TPDU.Data,
                sms_value, sizeof(sms_value));

        Slot_p->Storage = storage;
        Slot_p->Position = position;
        return SMS_EVENT_OK;
    } else {
        return SMS_EVENT_FAILED_UNPACKING;
    }
}


SMS_EventStatus_t Event_CBS_CellBroadcastMessageReceived(
    const void *const SigStruct_p,
    SMS_ClientTag_t * const ClientTag_p,
    CB_Pdu_t * const Pdu_p) /* points to allocated buffer with max pdu length */
{
    uint8_t cbs_content_of_message[] = { 0xC0, 0x10, 0x00, 0x32, 0x0F, 0x11, 0x41, 0x42, 0x43}; // reads "ABC"*/

    if (NULL != Pdu_p) {
        Pdu_p->PduLength = sizeof(cbs_content_of_message);
        memmove(Pdu_p->Pdu , cbs_content_of_message, sizeof(cbs_content_of_message));
        return SMS_EVENT_OK;
    } else {
        return SMS_EVENT_FAILED_UNPACKING;
    }
}



SMS_EventStatus_t Event_SMS_StorageStatusChanged(
    const void          *const  SigStruct_p,
    SMS_StorageStatus_t *const  StorageStatus_p)
{
    StorageStatus_p->StorageFullSIM = 1;
    return SMS_EVENT_OK;
}



SMS_RequestStatus_t Request_SMS_DeliverReportSend(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const SMS_RP_ErrorCause_t               rp_error_cause,
    const SMS_TPDU_t                *const  TPDU_p,
    SMS_Error_t                     *const  Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cnma_0:
        /* fall through*/
    case wb_testcase_cnma_1:

        if (0 != rp_error_cause) {
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_cnma_2:

        if (SMS_ERROR_RP_TEMPORARY_FAILURE != rp_error_cause) {
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_esmsfull_set_0:

        if (SMS_ERROR_RP_TEMPORARY_FAILURE != rp_error_cause) {
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_esmsfull_set_1:

        if (SMS_ERROR_RP_TEMPORARY_FAILURE != rp_error_cause) {
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_cnma_3:
        break;

    default:
        return SMS_REQUEST_FAILED;
    }

    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_SMS_ShortMessageSend(
    const void      *const  SigStruct_p,
    uint8_t         *const  SM_Reference_p,
    SMS_Error_t     *const  Error_p)
{
    if (CURRENT_TEST_CASE == wb_testcase_cmgs_cms_error) {
        *Error_p = SMS_ERROR_RP_NETWORK_OUT_OF_ORDER;
        *SM_Reference_p = 0;
        return SMS_REQUEST_FAILED;
    } else {
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        *SM_Reference_p = 3;
        return SMS_REQUEST_OK;
    }
}

SMS_RequestStatus_t Response_SMS_ShortMessageWrite(
    const void            *const  SigStruct_p,
    SMS_Slot_t      *const  Slot_p,
    SMS_Error_t     *const  Error_p)
{
    if (CURRENT_TEST_CASE == wb_testcase_cmgl) {
        *Slot_p = sms_stub_cmgl_slot_write;

        if (Slot_p->Position != SMS_STORAGE_POSITION_INVALID) {
            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
            return SMS_REQUEST_OK;
        } else {
            *Error_p = SMS_ERROR_INTERN_STORAGE_FULL;
            return SMS_REQUEST_FAILED_APPLICATION;
        }
    } else {
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        Slot_p->Position = 3;
        Slot_p->Storage = SMS_STORAGE_SM;
        return SMS_REQUEST_OK;
    }
}

SMS_RequestStatus_t R_Req_SMS_DeliverReportControlSet(
    const SMS_RequestControlBlock_t *const RequestCtrl_p,
    const SMS_NetworkAcknowledge_t NetworkAcknowledge,
    SMS_Error_t *const Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t R_Resp_SMS_DeliverReportControlSet(
    const void *const SigStruct_p,
    SMS_Error_t *const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t R_Req_SMS_DeliverReportControlGet(
    const SMS_RequestControlBlock_t *const RequestCtrl_p,
    SMS_NetworkAcknowledge_t *const NetworkAcknowledge_p,
    SMS_Error_t *const Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t R_Resp_SMS_DeliverReportControlGet(
    const void *const SigStruct_p,
    SMS_NetworkAcknowledge_t *const NetworkAcknowledge_p,
    SMS_Error_t *const Error_p)
{
    *NetworkAcknowledge_p = SMS_NETWORK_ACKNOWLEDGE_NORMAL;
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_csms_read_failure:
        *Error_p = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
        break;
    case wb_testcase_csms_0:
        break;
    case wb_testcase_csms_1:
        /* fallthrough */
    default:
        *NetworkAcknowledge_p = SMS_NETWORK_ACKNOWLEDGE_CLIENT;
        break;
    }

    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Response_SMS_DeliverReportSend(
    const void      *const  SigStruct_p,
    SMS_Error_t     *const  Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Response_SMS_MemCapacityAvailableSend(
    const void      *const  SigStruct_p,
    SMS_Error_t     *const  Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_ShortMessageFind(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const SMS_SearchInfo_t          *const  SearchInfo_p,
    SMS_Slot_t                *const  Slot_p,
    SMS_Error_t               *const  Error_p)
{
    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cmgd1:

        if (MSG_STATUS_READ != SearchInfo_p->Status) {
            *Error_p = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_cmgd2:

        if (MSG_STATUS_SENT != SearchInfo_p->Status) {
            *Error_p = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_cmgd3:

        if (MSG_STATUS_UNSENT != SearchInfo_p->Status) {
            *Error_p = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_cmgd4:

        if (MSG_STATUS_ANY_STATUS != SearchInfo_p->Status) {
            *Error_p = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
            return SMS_REQUEST_FAILED;
        }

        break;
    case wb_testcase_cmgl:
        sms_stub_cmgl_search_info.Status = SearchInfo_p->Status;
        sms_stub_cmgl_search_info.Slot.Position = SearchInfo_p->Slot.Position;
        sms_stub_cmgl_search_info.Slot.Storage = SearchInfo_p->Slot.Storage;
        sms_stub_cmgl_search_info.BrowseOption = SearchInfo_p->BrowseOption;
        EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        return SMS_REQUEST_PENDING;
    default:
        break;
    }

    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    Slot_p->Position = 1;
    Slot_p->Storage = 1;
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_SMS_ShortMessageFind(
    const void            *const  SigStruct_p,
    SMS_Slot_t      *const  Slot_p,
    SMS_Error_t     *const  Error_p)
{
    if (CURRENT_TEST_CASE == wb_testcase_cmgl) {
        if (sms_stub_cmgl_data_find(&sms_stub_cmgl_search_info, Slot_p)) {
            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else {
            *Error_p = SMS_ERROR_INTERN_MESSAGE_NOT_FOUND;
        }
    } else {
        static sms_no = 2;

        if (sms_no > 0) {
            Slot_p->Position = 1;
            Slot_p->Storage = 1;
            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
            sms_no --;
        } else {
            Slot_p->Position = 0;
            Slot_p->Storage = 0;
            *Error_p = SMS_ERROR_INTERN_MESSAGE_NOT_FOUND;
            sms_no = 2;
        }
    }

    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_ShortMessageDelete(
    const SMS_RequestControlBlock_t *const  RequestCtrl_p,
    const SMS_Slot_t                *const  Slot_p,
    SMS_Error_t               *const  Error_p)
{
    if (wb_testcase_cmgl == CURRENT_TEST_CASE) {
        sms_stub_cmgl_data_delete(Slot_p->Position);
        EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        return SMS_REQUEST_PENDING;
    } else {
        EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
        *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        return SMS_REQUEST_PENDING;
    }
}

SMS_RequestStatus_t Response_SMS_ShortMessageDelete(
    const void            *const  SigStruct_p,
    SMS_Error_t     *const  Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t R_Req_SMS_RelayLinkControlSet(
    const SMS_RequestControlBlock_t *const RequestCtrl_p,
    const SMS_RelayControl_t               RelayLinkControl,
    SMS_Error_t               *const Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t R_Resp_SMS_RelayLinkControlSet(
    const void                   *const SigStruct_p,
    SMS_Error_t            *const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t R_Req_SMS_RelayLinkControlGet(
    const SMS_RequestControlBlock_t *const RequestCtrl_p,
    SMS_RelayControl_t        *const RelayLinkControl_p,
    SMS_Error_t               *const Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t R_Resp_SMS_RelayLinkControlGet(
    const void                   *const SigStruct_p,
    SMS_RelayControl_t     *const RelayLinkControl_p,
    SMS_Error_t            *const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    *RelayLinkControl_p = SMS_RELAY_CONTROL_TIMEOUT_ENABLED;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t R_Req_SMS_MO_RouteSet(
    const SMS_RequestControlBlock_t *const RequestCtrl_p,
    const SMS_MO_Route_t Route,
    SMS_Error_t *const Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    sms_route = Route;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t R_Resp_SMS_MO_RouteSet(
    const void *const SigStruct_p,
    SMS_Error_t *const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t R_Req_SMS_MO_RouteGet(
    const SMS_RequestControlBlock_t *const RequestCtrl_p,
    SMS_MO_Route_t *const Route_p,
    SMS_Error_t *const Error_p)
{
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t R_Resp_SMS_MO_RouteGet(
    const void *const SigStruct_p,
    SMS_MO_Route_t *const Route_p,
    SMS_Error_t *const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    *Route_p = sms_route;
    return SMS_REQUEST_OK;

}

SMS_RequestStatus_t Request_SMS_ShortMessageStatusSet(
    const SMS_RequestControlBlock_t *const RequestCtrl_p,
    const SMS_Slot_t *const Slot_p,
    const SMS_Status_t Status,
    SMS_Error_t *const Error_p)
{
    if (wb_testcase_cmgl == CURRENT_TEST_CASE) {
        sms_stub_cmgl_data_status_update(Slot_p->Position, Status);
    }

    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_SMS_ShortMessageStatusSet(
    const void *const SigStruct_p,
    SMS_Error_t *const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_OK;
}

SMS_RequestStatus_t Request_SMS_MemCapacityFullReport(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Request_SMS_MemCapacityGetState(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    int * const memFullState_p,
    SMS_Error_t * const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    EXECUTOR.client_tag_p = (void *)((uintptr_t)RequestCtrl_p->ClientTag);
    *memFullState_p = 1;
    return SMS_REQUEST_PENDING;
}

SMS_RequestStatus_t Response_SMS_MemCapacityGetState(
    const void *const SigStruct_p,
    int * const memFullState_p,
    SMS_Error_t * const Error_p)
{
    *Error_p = SMS_ERROR_INTERN_NO_ERROR;
    *memFullState_p = 1;
    return SMS_REQUEST_OK;
}


void *Util_SMS_SignalReceiveOnSocket(
    const int                    Socket,
    uint32_t        *const Primitive_p,
    SMS_ClientTag_t *const ClientTag_p)
{
    SMS_SigselectWithClientTag_t *signal_p = malloc(sizeof(SMS_SigselectWithClientTag_t));

    switch (CURRENT_TEST_CASE) {
    case wb_testcase_cbm:
        *Primitive_p = EVENT_CBS_CELLBROADCASTMESSAGERECEIVED;
        break;
    case wb_testcase_cmt:
        *Primitive_p = EVENT_SMS_SHORTMESSAGERECEIVED;
        break;
    case wb_testcase_cmti:
        *Primitive_p = EVENT_SMS_SHORTMESSAGERECEIVED;
        break;
    case wb_testcase_cds:
        *Primitive_p = EVENT_SMS_STATUSREPORTRECEIVED;
        break;
    case wb_testcase_cdsi:
        *Primitive_p = EVENT_SMS_STATUSREPORTRECEIVED;
        break;
    case wb_testcase_cmer_unsol_ciev_10:
        *Primitive_p = EVENT_SMS_STORAGESTATUSCHANGED;
        break;
    default:
        break;
    }

    *ClientTag_p = (int)((uintptr_t)EXECUTOR.client_tag_p);
    return signal_p;
}


void Util_SMS_SignalFree(void *Signal_p)
{
    free(Signal_p);
}
