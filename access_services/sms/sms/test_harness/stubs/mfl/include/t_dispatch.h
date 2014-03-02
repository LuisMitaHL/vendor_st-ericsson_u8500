/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : t_dispatch.h 
 * Description     : Header file containing type definitions required for 
 *                   the Event Channel request function
 * 
 * 
 */
#ifndef INCLUSION_GUARD_T_DISPATCH_H
#define INCLUSION_GUARD_T_DISPATCH_H

/*************************************************************************
* Includes
*************************************************************************/

#include "t_basicdefinitions.h"
#include "t_catdefinitions.h"

/** The definition of the Dispatch category id. */
#define CAT_ID_DISPATCH 3
CAT_INTERVAL_DEFINITION(CAT_ID_DISPATCH)

/*************************************************************************
* Types, constants and external variables
*************************************************************************/
/**
 * Used to specify whether Request_EventChannel_Subscription() is called in order to
 * subscribe or to unsubscribe from an event channel.
 *
 * @param EVENT_SUBSCRIBE Subscribing when using legacy subscription function.
 * @param EVENT_UNSUBSCRIBE Unsubscribing when using legacy subscription function. 
 */
    TYPEDEF_ENUM
{
EVENT_SUBSCRIBE, EVENT_UNSUBSCRIBE}

ENUM8(Event_SubscriptionInfo_t);

/**
 * Used when activating and deactivating the dispatcher process.
 *
 * @param DISPATCHER_REACTIVATE Activating the dispatcher process.
 * @param DISPATCHER_DEACTIVATE Deactivating the dispatcher process.
 */
TYPEDEF_ENUM {
DISPATCHER_REACTIVATE, DISPATCHER_DEACTIVATE}

ENUM8(DispatcherState_t);


/**
 * Used when activating and deactivating the event subscriptions for a specific
 * event subscriber.
 *
 * @param EVENTSUBSCRIPTION_REACTIVATE Activating the event subscriptions for a specific event subscriber.
 * @param EVENTSUBSCRIPTION_DEACTIVATE Deactivating the event subscriptions for a specific event subscriber. 
 *           
 */
TYPEDEF_ENUM {
EVENTSUBSCRIPTION_REACTIVATE, EVENTSUBSCRIPTION_DEACTIVATE}

ENUM8(EventSubscriptionState_t);


/**
 * The type for an Event Id.
 */
typedef uint32 EventId_t;

/**
 * Support for backwards compatibility of Event Id type.
 */
typedef EventId_t Event_Id_t;

/**
 * This struct is used by Process Control module to subscribe to 
 * several event/service name pairs.
 * 
 * @param EventId The identification of the event to which 
 *                the client wants to subscribe to.
 * 
 * @param ServiceName_p Is a zero-terminated ASCII string containing
 *                      the name to the service to which 
 *                      the client wants to subscribe to.
 */
typedef struct {
    EventId_t EventId;
    char *ServiceName_p;
} Event_ServiceSubscription_t;

#endif                          // INCLUSION_GUARD_T_DISPATCH_H
