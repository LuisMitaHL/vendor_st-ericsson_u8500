#ifndef SMMNGR_H
#define SMMNGR_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION:
 *
 *  Short Message manager include file.
 *
 *************************************************************************/

/* Requester Interface definitions */
//#include "r_sys.h"

#include "t_sms.h"
#include "sm.h"                 // Linux port of RMS types
#include "sim.h"


typedef enum {
    SHORTMESSAGEMANAGER_SHORT_MESSAGE_SENT,
    SHORTMESSAGEMANAGER_MEMCAPACITYAVAILABLE_SENT,
    SHORTMESSAGEMANAGER_DELIVERREPORT_CONFIRM,
    SHORTMESSAGEMANAGER_MORETOSEND_TIMEOUT,
    SHORTMESSAGEMANAGER_RECEIVE_NEW_STATUS_REPORT,
    SHORTMESSAGEMANAGER_PROCESS_RECEIVED_STATUS_REPORTS
} ShortMessageManagerEvent_t;

#define SMS_CAT_MO_SMS_CONTROL_TAG 0xD5
#define SMS_MCC_MNC_MAX_BCD_LENGTH 3

#define SMS_CN_REG_DATA_CLEAR   0x00
#define SMS_CN_REG_DATA_PRESENT 0x01
#define SMS_CN_REG_DATA_ERROR   0x02

typedef struct {
    uint8_t data_info;
    uint8_t use_extended_cid;
    uint32_t cid;
    uint16_t lac;
    uint8_t mcc_mnc[SMS_MCC_MNC_MAX_BCD_LENGTH];        // BCD format
} ShortMessageManager_CN_CellInfo_t;

typedef struct ShortMessageManagerObject {
    SMS_SwBP_SignalInfo_t MoreToSendSignalInfo; // Client process requesting send SM with MoreToSend set, used to block all other clients
    SMS_SwBP_SignalInfo_t MessageSenderSignalInfo;
    SMS_SwBP_SignalInfo_t DeliverReportSenderSignalInfo;
    uint8_t SM_ReferenceNumber;
    uint8_t LastReceived_SMS_PID;
    SMS_Position_t LastReceived_SMS_Position;
    MoreToSendState_t MoreToSendState;  // To determine if RLC was set by MoreToSend and the current state, otherwise by CMMS
    uint8_t ClientMemoryCapacityFull;
    ShortMessageManager_CN_CellInfo_t CN_CellInfo;
    ShortMessage_p MO_SMS_InProgress;
    uint8_t MoreToSend;
} ShortMessageManager_t;

typedef struct ShortMessageManagerObject *ShortMessageManager_p;

void ShortMessageManager_HandleEvent(
    const ShortMessageManagerEvent_t ShortMessageManagerEvent,
    EventData_p EventData);
void ShortMessageManager_HandleStatusReport(
    void *thisObject,
    EventData_p eventData);

SMS_Error_t ShortMessageManager_SMS_Read(
    const SMS_Slot_t * const,
    SMS_Status_t * const,
    SMS_SMSC_Address_TPDU_t * const);

SMS_Error_t ShortMessageManager_SMS_Write(
    const SMS_Status_t,
    const SMS_SMSC_Address_TPDU_t * const,
    SMS_Slot_t * const,
    SMS_Slot_t * const);

SMS_Error_t ShortMessageManager_TPDUDeliver(
    SMS_SMSC_Address_TPDU_t *);

SMS_Error_t ShortMessageManager_SMS_Delete(
    const SMS_Slot_t * const);
SMS_Error_t ShortMessageManager_SMS_DeletePart(
    const SMS_Slot_t * const InputSlot_p,
    uint8_t deleteFromStorage);

SMS_Error_t ShortMessageManager_StatusReportRead(
    const SMS_Slot_t * const,
    SMS_StatusReport_t * const);

SMS_Error_t ShortMessageManager_SMS_StatusSet(
    const SMS_Slot_t * const,
    const SMS_Status_t);

