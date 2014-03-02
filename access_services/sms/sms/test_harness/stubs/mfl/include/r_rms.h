#ifndef INCLUSION_GUARD_R_RMS_H
#define INCLUSION_GUARD_R_RMS_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "r_sys.h"
#include "t_rms.h"

/*
**====================================================================
**  Signal id
**====================================================================
*/











// Use offsets 0-999 for RESPONSE/R_RESP signals
SIGID_DEFINITION_V3(CAT_ID_RMS, R_RESP_RMS_V2_MESSAGE_SEND, 0, RMS_V2_Response_Cnf_t)
    SIGID_DEFINITION_V3(CAT_ID_RMS, R_RESP_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT, 2, RMS_V2_Response_Cnf_t)
    SIGID_DEFINITION_V3(CAT_ID_RMS, R_RESP_RMS_V2_MESSAGE_ABORT, 3, RMS_V2_Abort_Cnf_t)
    SIGID_DEFINITION_V3(CAT_ID_RMS, R_RESP_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE, 4, RMS_V2_Response_Cnf_t)
// Use offsets 2000-2999 for EVENT signals
    SIGID_DEFINITION_V3(CAT_ID_RMS, EVENT_RMS_V2_MESSAGERECEIVEDIND, 2000, Event_RMS_V2_MessageReceivedInd_t)
    SIGID_DEFINITION_V3(CAT_ID_RMS, EVENT_RMS_V2_FAILURERECEIVEDIND, 2001, Event_RMS_V2_FailureReceivedInd_t)
// The EVENT_ID_RMS_V2_* event IDs are not defined in the tag being built. These #defines are temporary to solve
// a short-term build problem.
#define EVENT_ID_RMS_V2_MESSAGERECEIVEDIND EVENT_RMS_V2_MESSAGERECEIVEDIND
#define EVENT_ID_RMS_V2_FAILURERECEIVEDIND EVENT_RMS_V2_FAILURERECEIVEDIND
/*
**====================================================================
**  Function declarations
**====================================================================
*/
/*************************************************************************
*
* Function Prototypes for the Restricted Messaging (RMS)Interface 
*
* Note: V2 indicates that this is the 3G (Version 2) of the RMS interface
*
**************************************************************************/
// Send a message to the network
/**
 * The Messaging Server requests the Short Message Relay layer to convey the 
 * short message to the network. The user data containing the short message  
 * (packed in TPDU) is included in the UserData_p. The transmission parameters 
 * indicate whether the required transfer is part of a concatenated message.
 *
 * The Short Message Relay layer conveys the network acknowledgement (positive 
 * or negative) in the response function. SMR will include the potential 
 * failure cause in case of negative response from the network and optional 
 * user data (if supplied).
 *
 * NOTE: The function should be always called in NO WAIT mode, please see 
 * User's Guide (section 10).
 *
 * @param [in] RequestCtrl_p Pointer to struct controlling whether the request 
 *                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p   Response signal to be unpacked.
 * @param [in] SC_Address_p  Service Center Address
 * @param [in] TxParams_p    Transmission parameters valid for the transfer.
 * @param [in,out] UserData_p User Data (short message) to be sent to the 
 *                           network, and when called in wait mode or the response
 *                           function is called it holds the data received from the
 *                           network
 * @param [out] Status_p     Status of the requested functionality - includes 
 *                           response from the network with the failure cause; 
 *                           In case there was a failure to establish a 
 *                           connection for this SMS transfer, includes 
 *                           appropriate error code.
 *
 * @retval REQUEST_OK                 
 * @retval REQUEST_PENDING            
 * @retval REQUEST_FAILED_APPLICATION 
 *
 * @signalid R_RESP_RMS_V2_MESSAGE_SEND
 * @waitmode Wait mode and No wait mode.
 * @clienttag Yes
 * @related None
 * @reserved Messaging Transport Server
 */
