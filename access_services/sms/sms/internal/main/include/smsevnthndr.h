#ifndef EVNTHNDR_H
#define EVNTHNDR_H
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
 * SMS Server internal event handler include file.
 *
 *************************************************************************/
#include <stdint.h>

#include "t_sms.h"

#include "sm.h"


//Special values used by Event Tables
#define END_OF_TABLE 0
#define END_OF_TABLE_FUNCTION EventHandler_IgnoreEvent
#define DELIBERATE_IGNORE EventHandler_DeliberateIgnore

#define ALL_STATES SMS_MAX_UINT8

typedef uint8_t State_t;
typedef uint8_t Event_t;


/*
 * TYPEDEF
 */
typedef struct {
    union SMS_SIGNAL *DataPointer;
    ShortMessage_p ShortMessage;
    SMS_RP_ErrorCause_t RMS_V2_ErrorCode;
    CallbackFunction_p CallbackFunction;
    SMS_Error_t ErrorCode;
} EventData_t;

typedef EventData_t *EventData_p;

typedef void (
    *EventHandlerFunction_p) (
    void *,
    EventData_p);

typedef const struct {
    Event_t Event;
    State_t State;
    EventHandlerFunction_p Function;
} EventAction_t;

typedef EventAction_t *EventAction_p;
typedef EventAction_t *EventTable_p;

/*
 * FUNCTION PROTOTYPES
 */
EventHandlerFunction_p EventHandler_Find(
    void *,
    EventTable_p,
    Event_t,
    State_t);
void EventHandler_IgnoreEvent(
    void *,
    EventData_p);
void EventHandler_DeliberateIgnore(
    void *,
    EventData_t *);

#endif                          // EVNTHNDR_H
