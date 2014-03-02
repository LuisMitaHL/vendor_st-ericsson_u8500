#ifndef INCLUSION_GUARD_R_DISPATCH_H
#define INCLUSION_GUARD_R_DISPATCH_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* DESCRIPTION:
* 
* Header file containing the prototypes for the Event Channel registration
* request/response functions and the macros that can be used when writing
* a dispatch function.
*
*************************************************************************/


/*************************************************************************
* Includes
*************************************************************************/

#include <stdlib.h>

#include "r_sys.h"
#include "t_dispatch.h"
//#include "r_textcon.h"
#include "t_catdefinitions.h"
#include "string.h"
#include "t_os.h"

/*************************************************************************
* Declaration of signal identities
*************************************************************************/

/** 
 * Signal identities for the Request function response signals.
 * Use offsets 0-999 for RESPONSE/R_RESP signals
 */
SIGID_DEFINITION_V3(CAT_ID_DISPATCH, RESPONSE_EVENTCHANNEL_SUBSCRIBE, 0, Response_EventChannel_Subscribe_t)
    SIGID_DEFINITION_V3(CAT_ID_DISPATCH, R_RESP_DISPATCHERSTATE_SET, 1, Response_DispatcherState_Set_t)
    SIGID_DEFINITION_V3(CAT_ID_DISPATCH, RESPONSE_EVENTCHANNEL_SUBSCRIBE_V2, 2, Response_EventChannel_Subscribe_V2_t)
    SIGID_DEFINITION_V3(CAT_ID_DISPATCH, RESPONSE_EVENTCHANNEL_UNSUBSCRIBE_V2, 3, Response_EventChannel_Unsubscribe_V2_t)
    SIGID_DEFINITION_V3(CAT_ID_DISPATCH, RESPONSE_EVENTCHANNEL_STOPSERVICE, 4, Response_EventChannel_StopService_t)
    SIGID_DEFINITION_V3(CAT_ID_DISPATCH, R_RESP_EVENTCHANNEL_SUBSCRIBELIST, 5, Response_EventChannel_SubscribeList_t)

/**
 * Signal identity for the Do function response signals
 * Use offsets 1000-1999 for DONE/R_DONE signals
 */
    SIGID_DEFINITION_V3(CAT_ID_DISPATCH, DONE_DISPATCH, 1000, Done_Dispatch_t)


/*************************************************************************
* Declaration of functions 
**************************************************************************/
/**
 * This request function is used by Event Subscribers to subscribe to and 
 * unsubscribe from events. After subscribing to an event, the module must be 
 * prepared to handle the associated event signal (signal name has the form 
 * EVENT_<NOTIFICATION>). 
 *
 * Events are reported by Event Detector modules through the use of the 
 * dispatch mechanism. When an event is detected, an event signal is sent to 
 * all modules that are currently registered as subscriber to the event. Refer 
 * to chapter 10 for an example of how this function can be used to subscribe 
 * to and unsubscribe from events.
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the 
 *                                    request is called in wait mode or no wait 
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] EventSubscriptionInfo Indicates whether the request is used to 
 *                                    subscribe to an event or unsubscribe from 
 *                                    an already subscribed event.
 * @param [in] EventId               The identification of the event to which 
 *                                    the client wants to subscribe to or to 
 *                                    unsubscribe from (EVENT_ID_<NOTIFICATION>.
 *
 * @retval REQUEST_PENDING            In no wait mode to indicate the request 
 *                                    has been sent to the server
 * @retval REQUEST_FAILED_UNPACKING   Wrong signal id in the response signal
 * @retval REQUEST_FAILED_APPLICATION When trying to subscribe to an already 
 *                                    subscribed event (i.e. same process 
 *                                    identification and same tag information) 
 *                                    or when trying to unsubscribe from an 
 *                                    event to which the user has not 
 *                                    previously subscribed to.
 * @retval REQUEST_FAILED_PARAMETER   A non existing event id or a non 
 *                                    existing EventSubscriptionInfo (refer to 
 *                                    the definition of 
 *                                    Event_SubscriptionInfo_t and EventId_t 
 *                                    for allowed values).
 * @retval REQUEST_OK                 Service has been performed successfully
 *
 * @signalid RESPONSE_EVENTCHANNEL_SUBSCRIBE
 * @waitmode Wait mode and No wait mode supported
 * @clienttag Yes
 * @related None
 */
