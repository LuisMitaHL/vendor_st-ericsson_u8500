/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*----------------------------------------------------------------------------------*/
/* Name: mfl_cas_stub.c                                                             */
/* MFL CAS stub implementation file for SMS Library                                 */
/* version:         0.1                                                             */
/*----------------------------------------------------------------------------------*/

#include "mfl.h"
#include "r_rms.h"
#include "mfl_stub.h"
#include "r_dispatch.h"

#define LOG_TAG "MFL CAS Stub"

#define MFL_LOG_D(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] DBG:" format, ## __VA_ARGS__); fflush(stdout);})
#define MFL_LOG_E(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] ERR:" format, ## __VA_ARGS__); fflush(stdout);})
#define MFL_LOG_I(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] INF:" format, ## __VA_ARGS__); fflush(stdout);})
#define MFL_LOG_V(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] VBE:" format, ## __VA_ARGS__); fflush(stdout);})
#define MFL_LOG_W(format, ...)  ({(void)fprintf(stdout, "\n[" LOG_TAG "] WRG:" format, ## __VA_ARGS__); fflush(stdout);})

// Stubbed versions of Event Subscribe functions
RequestStatus_t Request_EventChannel_Subscribe(
    const RequestControlBlock_t * const RequestCtrl_p,
    const Event_SubscriptionInfo_t EventSubscriptionInfo,
    const EventId_t EventId)
{
    MFL_LOG_I("Called Request_EventChannel_Subscribe");

    if (RequestCtrl_p == NULL) {
        MFL_LOG_E("Request_EventChannel_Subscribe: RequestCtrl_p NULL Parameter");
        return REQUEST_FAILED;
    }

    if (RequestCtrl_p->modem_session_ptr == NULL) {
        MFL_LOG_E("Request_EventChannel_Subscribe: modem_session_ptr NULL Parameter");
        return REQUEST_FAILED;
    }

    if (RequestCtrl_p->WaitMode != NO_WAIT_MODE) {
        MFL_LOG_E("Request_EventChannel_Subscribe: WAIT MODE not supported");
        return REQUEST_FAILED_PARAMETER;
    }

    {
        Request_EventChannel_Subscribe_Stub_t *Req_p = SMS_SIGNAL_ALLOC(Request_EventChannel_Subscribe_Stub_t, REQUEST_EVENTCHANNEL_SUBSCRIBE);

        if (Req_p != NULL) {
            int fd;

            Req_p->SigselectWithClientTag.ClientTag = RequestCtrl_p->ClientTag;
            Req_p->Event_SubscriptionInfo = EventSubscriptionInfo;
            Req_p->Event_Id = EventId;

            mfl_session_get_file_descriptor(RequestCtrl_p->modem_session_ptr, &fd);

            SMS_SEND(&Req_p, fd);
            MFL_LOG_I("Called Request_EventChannel_Subscribe: Request Pending");
            return REQUEST_PENDING;
        } else {
            MFL_LOG_E("Request_EventChannel_Subscribe: SIGNAL ALLOC failed");
            return REQUEST_FAILED_RESOURCE;
        }
    }

    return REQUEST_FAILED;      // something has gone wrong that has not been trapped above.
}

RequestStatus_t Response_EventChannel_Subscribe(
    const void *const SigStruct_p)
{
    Response_EventChannel_Subscribe_Stub_t *Rec_p = (Response_EventChannel_Subscribe_Stub_t *) SigStruct_p;

    MFL_LOG_I("Response_EventChannel_Subscribe");

    if (Rec_p->SigselectWithClientTag.Primitive != RESPONSE_EVENTCHANNEL_SUBSCRIBE) {
        return REQUEST_FAILED_UNPACKING;
    } else {
        return Rec_p->Status;
    }
}


