#ifndef INCLUSION_GUARD_R_SMS_CB_SESSION_H
#define INCLUSION_GUARD_R_SMS_CB_SESSION_H
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
 * SMS CB Session include file for exported interface Requester Functions.
 *
 *************************************************************************/
#include "t_sms.h"
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

//========================================================================

/**
 * This function is used to create a communication session with the
 * SMS & CB Servers and receive unsolicited storage status and error events.
 *
 * @param [out] RequestResponseSocket_p : Socket to use in Request_SMS_x, Request_CBS_x
 *                                         and RECEIVE_ON_SOCKET asynch response.
 * @param [out] EventSocket_p           : Socket to use for unsolicited
 *                                         events via RECEIVE_ON_SOCKET.
 * @param [out] Error_p                   SMS Server internal error
 *                                        cause returned by the server.
 *                                        \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                        \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                        \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                        \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                        \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                        \li \c SMS_ERROR_INTERN_APP_PORT_IN_USE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
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
 * @signalid None
 * @waitmode Wait mode
 * @clienttag No.
 * @related No Related Events.
 */

SMS_RequestStatus_t Request_SMS_CB_SessionCreate(
    int *const RequestResponseSocket_p,
    int *const EventSocket_p,
    SMS_Error_t * const Error_p);


/**
 * Response function for Request_SMS_EventsSubscribe
 *
 */

SMS_RequestStatus_t Response_SMS_CB_SessionCreate(
    const void *const SigStruct_p,
    SMS_Error_t * const Error_p);

//========================================================================

/**
 * This function is used to destroy a communication session with the
 * SMS & CB Servers and stop reception of unsolicited storage status and error events.
 *
 * Clients should unsubscribe from any SMS or CB events, to which they have previously
 * subscribed, before calling this SessionDestroy API.
 *
 * @param [in] RequestCtrl_p              Pointer to struct controlling whether the
 *                                        request is called in wait mode or no wait mode.
 * @param [out] Error_p                   SMS Server internal error
 *                                        cause returned by the server.
 *                                        \li \c SMS_ERROR_INTERN_NO_ERROR
 *                                        \li \c SMS_ERROR_INTERN_SERVER_BUSY
 *                                        \li \c SMS_ERROR_INTERN_MEMORY_ALLOCATION_FAILED
 *                                        \li \c SMS_ERROR_INTERN_INVALID_WAIT_MODE
 *                                        \li \c SMS_ERROR_INTERN_UNSPECIFIED_ERROR
 *                                        \li \c SMS_ERROR_INTERN_APP_PORT_IN_USE
 *
 * @retval SMS_REQUEST_OK                 Request performed successfully
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
 * @signalid None
 * @waitmode Wait mode
 * @clienttag No.
 * @related No Related Events.
 */
SMS_RequestStatus_t Request_SMS_CB_SessionDestroy(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const int EventSocket,
    SMS_Error_t * const Error_p);

SMS_RequestStatus_t Request_SMS_CB_Shutdown(
    const SMS_RequestControlBlock_t * const RequestCtrl_p,
    const int EventSocket);

/**
 * Response function for Request_SMS_EventsUnsubscribe
 *
 */
SMS_RequestStatus_t Response_SMS_CB_SessionDestroy(
    const void *const SigStruct_p,
    const int RequestSocket,
    const int EventSocket,
    SMS_Error_t * const Error_p);

// Code is being run through a linux formatting tool (indent). The existance of
// these C/C++ extern declaration would cause all code within their braces to be
// indented. To ensure that that does not happen, we disable the indent tool
// when processing them using the *INDENT-OFF* and *INDENT-ON* commands.

// *INDENT-OFF*
#ifdef __cplusplus
}
#endif
// *INDENT-ON*
#endif                          // INCLUSION_GUARD_R_SMS_CB_SESSION_H
