
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
 * This is the implementation of the interface of the SMS Server
 *
 *************************************************************************/

#include "d_sms.h"
#include "r_sms.h"
#include "r_sms_cb_session.h"
#include "g_sms.h"
#include "r_smslinuxporting.h"


/*
**========================================================================
** Short Message Layer Requester/Response function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Request_SMS_CB_SessionCreate
 *
 * @description  This function is used to create a communication session
 *               with the SMS & CB Servers.
 *
 * @param        RequestResponseSocket_p : Socket to use in Request_SMS_x
 *                                         and SMS_RECEIVE_ON_SOCKET asynch response.
 * @param        EventSocket_p           : Socket to use for unsolicited
 *                                         events via SMS_RECEIVE_ON_SOCKET.
 * @param        Error_p                 : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_CB_SessionCreate(
    int *const RequestResponseSocket_p,
    int *const EventSocket_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestResponseSocket_p);
    SMS_IDENTIFIER_NOT_USED(EventSocket_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestControlBlock_t RequestCtrl;
    SMS_RequestControlBlock_t *RequestCtrl_p = &RequestCtrl;
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    unsigned int ClientId;
    SMS_CB_SessionCreate_Req_t *Send_p;

    if (RequestResponseSocket_p == NULL || EventSocket_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(SMS_CB_SessionCreate_Req_t, REQUEST_SMS_CB_SESSIONCREATE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Connect to SMS Server socket to use requesters, initialising unique ClientId
        if (!ipc_connect_request(SMS_SERVERTHREAD_, RequestResponseSocket_p, &ClientId, SMS_SERVER_SOCK_PATH)) {
            SMS_A_(SMS_LOG_E("r_sms.c: Request_SMS_CB_SessionCreate Failed to connect to request SMS_SERVER_SOCK_PATH"));
            SMS_SIGNAL_FREE(&Send_p);
            return SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
        } else {
            RequestCtrl_p->Socket = *RequestResponseSocket_p;
            SMS_A_(SMS_LOG_I("r_sms.c: Request_SMS_CB_SessionCreate RX socket = 0x%08x, ClientId = 0x%08x", RequestCtrl_p->Socket, ClientId));
        }

        // connect to server in order to receive events
        if (!ipc_connect_event(SMS_SERVERTHREAD_, EventSocket_p, ClientId, SMS_SERVER_SOCK_PATH)) {
            SMS_A_(SMS_LOG_E("r_sms.c: Request_SMS_CB_SessionCreate Failed to connect to event SMS_SERVER_SOCK_PATH"));
            SMS_SIGNAL_FREE(&Send_p);
            return SMS_ERROR_INTERN_UNSPECIFIED_ERROR;
        } else {
            SMS_A_(SMS_LOG_I("r_sms.c: Request_SMS_CB_SessionCreate Evnt socket = 0x%08x", *EventSocket_p));
        }

        {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_CB_SESSIONCREATE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            // Wait for response from the Messaging application
            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_CB_SessionCreate(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_CB_SessionCreate
 *
 * @description  Response Function
 *
 * @param        SigStruct_p     : void.
 * @param        Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_CB_SessionCreate(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_CB_SessionCreate_Resp_t *Rec_p = (SMS_CB_SessionCreate_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_CB_SessionDestroy
 *
 * @description  This function is used to destroy a communication session
 *               with the SMS & CB Servers.
 *
 * @param        RequestCtrl_p : as type of RequestControlBlock_t.
 * @param        EventSocket   : The EventSocket which the client obtained
 *                               through an earlier call to the corresponding
 *                               RRequest_SMS_CB_SessionCreate function.
 * @param        Error_p       : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_CB_SessionDestroy(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const int EventSocket,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_CB_SessionDestroy_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_CB_SessionDestroy_Req_t, REQUEST_SMS_CB_SESSIONDESTROY);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {

            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_CB_SESSIONDESTROY };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            // Wait for response from the Messaging application
            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_CB_SessionDestroy(Rec_p, RequestCtrl_p->Socket, EventSocket, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else {
            SMS_SIGNAL_FREE(&Send_p);
            RequestStatus = SMS_REQUEST_PENDING;
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_CB_SessionDestroy
 *
 * @description  Response Function
 *
 * @param        SigStruct_p   : void.
 * @param        RequestSocket : The RequestSocket which the client obtained
 *                               through an earlier call to the corresponding
 *                               Request_SMS_CB_SessionCreate function.
 * @param        EventSocket   : The EventSocket which the client obtained
 *                               through an earlier call to the corresponding
 *                               Request_SMS_CB_SessionCreate function.
 * @param        Error_p       : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_CB_SessionDestroy(
    const void *const SigStruct_p,
    const int RequestSocket,
    const int EventSocket,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    /*
     * NOTE: If the connection to the server is broken, then
     * Rec_p/SigStruct_p will have a NULL value. Handle this.
     */

    SMS_CB_SessionDestroy_Resp_t *Rec_p = (SMS_CB_SessionDestroy_Resp_t *) SigStruct_p;

    if (Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    if (Rec_p) {
        // Set up the error code. No error default is set up by the Requester.
        *Error_p = Rec_p->ErrorCode;
    }

    if (Rec_p == NULL || Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;

        // Close the client's sockets
        if (RequestSocket != SMS_NO_PROCESS) {
            SMS_A_(SMS_LOG_I("r_sms.c: Response_SMS_CB_SessionDestroy closing Request-Socket = %d", RequestSocket));
            close(RequestSocket);
        }

        if (EventSocket != SMS_NO_PROCESS) {
            SMS_A_(SMS_LOG_I("r_sms.c: Response_SMS_CB_SessionDestroy closing Event-Socket = %d", EventSocket));
            close(EventSocket);
        }
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}

/********************************************************************/
/**
 *
 * @function     Request_SMS_CB_Shutdown
 *
 * @description  This function is used to shutdown the sms_server.
 *
 * @param        RequestCtrl_p : as type of RequestControlBlock_t.
 * @param        EventSocket   : The EventSocket which the client obtained
 *                               through an earlier call to the corresponding
 *                               RRequest_SMS_CB_SessionCreate function.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_CB_Shutdown(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const int EventSocket)
{


    SMS_RequestStatus_t RequestStatus;
    SMS_CB_Shutdown_Req_t *Send_p;

    if (RequestCtrl_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_CB_Shutdown_Req_t, REQUEST_SMS_CB_SHUTDOWN);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;
    } else {
        SMS_SEND(&Send_p, RequestCtrl_p->Socket);
        RequestStatus = SMS_REQUEST_OK;
    }

    // Close the client's sockets
    if (RequestCtrl_p->Socket != SMS_NO_PROCESS) {
        SMS_A_(SMS_LOG_I("r_sms.c: Request_SMS_CB_Shutdown closing Request-Socket = %d", RequestCtrl_p->Socket));
        close(RequestCtrl_p->Socket);
    }

    if (EventSocket != SMS_NO_PROCESS) {
        SMS_A_(SMS_LOG_I("r_sms.c: Request_SMS_CB_Shutdown closing Event-Socket = %d", EventSocket));
        close(EventSocket);
    }

    return RequestStatus;
}

/********************************************************************/
/**
 *
 * @function     Request_SMS_StorageClear
 *
 * @description
 * This function is used to delete all messages in a certain storage.
 * Please be careful when using this, as it can delete messages used by
 * another client as well
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether
 *                                    the request is called in wait mode or no
 *                                    wait mode.
 * @param [in] Storage                Storage that should be emptied.
 * @param [in] AllThisICCID_Messages  Set to TRUE if you want to delete all
 *                                    messages in the storage related to
 *                                    the current ICCID.
 * @param [in] AllOtherICCID_Messages Set to TRUE if you want to delete all
 *                                    messages in the storage related to
 *                                    other ICCID's.
 * @param [out] Error_p               SMS Server internal
 *                                    error cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                    \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *
 * @return       RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_StorageClear(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Storage_t Storage,
    const uint8_t AllThisICCID_Messages,
    const uint8_t AllOtherICCID_Messages,
    SMS_Error_t * const Error_p)
{

#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Storage);
    SMS_IDENTIFIER_NOT_USED(AllThisICCID_Messages);
    SMS_IDENTIFIER_NOT_USED(AllOtherICCID_Messages);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_StorageClear_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_StorageClear_Req_t, REQUEST_SMS_STORAGECLEAR);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Set up the element of the requester structure
        Send_p->Storage = Storage;
        Send_p->AllThisICCID_Messages = AllThisICCID_Messages;
        Send_p->AllOtherICCID_Messages = AllOtherICCID_Messages;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_STORAGECLEAR };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_StorageClear(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_StorageClear
 *
 * @description  Response Function
 *
 * @param [in]  SigStruct_p           Signal Data
 * @param [out] Error_p               SMS Server internal
 *                                    error cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                    \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_StorageClear(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_StorageClear_Resp_t *Rec_p = (SMS_StorageClear_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ShortMessageDeliver
 *
 * @description
 * This is used to deliver an MT SM TPDU into the Messaging Transport
 * Server from an external user (apart from the normal Network Signalling).
 * An example can be SM messages being received with MMS as a bearer.
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait mode.
 * @param [in] SMSC_Address_TPDU_p  The SMSC address and SMS TPDU that should be delivered
 *                                  to the SMS Server for routing.
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ShortMessageDeliver(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Error_t * const Error_p)
{

#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(SMSC_Address_TPDU_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageDeliver_Req_t *Send_p;

    if (RequestCtrl_p == NULL || SMSC_Address_TPDU_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageDeliver_Req_t, REQUEST_SMS_SHORTMESSAGEDELIVER);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Set the incoming data for the requested read information
        memcpy(&Send_p->SMSC_Address_TPDU, SMSC_Address_TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t));

        // Adjust the SMSC Address length to add the length of the TypeOfAddress
        // byte to match the format used internally by the SMS CB Server and
        // stored on the USIM
        if (Send_p->SMSC_Address_TPDU.ServiceCenterAddress.Length > 0) {
            Send_p->SMSC_Address_TPDU.ServiceCenterAddress.Length++;
        }

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGEDELIVER };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageDeliver(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;


#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageDeliver
 *
 * @description  Response Function
 *
 * @param [in]  SigStruct_p         Signal Data
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageDeliver(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageDeliver_Resp_t *Rec_p = (SMS_ShortMessageDeliver_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_DeliverReportSend
 *
 * @description
 * This function is used to send a deliver report (ACK or NACK) in
 * response to the receipt of a Short Message.
 *
 * @param        RequestCtrl_p     : As type of RequestControlBlock_t.
 * @param        DeliverReportType : Type of deliver report to be sent to the network.
 * @param        TPDU_p            : Deliver report TPDU to be sent to the network.
 * @param        Error_p           : As type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_DeliverReportSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_RP_ErrorCause_t RP_ErrorCause,
    const SMS_TPDU_t * const TPDU_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(RP_ErrorCause);
    SMS_IDENTIFIER_NOT_USED(TPDU_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_DeliverReportSend_Req_t *Send_p;

    if (RequestCtrl_p == NULL || TPDU_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_DeliverReportSend_Req_t, REQUEST_SMS_DELIVERREPORTSEND);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Set the deliver report type
        Send_p->RP_ErrorCause = RP_ErrorCause;

        // Set the deliver report TPDU to be sent the network
        memcpy(&Send_p->TPDU, TPDU_p, sizeof(SMS_TPDU_t));

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_DELIVERREPORTSEND };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_DeliverReportSend(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/********************************************************************/
/**
 *
 * @function     Response_SMS_DeliverReportSend
 *
 * @description  Response Function
 *
 * @param        SigStruct_p     : void.
 * @param        Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_DeliverReportSend(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_DeliverReportSend_Resp_t *Rec_p = (SMS_DeliverReportSend_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/********************************************************************/
/**
 *
 * @function     Request_SMS_MemCapacityAvailableSend
 *
 * @description
 * This function is used to send Memory Capacity Available to the Network.
 *
 * @param        RequestCtrl_p   : As type of RequestControlBlock_t.
 * @param        Error_p         : As type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_MemCapacityAvailableSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_MemCapacityAvailableSend_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_MemCapacityAvailableSend_Req_t, REQUEST_SMS_MEMCAPACITYAVAILABLESEND);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_MEMCAPACITYAVAILABLESEND };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_MemCapacityAvailableSend(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}

/********************************************************************/
/**
 *
 * @function     Response_SMS_MemCapacityAvailableSend
 *
 * @description  Response Function
 *
 * @param        SigStruct_p     : void.
 * @param        Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_MemCapacityAvailableSend(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_MemCapacityAvailableSend_Resp_t *Rec_p = (SMS_MemCapacityAvailableSend_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR
                || Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR_NO_ACTION) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}

/********************************************************************/
/**
 *
 * @function     Request_SMS_MemCapacityGetState
 *
 * @description
 * This function is used to get the Memory Capacity Full parameter message
 * from the SMS server.
 *
 * @param        RequestCtrl_p   : As type of RequestControlBlock_t.
 * @param        memFullState_p  : Tells if memory is full or not.
 * @param        Error_p         : As type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_MemCapacityGetState(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    int * const memFullState_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(memFullState_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_MemCapacityGetState_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_MemCapacityGetState_Req_t, REQUEST_SMS_MEMCAPACITYGETSTATE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_MEMCAPACITYGETSTATE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_MemCapacityGetState(Rec_p, memFullState_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}

/********************************************************************/
/**
 *
 * @function     Response_SMS_MemCapacityGetState
 *
 * @description  Response Function
 *
 * @param        SigStruct_p     : void.
 * @param        memFullState_p  : Tells if memory is full or not.
 * @param        Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_MemCapacityGetState(
    const void *const SigStruct_p,
    int* memFullState_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(memFullState_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_MemCapacityGetState_Resp_t *Rec_p = (SMS_MemCapacityGetState_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    //Set memFullState parameter
    *memFullState_p = Rec_p->memFullState;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR
                || Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR_NO_ACTION) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/*
**========================================================================
** Short Message Storage Layer Requester/Response function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Request_SMS_ShortMessageRead
 *
 * @description  This function reads the short message from a given
 *               storage slot.
 *
 * @param        *RequestCtrl_p        : as type of RequestControlBlock_t.
 * @param        *Slot_p               : as type of SMS_Slot_t.
 * @param        *Status_p             : as type of SMS_Status_t.
 * @param        *SMSC_Address_TPDU_p  : as type of SMS_SMSC_Address_TPDU_t.
 * @param        *Error_p              : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ShortMessageRead(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_Status_t * const Status_p,
    SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(Status_p);
    SMS_IDENTIFIER_NOT_USED(SMSC_Address_TPDU_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageRead_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageRead_Req_t, REQUEST_SMS_SHORTMESSAGEREAD);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        memcpy(&Send_p->Slot, Slot_p, sizeof(SMS_Slot_t));

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGEREAD };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageRead(Rec_p, Status_p, SMSC_Address_TPDU_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageRead
 *
 * @description  Response Function
 *
 * @param        *SigStruct_p           : void.
 * @param        *Status_p              : as type of SMS_Status_t
 * @param        *SMSC_Address_TPDU_p   : as type of SMS_SMSC_Address_TPDU_t.
 * @param        *Error_p               : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageRead(
    const void *const SigStruct_p,
    SMS_Status_t * const Status_p,
    SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Status_p);
    SMS_IDENTIFIER_NOT_USED(SMSC_Address_TPDU_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageRead_Resp_t *Rec_p = (SMS_ShortMessageRead_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Status_p == NULL || SMSC_Address_TPDU_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        memcpy(SMSC_Address_TPDU_p, &Rec_p->SMSC_Address_TPDU, sizeof(SMS_SMSC_Address_TPDU_t));

        // Adjust the SMSC Address length to subtract the length of the TypeOfAddress
        // byte to abstract from the format used internally by the SMS CB Server and
        // stored on the USIM
        if (SMSC_Address_TPDU_p->ServiceCenterAddress.Length > 0) {
            SMSC_Address_TPDU_p->ServiceCenterAddress.Length--;
        }

        *Status_p = Rec_p->Status;

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ShortMessageWrite
 *
 * @description  This function writes a short message to the given
 *               storage slot.
 *
 * @param        *RequestCtrl_p        : as type of RequestControlBlock_t.
 * @param        Status                : as type of SMS_Status_t.
 * @param        *SMSC_Address_TPDU_p  : as type of SMS_SMSC_Address_TPDU_t.
 * @param        *Slot_p               : as type of SMS_Slot_t.
 * @param        *Error_p              : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ShortMessageWrite(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Status_t Status,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Status);
    SMS_IDENTIFIER_NOT_USED(SMSC_Address_TPDU_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageWrite_Req_t *Send_p;

    if (RequestCtrl_p == NULL || SMSC_Address_TPDU_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageWrite_Req_t, REQUEST_SMS_SHORTMESSAGEWRITE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        memcpy(&Send_p->Slot, Slot_p, sizeof(SMS_Slot_t));
        memcpy(&Send_p->SMSC_Address_TPDU, SMSC_Address_TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t));

        // Adjust the SMSC Address length to add the length of the TypeOfAddress
        // byte to match the format used internally by the SMS CB Server and
        // stored on the USIM
        if (Send_p->SMSC_Address_TPDU.ServiceCenterAddress.Length > 0) {
            Send_p->SMSC_Address_TPDU.ServiceCenterAddress.Length++;
        }

        Send_p->Status = Status;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGEWRITE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageWrite(Rec_p, Slot_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageWrite
 *
 * @description  Response Function
 *
 * @param        *SigStruct_p     : void.
 * @param        *Slot_p          : as type of SMS_Slot_t.
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageWrite(
    const void *const SigStruct_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageWrite_Resp_t *Rec_p = (SMS_ShortMessageWrite_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        memcpy(Slot_p, &Rec_p->Slot, sizeof(SMS_Slot_t));

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ShortMessageDelete
 *
 * @description  This function deletes the short message in the given
 *               storage slot.
 *
 * @param        *RequestCtrl_p : as type of RequestControlBlock_t.
 * @param        *Slot_p        : as type of SMS_Slot_t.
 * @param        *Error_p       : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ShortMessageDelete(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageDelete_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageDelete_Req_t, REQUEST_SMS_SHORTMESSAGEDELETE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        memcpy(&Send_p->Slot, Slot_p, sizeof(SMS_Slot_t));

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGEDELETE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageDelete(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageDelete
 *
 * @description  Response Function
 *
 * @param        *SigStruct_p     : void.
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageDelete(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageDelete_Resp_t *Rec_p = (SMS_ShortMessageDelete_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_StatusReportRead
 *
 * @description  This function reads the status report associated
 *               with the given short message storage slot.
 *
 * @param        *RequestCtrl_p   : as type of RequestControlBlock_t.
 * @param        *Slot_p          : as type of SMS_Slot_t.
 * @param        *StatusReport_p  : as type of SMS_StatusReport_t.
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_StatusReportRead(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_StatusReport_t * const StatusReport_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(StatusReport_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_StatusReportRead_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_StatusReportRead_Req_t, REQUEST_SMS_STATUSREPORTREAD);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        memcpy(&Send_p->Slot, Slot_p, sizeof(SMS_Slot_t));

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_STATUSREPORTREAD };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_StatusReportRead(Rec_p, StatusReport_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_StatusReportRead
 *
 * @description  Response Function
 *
 * @param        *SigStruct_p     : void.
 * @param        *StatusReport_p  : as type of SMS_StatusReport_t
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_StatusReportRead(
    const void *const SigStruct_p,
    SMS_StatusReport_t * const StatusReport_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(StatusReport_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_StatusReportRead_Resp_t *Rec_p = (SMS_StatusReportRead_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || StatusReport_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        memcpy(StatusReport_p, &Rec_p->TPDU_StatusReport, sizeof(SMS_StatusReport_t));

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ShortMessageStatusSet
 *
 * @description  This function sets the status of the short message in
 *               a given slot. i.e. read,to be read or sent,etc.
 *
 * @param        *RequestCtrl_p   : as type of RequestControlBlock_t.
 * @param        *Slot_p          : as type of SMS_Slot_t.
 * @param        Status           : as type of SMS_Status_t.
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ShortMessageStatusSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    const SMS_Status_t Status,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(Status);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageStatusSet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageStatusSet_Req_t, REQUEST_SMS_SHORTMESSAGESTATUSSET);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        memcpy(&Send_p->Slot, Slot_p, sizeof(SMS_Slot_t));
        Send_p->Status = Status;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGESTATUSSET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageStatusSet(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageStatusSet
 *
 * @description  Response Function
 *
 * @param        *SigStruct_p     : void.
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageStatusSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageStatusSet_Resp_t *Rec_p = (SMS_ShortMessageStatusSet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ShortMessageFind
 *
 * @description  This function is used for searching for messages fitting
 *               a certain search criteria.
 *
 * @param        *RequestCtrl_p    :     as type of RequestControlBlock_t.
 * @param        *SearchInfo_p     :     as type of SMS_Search_Info_t.
 * @param        *Slot_p           :     as type of SMS_Slot_t.
 * @param        *Error_p          :     as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ShortMessageFind(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SearchInfo_t * const SearchInfo_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(SearchInfo_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageFind_Req_t *Send_p;

    if (RequestCtrl_p == NULL || SearchInfo_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageFind_Req_t, REQUEST_SMS_SHORTMESSAGEFIND);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        memcpy(&Send_p->SearchInfo, SearchInfo_p, sizeof(SMS_SearchInfo_t));

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGEFIND };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageFind(Rec_p, Slot_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageFind
 *
 * @description  Response Function
 *
 * @param        *SigStruct_p     : void.
 * @param        *Slot_p          : as type of SMS_Slot_t.
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageFind(
    const void *const SigStruct_p,
    SMS_Slot_t * const Slot_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageFind_Resp_t *Rec_p = (SMS_ShortMessageFind_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR || Rec_p->ErrorCode == SMS_ERROR_INTERN_MESSAGE_NOT_FOUND) {
        // Copy slot information for SMS_ERROR_INTERN_MESSAGE_NOT_FOUND case to ensure backwards compatibility
        // for applications which used slot information to determine that the find was unsuccessful.
        memcpy(Slot_p, &Rec_p->Slot, sizeof(SMS_Slot_t));

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_StorageCapacityGet
 *
 * @description  This function is used for analysing the status of
 * the short message store
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait
 *                                    mode.
 * @param [in] Storage                The type of storage medium.
 * @param [out] SlotInformation_p     The slot status
 * @param [out] StorageStatus_p       The status of the SMS Server storage
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_StorageCapacityGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Storage_t Storage,
    SMS_SlotInformation_t * const SlotInformation_p,
    SMS_StorageStatus_t * const StorageStatus_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Storage);
    SMS_IDENTIFIER_NOT_USED(SlotInformation_p);
    SMS_IDENTIFIER_NOT_USED(StorageStatus_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_StorageCapacityGet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_StorageCapacityGet_Req_t, REQUEST_SMS_STORAGECAPACITYGET);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->Storage = Storage;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_STORAGECAPACITYGET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_StorageCapacityGet(Rec_p, SlotInformation_p, StorageStatus_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_StorageCapacityGet
 *
 * @description  Response Function
 *
 * @param [in]  SigStruct_p           Signal Data
 * @param [out] SlotInformation_p     The slot status
 * @param [out] StorageStatus_p       The status of the SMS Server storage
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_StorageCapacityGet(
    const void *const SigStruct_p,
    SMS_SlotInformation_t * const SlotInformation_p,
    SMS_StorageStatus_t * const StorageStatus_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(SlotInformation_p);
    SMS_IDENTIFIER_NOT_USED(StorageStatus_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_StorageCapacityGet_Resp_t *Rec_p = (SMS_StorageCapacityGet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || SlotInformation_p == NULL || StorageStatus_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        memcpy(SlotInformation_p, &Rec_p->SlotInfo, sizeof(SMS_SlotInformation_t));
        memcpy(StorageStatus_p, &Rec_p->StorageStatus, sizeof(SMS_StorageStatus_t));

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/*
 * Internal helper function to send a short message.
 *
 * Contains the bulk of the implementation of Request_SMS_ShortMessageSend
 * and Request_SMS_SatShortMessageSend
 */
static SMS_RequestStatus_t Do_Request_SMS_ShortMessageSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    const SMS_Slot_t * const Slot_p,
    const uint8_t MoreToSend,
    uint8_t * const SM_Reference_p,
    SMS_Error_t * const Error_p,
    const uint8_t SatInitiated)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(SMSC_Address_TPDU_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(MoreToSend);
    SMS_IDENTIFIER_NOT_USED(SM_Reference_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageSend_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageSend_Req_t, REQUEST_SMS_SHORTMESSAGESEND);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        if (SMSC_Address_TPDU_p != NULL) {
            // Attempting to send a valid TPDU so set invalid storage slot
            Send_p->Slot.Storage = SMS_STORAGE_MT;
            Send_p->Slot.Position = SMS_STORAGE_POSITION_INVALID;

            memcpy(&Send_p->SMSC_Address_TPDU, SMSC_Address_TPDU_p, sizeof(SMS_SMSC_Address_TPDU_t));

        } else {
            // TPDU is null so set invalid TPDU signal data
            memset(&Send_p->SMSC_Address_TPDU, 0, sizeof(SMS_SMSC_Address_TPDU_t));

            if (Slot_p != NULL) {
                // Attempting to send from a valid storage slot
                memcpy(&Send_p->Slot, Slot_p, sizeof(SMS_Slot_t));
            } else {
                // Storage slot is null so set invalid storage slot signal data
                Send_p->Slot.Storage = SMS_STORAGE_MT;
                Send_p->Slot.Position = SMS_STORAGE_POSITION_INVALID;
            }
        }

        Send_p->MoreToSend = MoreToSend;
        Send_p->SatInitiated = SatInitiated;

        if (RequestCtrl_p->Socket == SMS_NO_PROCESS) {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

            SMS_SIGNAL_FREE(&Send_p);
        } else if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGESEND };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            // Wait for response from the Messaging application
            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageSend(Rec_p, SM_Reference_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}