// Stubbed versions of Event unpack functions
EventStatus_t Event_RMS_V2_MessageReceivedInd(
    const void *const SigStruct_p,
    RMS_V2_Address_t * const SC_Address_p,
    RMS_V2_UserData_t * const UserData_p,
    boolean * const AcknowledgeRequired_p)
{
    MFL_LOG_I("Called Event_RMS_V2_MessageReceivedInd");

    if (SigStruct_p == NULL || SC_Address_p == NULL || UserData_p == NULL || AcknowledgeRequired_p == NULL) {
        MFL_LOG_E("Event_RMS_V2_MessageReceivedInd NULLs");
        return GS_EVENT_FAILED_UNPACKING;
    }

    {
        RMS_Event_V2_MessageReceivedInd_Stub_t *Event_p = (RMS_Event_V2_MessageReceivedInd_Stub_t *) SigStruct_p;

        if (Event_p->SigselectWithClientTag.Primitive != EVENT_RMS_V2_MESSAGERECEIVEDIND) {
            MFL_LOG_E("Event_RMS_V2_MessageReceivedInd Invalid Event");
            return GS_EVENT_FAILED_UNPACKING;
        }

        memcpy(SC_Address_p, &Event_p->SC_Address, sizeof(RMS_V2_Address_t));
        memcpy(UserData_p, &Event_p->UserData, sizeof(RMS_V2_UserData_t));
        *AcknowledgeRequired_p = Event_p->AcknowledgeRequired;
    }

    MFL_LOG_I("Event_RMS_V2_MessageReceivedInd Event Unpacked OK");
    return GS_EVENT_OK;
}


EventStatus_t Event_RMS_V2_FailureReceivedInd(
    const void *const SigStruct_p,
    RMS_V2_Status_t * const FailureReport_p)
{
    IDENTIFIER_NOT_USED(SigStruct_p);
    IDENTIFIER_NOT_USED(FailureReport_p);

    MFL_LOG_I("Called Event_RMS_V2_FailureReceivedInd");

    return 0;
}

// Stubbed versions of Request / Response functions.
RequestStatus_t R_Req_RMS_V3_Message_SendAcknowledgement(
    const RequestControlBlock_t * const RequestCtrl_p,
    const RMS_V2_UserData_t * const UserData_p,
    const RMS_V2_Status_t * const Status_p,
    RMS_V2_Status_t * const ResponseStatus_p)
{
    IDENTIFIER_NOT_USED(ResponseStatus_p);

    MFL_LOG_I("Called R_Req_RMS_V3_Message_SendAcknowledgement");
    if (RequestCtrl_p == NULL) {
        MFL_LOG_E("R_Req_RMS_V3_Message_SendAcknowledgement: RequestCtrl_p NULL Parameter");
        return REQUEST_FAILED;
    }

    if (RequestCtrl_p->modem_session_ptr == NULL) {
        MFL_LOG_E("R_Req_RMS_V3_Message_SendAcknowledgement: modem_session_ptr NULL Parameter");
        return REQUEST_FAILED;
    }

    if (RequestCtrl_p->WaitMode != NO_WAIT_MODE) {
        MFL_LOG_E("R_Req_RMS_V3_Message_SendAcknowledgement: WAIT MODE not supported");
        return REQUEST_FAILED_PARAMETER;
    }

    {
        RMS_Req_Message_SendAcknowledgement_Stub_t *Req_p = SMS_SIGNAL_ALLOC(RMS_Req_Message_SendAcknowledgement_Stub_t,
                                                                             R_REQ_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT);

        if (Req_p != NULL) {
            int fd;

            Req_p->SigselectWithClientTag.ClientTag = RequestCtrl_p->ClientTag;

            if (UserData_p != NULL) {
                MFL_LOG_I("R_Req_RMS_V3_Message_SendAcknowledgement: Copy UserData");
                memcpy(&Req_p->UserData, UserData_p, sizeof(RMS_V2_UserData_t));
            } else {
                MFL_LOG_I("R_Req_RMS_V3_Message_SendAcknowledgement: No UserData");
                memset(&Req_p->UserData, '\0', sizeof(RMS_V2_UserData_t));
            }

            if (Status_p != NULL) {
                MFL_LOG_I("R_Req_RMS_V3_Message_SendAcknowledgement: Copy Status");
                memcpy(&Req_p->Status, Status_p, sizeof(RMS_V2_Status_t));
            } else {
                MFL_LOG_I("R_Req_RMS_V3_Message_SendAcknowledgement: No Status");
                memset(&Req_p->Status, '\0', sizeof(RMS_V2_Status_t));
            }


            mfl_session_get_file_descriptor(RequestCtrl_p->modem_session_ptr, &fd);

            SMS_SEND(&Req_p, fd);
            MFL_LOG_I("Called R_Req_RMS_V3_Message_SendAcknowledgement: Request Pending");
            return REQUEST_PENDING;
        } else {
            MFL_LOG_E("R_Req_RMS_V3_Message_SendAcknowledgement: SIGNAL ALLOC failed");
            return REQUEST_FAILED_RESOURCE;
        }
    }

    return REQUEST_FAILED;      // something has gone wrong that has not been trapped above.
}

