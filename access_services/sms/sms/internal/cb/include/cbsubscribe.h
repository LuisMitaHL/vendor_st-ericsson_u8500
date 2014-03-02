/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     Include file for the subscribe functionality
*
********************************************************************************************
*/

#ifndef INCLUSION_GUARD_CBS_SUBSCRIBE_H
#define INCLUSION_GUARD_CBS_SUBSCRIBE_H

#include <stdint.h>

#include "t_smslinuxporting.h"

#include "t_cbs.h"

#ifndef CBS_MAX_NUMBER_SUBSCRIPTIONS
#define CBS_MAX_NUMBER_SUBSCRIPTIONS 0xFF       // Number of subscriptions, on API, uses uint8_t so 0xFF is max possible value.
#endif

/*
**========================================================================
**    Structure declarations
**========================================================================
*/

// Data structure to hold list of clients.
typedef struct SubscribeClientWithClientTagList_tag {
    struct SubscribeClientWithClientTagList_tag *Next_p;
    SMS_PROCESS Client;
    SMS_ClientTag_t ClientTag;
    uint8_t IsSim;
} CBS_Subscribe_ClientWithClientTagList_t;


/*
**========================================================================
**    External function declarations
**========================================================================
*/

CBS_Error_t CBS_Subscribe_AddSubscription(
    SMS_PROCESS Client,
    SMS_ClientTag_t ClientTag,
    uint8_t SubscribeToAll,
    uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p);

CBS_Error_t CBS_Subscribe_RemoveSubscription(
    SMS_PROCESS Client,
    uint8_t SubscribeToAll,
    uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p);

void CBS_Subscribe_RemoveAllSimSubscriptions(
    void);

uint8_t CBS_Subscribe_CheckForSubscriptions(
    void);

uint8_t CBS_Subscribe_CountSubscriptions(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t IsSim);

void CBS_Subscribe_GetSubscriptions(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t NumberOfSubscriptions,
    const uint8_t IsSim,
    CBS_Subscription_t * const Subscription_p);

uint8_t IsClientSubscribedToMessage(CBS_Subscribe_ClientWithClientTagList_t *Client_p, const CBS_MessageHeader_t * const Header_p, const uint8_t IsClientActivated);


CBS_Subscribe_ClientWithClientTagList_t *CBS_Subscribe_GetClientList(void);


#endif                          // INCLUSION_GUARD_CBS_SUBSCRIBE_H
