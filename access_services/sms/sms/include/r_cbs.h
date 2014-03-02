#ifndef INCLUSION_GUARD_R_CBS_H
#define INCLUSION_GUARD_R_CBS_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     Public include file for SW Backplane CBS Requester Functions
*
********************************************************************************************
*/

#include "t_cbs.h"
#include "t_sms_cb_session.h"


// Code is being run through a linux formatting tool (indent). The existance of
// these C/C++ extern declaration would cause all code within their braces to be
// indented. To ensure that that does not happen, we disable the indent tool
// when processing them using the *INDENT-OFF* and *INDENT-ON* commands.

// *INDENT-OFF*
// Ensure that "C++" compiled files can link with these "C" compiled functions.
#ifdef __cplusplus
extern "C" {
#endif
// *INDENT-ON*


/**
  * The Request_CBS_Subscribe() function subscribes to receive Cell Broadcast messages.
  *
  * This function enables a client to subscribe to Cell Broadcast service to enable it to 
  * receive messages within the specified Message Identifier and Data Coding Scheme ranges,
  * as described in document 3G TS 23.041[1].
  *
  * @param [in]  RequestCtrl_p          Pointer to struct controlling whether the 
  *                                     request is called in wait mode or no wait 
  *                                     mode.
  * @param [in]  SubscribeAll_CBMI      1 - subscribe to all CBMI, 0 - subscription determined by SubscriptionList_p.
  * @param [in]  NumberOfSubscriptions  Number of subscriptions referenced by Subscriptions_p.
  * @param [in]  Subscriptions_p        List of subscriptions of CBMI and DCS ranges.
  *                                     Valid CBMI range 0000-FFFFh. Note that FFFFh is recognized as a valid code
  *                                     and SIM empty field values should be removed.
  *                                     Valid DCS range 00-FFh.
  * @param [out] Error_p                Error cause returned by the CB Server.
  *                                     \li \c CBS_ERROR_NONE 
  *                                     \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                                     \li \c CBS_ERROR_TOO_MANY_SUBSCRIBERS 
  *                                     \li \c CBS_ERROR_PROCESS_ALREADY_SUBSCRIBED 
  *                                     \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @see Request_CBS_Unsubscribe.
  *
  * @signalid  RESPONSE_CBS_SUBSCRIBE
  * @waitmode  Wait mode and No Wait mode allowed.
  * @clienttag Yes
  * @related   The following events are related: 
  *            \li EVENT_CBS_CELLBROADCASTMESSAGERECEIVED
  */
SMS_RequestStatus_t Request_CBS_Subscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const uint8_t SubscribeAll_CBMI,
    const uint8_t NumberOfSubscriptions,
    const CBS_Subscription_t * const Subscriptions_p,
    CBS_Error_t * const Error_p);


/**
  * Response function for Request_CBS_Subscribe 
  */
SMS_RequestStatus_t Response_CBS_Subscribe(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p);



/**
  * The Request_CBS_Unsubscribe() function unsubscribes from receiving Cell Broadcast messages.
  *
  * This function enables a client to unsubscribe from Cell Broadcast messages.
  *
  * @param [in]  RequestCtrl_p          Pointer to struct controlling whether the 
  *                                     request is called in wait mode or no wait 
  *                                     mode.
  * @param [in]  UnsubscribeAll_CBMI    1 - unubscribe to all CBMI, 0 - unsubscription determined by SubscriptionList_p.
  * @param [in]  NumberOfSubscriptions  Number of subscriptions referenced by Subscriptions_p.
  * @param [in]  Subscriptions_p        List of subscriptions of CBMI and DCS ranges.
  *                                     Valid CBMI range 0000-FFFFh. Note that FFFFh is recognized as a valid code
  *                                     and SIM empty field values should be removed.
  *                                     Valid DCS range 00-FFh.
  * @param [out] Error_p                Error cause returned by the CB Server.
  *                                     \li \c CBS_ERROR_NONE 
  *                                     \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                                     \li \c CBS_ERROR_TOO_MANY_SUBSCRIBERS 
  *                                     \li \c CBS_ERROR_PROCESS_ALREADY_SUBSCRIBED 
  *                                     \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @see Request_CBS_Subscribe.
  *
  * @signalid  RESPONSE_CBS_UNSUBSCRIBE
  * @waitmode  Wait mode and No Wait mode allowed.
  * @clienttag Yes
  * @related   None
  */