RequestStatus_t R_Resp_RMS_V3_Message_SendAcknowledgement(
    const void *const SigStruct_p,
    RMS_V2_Status_t * const ResponseStatus_p)
{
    MFL_LOG_I("Called R_Resp_RMS_V3_Message_SendAcknowledgement");

    if (SigStruct_p == NULL) {
        MFL_LOG_E("R_Resp_RMS_V3_Message_SendAcknowledgement: SigStruct_p NULL Parameter");
        return REQUEST_FAILED;
    }

    {
        RMS_Resp_Message_SendAcknowledgement_Stub_t *Resp_p = (RMS_Resp_Message_SendAcknowledgement_Stub_t *) SigStruct_p;

        if (ResponseStatus_p != NULL) {
            ResponseStatus_p->ErrorCode = Resp_p->RMS_ErrorCode;

            if (ResponseStatus_p->ErrorCode == RMS_V2_ERROR_CODE_INTERN_NO_ERROR || ResponseStatus_p->ErrorCode == RMS_V2_ERROR_CODE_INTERN_NO_ERROR_NO_ACK) {
                ResponseStatus_p->SuccessfulOutcome = TRUE;
            } else {
                ResponseStatus_p->SuccessfulOutcome = FALSE;
            }
        }
    }

    return REQUEST_OK;
}


RequestStatus_t R_Req_RMS_V2_Message_Send(
    const RequestControlBlock_t * const RequestCtrl_p,
    const RMS_V2_Address_t * const SC_Address_p,
    const RMS_V2_TransmissionParams_t * const TxParams_p,
    RMS_V2_UserData_t * const UserData_p,
    RMS_V2_Status_t * const Status_p)
{
    IDENTIFIER_NOT_USED(RequestCtrl_p);
    IDENTIFIER_NOT_USED(SC_Address_p);
    IDENTIFIER_NOT_USED(TxParams_p);
    IDENTIFIER_NOT_USED(UserData_p);
    IDENTIFIER_NOT_USED(Status_p);

    MFL_LOG_I("Called R_Req_RMS_V2_Message_Send");
    if (RequestCtrl_p == NULL) {
        MFL_LOG_E("R_Req_RMS_V2_Message_Send: RequestCtrl_p NULL Parameter");
        return REQUEST_FAILED;
    }

    if (RequestCtrl_p->modem_session_ptr == NULL) {
        MFL_LOG_E("R_Req_RMS_V2_Message_Send: modem_session_ptr NULL Parameter");
        return REQUEST_FAILED;
    }

    if (RequestCtrl_p->WaitMode != NO_WAIT_MODE) {
        MFL_LOG_E("R_Req_RMS_V2_Message_Send: WAIT MODE not supported");
        return REQUEST_FAILED_PARAMETER;
    }

    {
        RMS_Req_MessageSend_Stub_t *Req_p = SMS_SIGNAL_ALLOC(RMS_Req_MessageSend_Stub_t, R_REQ_RMS_V2_MESSAGE_SEND);

        if (Req_p != NULL) {
            int fd;

            Req_p->SigselectWithClientTag.ClientTag = RequestCtrl_p->ClientTag;

            if (UserData_p != NULL) {
                MFL_LOG_I("R_Req_RMS_V2_Message_Send: Copy UserData");
                memcpy(&Req_p->UserData, UserData_p, sizeof(RMS_V2_UserData_t));
            } else {
                MFL_LOG_I("R_Req_RMS_V2_Message_Send: No UserData");
                memset(&Req_p->UserData, '\0', sizeof(RMS_V2_UserData_t));
            }


            mfl_session_get_file_descriptor(RequestCtrl_p->modem_session_ptr, &fd);

            SMS_SEND(&Req_p, fd);
            MFL_LOG_I("Called R_Req_RMS_V2_Message_Send: Request Pending");
            return REQUEST_PENDING;
        } else {
            MFL_LOG_E("R_Req_RMS_V2_Message_Send: SIGNAL ALLOC failed");
            return REQUEST_FAILED_RESOURCE;
        }
    }

    return REQUEST_FAILED;      // something has gone wrong that has not been trapped above.
}

