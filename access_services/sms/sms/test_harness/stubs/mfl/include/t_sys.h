#ifndef INCLUSION_GUARD_T_SYS_H
#define INCLUSION_GUARD_T_SYS_H

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
* Header file containing type definitions required for SwBP
*
*
*************************************************************************/

/*************************************************************************
* Includes
*************************************************************************/

#include "r_os.h"
#include "t_os.h"
#include "r_basicdefinitions.h"
#include "t_basicdefinitions.h"

/**
 * Used to specify whether a Global Service request is to be executed
 * synchronously (WAIT_MODE) or asynchronously (NO_WAIT_MODE).
 *
 * @param WAIT_MODE A Global Service will execute synchronously (i.e. "hang" until the call returns).
 * @param NO_WAIT_MODE A Global Service will execute asynchronously.
 */
/*TYPEDEF_ENUM 
{
  WAIT_MODE,
  NO_WAIT_MODE
} ENUM8(WaitMode_t);*/
//For MFL modem, we need a version of this so that RequestControlBlock_t and SMS_RequestControlBlock_t
// Data structures can be cast onto each other.
typedef enum {
    WAIT_MODE,
    NO_WAIT_MODE
} WaitMode_t;


/**
 * Client Tag is used in all Global Service request functions and their
 * associated request and response signals. It is also used in the Event
 * Channel event signals. In the latter case, the Client Tag information is
 * added to the signal by the dispatcher process. The client tag is used to
 * uniquely identify a service request within a client process. Note that it
 * can not be used in a server in anyway (for example to identify a session
 * with a client).
 */
typedef uint32 ClientTag_t;


/**
 * Confirm Handle is used in the Unified SwBP style in all Global - and Simple
 * Services that use Event with Confirm. The value is selected by the server
 * and provided as a parameter in an Event call-back. The same value is then
 * returned in the Confirm call-back made by the client. The purpose of the Confirm 
 * Handle is to allow the server to uniquely match a Confirm against an Event.
 */
typedef uint32 ConfirmHandle_t;


/**
 * Defines the value that is used to indicate that a client application did not
 * specify a client tag. This is only defined here for legacy software.
 */
#define CLIENT_TAG_NOT_USED 0


/**
 * Return type for all the client tag help functions. See chapter 4 for more
 * details.
 *
 * @param GS_CLIENT_TAG_OK Client Tag value is OK.
 * @param GS_CLIENT_TAG_FAILED_UNPACKING An error ocurred while unpacking.
 * @param GS_CLIENT_TAG_FAILED_PACKING An error ocurred while packing.
 * @param GS_CLIENT_TAG_NOT_SUPPORTED Client Tag is not supported.
 */
TYPEDEF_ENUM {
GS_CLIENT_TAG_OK, GS_CLIENT_TAG_FAILED_UNPACKING, GS_CLIENT_TAG_FAILED_PACKING, GS_CLIENT_TAG_NOT_SUPPORTED} ENUM8(ClientTagStatus_t);


/**
 * Used in all Global Service request functions. It specifies the function
 * execution mode, WAIT_MODE or NO_WAIT_MODE, Client Tag and the client callback
 * specification.
 *
 * @param WaitMode Specifies whether the service executes in WAIT_MODE or NO_WAIT_MODE
 * @param ClientTag The Client Tag value for the service.
 * @param ClientHandlers_p Pointer to the callback specification.
 */
/*
typedef struct RequestControlBlock_t
{
  WaitMode_t  WaitMode;
  ClientTag_t ClientTag;
  const void* ClientHandlers_p;
} RequestControlBlock_t;
*/

/*************************************************************************
*   MFL specific                 
**************************************************************************/
typedef struct modem_session_s modem_session_t;
/*************************************************************************/