SMS_RequestStatus_t Request_CBS_Unsubscribe(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const uint8_t UnsubscribeAll_CBMI,
    const uint8_t NumberOfSubscriptions,
    const CBS_Subscription_t * const Subscriptions_p,
    CBS_Error_t * const Error_p);


/**
  * Response function for Request_CBS_Unsubscribe 
  */
SMS_RequestStatus_t Response_CBS_Unsubscribe(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p);



/**
  * The Request_CBS_GetNumberOfSubscriptions() function gets the number of Cell Broadcast subscriptions.
  *
  * This function enables a client to retrieve the current number of Cell Broadcast subscriptions
  * to be used to allocate enough memory to return the subscriptions via Request_CBS_GetSubscriptions(). 
  *
  * @param [in]  RequestCtrl_p            Pointer to struct controlling whether the 
  *                                       request is called in wait mode or no wait 
  *                                       mode.
  * @param [in]  AllSubscriptions         1 - retreive all client subscriptons, 0 - retreive this clients subscriptons.
  * @param [out] NumberOfSubscriptions_p  Number of subscriptions to be returned by Subscriptions_p.
  * @param [out] Error_p                  Error cause returned by the CB Server.
  *                                       \li \c CBS_ERROR_NONE 
  *                                       \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                                       \li \c CBS_ERROR_TOO_MANY_SUBSCRIBERS 
  *                                       \li \c CBS_ERROR_PROCESS_ALREADY_SUBSCRIBED 
  *                                       \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @signalid  RESPONSE_CBS_GETSUBSCRIPTIONS
  * @waitmode  Wait mode.
  * @clienttag Yes
  * @related   None
  */
SMS_RequestStatus_t Request_CBS_GetNumberOfSubscriptions(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const uint8_t AllSubscriptions,
    uint8_t * const NumberOfSubscriptions_p,
    CBS_Error_t * const Error_p);


/**
  * The Response_CBS_GetNumberOfSubscriptions() function gets the number of Cell Broadcast subscriptions.
  *
  * This function enables a client to retrieve the current number of Cell Broadcast subscriptions
  * to be used to allocate enough memory to return the subscriptions via Request_CBS_GetSubscriptions(). 
  *
  * @param [in]  SigStruct_p               Response signal to be unpacked.
  * @param [out] NumberOfSubscriptions_p   Number of subscriptions to be returned by Subscriptions_p.
  * @param [out] Error_p                   Error cause returned by the CB Server.
  *                                        \li \c CBS_ERROR_NONE 
  *                                        \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                                        \li \c CBS_ERROR_INVALID_SUBSCRIPTION
  *                                        \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @signalid  RESPONSE_CBS_GETSUBSCRIPTIONS
  * @related   None
  */
SMS_RequestStatus_t Response_CBS_GetNumberOfSubscriptions(
    const void *const SigStruct_p,
    uint8_t * const NumberOfSubscriptions_p,
    CBS_Error_t * const Error_p);


/**
  * The Request_CBS_GetSubscriptions() function gets the Cell Broadcast subscriptions.
  *
  * This function enables a client to retrieve the current Cell Broadcast subscriptions.
  * NumberOfSubscriptions should indicate the number of subscriptions of sizeof(CBS_Subscription_t)
  * that may be returned by Subscriptions_p and may be determined by first calling Request_CBS_GetNumberOfSubscriptions().
  *
  * The client should allocate and free the Subscriptions_p buffer. The buffer size should be
  * (*NumberOfSubscriptions_p * sizeof (CBS_Subscription_t)).
  *
  * @param [in]  RequestCtrl_p                Pointer to struct controlling whether the 
  *                                           request is called in wait mode or no wait 
  *                                           mode.
  * @param [in]  AllSubscriptions             1 - retreive all client subscriptons, 0 - retreive this clients subscriptons.
  * @param [in, out] NumberOfSubscriptions_p  Number of subscriptions to be returned by Subscriptions_p.
  * @param [out]  Subscriptions_p             List of subscriptions of CBMI and DCS ranges.
  *                                           Valid CBMI range 0000-FFFFh. Note that FFFFh is recognized as a valid code
  *                                           and SIM empty field values should be removed.
  *                                           Valid DCS range 00-FFh.
  * @param [out] Error_p                      Error cause returned by the CB Server.
  *                                           \li \c CBS_ERROR_NONE 
  *                                           \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                                           \li \c CBS_ERROR_TOO_MANY_SUBSCRIBERS 
  *                                           \li \c CBS_ERROR_PROCESS_ALREADY_SUBSCRIBED 
  *                                           \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @signalid  RESPONSE_CBS_GETSUBSCRIPTIONS
  * @waitmode  Wait mode.
  * @clienttag Yes
  * @related   None
  */