RequestStatus_t R_Resp_RMS_V2_Message_Send(
    const void *const SigStruct_p,
    RMS_V2_UserData_t * const UserData_p,
    RMS_V2_Status_t * const Status_p)
{
    IDENTIFIER_NOT_USED(SigStruct_p);
    IDENTIFIER_NOT_USED(UserData_p);
    IDENTIFIER_NOT_USED(Status_p);

    MFL_LOG_I("Called R_Resp_RMS_V2_Message_Send");

    if (SigStruct_p == NULL) {
        MFL_LOG_E("R_Resp_RMS_V2_Message_Send: SigStruct_p NULL Parameter");
        return REQUEST_FAILED;
    }

    {
        RMS_Resp_MessageSend_Stub_t *Resp_p = (RMS_Resp_MessageSend_Stub_t *) SigStruct_p;

        if (Status_p != NULL) {
            Status_p->ErrorCode = Resp_p->RMS_ErrorCode;

            if (Status_p->ErrorCode == RMS_V2_ERROR_CODE_INTERN_NO_ERROR || Status_p->ErrorCode == RMS_V2_ERROR_CODE_INTERN_NO_ERROR_NO_ACK) {
                Status_p->SuccessfulOutcome = TRUE;
            } else {
                Status_p->SuccessfulOutcome = FALSE;
            }
        }

        if (UserData_p != NULL) {
            MFL_LOG_I("R_Req_RMS_V2_Message_Send: Copy UserData");
            memcpy(UserData_p, &Resp_p->UserData, sizeof(RMS_V2_UserData_t));
        }
    }

    return REQUEST_OK;
}


RequestStatus_t R_Req_RMS_V2_Message_SendMemoryAvailable(
    const RequestControlBlock_t * const RequestCtrl_p,
    RMS_V2_UserData_t * const RMS_UserData_p,
    RMS_V2_Status_t * const Status_p)
{
    IDENTIFIER_NOT_USED(RequestCtrl_p);
    IDENTIFIER_NOT_USED(RMS_UserData_p);
    IDENTIFIER_NOT_USED(Status_p);

    MFL_LOG_I("Called R_Req_RMS_V2_Message_SendMemoryAvailable");

    return 0;
}

RequestStatus_t R_Resp_RMS_V2_Message_SendMemoryAvailable(
    const void *const SigStruct,
    RMS_V2_UserData_t * const RMS_UserData_p,
    RMS_V2_Status_t * const Status_p)
{
    IDENTIFIER_NOT_USED(SigStruct);
    IDENTIFIER_NOT_USED(RMS_UserData_p);
    IDENTIFIER_NOT_USED(Status_p);

    MFL_LOG_I("Called R_Resp_RMS_V2_Message_SendMemoryAvailable");

    return 0;
}
