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
 *  Cell Broadcast manager include file.
 *
 *************************************************************************/
#ifndef INCLUSION_GUARD_CBMNGR_H
#define INCLUSION_GUARD_CBMNGR_H

#include "t_cbs.h"
#include "t_sms_cb_session.h"
#include "g_cbs.h"

#include "cbsubscribe.h"
#include "cbmsgs.h"



#define CB_CAT_CB_DOWNLOAD_TAG 0xD2


void CBS_Manager_Init(
    void);

void CBS_Manager_Init_CBMID(
    void);

#ifndef SMS_USE_MFL_MODEM
void CBS_Manager_Init_MAL_Info(
    const uint8_t SubscriptionNumber);
#endif

CBS_Error_t CBS_Manager_Activate(
    void);
CBS_Error_t CBS_Manager_Deactivate(
    void);
void CBS_Manager_CheckActivation(
    void);

void CBS_Manager_ToggleModemActivationState(void);

CBS_Error_t CBS_Manager_MessageSubscribe(
    const SMS_PROCESS Client,
    const SMS_ClientTag_t ClientTag,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const uint8_t NumberOfSubscriptions,
    const CBS_Subscription_t Subcriptions[],
    CBS_MessagesList_t ** CachedMessagesList_pp);

CBS_Error_t CBS_Manager_ConvertGsmMessageToPdu(const CBS_Message_t * const Message_p, uint16_t *PduLength_p, uint8_t **Pdu_p);
CBS_Error_t CBS_Manager_ConvertEtwsMessageToPdu(const CBS_Message_t * const Message_p, uint16_t *PduLength_p, uint8_t **Pdu_p);
CBS_Error_t CBS_Manager_ConvertUmtsMessagesToPdu(CBS_MessagesList_t *Messages_p, uint16_t *PduLength_p, uint8_t **Pdu_pp);
CBS_Error_t CBS_Manager_HandleMessage(const CBS_Subscribe_ClientWithClientTagList_t * const Client_p,
                                      const CBS_Message_t * const Message_p,
                                      const bool Dispatch,
                                      CBS_MessagesList_t *Buffer_p,
                                      uint16_t *PduLength_p,
                                      uint8_t **Pdu_pp);
CBS_Error_t CBS_Manager_DispatchUmtsBuffer(const CBS_Subscribe_ClientWithClientTagList_t * const Client_p,
                                           CBS_MessagesList_t *Buffer_p);
CBS_Error_t CBS_Manager_Dispatch(const CBS_Subscribe_ClientWithClientTagList_t * const Client_p,
                                 const uint16_t PduLength,
                                 const uint8_t * const Pdu_p);

CBS_Error_t CBS_Manager_MessageUnSubscribe(
    const SMS_PROCESS Client,
    const uint8_t UnsubscribeFromAll,
    const uint8_t IsSim,
    const uint8_t NumberOfSubscriptions,
    const CBS_Subscription_t Subscriptions[]);

uint8_t CBS_Manager_MessageNumberOfSubscriptionsGet(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t IsSim);

void CBS_Manager_Message_SubscriptionsGet(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t IsSim,
    const uint8_t NumberOfSubscriptions,
    CBS_Subscription_t Subscription[]);

void CBS_Manager_CAT_Refresh(
    void);

void CBS_Manager_HandleNewCbIndication(
    const SMS_MAL_Callback_CB_Routing_Ind_Data_t * const CbRoutingIndData_p);


#endif                          // INCLUSION_GUARD_CBMNGR_H
