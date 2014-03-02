#ifndef _MFL_STUB_H_
#define _MFL_STUB_H_
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "r_rms.h"
#include "r_dispatch.h"

// MFL Socket Path
#define MFL_SMS_SOCK_PATH "/tmp/socket_mflsms"

// MFL Modem Thread
#define MFL_MODEM_THREAD 124



// Define stubbed Request Signal Definitions as these are not in r_rms.h or r_dispatch.h
#define R_REQ_RMS_V2_MESSAGE_SEND                (R_RESP_RMS_V2_MESSAGE_SEND                + 1234)     // Should be unique.
#define R_REQ_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT (R_RESP_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT + 1234)     // Should be unique.
#define R_REQ_RMS_V2_MESSAGE_ABORT               (R_RESP_RMS_V2_MESSAGE_ABORT               + 1234)     // Should be unique.
#define R_REQ_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE (R_RESP_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE + 1234)     // Should be unique.
#define REQUEST_EVENTCHANNEL_SUBSCRIBE           (RESPONSE_EVENTCHANNEL_SUBSCRIBE           + 1234)     // Should be unique.


// Signal data structures
typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    RMS_V2_UserData_t UserData;
} RMS_Req_MessageSend_Stub_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    RMS_V2_UserData_t UserData;
    RMS_V2_ErrorCode_t RMS_ErrorCode;
} RMS_Resp_MessageSend_Stub_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    RMS_V2_UserData_t UserData;
    RMS_V2_Status_t Status;
} RMS_Req_Message_SendAcknowledgement_Stub_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    RMS_V2_ErrorCode_t RMS_ErrorCode;
} RMS_Resp_Message_SendAcknowledgement_Stub_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    RMS_V2_Address_t SC_Address;
    RMS_V2_UserData_t UserData;
    boolean AcknowledgeRequired;
} RMS_Event_V2_MessageReceivedInd_Stub_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    Event_SubscriptionInfo_t Event_SubscriptionInfo;
    EventId_t Event_Id;
} Request_EventChannel_Subscribe_Stub_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    RequestStatus_t Status;
} Response_EventChannel_Subscribe_Stub_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} SMS_SignalHeaderStructure_t;



// Stub dispatch functions
void Dispatch_RMS_V2_MessageReceivedInd_Stub(
    const RMS_V2_Address_t * const SC_Address_p,
    const RMS_V2_UserData_t * const UserData_p,
    const boolean AcknowledgeRequired,
    int fd);


// MFL Modem stub
void mfl_main(
    void);






#endif                          //_MFL_STUB_H_