SMS_RequestStatus_t Request_CBS_GetSubscriptions(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const uint8_t AllSubscriptions,
    uint8_t * const NumberOfSubscriptions_p,
    CBS_Subscription_t * const Subscriptions_p,
    CBS_Error_t * const Error_p);


/**
  * The Response_CBS_GetSubscriptions() function gets the Cell Broadcast subscriptions.
  *
  * This function enables a client to retrieve the current Cell Broadcast subscriptions.
  * NumberOfSubscriptions should indicate the number of subscriptions of sizeof(CBS_Subscription_t)
  * that may be returned by Subscriptions_p and may be determined by first calling Request_CBS_GetNumberOfSubscriptions().
  *
  * The client should allocate and free the Subscriptions_p buffer. The buffer size should be
  * (*NumberOfSubscriptions_p * sizeof (CBS_Subscription_t)).
  *
  * @param [in]  SigStruct_p               Response signal to be unpacked.
  * @param [out] NumberOfSubscriptions_p   Number of subscriptions returned by Subscriptions_p.
  * @param [out] Subscriptions_p           List of subscriptions of CBMI and DCS ranges.
  *                                        Valid CBMI range 0000-FFFFh. Note that FFFFh is recognized as a valid code
  *                                        and SIM empty field values should be removed.
  *                                        Valid DCS range 00-FFh.
  * @param [out] Error_p                   Error cause returned by the CB Server.
  *                                        \li \c CBS_ERROR_NONE 
  *                                        \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                                        \li \c CBS_ERROR_INVALID_SUBSCRIPTION
  *                                        \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @signalid  RESPONSE_CBS_GETSUBSCRIPTIONS
  * @related   None
  */
SMS_RequestStatus_t Response_CBS_GetSubscriptions(
    const void *const SigStruct_p,
    uint8_t * const NumberOfSubscriptions_p,
    CBS_Subscription_t * const Subscriptions_p,
    CBS_Error_t * const Error_p);



/**
  * The Request_CBS_Activate() function activates the current Cell Broadcast subscriptions.
  *
  * This function enables a client to activate the current Cell Broadcast subscriptions. 
  *
  * @param [in]  RequestCtrl_p  Pointer to struct controlling whether the 
  *                             request is called in wait mode or no wait 
  *                             mode.
  * @param [out] Error_p        Error cause returned by the CB Server.
  *                             \li \c CBS_ERROR_NONE 
  *                             \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                             \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @see Request_CBS_Deactivate
  * 
  * @signalid  RESPONSE_CBS_ACTIVATE
  * @waitmode  Wait mode and No Wait mode allowed.
  * @clienttag Yes
  * @related   None
  */
SMS_RequestStatus_t Request_CBS_Activate(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    CBS_Error_t * const Error_p);


/**
  * Response function for Request_CBS_Activate 
  */
SMS_RequestStatus_t Response_CBS_Activate(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p);



