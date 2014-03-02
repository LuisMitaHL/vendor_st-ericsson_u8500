#ifndef INCLUSION_GUARD_G_CBS_H
#define INCLUSION_GUARD_G_CBS_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     Cell Broadcast Transport Server requester function gateway header file
*   
*******************************************************************************
*/

#include "t_cbs.h"
#include "t_smslinuxporting.h"

/*************************************************************************
* Cell Broadcast Server thread id
*************************************************************************/
#define CB_SERVERTHREAD_    (998)
#ifndef HAVE_ANDROID_OS
#define CB_SERVER_SOCK_PATH "/tmp/socket_cbs_server"
#else
#define CB_SERVER_SOCK_PATH "/dev/socket/cbs_server"
#endif


/*************************************************************************
* CB Server Timer Definitions
*************************************************************************/
#define CBS_TIMER_DEFAULT_CLIENT_TAG  (0)
#define CBS_TIMER_S_INTO_MS        (1000)


// Used for subscibe/unsubscribe  
typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint8_t All_CBMI;
    uint8_t NumberOfSubscriptions;
    CBS_Subscription_t FirstSubscription[1];
} CBS_Subscription_Req_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    CBS_Error_t Error;
} CBS_Subscription_Resp_t;

// Used for get number of subscriptions/get subscriptions
typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint8_t AllSubscriptions;
    uint8_t NumberOfSubscriptions;
} CBS_GetSubscriptions_Req_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    CBS_Error_t Error;
    uint8_t NumberOfSubscriptions;
    CBS_Subscription_t FirstSubscription[1];
} CBS_GetSubscriptions_Resp_t;

// Used for activate/deactivate
typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
} CBS_Activation_Req_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    CBS_Error_t Error;
} CBS_Activation_Resp_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    CBS_MessageHeader_t MessageHeader;
} CBS_CellBroadcastMessageRead_Req_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    CBS_Error_t Error;
    uint16_t PduLength;
    uint8_t Pdu[1];
} CBS_CellBroadcastMessageRead_Resp_t;

typedef struct {
    SMS_SigselectWithClientTag_t SigselectWithClientTag;
    uint16_t PduLength;
    uint8_t Pdu[1];
} Event_CBS_CellBroadcastMessageReceived_t;

#endif                          // INCLUSION_GUARD_G_CBS_H