RequestStatus_t Request_EventChannel_Subscribe(
    const RequestControlBlock_t * const RequestCtrl_p,
    const Event_SubscriptionInfo_t EventSubscriptionInfo,
    const EventId_t EventId);


/**
 * Response function for Request_EventChannel_Subscribe
 *
 */

RequestStatus_t Response_EventChannel_Subscribe(
    const void *const SigStruct_p);

/**
 * This request function is used by Event Subscribers to subscribe to 
 * events using a service name. The reason for using a service name instead
 * of process id's is that you can be able to start and terminate a process
 * several times. The same service name will remain the same but the pid will
 * change each time a process restarted. After subscribing to an event, 
 * the module must be prepared to handle the associated event signal. 
 * The Signal name has the form EVENT_<NOTIFICATION>. 
 *
 * Events are reported by Event Detector modules through the use of the 
 * dispatch mechanism. When an event is detected, an event signal is sent to 
 * all modules that are currently registered as subscriber to the event. 
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the 
 *                                    request is called in wait mode or no wait 
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] EventId                The identification of the event to which 
 *                                    the client wants to subscribe to 
 *                                    EVENT_ID_<NOTIFICATION>.
 * @param [in] ServiceName_p          Is a zero-terminated ASCII string containing
 *                                    the name to the service to which 
 *                                    the client wants to subscribe to.
 * @retval REQUEST_PENDING            In no wait mode to indicate the request 
 *                                    has been sent to the server
 * @retval REQUEST_FAILED_UNPACKING   Wrong signal id in the response signal
 * @retval REQUEST_FAILED_APPLICATION When trying to subscribe to an already 
 *                                    subscribed event (i.e. same process 
 *                                    identification and same tag information) 
 *                                    or when trying to unsubscribe from an 
 *                                    event to which the user has not 
 *                                    previously subscribed to.
 * @retval REQUEST_FAILED_RESOURCE    Indication that there are not enough 
 *                                    resources to perform the service.
 * @retval REQUEST_FAILED_PARAMETER   A non existing event id or a non 
 *                                    existing EventSubscriptionInfo (refer to 
 *                                    the definition of 
 *                                    Event_SubscriptionInfo_t and EventId_t 
 *                                    for allowed values).
 * @retval REQUEST_OK                 Service has been performed successfully
 *
 * @signalid RESPONSE_EVENTCHANNEL_SUBSCRIBE_V2
 * @waitmode Wait mode and No wait mode supported
 * @clienttag Yes
 * @related None
 */
RequestStatus_t Request_EventChannel_Subscribe_V2(
    const RequestControlBlock_t * const RequestCtrl_p,
    const EventId_t EventId,
    const char *const ServiceName_p);

/**
 * Response function for Request_EventChannel_Subscribe_V2
 *
 */

RequestStatus_t Response_EventChannel_Subscribe_V2(
    const void *const SigStruct_p);



/**
 * This request function is used by Event Subscribers to unsubscribe to 
 * events using a service name. 
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the 
 *                                    request is called in wait mode or no wait 
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] EventId                The identification of the event to which 
 *                                    the client wants to unsubscribe from 
 *                                    (EVENT_ID_<NOTIFICATION>).
 * @param [in] ServiceName_p          Is a zero-terminated ASCII string containing
 *                                    the name to the service to which 
 *                                    the client wants to unsubscribe to.
 * @retval REQUEST_PENDING            In no wait mode to indicate the request 
 *                                    has been sent to the server
 * @retval REQUEST_FAILED_UNPACKING   Wrong signal id in the response signal
 * @retval REQUEST_FAILED_APPLICATION When trying to subscribe to an already 
 *                                    subscribed event (i.e. same process 
 *                                    identification and same tag information) 
 *                                    or when trying to unsubscribe from an 
 *                                    event to which the user has not 
 *                                    previously subscribed to.
 * @retval REQUEST_FAILED_RESOURCE    Indication that there are not enough 
 *                                    resources to perform the service.
 * @retval REQUEST_FAILED_PARAMETER   A non existing event id or a non 
 *                                    existing EventSubscriptionInfo (refer to 
 *                                    the definition of 
 *                                    Event_SubscriptionInfo_t and EventId_t 
 *                                    for allowed values).
 * @retval REQUEST_OK                 Service has been performed successfully
 *
 * @signalid RESPONSE_EVENTCHANNEL_UNSUBSCRIBE_V2
 * @waitmode Wait mode and No wait mode supported
 * @clienttag Yes
 * @related None
 */
