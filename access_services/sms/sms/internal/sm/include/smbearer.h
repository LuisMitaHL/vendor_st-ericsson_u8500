#ifndef SMBEARER_H
#define SMBEARER_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION: Short message bearer header file
 *
 *************************************************************************/

#include "smsmain.h"            // for SMS_MAL_Callback_Data_t


#ifdef SMS_USE_MFL_MODEM
// Default client tag used with MFL modem request/response functions.
#define SMS_MFL_DEFAULT_CLIENT_TAG  0
#endif

// The events coming to the Short Message Bearer
typedef enum {
    SHORTMESSAGEBEARER_NETWORK_ACK = 1, // report indication from network (i.e. SMR)
    SHORTMESSAGEBEARER_RECEIVE_SHORT_MESSAGE,   // new message from network
    SHORTMESSAGEBEARER_RECEIVE_FAILURE, // failure cause event form NS module
    SHORTMESSAGEBEARER_APPLICATION_ACK_WITH_CNF,        // acknowledgement from application specific s/w Response message expected.
    SHORTMESSAGEBEARER_COMMAND_REQ,     // command request from aplication specific s/w
    SHORTMESSAGEBEARER_MEMORY_NOTIFICATION_REQ, // memory notification from the storage
    SHORTMESSAGEBEARER_RELAY_LINK_TIMEOUT,      // The relay link timout has expired
    SHORTMESSAGEBEARER_DELIVERREPORT_CONFIRM,   // The result of NS sending the DELIVER REPORT
    SHORTMESSAGEBEARER_TPDUDELIVER      // event indicating a TPDU deliver 
} ShortMessageBearerEvent_t;

typedef struct ShortMessageBearerObject *ShortMessageBearer_p;


//-----------------------------------------------------------------------------------
// Public operations on a ShortMessageBearer object
//-----------------------------------------------------------------------------------
void ShortMessageBearer_Initialise(
    int *Modem_fd_p
#ifndef SMS_USE_MFL_MODEM
    ,
    int *nl_fd_p
#endif
    );
void ShortMessageBearer_MemCapacityInitialise(
    void);
ShortMessageBearer_p ShortMessageBearer_Create(
    void);
void ShortMessageBearer_AbortShortMessage(
    void);
void ShortMessageBearer_RelayLinkControl_Set(
    const SMS_RelayControl_t RelayLinkControl);
void ShortMessageBearer_RelayLinkControl_Get(
    SMS_RelayControl_t * const RelayLinkControl_p);
void ShortMessageBearer_MO_RouteSet(
    SMS_MO_Route_t Route);
SMS_MO_Route_t ShortMessageBearer_MO_RouteGet(
    void);
void ShortMessageBearer_HandleEvent(
    ShortMessageBearerEvent_t,
    EventData_p);
void ShortMessageBearer_HandleMalCallbackData(
    SMS_MAL_Callback_Data_t * MalCallbackData_p);

#endif                          // SMBEARER_H