/*
 * Documented in r_sms.h
 */
SMS_RequestStatus_t Request_SMS_ShortMessageSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    const SMS_Slot_t * const Slot_p,
    const uint8_t MoreToSend,
    uint8_t * const SM_Reference_p,
    SMS_Error_t * const Error_p)
{
    return Do_Request_SMS_ShortMessageSend(RequestCtrl_p, SMSC_Address_TPDU_p, Slot_p, MoreToSend, SM_Reference_p, Error_p, FALSE);
}

/*
 * Documented in r_sms.h
 */
SMS_RequestStatus_t Request_SMS_SatShortMessageSend(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_SMSC_Address_TPDU_t * const SMSC_Address_TPDU_p,
    const SMS_Slot_t * const Slot_p,
    const uint8_t MoreToSend,
    uint8_t * const SM_Reference_p,
    SMS_Error_t * const Error_p)
{
    return Do_Request_SMS_ShortMessageSend(RequestCtrl_p, SMSC_Address_TPDU_p, Slot_p, MoreToSend, SM_Reference_p, Error_p, TRUE);
}



/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageSend
 *
 * @description  Response Function
 *
 * @param        *SigStruct_p     : void.
 * @param        *SM_Reference    : the TP-MR assigned to the sent Short Message.
 * @param        *Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageSend(
    const void *const SigStruct_p,
    uint8_t * const SM_Reference_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(SM_Reference_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageSend_Resp_t *Rec_p = (SMS_ShortMessageSend_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || SM_Reference_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        *SM_Reference_p = Rec_p->SM_Reference;

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_EventsSubscribe
 *
 * @description  This function is used to subscribe to receive unsolicited
 *               MT SMS and Status Report received events from the SMS Server.
 *
 * @param        *RequestCtrl_p         : as type of RequestControlBlock_t.
 * @param        Error_p                : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_EventsSubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_Events_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_Events_Req_t, REQUEST_SMS_EVENTSSUBSCRIBE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->Socket == SMS_NO_PROCESS) {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_UNSPECIFIED_ERROR;

            SMS_SIGNAL_FREE(&Send_p);
        } else if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_EVENTSSUBSCRIBE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            // Wait for response from the Messaging application
            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_EventsSubscribe(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_EventsSubscribe
 *
 * @description  Response Function
 *
 * @param        SigStruct_p     : void.
 * @param        Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_EventsSubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_Events_Resp_t *Rec_p = (SMS_Events_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_EventsUnsubscribe
 *
 * @description  This function is used to unsubscribe to receive unsolicited
 *               MT SMS and Status Report received events from the SMS Server.
 *
 * @param        RequestCtrl_p : as type of RequestControlBlock_t.
 * @param        EventSocket   : The EventSocket which the client obtained
 *                               through an earlier call to the corresponding
 *                               Request_SMS_EventsSubscribe function.
 * @param        Error_p       : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_EventsUnsubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_Events_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_Events_Req_t, REQUEST_SMS_EVENTSUNSUBSCRIBE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {

            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_EVENTSUNSUBSCRIBE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            // Wait for response from the Messaging application
            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_EventsUnsubscribe(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else {
            SMS_SIGNAL_FREE(&Send_p);
            RequestStatus = SMS_REQUEST_PENDING;
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_EventsUnsubscribe
 *
 * @description  Response Function
 *
 * @param        SigStruct_p   : void.
 * @param        Error_p       : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_EventsUnsubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_Events_Resp_t *Rec_p = (SMS_Events_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ApplicationPortSubscribe
 *
 * @description  This function is used to subscribe to received SMS that contain a
 *               particular application port. If the given application port or range has
 *               already been subscribed to then SMS_ERROR_INTERN_APP_PORT_IN_USE will be
 *               returned. It is valid for a particular port to be subscribed to by
 *               two different clients if one has subscribed to the port as a destination
 *               and the other as an origination port.
 *
 * @param        RequestCtrl_p              : as type of RequestControlBlock_t.
 * @param        ApplicationPortRange_p     : as type of SMS_ApplicationPortRange_t.
 * @param        Error_p                    : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ApplicationPortSubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(ApplicationPortRange_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ApplicationPort_Req_t *Send_p;

    if (RequestCtrl_p == NULL || ApplicationPortRange_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ApplicationPort_Req_t, REQUEST_SMS_APPLICATIONPORTSUBSCRIBE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->ApplicationPortRange = *ApplicationPortRange_p;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_APPLICATIONPORTSUBSCRIBE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ApplicationPortSubscribe(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ApplicationPortSubscribe
 *
 * @description  Response Function
 *
 * @param        SigStruct_p     : void.
 * @param        Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ApplicationPortSubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ApplicationPort_Resp_t *Rec_p = (SMS_ApplicationPort_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ApplicationPortUnsubscribe
 *
 * @description  This function is used to unsubscribe to received SMS that contain a
 *               particular application port. If the given application port range is
 *               not mathed against the current subscriptions an error will be returned.
 *
 * @param        RequestCtrl_p              : as type of RequestControlBlock_t.
 * @param        ApplicationPortRange_p     : as type of SMS_ApplicationPortRange_t.
 * @param        Error_p                    : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ApplicationPortUnsubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(ApplicationPortRange_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ApplicationPort_Req_t *Send_p;

    if (RequestCtrl_p == NULL || ApplicationPortRange_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ApplicationPort_Req_t, REQUEST_SMS_APPLICATIONPORTUNSUBSCRIBE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->ApplicationPortRange = *ApplicationPortRange_p;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_APPLICATIONPORTUNSUBSCRIBE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ApplicationPortUnsubscribe(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ApplicationPortUnsubscribe
 *
 * @description  Response Function
 *
 * @param        SigStruct_p     : void.
 * @param        Error_p         : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ApplicationPortUnsubscribe(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ApplicationPort_Resp_t *Rec_p = (SMS_ApplicationPort_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Request_SMS_ShortMessageInfoRead
 *
 * @description  This function is used to read the information of a Message. The information
 *               about the message that can be determined without having to parse the TPDUs
 *               of that message is returned.
 *
 * @param        RequestCtrl_p     : as type of RequestControlBlock_t.
 * @param        Slot_p            : as type of SMS_Slot_t.
 * @param        ReadInformation_p : as type of SMS_ReadInformation_t.
 * @param        Error_p           : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_ShortMessageInfoRead(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Slot_t * const Slot_p,
    SMS_ReadInformation_t * const ReadInformation_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(ReadInformation_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_ShortMessageInfoRead_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Slot_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_ShortMessageInfoRead_Req_t, REQUEST_SMS_SHORTMESSAGEINFOREAD);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->Slot = *Slot_p;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_SHORTMESSAGEINFOREAD };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_ShortMessageInfoRead(Rec_p, ReadInformation_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     Response_SMS_ShortMessageInfoRead
 *
 * @description  Response Function
 *
 * @param        SigStruct_p       : void.
 * @param        ReadInformation_p : as type of SMS_ReadInformation_t.
 * @param        Error_p           : as type of SMS_Error_t.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Response_SMS_ShortMessageInfoRead(
    const void *SigStruct_p,
    SMS_ReadInformation_t * const ReadInformation_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(ReadInformation_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_ShortMessageInfoRead_Resp_t *Rec_p = (SMS_ShortMessageInfoRead_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || ReadInformation_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        memcpy(ReadInformation_p, &Rec_p->ReadInformation, sizeof(SMS_ReadInformation_t));

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}

//========================================================================

/**
 * This function is used to report that the client SMS Memory Capacity is full.
* When the platform is configured to send Deliver Reports the next MT SMS received will be
* negatively acknowledged and the platform memory capacity exceeded flag will be set if
* appropriate.
* This function should be called at start-up if client SMS Memory Capacity is full since the
* setting is not persistently stored by the platform.
* This function has no effect if the platform is not configured to send Deliver Reports.
* Request_SMS_MemCapacityAvailableSend() should be used to report when client
* SMS Memory Capacity becomes available.
 *
 * @param [in] RequestCtrl_p    Pointer to struct controlling whether the
 *                              request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p      Response signal to be unpacked.
 * @param [out] Error_p         Error cause returned by the server, indicating
 *                              if Memory Capacity Full was set.
 *                              \li \c SMS_ERROR_INTERN_NO_ERROR
 *                              \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                              \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
*
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @signalid RESPONSE_SMS_MEMCAPACITYFULLREPORT
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_MemCapacityFullReport(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_MemCapacityFullReport_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_MemCapacityFullReport_Req_t, REQUEST_SMS_MEMCAPACITYFULLREPORT);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_SMS_MEMCAPACITYFULLREPORT };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_SMS_MemCapacityFullReport(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;
#endif
}

/**
 * Response function for Request_SMS_MemCapacityFullReport
 *
 */
SMS_RequestStatus_t Response_SMS_MemCapacityFullReport(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    SMS_MemCapacityFullReport_Resp_t *Rec_p = (SMS_MemCapacityFullReport_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;
#endif
}

/*
**========================================================================
** Restricted Short Message Storage Layer Requester/Response function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     R_Req_SMS_PreferredStorageSet
 *
 * @description  This function is used for setting the prefered storage
 * for short message store. This affects the SMS routing rules
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] StoragePreferred     The preferred SMS Server storage
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t R_Req_SMS_PreferredStorageSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Storage_t StoragePreferred,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(StoragePreferred);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_PreferredStorageSet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_PreferredStorageSet_Req_t, R_REQ_SMS_PREFERREDSTORAGESET);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->PreferredStorage = StoragePreferred;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, R_RESP_SMS_PREFERREDSTORAGESET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = R_Resp_SMS_PreferredStorageSet(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     R_Resp_SMS_PreferredStorageSet
 *
 * @description  Response Function
 *
 * @param [in]  SigStruct_p         Signal Data
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t R_Resp_SMS_PreferredStorageSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_PreferredStorageSet_Resp_t *Rec_p = (SMS_PreferredStorageSet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}

/**
 * This function is used for defining the responsibility for acknowledgement
 * of MT SMS
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] NetworkAcknowledge   Define whether the network or client will
 *                                  send the deliver report in acknowledgement
 *                                  of the MT SMS.
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_DeliverReportControlSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_NetworkAcknowledge_t NetworkAcknowledge,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(NetworkAcknowledge);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus;
    SMS_DeliverReportControlSet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_DeliverReportControlSet_Req_t, R_REQ_SMS_DELIVERREPORTCONTROLSET);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->NetworkAcknowledge = NetworkAcknowledge;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, R_RESP_SMS_DELIVERREPORTCONTROLSET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = R_Resp_SMS_DeliverReportControlSet(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }
    return RequestStatus;
#endif
}

/**
 * Response function for R_Req_SMS_DeliverReportControlSet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_DeliverReportControlSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_DeliverReportControlSet_Resp_t *Rec_p = (SMS_DeliverReportControlSet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}

/**
 * This function is used for obtaining the current responsibility for
 * acknowledgement of MT SMS
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait
 *                                    mode.
 * @param [out] NetworkAcknowledge_p  Whether the network or client will
 *                                    send the deliver report in acknowledgement
 *                                    of MT SMS.
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                    \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_DeliverReportControlGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(NetworkAcknowledge_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus;
    SMS_DeliverReportControlGet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_DeliverReportControlGet_Req_t, R_REQ_SMS_DELIVERREPORTCONTROLGET);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, R_RESP_SMS_DELIVERREPORTCONTROLGET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = R_Resp_SMS_DeliverReportControlGet(Rec_p, NetworkAcknowledge_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }
    return RequestStatus;
#endif
}

/**
 * Response function for R_Req_SMS_DeliverReportControlGet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_DeliverReportControlGet(
    const void *const SigStruct_p,
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(NetworkAcknowledge_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_DeliverReportControlGet_Resp_t *Rec_p = (SMS_DeliverReportControlGet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL || NetworkAcknowledge_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the acknowledge setting
    *NetworkAcknowledge_p = Rec_p->NetworkAcknowledge;

    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}

/**
 * This function is used for setting the MO SMS route
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] Route                The MO SMS route
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_MO_RouteSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_MO_Route_t Route,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE
    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Route);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;
#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    SMS_MO_RouteSet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL || !(Route == SMS_MO_ROUTE_PS || Route == SMS_MO_ROUTE_CS || Route == SMS_MO_ROUTE_CS_PS)) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_MO_RouteSet_Req_t, R_REQ_SMS_MO_ROUTESET);
    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->Route = Route;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, R_RESP_SMS_MO_ROUTESET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = R_Resp_SMS_MO_RouteSet(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else {                // Invalid RequestCtrl_p->WaitMode
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }
    return RequestStatus;
#endif
}


/**
 * Response function for R_Req_SMS_MO_RouteSet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_MO_RouteSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE
    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;
#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    SMS_MO_RouteSet_Resp_t *Rec_p = (SMS_MO_RouteSet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;
#endif
}

/**
 * This function is used for retrieving the SMS routing
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [out] Route_p             The SMS Server routing
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
 * @retval SMS_REQUEST_PENDING            Used to indicate that the requester was
 *                                        called in NO_WAIT mode
 * @retval SMS_REQUEST_FAILED_APPLICATION Request failed by SMS Server,
 *                                        which could not perform what was
 *                                        asked for. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_PARAMETER   Request failed due to some wrong
 *                                        parameters. Look in Error_p for
 *                                        details of error
 * @retval SMS_REQUEST_FAILED_RESOURCE    Memory allocation failure
 * @retval SMS_REQUEST_NOT_AVAILABLE      Software back plane service not
 *                                        available
 *
 * @waitmode Wait mode & No wait mode
 * @clienttag Yes
 * @related No Related Events.
 * @reserved Platform Accessory Services, or Application Accessory Services
 */
SMS_RequestStatus_t R_Req_SMS_MO_RouteGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_MO_Route_t * const Route_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE
    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Route_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;
#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    SMS_MO_RouteGet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Route_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_MO_RouteGet_Req_t, R_REQ_SMS_MO_ROUTEGET);
    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, R_RESP_SMS_MO_ROUTEGET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = R_Resp_SMS_MO_RouteGet(Rec_p, Route_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else {                // Invalid RequestCtrl_p->WaitMode
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }
    return RequestStatus;
#endif
}


/**
 * Response function for R_Req_SMS_MO_RouteGet
 *
 */
SMS_RequestStatus_t R_Resp_SMS_MO_RouteGet(
    const void *const SigStruct_p,
    SMS_MO_Route_t * const Route_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE
    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Route_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;
#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    SMS_MO_RouteGet_Resp_t *Rec_p = (SMS_MO_RouteGet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Route_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    *Route_p = Rec_p->Route;    // Set up the route.
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }
    return RequestStatus;
#endif
}


/*
**========================================================================
** Restricted Requester/Response function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     R_Req_SMS_RelayLinkControlSet
 *
 * @description  This function is used to the control of the continuity
 *               of the SMS relay protocol link.
 *
 * @param [in] RequestCtrl_p        Pointer to struct controlling whether the
 *                                  request is called in wait mode or no wait
 *                                  mode.
 * @param [in] RelayLinkControl     The type that defines the control of the Relay Protocol link.
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *                                  \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                  \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                  \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t R_Req_SMS_RelayLinkControlSet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_RelayControl_t RelayLinkControl,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(RelayLinkControl);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;

    SMS_RelayLinkControlSet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_RelayLinkControlSet_Req_t, R_REQ_SMS_RELAYLINKCONTROLSET);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->RelayLinkControl = RelayLinkControl;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, R_RESP_SMS_RELAYLINKCONTROLSET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = R_Resp_SMS_RelayLinkControlSet(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     R_Resp_SMS_RelayLinkControlSet
 *
 * @description  Response Function
 *
 * @param [in]  SigStruct_p         Signal Data
 * @param [out] Error_p             SMS Server internal error
 *                                  cause returned by the server.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t R_Resp_SMS_RelayLinkControlSet(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_RelayLinkControlSet_Resp_t *Rec_p = (SMS_RelayLinkControlSet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/********************************************************************/
/**
 *
 * @function     R_Req_SMS_RelayLinkControlGet
 *
 * @description  This function is used to retrieve the SMS Relay control
 *               setting used to control the SMS relay protocol link.
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the
 *                                    request is called in wait mode or no wait
 *                                    mode.
 * @param [out] RelayLinkControl_p    The type that defines the control of the Relay Protocol link.
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *                                    \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                    \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                    \li \c v_ERROR_INTERN_INVALID_WAIT_MODE
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t R_Req_SMS_RelayLinkControlGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    SMS_RelayControl_t * const RelayLinkControl_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(RelayLinkControl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus;
    SMS_RelayLinkControlGet_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    RequestStatus = SMS_REQUEST_FAILED;

    Send_p = SMS_SIGNAL_ALLOC(SMS_RelayLinkControlGet_Req_t, R_REQ_SMS_RELAYLINKCONTROLGET);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            static const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, R_RESP_SMS_RELAYLINKCONTROLGET };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = R_Resp_SMS_RelayLinkControlGet(Rec_p, RelayLinkControl_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = SMS_ERROR_INTERN_NO_ERROR;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = SMS_ERROR_INTERN_INVALID_WAIT_MODE;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}

/********************************************************************/
/**
 *
 * @function     R_Resp_SMS_RelayLinkControlGet
 *
 * @description  Response Function
 *
 * @param [in]  SigStruct_p           Signal Data
 * @param [out] RelayLinkControl_p    The type that defines the control of the Relay Protocol link.
 * @param [out] Error_p               SMS Server internal error
 *                                    cause returned by the server.
 *
 * @return       SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t R_Resp_SMS_RelayLinkControlGet(
    const void *const SigStruct_p,
    SMS_RelayControl_t * const RelayLinkControl_p,
    SMS_Error_t * const Error_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(RelayLinkControl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    SMS_RelayLinkControlGet_Resp_t *Rec_p = (SMS_RelayLinkControlGet_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || RelayLinkControl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->ErrorCode;

    if (Rec_p->ErrorCode == SMS_ERROR_INTERN_NO_ERROR) {
        RequestStatus = SMS_REQUEST_OK;
        *RelayLinkControl_p = Rec_p->RelayLinkControl;
    } else if (Rec_p->ErrorCode == SMS_ERROR_INTERN_SERVER_BUSY) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE

}


/*
**========================================================================
** Short Message Layer Unpack function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Event_SMS_ServerStatusChanged
 *
 * @description  This function is used to unpack the status signal.
 *
 * @param [in] SigStruct_p             The signal being unpacked.
 * @param [out] ServerStatus_p         The actual data about the storage status
 *
 * @return       EventStatus_t
 */
/********************************************************************/
SMS_EventStatus_t Event_SMS_ServerStatusChanged(
    const void *const SigStruct_p,
    SMS_ServerStatus_t * const ServerStatus_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(ServerStatus_p);

    return SMS_EVENT_NOT_AVAILABLE;

#else

    Event_SMS_ServerStatusChanged_t *Event_p = (Event_SMS_ServerStatusChanged_t *) SigStruct_p;

    if (SigStruct_p == NULL || ServerStatus_p == NULL) {
        return SMS_EVENT_FAILED_PARAMETER;
    }

    if (Event_p->SigselectWithClientTag.Primitive != EVENT_SMS_SERVERSTATUSCHANGED) {
        return SMS_EVENT_FAILED_UNPACKING;
    } else {
        *ServerStatus_p = Event_p->ServerStatus;

        return SMS_EVENT_OK;
    }

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/********************************************************************/
/**
 *
 * @function     Event_SMS_ErrorOccurred
 *
 * @description  This function is used to unpack an unsolicited error
 * event to a subscriber.
 *
 * @param [in] SigStruct_p      Event signal to be unpacked.
 * @param [out] Error_p         The error information
 *
 * @return       EventStatus_t
 */
/********************************************************************/
SMS_EventStatus_t Event_SMS_ErrorOccurred(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_EVENT_NOT_AVAILABLE;

#else

    Event_SMS_ErrorOccurred_t *Event_p = (Event_SMS_ErrorOccurred_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_EVENT_FAILED_PARAMETER;
    }

    if (Event_p->SigselectWithClientTag.Primitive != EVENT_SMS_ERROROCCURRED) {
        return SMS_EVENT_FAILED_UNPACKING;
    } else {
        *Error_p = Event_p->ErrorCode;

        return SMS_EVENT_OK;
    }

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/********************************************************************/
/**
 *
 * @function     Event_SMS_ShortMessageReceived
 *
 * @description  The function unpacks the information about a short message.
 *
 * @param [in] SigStruct_p            Event signal to be unpacked.
 * @param [out] ShortMessageData_p    The information associated with the
 *                                    SMS. This contains the storage slot
 *                                    where the short message has been stored.
 *
 * @return       EventStatus_t
 */
/********************************************************************/
SMS_EventStatus_t Event_SMS_ShortMessageReceived(
    const void *const SigStruct_p,
    SMS_ShortMessageReceived_t * const ShortMessageData_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(ShortMessageData_p);

    return SMS_EVENT_NOT_AVAILABLE;

#else

    Event_SMS_ShortMessageReceived_t *Event_p = (Event_SMS_ShortMessageReceived_t *) SigStruct_p;

    if (SigStruct_p == NULL || ShortMessageData_p == NULL) {
        return SMS_EVENT_FAILED_PARAMETER;
    }

    if (Event_p->SigselectWithClientTag.Primitive != EVENT_SMS_SHORTMESSAGERECEIVED) {
        return SMS_EVENT_FAILED_UNPACKING;
    } else {
        ShortMessageData_p->Category = Event_p->Data.Category;
        ShortMessageData_p->IsReplaceType = Event_p->Data.IsReplaceType;
        ShortMessageData_p->Slot = Event_p->Data.Slot;

        memcpy(&ShortMessageData_p->SMSC_Address_TPDU, &Event_p->Data.SMSC_Address_TPDU, sizeof(SMS_SMSC_Address_TPDU_t));

        // Adjust the SMSC Address length to subtract the length of the TypeOfAddress
        // byte to abstract from the format used internally by the SMS CB Server and
        // stored on the USIM
        if (ShortMessageData_p->SMSC_Address_TPDU.ServiceCenterAddress.Length > 0) {
            ShortMessageData_p->SMSC_Address_TPDU.ServiceCenterAddress.Length--;
        }

        return SMS_EVENT_OK;
    }

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/********************************************************************/
/**
 *
 * @function     Event_SMS_ApplicationPortMatched
 *
 * @description  The function unpacks the information about an
 *               application port received short message.
 *
 * @param [in] SigStruct_p                  Event signal to be unpacked.
 * @param [out] ApplicationPortRange_p      The matched application port range.
 * @param [out] ShortMessageReceived_p      The information associated with the
 *                                          SMS. This contains the storage slot
 *                                          where the short message has been stored.
 *
 * @return       EventStatus_t
 */
/********************************************************************/
SMS_EventStatus_t Event_SMS_ApplicationPortMatched(
    const void *const SigStruct_p,
    SMS_ApplicationPortRange_t * const ApplicationPortRange_p,
    SMS_ShortMessageReceived_t * const ShortMessageReceived_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(ApplicationPortRange_p);
    SMS_IDENTIFIER_NOT_USED(ShortMessageReceived_p);

    return SMS_EVENT_NOT_AVAILABLE;

#else

    Event_SMS_ApplicationPortMatched_t *Event_p = (Event_SMS_ApplicationPortMatched_t *) SigStruct_p;

    if (SigStruct_p == NULL || ApplicationPortRange_p == NULL || ShortMessageReceived_p == NULL) {
        return SMS_EVENT_FAILED_PARAMETER;
    }

    if (Event_p->SigselectWithClientTag.Primitive != EVENT_SMS_APPLICATIONPORTMATCHED) {
        return SMS_EVENT_FAILED_UNPACKING;
    } else {
        ApplicationPortRange_p->ApplicationPort = Event_p->ApplicationPortRange.ApplicationPort;
        ApplicationPortRange_p->LowerPort = Event_p->ApplicationPortRange.LowerPort;
        ApplicationPortRange_p->UpperPort = Event_p->ApplicationPortRange.UpperPort;

        ShortMessageReceived_p->Category = Event_p->Data.Category;
        ShortMessageReceived_p->IsReplaceType = Event_p->Data.IsReplaceType;
        ShortMessageReceived_p->Slot = Event_p->Data.Slot;

        memcpy(&ShortMessageReceived_p->SMSC_Address_TPDU, &Event_p->Data.SMSC_Address_TPDU, sizeof(SMS_SMSC_Address_TPDU_t));

        // Adjust the SMSC Address length to subtract the length of the TypeOfAddress
        // byte to abstract from the format used internally by the SMS CB Server and
        // stored on the USIM
        if (ShortMessageReceived_p->SMSC_Address_TPDU.ServiceCenterAddress.Length > 0) {
            ShortMessageReceived_p->SMSC_Address_TPDU.ServiceCenterAddress.Length--;
        }

        return SMS_EVENT_OK;
    }

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/********************************************************************/
/**
 *
 * @function     Event_SMS_StatusReportReceived
 *
 * @description  The function unpacks the short message status report.
 *
 * @param [in] SigStruct_p               Event signal to be unpacked.
 * @param [out] Slot_p                   The slot number of the SMS that the status report is associated with.
 * @param [out] StatusReport_p           The SMSC address and status report in 164 byte TPDU format.
 *
 * @return       EventStatus_t
 */
/********************************************************************/
SMS_EventStatus_t Event_SMS_StatusReportReceived(
    const void *const SigStruct_p,
    SMS_Slot_t * const Slot_p,
    SMS_SMSC_Address_TPDU_t * const StatusReport_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Slot_p);
    SMS_IDENTIFIER_NOT_USED(StatusReport_p);

    return SMS_EVENT_NOT_AVAILABLE;

#else

    Event_SMS_StatusReportReceived_t *Event_p = (Event_SMS_StatusReportReceived_t *) SigStruct_p;

    if (SigStruct_p == NULL || StatusReport_p == NULL) {
        return SMS_EVENT_FAILED_PARAMETER;
    }

    if (Event_p->SigselectWithClientTag.Primitive != EVENT_SMS_STATUSREPORTRECEIVED) {
        return SMS_EVENT_FAILED_UNPACKING;
    } else {
        if (NULL != Slot_p) {
            *Slot_p = Event_p->SlotNumber;
        }
        memcpy(StatusReport_p, &Event_p->TPDU_StatusReport, sizeof(SMS_SMSC_Address_TPDU_t));

        // Adjust the SMSC Address length to subtract the length of the TypeOfAddress
        // byte to abstract from the format used internally by the SMS CB Server and
        // stored on the USIM
        if (StatusReport_p->ServiceCenterAddress.Length > 0) {
            StatusReport_p->ServiceCenterAddress.Length--;
        }

        return SMS_EVENT_OK;
    }

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/*
**========================================================================
** Short Message Storage Layer Unpack function proto-types
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Event_SMS_StorageStatusChanged
 *
 * @description  This function is used to unpack an event to a subscriber
 * indicating the state of the store
 *
 * @param [in] SigStruct_p          Event signal to be unpacked.
 * @param [out] StorageStatus_p     The actual data about the storage status
 *
 * @return       EventStatus_t
 */
/********************************************************************/
SMS_EventStatus_t Event_SMS_StorageStatusChanged(
    const void *const SigStruct_p,
    SMS_StorageStatus_t * const StorageStatus_p)
{

#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(StorageStatus_p);

    return SMS_EVENT_NOT_AVAILABLE;

#else

    Event_SMS_StorageStatusChanged_t *Event_p = (Event_SMS_StorageStatusChanged_t *) SigStruct_p;

    if (SigStruct_p == NULL || StorageStatus_p == NULL) {
        return SMS_EVENT_FAILED_PARAMETER;
    }

    if (Event_p->SigselectWithClientTag.Primitive != EVENT_SMS_STORAGESTATUSCHANGED) {
        return SMS_EVENT_FAILED_UNPACKING;
    } else {
        *StorageStatus_p = Event_p->StorageStatus;

        return SMS_EVENT_OK;
    }

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}

/*
 *
 *
 *  OSE --> IPC porting "utility" functions, implemented in r_sms.c
 *
 *
 */
// Should not be here. See comment in r_sms.h.
// Use Util_SMS_SignalReceiveOnSocket() instead.
union SMS_SIGNAL *ose_to_ipc_sig_receive(
    const SMS_SIGSELECT * pSigNoToWaitFor,
    int socket)
{
    union SMS_SIGNAL *pSignal = ipc_receive_signal_over_socket(socket);

    //If the caller specified a signal number to wait for...
    if (NULL != pSignal && pSigNoToWaitFor && pSigNoToWaitFor[0] == 1) {
        //Validate the signal number, and return error if no match.
        if (pSignal->Primitive == pSigNoToWaitFor[1])
            return pSignal;
        else {
            SMS_A_(SMS_LOG_E("ipc_sig_receive, error: received signal_no %u, but expected signal_no %u",
                             (unsigned int) (pSignal->Primitive), (unsigned int) pSigNoToWaitFor[1]));
            SMS_SIGNAL_FREE(&pSignal);
            return SMS_NIL;
        }
    }
    return pSignal;
}



/*
**========================================================================
** Utility functions for receiving asynchronous data on a socket.
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     Util_SMS_SignalReceiveOnSocket
 *
 * @description  Receives a signal on the supplied socket.
 *
 * @param [in] Socket           Socket to receive data on.  Will hang on this socket until / unless data is available.
 * @param [out] Primitive_p     Returns the Signal Primitive associated with the data returned.
 * @param [out] ClientTag_p     Returns the ClientTag associated with the data returned.
 *
 * @return       void* Pointer to signal data buffer. (Must be freed by calling Util_SMS_SignalFree().)
 */
/********************************************************************/
void *Util_SMS_SignalReceiveOnSocket(
    const int Socket,
    uint32_t * const Primitive_p,
    SMS_ClientTag_t * const ClientTag_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(Socket);
    SMS_IDENTIFIER_NOT_USED(Primitive_p);
    SMS_IDENTIFIER_NOT_USED(ClientTag_p);

    return NULL;

#else

    void *Signal_p = SMS_RECEIVE_ON_SOCKET(NULL, Socket);       // NULL because we are receiving any signal primitive on Socket.

    if (Signal_p != NULL) {
        if (Primitive_p != NULL) {
            // Extract Signal Primitive value
            *Primitive_p = ((union SMS_SIGNAL *) Signal_p)->Primitive;
        }

        if (ClientTag_p != NULL) {
            // Extract ClientTag
            (void) Do_SMS_ClientTag_Get(Signal_p, ClientTag_p);
        }
    }

    return Signal_p;

#endif
}

/********************************************************************/
/**
 *
 * @function     Util_SMS_SignalFree
 *
 * @description  Frees a signal which has been received using Util_SMS_SignalReceiveOnSocket().
 *
 * @param [in] Signal_p     Signal to be freed.
 *
 * @return       void
 */
/********************************************************************/
void Util_SMS_SignalFree(
    void *Signal_p)
{
#ifdef SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(Signal_p);
#else

    SMS_SIGNAL_FREE(&Signal_p);

#endif
}





/*
 * Test harness debug functionality.
 * Not enabled in standard build / delivery.
 */
#ifdef SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED
/********************************************************************/
/**
 *
 * @function     Request_SMS_TestUtil_MemoryStatusGet
 *
 * @description
 * This is a test harness function which is used when additional memory
 * checking is enabled. Calling this function sends a signal to the
 * SMS Server which causes it to dump the current memory allocation
 * status to the log.
 *
 * This function is needed because the test harness cannot access the
 * SMS Server data structures directly.
 *
 * There is no response signal.
 *
 * @param [in] RequestCtrl_p    Contains information to send data to
 *                              the SMS Server.
 *
 * @return     SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_TestUtil_MemoryStatusGet(
    const SMS_RequestControlBlock_t * const RequestCtrl_p)
{
    SMS_RequestStatus_t RequestStatus;
    SMS_TestUtil_MemoryStatusGet_Req_t *Send_p;

    if (RequestCtrl_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(SMS_TestUtil_MemoryStatusGet_Req_t, REQUEST_SMS_TEST_UTIL_MEMORY_STATUS_GET);

    if (Send_p == SMS_NIL) {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        SMS_SEND(&Send_p, RequestCtrl_p->Socket);

        RequestStatus = SMS_REQUEST_OK;
    }

    return RequestStatus;
}
#endif                          // SMS_TEST_HARNESS_MEMORY_CHECK_ENABLED


#ifdef SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED
/********************************************************************/
/**
 *
 * @function     Request_SMS_Test_CAT_Cause_PC_RefreshInd
 *
 * @description
 * This is a test harness function which is used to simulate a CAT
 * Refresh. It causes a signal to be sent to the SMS Process which is
 * then handled as if it had come from the UICC CAT Server callback.
 *
 * This function simulates the CAT_CAUSE_PC_REFRESH_IND.
 *
 * This function is needed because it is not possible to stub out the
 * UICC CAT server which is needed for other test cases.
 *
 * There is no response signal.
 *
 * @param [in] RequestCtrl_p    Contains information to send data to
 *                              the SMS Server.
 * @param [in] RefreshIndData_p CAT Refresh data to send to SMS Server.
 *
 * @return     SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_Test_CAT_Cause_PC_RefreshInd(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Test_copy_ste_cat_pc_refresh_ind_t * const RefreshIndData_p)
{
    SMS_RequestStatus_t RequestStatus;
    SMS_Test_CAT_Cause_PC_Refresh_Ind_t *Send_p;

    if (RequestCtrl_p == NULL || RefreshIndData_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(SMS_Test_CAT_Cause_PC_Refresh_Ind_t, REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_IND);

    if (Send_p == SMS_NIL) {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Copy refresh ind data structure.
        Send_p->refresh_ind_data = *RefreshIndData_p;

        SMS_SEND(&Send_p, RequestCtrl_p->Socket);

        RequestStatus = SMS_REQUEST_OK;
    }

    return RequestStatus;
}


/********************************************************************/
/**
 *
 * @function     Request_SMS_Test_CAT_Cause_PC_RefreshFileInd
 *
 * @description
 * This is a test harness function which is used to simulate a CAT
 * Refresh. It causes a signal to be sent to the SMS Process which is
 * then handled as if it had come from the UICC CAT Server callback.
 *
 * This function simulates the CAT_CAUSE_PC_REFRESH_FILE_IND.
 *
 * This function is needed because it is not possible to stub out the
 * UICC CAT server which is needed for other test cases.
 *
 * There is no response signal.
 *
 * @param [in] RequestCtrl_p    Contains information to send data to
 *                              the SMS Server.
 * @param [in] RefreshFileIndData_p CAT Refresh data to send to SMS Server.
 *
 * @return     SMS_RequestStatus_t
 */
/********************************************************************/
SMS_RequestStatus_t Request_SMS_Test_CAT_Cause_PC_RefreshFileInd(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const SMS_Test_copy_ste_cat_pc_refresh_file_ind_t * const RefreshFileIndData_p)
{
    SMS_RequestStatus_t RequestStatus;
    SMS_Test_CAT_Cause_PC_Refresh_File_Ind_t *Send_p;

    if (RequestCtrl_p == NULL || RefreshFileIndData_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(SMS_Test_CAT_Cause_PC_Refresh_File_Ind_t, REQUEST_SMS_TEST_CAT_CAUSE_PC_REFRESH_FILE_IND);

    if (Send_p == SMS_NIL) {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Copy refresh file ind data structure.
        Send_p->refresh_file_ind_data = *RefreshFileIndData_p;

        SMS_SEND(&Send_p, RequestCtrl_p->Socket);

        RequestStatus = SMS_REQUEST_OK;
    }

    return RequestStatus;
}

#endif                          // SMS_TEST_HARNESS_CAT_REFRESH_SIMULATION_ENABLED