RequestStatus_t Request_EventChannel_Unsubscribe_V2(
    const RequestControlBlock_t * const RequestCtrl_p,
    const EventId_t EventId,
    const char *const ServiceName_p);

/**
 * Response function for Request_EventChannel_Unsubscribe_V2
 *
 */

RequestStatus_t Response_EventChannel_Unsubscribe_V2(
    const void *const SigStruct_p);


/**
 * This request function is used by Process Control module to subscribe to a list of events, 
 * using service names as event subscribers. The R_Req_EventChannel_SubscribeList 
 * is a shortcut for multiple Request_EventChannel_Subscribe_V2 requests.
 *
 * Events are reported by Event Detector modules through the use of the 
 * dispatch mechanism. When an event is detected, an event signal is sent to 
 * all modules that are currently registered as subscriber to the event. 
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the 
 *                                    request is called in wait mode or no wait 
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] SubscriberTable_p      List of pairs containing service names
 *                                    and events on which to subscribe to.
                                      The Pointer points to the first element in the list.
                                      The length of the list should be stated in the 
                                      NoOfSubscriptions
                                      parameter.
 * @param [in] NoOfSubscriptions      The name to the service to which 
 *                                    the client wants to subscribe to.
 * @retval REQUEST_PENDING            In no wait mode to indicate the request 
 *                                    has been sent to the server
 * @retval REQUEST_FAILED_UNPACKING   Wrong signal id in the response signal
 * @retval REQUEST_FAILED_APPLICATION When trying to subscribe to an already 
 *                                    subscribed event (i.e. same process 
 *                                    identification and same tag information) 
 *                                    or when trying to unsubscribe from an 
 *                                    event to which the user has not 
 *                                    previously subscribed to.
 * @retval REQUEST_FAILED_RESOURCE    Indication that there are not enough 
 *                                    resources to perform the service.
 * @retval REQUEST_FAILED_PARAMETER   A non existing event id or a non 
 *                                    existing EventSubscriptionInfo (refer to 
 *                                    the definition of 
 *                                    Event_SubscriptionInfo_t and EventId_t 
 *                                    for allowed values).
 * @retval REQUEST_OK                 Service has been performed successfully
 *
 * @signalid R_RESP_EVENTCHANNEL_SUBSCRIBELIST
 * @waitmode Wait mode and No wait mode supported
 * @clienttag Yes
 * @reserved Process Control module 
 * @related None
 */
RequestStatus_t R_Req_EventChannel_SubscribeList(
    const RequestControlBlock_t * const RequestCtrl_p,
    const Event_ServiceSubscription_t * const SubscriberTable_p,
    const uint16 NoOfSubscriptions);

/**
 * Response function for R_Req_EventChannel_SubscribeList
 *
 */
RequestStatus_t R_Resp_EventChannel_SubscribeList(
    const void *const SigStruct_p);



