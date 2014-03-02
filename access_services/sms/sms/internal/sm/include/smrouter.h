#ifndef SMROUTER_H
#define SMROUTER_H
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
 *  The Short Message Router Object and it's Methods.
 *
 *************************************************************************/

#include "t_smslinuxporting.h"


typedef enum {
    SHORTMESSAGEROUTER_NEW_SHORT_MESSAGE = 1,
    SHORTMESSAGEROUTER_APPLICATION_ACKNOWLEDGE_WITH_CNF,
    SHORTMESSAGEROUTER_PROCESS_RECEIVED_STATUS_REPORTS,
    SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND,
    SHORTMESSAGEROUTER_SHORT_MESSAGE_SEND_ACKNOWLEDGE,
    SHORTMESSAGEROUTER_DELIVERREPORT_CONFIRM,
    SHORTMESSAGEROUTER_MEMCAPACITYAVAILABLE_SENT,
} ShortMessageRouterEvent_t;

typedef struct ShortMessageRouterObject *ShortMessageRouter_p;

void ShortMessageRouter_HandleEvent(
    ShortMessageRouterEvent_t event,
    EventData_p eventData);
void ShortMessageRouter_PreferredStorageSet(
    SMS_Storage_t PreferredStorage);

void ShortMessageRouter_DeliverReportControlSet(
    SMS_NetworkAcknowledge_t NetworkAcknowledge);

void ShortMessageRouter_DeliverReportControlGet(
    SMS_NetworkAcknowledge_t * const NetworkAcknowledge_p);

SMS_Error_t ShortMessageRouter_Port_Add(
    const SMS_PROCESS Process,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p);
SMS_Error_t ShortMessageRouter_Port_Delete(
    const SMS_PROCESS Process,
    const SMS_ClientTag_t ClientTag,
    const SMS_ApplicationPortRange_t * const ApplicationPortRange_p);

void ShortMessageRouter_Deinitialise(
    void);

#endif                          // SMROUTER_H
