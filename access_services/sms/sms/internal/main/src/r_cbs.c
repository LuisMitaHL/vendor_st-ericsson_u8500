/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     SW Backplane CBS Requester Functions
*
********************************************************************************************
*/


#include "r_cbs.h"
#include "g_cbs.h"

#include "r_sms.h"              // This should not be included. However a problem in r_sms.h and r_smslinuxporting.h means that some signal handling code is in r_sms.h. See r_sms.h for more comments.
#include "r_smslinuxporting.h"


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
  * @retval SMSS_REQUEST_PENDING Request is in progress (No Wait mode only).
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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(SubscribeAll_CBMI);
    SMS_IDENTIFIER_NOT_USED(NumberOfSubscriptions);
    SMS_IDENTIFIER_NOT_USED(Subscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    CBS_Subscription_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL || (Subscriptions_p == NULL && !SubscribeAll_CBMI)) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // When subscribing to all, NumberOfSubscriptions maybe zero.
    // Signal buffer needs to be:
    //    size of CBS_Subscription_Req_t signal structure
    //  + size of CBS_Subscription_t subscription data structures for each supplied subscription
    //  - sizeof CBS_Subscription_t subscription data structure which is already part of CBS_Subscription_Req_t
    Send_p = SMS_SIGNAL_UNTYPED_ALLOC(sizeof(CBS_Subscription_Req_t) + (NumberOfSubscriptions * sizeof(CBS_Subscription_t)) - sizeof(CBS_Subscription_t), REQUEST_CBS_SUBSCRIBE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->All_CBMI = SubscribeAll_CBMI;
        Send_p->NumberOfSubscriptions = NumberOfSubscriptions;

        if (NumberOfSubscriptions > 0) {
            if (Subscriptions_p != NULL) {
                memcpy(&Send_p->FirstSubscription[0], Subscriptions_p, NumberOfSubscriptions * sizeof(CBS_Subscription_t));
            } else {
                // The client has not supplied the correct subscription data, or is subscribing to all.
                memset(&Send_p->FirstSubscription[0], 0xFF, NumberOfSubscriptions * sizeof(CBS_Subscription_t));
            }
        }

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_CBS_SUBSCRIBE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_CBS_Subscribe(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = CBS_ERROR_NONE;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/**
  * Response function for Request_CBS_Subscribe
  */
SMS_RequestStatus_t Response_CBS_Subscribe(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    CBS_Subscription_Resp_t *Rec_p = (CBS_Subscription_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->Error;

    if (Rec_p->Error == CBS_ERROR_NONE) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->Error == CBS_ERROR_TEMPORARY_FAILURE) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}



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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(UnubscribeAll_CBMI);
    SMS_IDENTIFIER_NOT_USED(NumberOfSubscriptions);
    SMS_IDENTIFIER_NOT_USED(Subscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    CBS_Subscription_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL || (Subscriptions_p == NULL && !UnsubscribeAll_CBMI)) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // When subscribing to all, NumberOfSubscriptions maybe zero.
    // Signal buffer needs to be:
    //    size of CBS_Subscription_Req_t signal structure
    //  + size of CBS_Subscription_t subscription data structures for each supplied subscription
    //  - sizeof CBS_Subscription_t subscription data structure which is already part of CBS_Subscription_Req_t
    Send_p = SMS_SIGNAL_UNTYPED_ALLOC(sizeof(CBS_Subscription_Req_t) + (NumberOfSubscriptions * sizeof(CBS_Subscription_t)) - sizeof(CBS_Subscription_t), REQUEST_CBS_UNSUBSCRIBE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        // Load up the data
        Send_p->All_CBMI = UnsubscribeAll_CBMI;
        Send_p->NumberOfSubscriptions = NumberOfSubscriptions;
        if (NumberOfSubscriptions > 0) {
            if (Subscriptions_p != NULL) {
                memcpy(&Send_p->FirstSubscription[0], Subscriptions_p, NumberOfSubscriptions * sizeof(CBS_Subscription_t));
            } else {
                // The client has not supplied the correct subscription data, or is unsubscribing from all.
                memset(&Send_p->FirstSubscription[0], 0xFF, NumberOfSubscriptions * sizeof(CBS_Subscription_t));
            }
        }

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_CBS_UNSUBSCRIBE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_CBS_Unsubscribe(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = CBS_ERROR_NONE;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/**
  * Response function for Request_CBS_Unsubscribe 
  */
SMS_RequestStatus_t Response_CBS_Unsubscribe(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    CBS_Subscription_Resp_t *Rec_p = (CBS_Subscription_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->Error;

    if (Rec_p->Error == CBS_ERROR_NONE) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->Error == CBS_ERROR_TEMPORARY_FAILURE) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}



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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(AllSubscriptions);
    SMS_IDENTIFIER_NOT_USED(NumberOfSubscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    CBS_GetSubscriptions_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(CBS_GetSubscriptions_Req_t, REQUEST_CBS_GETNUMBEROFSUBSCRIPTIONS);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->AllSubscriptions = AllSubscriptions;
        Send_p->NumberOfSubscriptions = 0;      // NumberOfSubscriptions is not needed for this requester, however we initialise it to something for good practice.

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_CBS_GETNUMBEROFSUBSCRIPTIONS };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_CBS_GetNumberOfSubscriptions(Rec_p, NumberOfSubscriptions_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = CBS_ERROR_NONE;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif
}


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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(NumberOfSubscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    CBS_GetSubscriptions_Resp_t *Rec_p = (CBS_GetSubscriptions_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || NumberOfSubscriptions_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->Error;

    if (Rec_p->Error == CBS_ERROR_NONE) {
        *NumberOfSubscriptions_p = Rec_p->NumberOfSubscriptions;

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->Error == CBS_ERROR_TEMPORARY_FAILURE) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif
}


/**
  * The Request_CBS_GetSubscriptions() function gets the Cell Broadcast subscriptions.
  *
  * This function enables a client to retrieve the current Cell Broadcast subscriptions.
  *
  * @param [in]  RequestCtrl_p                 Pointer to struct controlling whether the
  *                                            request is called in wait mode or no wait
  *                                            mode.
  * @param [in]  AllSubscriptions              1 - retreive all client subscriptons, 0 - retreive this clients subscriptons.
  * @param [in, out]  NumberOfSubscriptions_p  Number of subscriptions to be returned by Subscriptions_p.
  * @param [out] Subscriptions_p               List of subscriptions of CBMI and DCS ranges.
  *                                            Valid CBMI range 0000-FFFFh. Note that FFFFh is recognized as a valid code
  *                                            and SIM empty field values should be removed.
  *                                            Valid DCS range 00-FFh.
  * @param [out] Error_p                       Error cause returned by the CB Server.
  *                                            \li \c CBS_ERROR_NONE
  *                                            \li \c CBS_ERROR_TEMPORARY_FAILURE
  *                                            \li \c CBS_ERROR_TOO_MANY_SUBSCRIBERS
  *                                            \li \c CBS_ERROR_PROCESS_ALREADY_SUBSCRIBED
  *                                            \li \c CBS_ERROR_CLIENT_CONTROL_DATA_MISSING_OR_WRONG
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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(AllSubscriptions);
    SMS_IDENTIFIER_NOT_USED(NumberOfSubscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Subscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    CBS_GetSubscriptions_Req_t *Send_p;

    if (RequestCtrl_p == NULL || NumberOfSubscriptions_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(CBS_GetSubscriptions_Req_t, REQUEST_CBS_GETSUBSCRIPTIONS);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->AllSubscriptions = AllSubscriptions;
        Send_p->NumberOfSubscriptions = *NumberOfSubscriptions_p;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_CBS_GETSUBSCRIPTIONS };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_CBS_GetSubscriptions(Rec_p, NumberOfSubscriptions_p, Subscriptions_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = CBS_ERROR_NONE;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/**
  * The Response_CBS_GetSubscriptions() function gets the Cell Broadcast subscriptions.
  *
  * This function is the response to retrieve the current Cell Broadcast subscriptions.
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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(NumberOfSubscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Subscriptions_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    CBS_GetSubscriptions_Resp_t *Rec_p = (CBS_GetSubscriptions_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || NumberOfSubscriptions_p == NULL || Subscriptions_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->Error;

    if (Rec_p->Error == CBS_ERROR_NONE) {
        // Ensure that we don't copy more than the Subscriptions_p buffer can hold.
        *NumberOfSubscriptions_p = SMS_MIN(Rec_p->NumberOfSubscriptions, *NumberOfSubscriptions_p);

        memcpy(Subscriptions_p, &Rec_p->FirstSubscription[0], *NumberOfSubscriptions_p * sizeof(CBS_Subscription_t));

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->Error == CBS_ERROR_TEMPORARY_FAILURE) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}



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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    CBS_Activation_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(CBS_Activation_Req_t, REQUEST_CBS_ACTIVATE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_CBS_ACTIVATE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_CBS_Activate(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = CBS_ERROR_NONE;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/**
  * Response function for Request_CBS_Activate
  */
SMS_RequestStatus_t Response_CBS_Activate(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    CBS_Activation_Resp_t *Rec_p = (CBS_Activation_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->Error;

    if (Rec_p->Error == CBS_ERROR_NONE) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->Error == CBS_ERROR_TEMPORARY_FAILURE) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}



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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    CBS_Activation_Req_t *Send_p;

    if (RequestCtrl_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(CBS_Activation_Req_t, REQUEST_CBS_DEACTIVATE);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_CBS_DEACTIVATE };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_CBS_Activate(Rec_p, Error_p);

            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = CBS_ERROR_NONE;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/**
  * Response function for Request_CBS_Deactivate
  */
SMS_RequestStatus_t Response_CBS_Deactivate(
    const void *const SigStruct_p,
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    CBS_Activation_Resp_t *Rec_p = (CBS_Activation_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->Error;

    if (Rec_p->Error == CBS_ERROR_NONE) {
        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->Error == CBS_ERROR_TEMPORARY_FAILURE) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}



/**
  * The Request_CBS_CellBroadcastMessageRead() function returns a previously received CB Message.
  *
  * This function enables a client to read a previously received Cell Broadcast message.
  *
  * @param [in]  RequestCtrl_p      Pointer to struct controlling whether the
  *                                 request is called in wait mode or no wait
  *                                 mode.
  * @param [in]  MessageHeader_p    Header information to identify when CB Message is being
  *                                 requested.
  * @param [out] PduLength_p        Cell Broadcast message data length.
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
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(RequestCtrl_p);
    SMS_IDENTIFIER_NOT_USED(MessageIdentifier);
    SMS_IDENTIFIER_NOT_USED(SerialNumber);
    SMS_IDENTIFIER_NOT_USED(PageNumber);
    SMS_IDENTIFIER_NOT_USED(Pdu_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else
    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;
    CBS_CellBroadcastMessageRead_Req_t *Send_p;

    if (RequestCtrl_p == NULL || MessageHeader_p == NULL || Pdu_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }

    Send_p = SMS_SIGNAL_ALLOC(CBS_CellBroadcastMessageRead_Req_t, REQUEST_CBS_CELLBROADCASTMESSAGEREAD);

    if (Send_p == SMS_NIL)      // Check for memory allocation failure
    {
        RequestStatus = SMS_REQUEST_FAILED_RESOURCE;

        *Error_p = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
    } else {
        // Set up the client tag
        Do_SMS_ClientTag_Set(Send_p, RequestCtrl_p->ClientTag);

        // Load up the data
        Send_p->MessageHeader.MessageIdentifier = MessageHeader_p->MessageIdentifier;
        Send_p->MessageHeader.SerialNumber = MessageHeader_p->SerialNumber;
        Send_p->MessageHeader.DataCodingScheme = MessageHeader_p->DataCodingScheme;
        Send_p->MessageHeader.PageParameter = MessageHeader_p->PageParameter;

        if (RequestCtrl_p->WaitMode == SMS_WAIT_MODE) {
            const SMS_SIGSELECT SigMSG_Command_Cnf[] = { 1, RESPONSE_CBS_CELLBROADCASTMESSAGEREAD };
            void *Rec_p;

            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            Rec_p = SMS_RECEIVE(SigMSG_Command_Cnf);

            // Unpack the returned signal into the output parameters
            RequestStatus = Response_CBS_CellBroadcastMessageRead(Rec_p, Pdu_p, Error_p);
            SMS_SIGNAL_FREE(&Rec_p);
        } else if (RequestCtrl_p->WaitMode == SMS_NO_WAIT_MODE) {
            SMS_SEND(&Send_p, RequestCtrl_p->Socket);

            RequestStatus = SMS_REQUEST_PENDING;

            *Error_p = CBS_ERROR_NONE;
        } else                  // Invalid RequestCtrl_p->WaitMode
        {
            RequestStatus = SMS_REQUEST_FAILED_PARAMETER;

            *Error_p = CBS_ERROR_CONTROL_DATA_MISSING_OR_WRONG;

            SMS_SIGNAL_FREE(&Send_p);
        }
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}


/**
  * Response function for Request_CellBroadcastMessageRead
  */
SMS_RequestStatus_t Response_CBS_CellBroadcastMessageRead(
    const void *const SigStruct_p,
    CB_Pdu_t * const Pdu_p,
    CBS_Error_t * const Error_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(Pdu_p);
    SMS_IDENTIFIER_NOT_USED(Error_p);

    return SMS_REQUEST_NOT_AVAILABLE;

#else

    SMS_RequestStatus_t RequestStatus = SMS_REQUEST_FAILED;

    CBS_CellBroadcastMessageRead_Resp_t *Rec_p = (CBS_CellBroadcastMessageRead_Resp_t *) SigStruct_p;

    if (SigStruct_p == NULL || Pdu_p == NULL || Error_p == NULL) {
        return SMS_REQUEST_FAILED_PARAMETER;
    }
    // Set up the error code. No error default is set up by the Requester.
    *Error_p = Rec_p->Error;

    if (Rec_p->Error == CBS_ERROR_NONE) {

        Pdu_p->PduLength = (uint16_t)(Rec_p->PduLength < CBS_MESSAGE_MAX_LENGTH) ? Rec_p->PduLength : CBS_MESSAGE_MAX_LENGTH;

        (void) memcpy(Pdu_p, Rec_p->Pdu, (size_t) Pdu_p->PduLength);

        RequestStatus = SMS_REQUEST_OK;
    } else if (Rec_p->Error == CBS_ERROR_TEMPORARY_FAILURE) {
        RequestStatus = SMS_REQUEST_NOT_AVAILABLE;
    } else {
        RequestStatus = SMS_REQUEST_FAILED_APPLICATION;
    }

    return RequestStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}



/**
  * The Event_CBS_CellBroadcastMessageReceived() function unpacks the received Cell Broadcast message signal data.
  *
  * This function unpacks a Cell Broadcast message from a received signal.
  *
  * @param [in]  SigStruct_p   Event signal to be unpacked.
  * @param [out] ClientTag_p   ClientTag used when CB Subscription was made.
  * @param [out] PduLength_p  The Cell Broadcast message length.
  * @param [out] Pdu_p        The unpacked Cell Broadcast message.
  *
  * @return SMS_EventStatus_t:
  * @retval SMS_EVENT_FAILED_UNPACKING   Signal unpack failed.
  * @retval SMS_EVENT_FAILED_APPLICATION The unpack function is not available.
  * @retval SMS_EVENT_OK                 Signal unpack performed successfully.
  *
  * @signalid  EVENT_CBS_CELLBROADCASTMESSAGERECEIVED
  * @related   None
  */
SMS_EventStatus_t Event_CBS_CellBroadcastMessageReceived(
    const void *const SigStruct_p,
    SMS_ClientTag_t * const ClientTag_p,
    CB_Pdu_t * const Pdu_p)
{
#if defined SWBP_SERVICE_NOT_AVAILABLE

    SMS_IDENTIFIER_NOT_USED(SigStruct_p);
    SMS_IDENTIFIER_NOT_USED(ClientTag_p);
    SMS_IDENTIFIER_NOT_USED(PduLength_p);
    SMS_IDENTIFIER_NOT_USED(Pdu_p);

    return SMS_EVENT_NOT_AVAILABLE;

#else

    SMS_EventStatus_t EventStatus = SMS_EVENT_FAILED_APPLICATION;

    Event_CBS_CellBroadcastMessageReceived_t *Event_p = (Event_CBS_CellBroadcastMessageReceived_t *) SigStruct_p;

    if (SigStruct_p == NULL || Pdu_p == NULL) {
        return SMS_EVENT_FAILED_PARAMETER;
    }

    if (Event_p->SigselectWithClientTag.Primitive != EVENT_CBS_CELLBROADCASTMESSAGERECEIVED) {
        EventStatus = SMS_EVENT_FAILED_UNPACKING;
    } else {

        /* Check if optional ClientTag_p is not NULL. */
        if (ClientTag_p != NULL) {
            *ClientTag_p = Event_p->SigselectWithClientTag.ClientTag;
        }

        Pdu_p->PduLength = (uint16_t)(Event_p->PduLength < CBS_MESSAGE_MAX_LENGTH) ? Event_p->PduLength : CBS_MESSAGE_MAX_LENGTH;

        /* Pdu_p is allocated to to hold max PDU length */
        (void) memcpy(Pdu_p->Pdu, Event_p->Pdu, (size_t) Pdu_p->PduLength);

        EventStatus = SMS_EVENT_OK;
    }

    return EventStatus;

#endif                          //SWBP_SERVICE_NOT_AVAILABLE
}
