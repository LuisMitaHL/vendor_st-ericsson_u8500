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
 * Implementation of the SMS Server internal event handler.
 *
 *************************************************************************/

/* Utility Include Files */
#include <stdio.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"


#include "smsevnthndr.h"


/********************************************************************/
/**
 *
 * @function     EventHandler_Find
 *
 * @description  Function to find an event handler for the event in the eventTable
 *
 * @param        *object :      void type.
 * @param        eventTable :   as type of EventTable_p.
 * @param        event :        as type of Event_t.
 * @param        currentState : as type of State_t.
 *
 * @return       EventHandlerFunction_p
 */
/********************************************************************/
EventHandlerFunction_p EventHandler_Find(
    void *object,
    EventTable_p eventTable,
    Event_t event,
    State_t currentState)
{
    EventAction_p eventTablePtr;
    uint8_t matchFound = FALSE;
    EventHandlerFunction_p eventHandlerFunction = EventHandler_IgnoreEvent;

    SMS_IDENTIFIER_NOT_USED(object);
    eventTablePtr = eventTable;

    //until we reach end of Event Handler table or we find a match
    while (eventTablePtr->Function != END_OF_TABLE_FUNCTION && matchFound == FALSE) {
        //if the event matches and
        if (event == eventTablePtr->Event &&
            //the state matched or
            (currentState == eventTablePtr->State ||
             //this is an any state event
             eventTablePtr->State == ALL_STATES)) {
            matchFound = TRUE;
        }
        //that entry was not a match
        else {
            //move to next entry in Event Handler table
            eventTablePtr++;
        }
    }
    if (matchFound) {
        //Call the appropriate Event Handler Function
        eventHandlerFunction = eventTablePtr->Function;
    } else {
        //Match wasn't found in table, print out something appropriate
        SMS_A_(SMS_LOG_W("evnthndr.c: Event handler not found ignored event"));

        eventHandlerFunction = EventHandler_IgnoreEvent;
    }
    return (eventHandlerFunction);
}


/********************************************************************/
/**
 *
 * @function     EventHandler_IgnoreEvent
 *
 * @description  Function to ignore the event
 *
 * @param        *object :   void type.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void EventHandler_IgnoreEvent(
    void *object,
    EventData_p eventData)
{
    SMS_IDENTIFIER_NOT_USED(object);
    SMS_IDENTIFIER_NOT_USED(eventData);
}


/********************************************************************/
/**
 *
 * @function     EventHandler_DeliberateIgnore
 *
 * @description  Function to ignore the event deliberately
 *
 * @param        *object :   void type.
 * @param        eventData : as type of EventData_p.
 *
 * @return       void
 */
/********************************************************************/
void EventHandler_DeliberateIgnore(
    void *object,
    EventData_p eventData)
{
    SMS_IDENTIFIER_NOT_USED(object);
    SMS_IDENTIFIER_NOT_USED(eventData);
}