/**
 * This request function is used to avoid race conditions when events are being 
 * dispatched to a process that is about to be terminated. This is to ensure that
 * the dispatcher has cleaned its tables and the process has completely terminated in 
 * case the event triggers again. If the scenario described here occurs a new process
 * will be created handling the event, this will take place after the StopService 
 * has finished executing. This function should be called in WAIT_MODE only and
 * will not function properly otherwise, the reason for this is that we want to avoid
 * that the process has terminated before the tables have been updated. 
 *
 * @param [in] RequestCtrl_p          Pointer to struct controlling whether the 
 *                                    request is called in wait mode or no wait 
 *                                    mode.
 * @param [in] SigStruct_p            Response signal to be unpacked.
 * @param [in] ServiceName_p          Is a zero-terminated ASCII string containing
 *                                    the name to the service to which 
 *                                    the client to terminate.
 * @retval REQUEST_PENDING            In no wait mode to indicate the request 
 *                                    has been sent to the server
 * @retval REQUEST_FAILED_UNPACKING   Wrong signal id in the response signal
 * @retval REQUEST_FAILED_APPLICATION When trying to subscribe to an already 
 *                                    subscribed event (i.e. same process 
 *                                    identification and same tag information) 
 *                                    or when trying to unsubscribe from an 
 *                                    event to which the user has not 
 *                                    previously subscribed to.
 * @retval REQUEST_FAILED_RESOURCE    Indication that there are not enough 
 *                                    resources to perform the service.
 * @retval REQUEST_FAILED_PARAMETER   A non existing event id or a non 
 *                                    existing EventSubscriptionInfo (refer to 
 *                                    the definition of 
 *                                    Event_SubscriptionInfo_t and EventId_t 
 *                                    for allowed values).
 * @retval REQUEST_OK                 Service has been performed successfully
 *
 * @signalid RESPONSE_EVENTCHANNEL_STOPSERVICE
 * @waitmode Wait mode (see explanation in function description)
 * @clienttag Yes
 * @related None
 */
RequestStatus_t Request_EventChannel_StopService(
    const RequestControlBlock_t * const RequestCtrl_p,
    const char *const ServiceName_p);

 /**
 * Response function for Request_EventChannel_StopService
 *
 */

RequestStatus_t Response_EventChannel_StopService(
    const void *const SigStruct_p);

/**
 * This request function is used when configuring the system for test. The use 
 * case that has been identified is when the modules of the Hardware 
 * Abstraction Layer are tested. In this scenario, it is desirable to have all 
 * the modules above the HAL disabled and therefore events generated by the HAL 
 * should not be dispatched above the HAL. 
 *
 * A request to activate an already activated dispatcher will not have any 
 * effect and will return successfully.
 *
 * A request to de-activate an already de-activated dispatcher will not have 
 * any effect and will return successfully.
 *
 * The dispatcher is in ACTIVE state after having been started. The state of 
 * the dispatcher is not saved in flash when the system is powered off.
 *
 * @param [in] RequestCtrl_p   Pointer to struct controlling whether the 
 *                             request is called in wait mode or no wait mode.
 * @param [in] SigStruct_p     Response signal to be unpacked.
 * @param [in] DispatcherState Indicates whether the request is used to 
 *                             activate or deactivate the dispatcher.
 *
 * @retval REQUEST_PENDING          In no wait mode to indicate the request 
 *                                  has been sent to the server
 * @retval REQUEST_FAILED_UNPACKING Wrong signal id in the response signal
 * @retval REQUEST_FAILED_PARAMETER A non existing dispatcher state (refer to 
 *                                  the definition of  DispatcherState_t for 
 *                                  allowed values).
 * @retval REQUEST_OK               Service has been performed successfully
 *
 * @signalid R_RESP_DISPATCHERSTATE_SET
 * @waitmode Wait mode and No wait mode supported
 * @clienttag Yes
 * @reserved Only used by modules during test
 * @related None
 */

RequestStatus_t R_Req_DispatcherState_Set(
    const RequestControlBlock_t * const RequestCtrl_p,
    const DispatcherState_t DispatcherState);


/**
 * Response function for R_Req_DispatcherState_Set
 *
 */

RequestStatus_t R_Resp_DispatcherState_Set(
    const void *const SigStruct_p);