RequestStatus_t R_Req_RMS_V2_Message_Send(
    const RequestControlBlock_t * const RequestCtrl_p,
    const RMS_V2_Address_t * const SC_Address_p,
    const RMS_V2_TransmissionParams_t * const TxParams_p,
    RMS_V2_UserData_t * const UserData_p,
    RMS_V2_Status_t * const Status_p);



/**
 * Response function for R_Req_RMS_V2_Message_Send
 *
 */

RequestStatus_t R_Resp_RMS_V2_Message_Send(
    const void *const SigStruct_p,
    RMS_V2_UserData_t * const UserData_p,
    RMS_V2_Status_t * const Status_p);


/**
 * This requester function is used in Mobile Terminated SMS transfer.
 *
 * The Messaging Server requests the Short Message Relay (SMR) layer to convey 
 * the acknowledgement for the previously received short message back to the 
 * network and wait for the outcome of this acknowledgement, as opposed to the
 * R_Req_RMS_V2_Message_SendAcknowledgement() function which will not wait for 
 * the outcome.
 * The Messaging server supplies the information on the general 
 * outcome (success/failure), failure cause if relevant, and optionally data 
 * associated with the response.
 *
 * @param [in] RequestCtrl_p     Pointer to struct controlling whether the request 
 *                               is called in wait mode or no wait mode.
 * @param [in] SigStruct_p       Response signal to be unpacked.
 * @param [in] Status_p          Acknowledgement from upper layers to be sent to 
 *                               the network - includes the failure cause in case 
 *                               of failure
 * @param [in] UserData_p        Includes (optional) data associated with the 
 *                               acknowledgement from upper layers
 * @param [out] ResponseStatus_p  Will contain the outcome of the request upon the
 *                               function return. Note the data will be invalid
 *                               in R_Req_RMS_V3_Message_SendAcknowledgement if it 
 *                               is called in No wait mode.
 *
 * @retval REQUEST_OK                 
 * @retval REQUEST_PENDING            
 * @retval REQUEST_FAILED_APPLICATION 
 *
 * @signalid R_RESP_RMS_V3_MESSAGE_SENDACKNOWLEDGEMENT
 * @waitmode Wait mode and No wait mode.
 * @clienttag Yes
 * @related None
 * @reserved Messaging Transport Server
 */

RequestStatus_t R_Req_RMS_V3_Message_SendAcknowledgement(
    const RequestControlBlock_t * const RequestCtrl_p,
    const RMS_V2_UserData_t * const UserData_p,
    const RMS_V2_Status_t * const Status_p,
    RMS_V2_Status_t * const ResponseStatus_p);




/**
 * Response function for R_Req_RMS_V3_Message_SendAcknowledgement
 *
 */

RequestStatus_t R_Resp_RMS_V3_Message_SendAcknowledgement(
    const void *const SigStruct_p,
    RMS_V2_Status_t * const ResponseStatus_p);



// Abort the sending of a message

/**
 * This requester function is used to abort the SMS transfer in process.
 *
 * The Messaging Server requests the Short Message Relay (SMR) layer to abort 
 * the Mobile Originated SMS transfer that has been started.
 *
 * NOTE: There is no guarantee that this request will result in aborting the 
 * current SMS transfer to the destination address. There is also no way for 
 * the SMR layer to inform the Messaging Server whether the abort was 
 * successful or not.
 *
 * @param [in] RequestCtrl_p Pointer to struct controlling whether the request 
 *                           is called in wait mode or no wait mode.
 * @param [in] SigStruct_p   Response signal to be unpacked.
 *
 * @retval REQUEST_OK      
 * @retval REQUEST_PENDING 
 *
 * @signalid R_RESP_RMS_V2_MESSAGE_ABORT
 * @waitmode Wait mode and No wait mode.
 * @clienttag Yes
 * @related None
 * @reserved Messaging Server
 */

RequestStatus_t R_Req_RMS_V2_Message_Abort(
    const RequestControlBlock_t * const RequestCtrl_p);



/**
 * Response function for R_Req_RMS_V2_Message_Abort
 *
 */