typedef struct {
    WaitMode_t WaitMode;
    ClientTag_t ClientTag;      /* MFL specific. Used to point to
                                 * the same request control block where it
                                 * is situated. This is the only part that
                                 * is sent onwards when doing a SEND in a request */

    int mfl_modem_stub_socket;  // This is required so that this structure can be used in place of the SMS_RequestControlBlock_t.

    modem_session_t *modem_session_ptr; /* MFl specific. Session handle */
    ClientTag_t client_client_tag;      /* MFL specific. Contains the actual client tag */
} RequestControlBlock_t;


/**
 * Used in the signals sent from within a Global Service request function
 * towards the serving process and in the response from the serving process to
 * the client. It is also used when sending an Event Channel signal to event
 * subscribers.
 *
 * @param Primitive The signal identity.
 * @param ClientTag The Client tag value.
 */

typedef struct {
    SIGSELECT Primitive;
    ClientTag_t ClientTag;
} SigselectWithClientTag_t;


/**
 * Used for Events with Confirm in the classical SwBP style.
 *
 * @param ProcessId The process id
 * @param ClientTag The Client tag value.
 */
typedef struct {
    PROCESS ProcessId;
    ClientTag_t ClientTag;
} SwBP_SignalInfo_t;


/**
 * Return type for Global Service request and response functions.
 *
 * @param REQUEST_OK Used to indicate that the service has been performed successfully.
 * @param REQUEST_PENDING Used in NO_WAIT_MODE to indicate that the request has been sent to the server.
 * @param REQUEST_NOT_AVAILABLE Service is not available. Often depends on that the service is configured out.
 * @param REQUEST_FAILED Service failed.
 * @param REQUEST_FAILED_RESOURCE Used to indicate that there were not enough resources to perform the service.
 * @param REQUEST_TIME_OUT Used to indicate that the service timed out.
 * @param REQUEST_FAILED_PARAMETER Used to indicate that one/several of the parameters are incorrect.
 * @param REQUEST_FAILED_APPLICATION Application failed.
 * @param REQUEST_FAILED_UNPACKING Used to indicate that something went wrong when trying to unpack the signal.
 */
TYPEDEF_ENUM {
REQUEST_OK,
        REQUEST_PENDING,
        REQUEST_NOT_AVAILABLE, REQUEST_FAILED, REQUEST_FAILED_RESOURCE, REQUEST_TIME_OUT, REQUEST_FAILED_PARAMETER, REQUEST_FAILED_APPLICATION, REQUEST_FAILED_UNPACKING} ENUM8(RequestStatus_t);


/**
 * Return type for Event functions. Event functions are used to unpack signal
 * data from event signals. Event signals are those used to signal Event
 * Channel subscriber (when an event occurred) or those used to signal a client
 * process when a service with event has been requested (using the
 * Request-Response-Event design pattern).
 *
 * @param GS_EVENT_OK Used to indicate that the event data was successfully retrieved.
 * @param GS_EVENT_FAILED_UNPACKING Used to indicate that something went wrong when trying to unpack the event.
 * @param GS_EVENT_FAILED_APPLICATION Used to indicate that the application failed.
 * @param GS_EVENT_FAILED_PARAMETER Used to indicate that one/several of the parameters are incorrect.
 * @param GS_EVENT_NOT_AVAILABLE Used to indicate that the event unpack function is not available.
 */
TYPEDEF_ENUM {
GS_EVENT_OK, GS_EVENT_FAILED_UNPACKING, GS_EVENT_FAILED_APPLICATION, GS_EVENT_FAILED_PARAMETER, GS_EVENT_NOT_AVAILABLE} ENUM8(EventStatus_t);


/**
 * Struct containing signal information.
 *
 * @param Primitive The signal identity.
 * @param ClientTag The Client tag value.
 * @param ClientHandlers_p Pointer to client callback specification.
 */
typedef struct {
    SIGSELECT Primitive;
    ClientTag_t ClientTag;
    const void *ClientHandlers_p;
} GS_SignalBlock_t;