/**
 * Do_Dispatch() is used by the event detector to report an event (identified 
 * by EVENT_ID) to its subscriber(s). 
 *
 * Events are dispatched to subscribers having done the subscriptions both 
 * using Request_EventChannel_Subscribe_V2 and Request_EventChannel_Subscribe.
 * New subscribers should use Request_EventChannel_Subscribe_V2.
 * 
 * When configured to be signal based, Do_Dispatch() sends a signal (signal 
 * name is DO_DISPATCH) to the dispatcher process. The dispatcher will route 
 * the corresponding event signal to the event subscriber(s). Do_Dispatch() 
 * will always execute in Wait Mode (i.e. once Do_Dispatch() returns, the event 
 * signals have been sent to all the subscribers).
 *
 * If Confirm is TRUE, the function will returns the number of subscribers 
 * currently registered for EVENT_ID. 
 *
 * Note that using Confirm set to TRUE is an exception rather than the rule. 
 * Event Channels normally don't require confirmation from subscribers.
 *
 * It is also important to notice that the event subscribers must determine 
 * themselves whether an event requires confirmation or not. This information 
 * is not part of the event signal sent to the event subscribers. Event 
 * subscribers must refer to the Reference Manual for the category providing 
 * the event channel in question to determine if it requires confirmation or 
 * not.
 *
 * Note also that macros have been defined to help writing event dispatching 
 * functions. When these macros are used, Do_Dispatch() does not have to be 
 * called directly.
 *
 * @param [in] EVENT_ID  The identification of the event that has been 
 *                       detected.
 * @param [in] Confirm   TRUE indicates that the caller wants to know the 
 *                       number of subscribers which the event was sent to. 
 *                       FALSE indicates that the number of subscribers to the 
 *                       event is not required.
 * @param [in] AllocSize Size of the of the event signal in bytes.
 * @param [in] Data_p    Pointer to the event signal data buffer.
 *
 * @return When called with Confirm equal to TRUE, the function returns the
 *         number of subscribers for EventID (a value between 0 and
 *         SIZE_OF_PROCESSLIST). Otherwise, the function returns always 0.
 *
 * @sigbased Yes
 * @signalid DONE_DISPATCH
 * @waitmode Wait mode
 * @related None
 */

uint8 Do_Dispatch(
    const EventId_t EVENT_ID,
    const boolean Confirm,
    const uint16 AllocSize,
    const union SIGNAL *Data_p);



/*************************************************************************
 *  Macros 
 **************************************************************************/

/* Usage of macros defined below
 
     // (No code here, but declarations allowed)
     INIT_DISPATCH(SignalDataType, SignalIdName, EVENT_ID)
    
    // (Code may be inserted here)

    SET_DATA_STRUCTURE = *pointer to data;  OR
    SET_DATA_ELEMENT(Element in Data Type structure) = value; OR

    SET_DATA_ARRAY(Pointer to Array Element in Data Type structure ,
                       Pointer to Source Array, No of bytes); OR
    SET_TEXT_UNIVERSAL(Pointer to Array Element in Data Type structure where UTTF should be stored,
                       Pointer to Source Array in Universal Text format,
                       Maximum number of bytes allowed for the resulting UTTF string, including header);

    // (Code may be inserted here)

    DISPATCH The signal is sent to the processes in the 'ProcessList[EVENT_ID]]' 
    defined in c_dispatch.c and the processes the dynamically registered to EVENT_ID

    // (Code may be inserted here)
    ----------------------------------------------------------
*/

/**
 * This macro is used to declare and initialize the variables that are
 * required prior to dispatching an event. INIT_DISPATCH is to be used for all
 * events that do not require confirmation from the subscribers.
 * This macro is used in the Dispatch_<Notification>() function in the Event
 * Detector module.
 *
 * @param a Name of the signal structure type definition for the signal to be
 *          dispatched (e.g. Event_<Notification>_t).
 * @param b Name of the signal ID for the signal to be dispatched (e.g.
 *          EVENT_<NOTIFICATION>)
 * @param c Identification of the event to be sent. This is defined in
 *          EventId_t. (e.g. EVENT_ID_<NOTIFICATION>)
 *
 * @return void
 */
#define INIT_DISPATCH(a,b,c) INIT_DISPATCH_VAR_SIZE(a,b,c,sizeof(a))

/**
 * This macro is used to declare and initialize the variables that are
 * required prior to dispatching an event. INIT_DISPATCH_VAR_SIZE is to be used
 * for all events that do not require confirmation from the subscribers but where
 * it should be possible to specify the signal size.
 * This macro is used in the Dispatch_<Notification>() function in the Event
 * Detector module.
 *
 * @param SignalDataType Name of the signal structure type definition for the
 *                       signal to be dispatched (e.g. Event_<Notification>_t).
 * @param SignalIdName   Name of the signal ID for the signal to be dispatched
 *                       (e.g. EVENT_<NOTIFICATION>).
 * @param EVENT_ID       Identification of the event to be sent. This is
 *                       defined in EventId_t. (e.g. EVENT_ID_<NOTIFICATION>)
 * @param SignalSize     The size of the signal data type.
 *
 * @return void
 */