SMS_Error_t ShortMessageManager_SMS_Send(
    ShortMessageManager_p thisShortMessageManager,
    const SMS_SMSC_Address_TPDU_t * const MSG_SMS_TPDU_p,
    const SMS_Slot_t * const MSG_SMS_Slot_p,
    const uint8_t MSG_MoreToSend,
    const uint8_t satInitiated,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p);

SMS_Error_t ShortMessageManager_SMS_Find(
    SMS_Slot_t,
    SMS_Status_t,
    SMS_BrowseOption_t,
    SMS_Slot_t *);

SMS_Error_t ShortMessageManager_SMS_CapacityGet(
    SMS_Storage_t,
    SMS_SlotInformation_t *,
    SMS_StorageStatus_t *);

SMS_Error_t ShortMessageManager_StorageClear(
    const SMS_Storage_t MSG_SMS_StorageMedia_p,
    const uint8_t AllThisICCID_Messages,
    const uint8_t AllOtherICCID_Messages);

SMS_Error_t ShortMessageManager_SMS_InformationRead(
    const SMS_Slot_t * const,
    SMS_ReadInformation_t *);

void ShortMessageManager_SMS_MemCapacityFullSet(
    void);

void ShortMessageManager_SMS_PreferredStoreSet(
    SMS_Storage_t);

void ShortMessageManager_SMS_DeliverReportControlSet(
    SMS_NetworkAcknowledge_t NetworkAcknowledge);

void ShortMessageManager_SMS_DeliverReportControlGet(
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p);

void ShortMessageManager_SMS_MO_RouteSet(
    SMS_MO_Route_t Route);

SMS_MO_Route_t ShortMessageManager_SMS_MO_RouteGet(
    void);

void ShortMessageManager_CN_CellInfoSet(
    const ShortMessageManager_CN_CellInfo_t * const CellInfo_p);

uint8_t ShortMessageManager_CN_CellInfoAvailable(
    void);

void ShortMessageManager_SMS_ControlSendEnvelope(
    void);

void ShortMessageManager_SMS_ControlHandleResponse(
    ste_cat_ec_response_t * cat_ec_response_p);

void ShortMessageManager_SMS_Ack(
    ShortMessage_p theShortMessage,
    const SMS_Error_t RMS_V2_ErrorCode);

void ShortMessageManager_PollStorage(
    void);

uint8_t ShortMessageManager_StoreMatchedStatusReport(
    const ShortMessage_p statusReport,
    const ShortMessage_p shortMessageToMatch_p,
    SMS_Slot_t * const statusReportSlotNumber_p);

void ShortMessageManager_DispatchStatusReport(
    const ShortMessage_p statusReport_p,
    const SMS_Slot_t * const shortMessageSlotNumber_p);

SMS_Error_t ShortMessageManager_SendDeliverReport(
    ShortMessageManager_p thisShortMessageManager,
    const SMS_RP_ErrorCause_t RP_ErrorCause,
    const SMS_TPDU_t * const deliverReport_TPDU_p,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p);

SMS_Error_t ShortMessageManager_ShortMessageNack(
    ShortMessageManager_p thisShortMessageManager);

SMS_Error_t ShortMessageManager_Send_MemCapacityAvailable(
    ShortMessageManager_p thisShortMessageManager,
    EventData_p eventData,
    const SMS_SwBP_SignalInfo_t * const senderSignalInfo_p);

uint8_t ShortMessageManager_Send_MemCapacityGetState();

void ShortMessageManager_SIMRefresh(
    void);

SMS_Error_t ShortMessageManager_MasterReset(
    ShortMessageManager_p);

void ShortMessageManager_SMS_RelayLinkControl_Set(
    const SMS_RelayControl_t RelayLinkControl);
void ShortMessageManager_SMS_RelayLinkControl_Get(
    SMS_RelayControl_t * const RelayLinkControl_p);


SMS_Error_t ShortMessageManager_AddPortSubscription(
    const SMS_PROCESS SenderProcess,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p);

SMS_Error_t ShortMessageManager_DeletePortSubscription(
    const SMS_PROCESS SenderProcess,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p);

#endif                          // SMMNGR_H