/**
  * The Request_CBS_Deactivate() function deactivates the current Cell Broadcast subscriptions.
  *
  * This function enables a client to deactivate the current Cell Broadcast subscriptions. 
  *
  * @param [in]  RequestCtrl_p  Pointer to struct controlling whether the 
  *                             request is called in wait mode or no wait 
  *                             mode.
  * @param [out] Error_p        Error cause returned by the CB Server.
  *                             \li \c CBS_ERROR_NONE 
  *                             \li \c CBS_ERROR_TEMPORARY_FAILURE 
  *                             \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @see Request_CBS_Activate
  * 
  * @signalid  RESPONSE_CBS_DEACTIVATE
  * @waitmode  Wait mode and No Wait mode allowed.
  * @clienttag Yes
  * @related   None
  */
SMS_RequestStatus_t Request_CBS_Deactivate(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    CBS_Error_t * const Error_p);


/**
  * Response function for Request_CBS_Deactivate 
  */
SMS_RequestStatus_t Response_CBS_Deactivate(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p);



/**
  * The Request_CBS_CellBroadcastMessageRead() function returns a previously received Cell Broadcast message.
  *
  * This function enables a client to read a previously received Cell Broadcast message.
  *
  * @param [in]  RequestCtrl_p      Pointer to struct controlling whether the
  *                                 request is called in wait mode or no wait
  *                                 mode.
  * @param [in]  MessageHeader_     Header information to identify when CB Message is being
  *                                 requested.
  * @param [out] Pdu_p              Cell Broadcast message data returned from in CB Server RAM.
  * @param [out] Error_p            Error cause returned by the CB Server.
  *                                 \li \c CBS_ERROR_NONE
  *                                 \li \c CBS_ERROR_TEMPORARY_FAILURE
  *                                 \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
  *                                 \li \c CBS_ERROR_INVALID_PARAMETER
  *                                 \li \c CBS_ERROR_REQUESTED_MESSAGE_NO_LONGER_EXISTS
  *
  * @return SMS_RequestStatus_t:
  * @retval SMS_REQUEST_FAILED  CB Server could not perform request
  * @retval SMS_REQUEST_OK      Request performed successfully.
  * @retval SMS_REQUEST_PENDING Request is in progress (No Wait mode only).
  *
  * @signalid  RESPONSE_CBS_CELLBROADCASTMESSAGEREAD
  * @waitmode  Wait mode and No Wait mode allowed.
  * @clienttag Yes
  * @related   None
  */
SMS_RequestStatus_t Request_CBS_CellBroadcastMessageRead(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const CBS_MessageHeader_t * const MessageHeader_p,
    CB_Pdu_t * const Pdu_p,
    CBS_Error_t * const Error_p);


/**
  * Response function for Request_CellBroadcastMessageRead
  */
SMS_RequestStatus_t Response_CBS_CellBroadcastMessageRead(
    const void *const SigStruct_p,
    CB_Pdu_t * const Pdu_p,
    CBS_Error_t * const Error_p);



/**
  * The Event_CBS_CellBroadcastReceived() function unpacks the received Cell Broadcast page signal data.
  *
  * This function unpacks a Cell Broadcast page from a received signal.
  *
  * @param [in]  SigStruct_p   Event signal to be unpacked.
  * @param [out] ClientTag_p   ClientTag used when CB Subscription was made.
  * @param [out] Pdu_p The unpacked Cell Broadcast message data.
  *
  * @return SMS_EventStatus_t:
  * @retval SMS_EVENT_FAILED_UNPACKING   Signal unpack failed.
  * @retval SMS_EVENT_FAILED_APPLICATION The unpack function is not available.
  * @retval SMS_EVENT_OK                 Signal unpack performed successfully.
  *
  * @signalid  EVENT_CBS_CELLBROADCASTPAGERECEIVED
  * @related   None
  */
SMS_EventStatus_t Event_CBS_CellBroadcastMessageReceived(
    const void *const SigStruct_p,
    SMS_ClientTag_t * const ClientTag_p,
    CB_Pdu_t * const Pdu_p);


// Code is being run through a linux formatting tool (indent). The existance of
// these C/C++ extern declaration would cause all code within their braces to be
// indented. To ensure that that does not happen, we disable the indent tool
// when processing them using the *INDENT-OFF* and *INDENT-ON* commands.

// *INDENT-OFF*
#ifdef __cplusplus
}
#endif
// *INDENT-ON*
#endif                          // INCLUSION_GUARD_R_CBS_H