#define INIT_DISPATCH_VAR_SIZE(SignalDataType,SignalIdName,EVENT_ID, SignalSize)    \
                                                                                    \
  static const boolean __Confirm = FALSE;                                           \
  uint16 __Size = SignalSize;                                                       \
  static const SIGSELECT __Sig = SignalIdName;                                      \
  static const EventId_t __EVENT_ID = EVENT_ID;                                    \
  SignalDataType* __p = (SignalDataType *)malloc(__Size + sizeof(SIGSELECT));

/**
 * This is a macro used to set the entire data structure of the signal to be
 * dispatched. It is used by the Event Detector after having expanded
 * INIT_DISPATCH or INIT_DISPATCH_WITH_ CONFIRM.
 */
#define SET_DATA_STRUCTURE  (*__p)

/**
 * This is a macro used to set one element of data structure of the signal to
 * be dispatched. It is used by the Event Detector after having expanded
 * INIT_DISPATCH or INIT_DISPATCH_WITH_ CONFIRM.
 *
 * @param el Name of the element whose value will be set. Element is an element in
 *           the structure of the signal to be dispatched.
 *
 * @return void
 */
#define SET_DATA_ELEMENT(el)  (__p->el)

/**
 * This is a macro used to set an entire data structure in the signal to be
 * dispatched. It is used by the Event Detector after having expanded
 * INIT_DISPATCH or INIT_DISPATCH_WITH_ CONFIRM. 
 * 
 * @param El_ptr    Name of an array within the structure of the signal to be
 *                  dispatched.
 * @param Src_ptr   Pointer to the data to be copied in the signal structure
 *                  pointed to by Array_p.
 * @param NoOfBytes Number of bytes to be copied.
 *
 * @return void
 */
#define SET_DATA_ARRAY(El_ptr, Src_ptr, NoOfBytes) memcpy(__p->El_ptr, Src_ptr, NoOfBytes)

/**
 * This is a macro used to convert a Universal Text (UT) string to Universal
 * Text Transport Format (UTTF) string. The UTTF must be used when sending
 * universal text in the event signal. It is used by the Event Detector after
 * having expanded INIT_DISPATCH or INIT_DISPATCH_WITH_ CONFIRM. 
 *
 * @param El_ptr            Pointer to an address within the structure of the
 *                          signal to be dispatched where the UTTF string will
 *                          be written.
 * @param Src_ptr           Pointer to the UT string to be converted. 
 * @param MaxNoOfUTTF_Bytes Maximum number of UTTF bytes that can be written in
 *                          the signal structure.
 *
 * @return void
 */
#define SET_TEXT_UNIVERSAL(El_ptr, Src_ptr, MaxNoOfUTTF_Bytes)  \
  (void) Util_CopyUT_ToUTTF((UniversalText_t *) Src_ptr,        \
                 (UniversalTextTransportFormat_t *)__p->El_ptr,  \
                  MaxNoOfUTTF_Bytes)

/**
 * This macro is used to send the event signal specified in the
 * INIT_DISPATCH (or INIT_DISPATCH_WITH_CONFIRM). It is used by the Event
 * Detector after having expanded INIT_DISPATCH or INIT_DISPATCH_WITH_CONFIRM
 * and after having set the event signal data, if necessary, using the SET_aaa
 * macros described above. Note that DISPATCH must be the last
 * line of code in the Dispatch_Function().
 */
/* #define DISPATCH                                                          \
                                                                          \
  __p->SigselectWithClientTag.Primitive = __Sig;                          \
                                                                          \
  (void) Do_Dispatch(__EVENT_ID,                                          \
                     __Confirm,                                           \
                     __Size,                                              \
                    (const union SIGNAL *)__p);
*/
/* MFL specific It is not allowed to use these macros */
#define DISPATCH                                                          \
             {char *foo = NULL; *foo = 1;}