/**
 * Struct containing signal information.
 *
 * @param ClientProcess The process of the client sending the signal.
 * @param Primitive The signal identity.
 * @param ClientTag The Client tag value.
 * @param ClientHandlers_p Pointer to client callback specification.
 */
typedef struct {
    PROCESS ClientProcess;
    SIGSELECT Primitive;
    ClientTag_t ClientTag;
    const void *ClientHandlers_p;
} GS_ClientInfoBlock_t;


/**
* Holds the version for a category.
*/
typedef uint16 GS_CategoryVersion_t;

extern const RequestControlBlock_t REQUEST_CONTROL_BLOCK_WAIT_MODE;
extern const RequestControlBlock_t REQUEST_CONTROL_BLOCK_NO_WAIT_MODE;

/**
 * This is a constant pointer to a RequestControlBlock_t constant. Its request
 * mode is set to NO_WAIT_MODE, its ClientTag is set to CLIENT_TAG_NOT_USED and
 * ClientHandlers_p is set to NULL. This is used in the normal case when the 
 * client is satisfied with default values in the control block.
 *
 * Example:
 * Request_<Object>_<Operation>(REQUEST_CONTROL_BLOCK_NO_WAIT_MODE_p,
 *                              AnyClientInParmeter, 
 *                              AnyClientOutParameter_p);
 */

extern const struct RequestControlBlock_t *const REQUEST_CONTROL_BLOCK_NO_WAIT_MODE_p;

/**
 * This is constant pointer to a RequestControlBlock_t constant. Its request
 * mode is set to WAIT_MODE, its ClientTag is set to CLIENT_TAG_NOT_USED and 
 * ClientHandlers_p is set to NULL. This is used in the normal case when the 
 * client are satisfied with default values in the control block.
 *
 * Example:
 * Request_<Object>_<Operation>(REQUEST_CONTROL_BLOCK_WAIT_MODE_p,
 *                              AnyClientInParmeter, 
 *                              AnyClientOutParameter_p);
 */
extern const struct RequestControlBlock_t *const REQUEST_CONTROL_BLOCK_WAIT_MODE_p;

/* Alternative names for REQUEST_CONTROL_BLOCK_NO_WAIT_MODE_p and REQUEST_CONTROL_BLOCK_WAIT_MODE_p. */
extern const struct RequestControlBlock_t *const DONT_WAIT;     /* Deprecated */
extern const struct RequestControlBlock_t *const WAIT_RESPONSE; /* Deprecated */

#define REQUEST_CONTROL_BLOCK_INIT_2A(RequestControlBlock, WaitModeIn, ClientTagIn) \
do  \
{   \
  RequestControlBlock.WaitMode         = WaitModeIn;      \
  RequestControlBlock.ClientTag        = ClientTagIn;     \
  RequestControlBlock.ClientHandlers_p = NULL;            \
} while (0)

#define REQUEST_CONTROL_BLOCK_INIT_2B(RequestControlBlock, WaitModeIn, ClientHandlersIn_p) \
do  \
{   \
  RequestControlBlock.WaitMode         = WaitModeIn;          \
  RequestControlBlock.ClientTag        = CLIENT_TAG_NOT_USED; \
  RequestControlBlock.ClientHandlers_p = ClientHandlersIn_p;  \
} while (0)

#define REQUEST_CONTROL_BLOCK_INIT_3(RequestControlBlock, WaitModeIn, ClientTagIn, ClientHandlersIn_p) \
do  \
{   \
  RequestControlBlock.WaitMode         = WaitModeIn;        \
  RequestControlBlock.ClientTag        = ClientTagIn;       \
  RequestControlBlock.ClientHandlers_p = ClientHandlersIn_p;\
} while (0)

typedef void (
    *GS_Handle_t) (
    const union SIGNAL ** const Signal_pp);

#define DEFINE_GS_HANDLE(Handle) void Handle(const union SIGNAL** const Signal_pp)

#endif                          /* INCLUSION_GUARD_T_SYS_H */