RequestStatus_t R_Resp_RMS_V2_Message_Abort(
    const void *const SigStruct_p);


// Send Memory Available notification to the network

/**
 * The Messaging Server requests the Short Message Relay (SMR) layer to convey 
 * the notification to the network that the MS has enough memory available to 
 * store short messages.
 *
 * @param [in] RequestCtrl_p   Pointer to struct controlling whether the request 
 *                             is called in wait mode or no wait mode.
 * @param [in] SigStruct_p     Response signal to be unpacked.
 * @param [out] Status_p       Acknowledgement from the network - includes the 
 *                             failure cause in case of failure
 * @param [out] RMS_UserData_p Includes (optional) data associated with the 
 *                             network response
 *
 * @retval REQUEST_OK                 
 * @retval REQUEST_PENDING            
 * @retval REQUEST_FAILED_APPLICATION 
 *
 * @signalid R_RESP_RMS_V2_MESSAGE_SENDMEMORYAVAILABLE
 * @waitmode Wait mode and No wait mode.
 * @clienttag Yes
 * @related None
 * @reserved Messaging Transport Server
 */

RequestStatus_t R_Req_RMS_V2_Message_SendMemoryAvailable(
    const RequestControlBlock_t * const RequestCtrl_p,
    RMS_V2_UserData_t * const RMS_UserData_p,
    RMS_V2_Status_t * const Status_p);



/**
 * Response function for R_Req_RMS_V2_Message_SendMemoryAvailable
 *
 */

RequestStatus_t R_Resp_RMS_V2_Message_SendMemoryAvailable(
    const void *const SigStruct,
    RMS_V2_UserData_t * const RMS_UserData_p,
    RMS_V2_Status_t * const Status_p);



/*************************************************************************
*
* Unpack Functions prototypes
*
**************************************************************************/

/**
 * This function is used to unpack data when a Mobile Terminated SMS is 
 * dispatched to the Messaging Transport Server, by the Short Message Relay 
 * (SMR) layer.
 *
 * NOTE: When Multi-Media Messaging (MMS) entity is part of the platform 
 * product, the function is used by MMS, indicating that the acknowledge is not 
 * required.
 *
 * @param [in] SigStruct_p            Event signal to be unpacked.
 * @param [out] SC_Address_p          Service Center Address
 * @param [out] UserData_p            Short message TPDU
 * @param [out] AcknowledgeRequired_p Indication on whether the acknowledgement
 *                                    is required by the dispatching entity
 *                                    (SMR, MMS, please see note below).
 *                                    TRUE -> acknowledge required 
 *                                    FALSE -> acknowledge not required
 *
 * @retval GS_EVENT_OK                 
 * @retval GS_EVENT_FAILED_UNPACKING   
 * @retval GS_EVENT_FAILED_APPLICATION 
 *
 * @signalid EVENT_RMS_V2_MESSAGERECEIVEDIND
 */

EventStatus_t Event_RMS_V2_MessageReceivedInd(
    const void *const SigStruct_p,
    RMS_V2_Address_t * const SC_Address_p,
    RMS_V2_UserData_t * const UserData_p,
    boolean * const AcknowledgeRequired_p);


/**
 * This function is used to unpack data when a failure indication (received by 
 * SMR from lower layers while waiting for the response from Messaging Transfer 
 * Server) is passed from SMR to the Messaging Transport Server.
 *
 * @param [in] SigStruct_p      Event signal to be unpacked.
 * @param [out] FailureReport_p Includes the error code associated with the 
 *                              failure
 *
 * @retval GS_EVENT_OK                 
 * @retval GS_EVENT_FAILED_UNPACKING   
 * @retval GS_EVENT_FAILED_APPLICATION 
 *
 * @signalid EVENT_RMS_V2_FAILURERECEIVEDIND
 */


EventStatus_t Event_RMS_V2_FailureReceivedInd(
    const void *const SigStruct_p,
    RMS_V2_Status_t * const FailureReport_p);


#endif                          //INCLUSION_GUARD_R_RMS_H