/* The macros below are used when the number of subscribers 
 * is needed by the event detector 
 * The macros are used in the same way as the corresponding
 * macros above with one exception: no application code can be
 * put after DISPATCH_WITH_CONFIRM has been expanded since the macro
 * expands a return statement. 
 */

/**
 * This macro is used to declare and initialize the variables that are
 * required prior to dispatching an event. INIT_DISPATCH_WITH_CONFIRM must 
 * be used when the Event Detector wants to obtain the number of subscribers
 * registered for the event (i.e. the number of subscribers that have been notified
 * of the event by the dispatcher).
 * This macro is used in the Dispatch_<Notification>() function in the Event
 * Detector module.
 *
 * @param a Name of the signal structure type definition for the signal to be
 *          dispatched (e.g. Event_<Notification>_t).
 * @param b Name of the signal ID for the signal to be dispatched (e.g.
 *          EVENT_<NOTIFICATION>)
 * @param c Identification of the event to be sent. This is defined in
 *          EventId_t. (e.g. EVENT_ID_<NOTIFICATION>).
 *
 * @return void
 */
#define INIT_DISPATCH_WITH_CONFIRM(a,b,c)\
        INIT_DISPATCH_WITH_CONFIRM_VAR_SIZE(a,b,c,sizeof(a))

/**
 * This macro is used to declare and initialize the variables that are
 * required prior to dispatching an event. INIT_DISPATCH_WITH_CONFIRM_VAR_SIZE
 *  must be used when the Event Detector wants to obtain the number of subscribers 
 * registered for the event (i.e. the number of subscribers that have been notified 
 * of the event by the dispatcher) and also wants to specify the signal size.
 * This macro is used in the Dispatch_<Notification>() function in the Event
 * Detector module.
 *
 * @param SignalDataType Name of the signal structure type definition for the
 *                       signal to be dispatched (e.g. Event_<Notification>_t).
 * @param SignalIdName   Name of the signal ID for the signal to be dispatched
 *                       (e.g. EVENT_<NOTIFICATION>)
 * @param EVENT_ID       Identification of the event to be sent. This is
 *                       defined in EventId_t. (e.g. EVENT_ID_<NOTIFICATION>).
 * @param SignalSize    The size of the signal data type that is to dispatched.
 *
 * @return void
 */
#define INIT_DISPATCH_WITH_CONFIRM_VAR_SIZE(SignalDataType,           \
                                           SignalIdName,              \
                                           EVENT_ID,                  \
                                           SignalSize)                \
  static const boolean __Confirm = TRUE;                              \
  uint8 __NoOfProcesses = 0;                                          \
  uint16 __Size = SignalSize;                                         \
  static const SIGSELECT __Sig = SignalIdName;                        \
  static const EventId_t __EVENT_ID = EVENT_ID;                       \
  SignalDataType* __p = (SignalDataType *)malloc(__Size + sizeof(SIGSELECT));

/**
 * These macros are used to send the event signal specified in the
 * INIT_DISPATCH (or INIT_DISPATCH_WITH_CONFIRM). It is used by the Event
 * Detector after having expanded INIT_DISPATCH or INIT_DISPATCH_WITH_ CONFIRM
 * and after having set the event signal data, if necessary, using the SET_aaa
 * macros described above. Note that DISPATCH_WITH_CONFIRM must be the last
 * line of code in the Dispatch_Function(). The macro returns the number of
 * Event Subscribers currently registered to the event id specified in
 * INIT_DISPATCH_WITH_CONFIRM. 
 */
/*
#define DISPATCH_WITH_CONFIRM                                         \
                                                                      \
  __p->SigselectWithClientTag.Primitive = __Sig;                      \
                                                                      \
  __NoOfProcesses = Do_Dispatch(__EVENT_ID,                           \
                                __Confirm,                            \
                                __Size,                               \
                                (const union SIGNAL *)__p);           \
                                                                      \
  return (__NoOfProcesses);
*/
/* MFL specific It is not allowed to use these macros */
#define DISPATCH_WITH_CONFIRM                                                          \
             {char *foo = NULL; *foo = 1;}

#endif                          // INCLUSION_GUARD_R_DISPATCH_H
