/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     CBS Entry Point
*
********************************************************************************************
*/

/*
**========================================================================
**    Global declarations
**========================================================================
*/

#include <stdint.h>

/* SIM API Include Files */
#include "sim.h"

/* Operation System Include Files */
#include "r_cbslinuxporting.h"

#include "t_cbs.h"
#include "g_cbs.h"

#ifndef CBS_USE_MFL_MODEM
#include "mal_sms.h"
#endif                          // CBS_USE_MFL_MODEM

#include "cbmsgs.h"
#include "cbsubscribe.h"
#include "cbmngr.h"
#include "smsmain.h"
#include "smutil.h"


/*
 * The functionality in the next section of this file is to handle the state of the CB Server
 * and its connection to the modem.
 */
typedef struct {
    // Flag to indicate whether a client has "activated" the CB Server.
    uint8_t CBS_Activated_Client;

    // Flag to indicate whether valid SIM EFcbmid values have "activated" the CB Server.
    uint8_t CBS_Activated_CBMID;

    // Flag to indicate whether the modem has been enabled for CB messages.
    uint8_t Modem_Activated;

#ifndef SMS_USE_MFL_MODEM
    // Flag to indicate whether a response has been received from the MAL modem
    // containing a "subscription number"; i.e. the number needed to be supplied
    // to the modem when removing the subscription to it.
    uint8_t MAL_SubscriptionNumberRxd;

    // "Subscription number" received from MAL modem when subscribing with it to
    // receive CB messages.  It is this number that is required when unsubscribing
    // from the MAL modem.
    uint8_t MAL_SubscriptionNumber;
#endif
} CBS_StateInfo_t;


/*
**========================================================================
** Defines.
**========================================================================
*/

#define CB_MESSAGE_DATA_DCS_VALUE_MIN         0x00
#define CB_MESSAGE_DATA_DCS_VALUE_MAX         0xFF
#define CB_MESSAGE_DATA_MID_VALUE_MIN         0x0000
#define CB_MESSAGE_DATA_MID_VALUE_MAX         0xFFFF

// Defines for accessing EFcbmid data from SIM card.
#define EF_FILEPATH               NULL
#define EF_CBMID_FILENAME         0x6F48
#define EF_CBMID_FILE_READ_OFFSET 0
#define SIM_CLIENT_PROCESS        SMS_NO_PROCESS        // Arbitary value.
#define SIM_CLIENT_TAG            0
#define EF_CBMID_OCTETS_PER_MID   2
#define EF_CBMID_UNUSED_ENTRY     0xFFFF

// Defines for Card Application Toolkit (CAT) CB Data download
#define CB_CAT_DATA_DOWNLOAD_MAX_LENGTH      0xFF
#define CB_CAT_CELL_BROADCAST_MESSAGE_TAG    0x0C
#define CB_CAT_SIZE_CELL_BROADCAST_MESSAGE_TLV  (CB_CAT_SMS_CB_SIZE + 2)   // Ref. 31.111 section 8.5
#define CB_CAT_DEVICE_IDENTITIES_TAG         0x02
#define CB_CAT_DEVICE_IDENTITIES_TOTAL_SIZE  4  // Ref. TS 31.111 section 8.7
#define CB_CAT_DEVICE_IDENTITIES_LENGTH      2  // Ref. TS 31.111 section 8.7
#define CB_CAT_DEVICE_SIM                    0x81
#define CB_CAT_DEVICE_NETWORK                0x83
#define CB_CAT_COMPREHENSION_REQUIRED_MASK   0x80
#define CB_CAT_SMS_CB_SIZE                   88

#define CB_MESSAGE_ETWS_VALUE_MIN 0x1100  // Ref. TS 23.041 section 9.4.1.2.2
#define CB_MESSAGE_ETWS_VALUE_MAX 0x1107  // Ref. TS 23.041 section 9.4.1.2.2

/*
**========================================================================
**    Local variable definitions.
**========================================================================
*/

// Data structure containing state information associated with CB Server.
// CBS_StateInfo is initialised by a call to CBS_Manager_Init().
static CBS_StateInfo_t CBS_StateInfo;



/*
**========================================================================
**    Local function definitions
**========================================================================
*/

#ifdef SMS_PRINT_A_
/********************************************************************/
/**
 *
 * @function     CBS_Manager_Message_HexPrint
 *
 * @description  Prints the Header data and Hex bytes of the CB Message
 *
 * @param [in]   Message_p : CB Message data to be printed to log.
 *
 * @return       void
 */
/********************************************************************/
static void CBS_Manager_Message_HexPrint(
    const CBS_Message_t *Message_p)
{
    if (NULL != Message_p) {

        SMS_LOG_I("cbmngr.c: SerialNumber     : 0x%04X", Message_p->Header.SerialNumber);
        SMS_LOG_I("cbmngr.c: MID              : 0x%04X", Message_p->Header.MessageIdentifier);
        SMS_LOG_I("cbmngr.c: DataCodingScheme : 0x%02X", Message_p->Header.DataCodingScheme);
        SMS_LOG_I("cbmngr.c: PageParameter    : 0x%02X", Message_p->Header.PageParameter);
        SmUtil_HexPrint_Raw(Message_p->Data, CBS_MESSAGE_MAX_PAGE_LENGTH, "CBS: cbmngr.c: CB Message data");
    }
}
#endif



/********************************************************************/
/**
 *
 * @function     CBS_Retrieve_CachedMessages
 *
 * @description  Iterate through cached messages looking for any which
 *               match the new subscription just supplied by the client.
 *               If any are found, add them to a linked list and return
 *               them to the caller.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   ClientTag : ClientTag supplied by the calling client.
 * @param [in]   SubscribeToAll : Flag indicating whether the client has
 *                                subscribed to all CB messages, or not.
 * @param [in]   Subscription_p : Subscription information supplied by
 *                                client.
 * @param [inout] InCachedMessageList_p : Linked list to which matching messages
 *                                    are added.
 *
 * @return       Linked list of cached messages.
 */
/********************************************************************/
static CBS_MessagesList_t *CBS_Retrieve_CachedMessages(
    const SMS_ClientTag_t ClientTag,
    const uint8_t SubscribeToAll,
    const CBS_Subscription_t * const Subscription_p,
    CBS_MessagesList_t * const InCachedMessageList_p)
{
    CBS_MessagesList_t *OutCachedMessageList_p = InCachedMessageList_p;
    const CBS_MessagesList_t *CachedMessageList_p = CBS_Msgs_CachedMessageList_Get();

    SMS_B_(SMS_LOG_D("cbmngr: Enter Retrieve_CachedMessages: All %d", SubscribeToAll));

    while (CachedMessageList_p != NULL) {
        if (SubscribeToAll ||
            (Subscription_p->FirstMessageIdentifier <= CachedMessageList_p->Message.Header.MessageIdentifier &&
             Subscription_p->LastMessageIdentifier >= CachedMessageList_p->Message.Header.MessageIdentifier &&
             Subscription_p->FirstDataCodingScheme <= CachedMessageList_p->Message.Header.DataCodingScheme &&
             Subscription_p->LastDataCodingScheme >= CachedMessageList_p->Message.Header.DataCodingScheme)) {

            CBS_MessagesList_t *CachedMessage_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(CBS_MessagesList_t));

            if (CachedMessage_p != NULL) {
                if (OutCachedMessageList_p == NULL) {
                    OutCachedMessageList_p = CachedMessage_p;
                } else {
                    CBS_MessagesList_t *Curr_p = OutCachedMessageList_p;
                    while (Curr_p->Next_p != NULL) {
                        Curr_p = Curr_p->Next_p;
                    }

                    Curr_p->Next_p = CachedMessage_p;
                }
                CachedMessage_p->Next_p = NULL;
            } else {
                SMS_A_(SMS_LOG_E("cbmngr: CBS_Retrieve_CachedMessages: MALLOC FAILED"));
                return OutCachedMessageList_p;
            }

            // Now setup data in new list entry.
            CachedMessage_p->Message = CachedMessageList_p->Message;
        }

        CachedMessageList_p = CachedMessageList_p->Next_p;
    }
    SMS_B_(SMS_LOG_D("cbmngr: Exit CBS_Retrieve_CachedMessages"));

    return OutCachedMessageList_p;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_MessageSubscribe
 *
 * @description  Handle the new subscription(s) which have been supplied
 *               by a client and add them to the CB Server. If there are
 *               any cached CB messages which match these new subscriptions,
 *               add them to a linked list so that they can be dispatched
 *               to the client.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   ClientTag : ClientTag supplied by the calling client.
 * @param [in]   SubscribeToAll : Flag indicating whether the client has
 *                                subscribed to all CB messages, or not.
 * @param [in]   IsSim : TRUE if subscription is for EFcbmid.
 * @param [in]   NumberOfSubscriptions : Number of subscriptions supplied in
 *                                       Subscriptions_p parameter.
 * @param [in]   Subscriptions : Subscription data.
 * @param [out]  CachedMessageList_pp : Address of linked list of cached CB
 *                                      messages which are to be dispatched as a result
 *                                      of the new subscription(s).
 *                                      NULL if not required.
 *
 * @return       Error value.
 */
/********************************************************************/
CBS_Error_t CBS_Manager_MessageSubscribe(
    const SMS_PROCESS Client,
    const SMS_ClientTag_t ClientTag,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const uint8_t NumberOfSubscriptions,
    const CBS_Subscription_t Subcriptions[],
    CBS_MessagesList_t ** CachedMessageList_pp)
{
    uint8_t index;
    CBS_Error_t CBS_Error = CBS_ERROR_NONE;

    SMS_B_(SMS_LOG_D("cbmngr: Enter MessageSubscribe: All %d, IsSim %d, Num Subs %d", SubscribeToAll, IsSim, NumberOfSubscriptions));
    // Validate input parameters
    if (!SubscribeToAll) {
        // As we are not subscribing to all, validate that the supplied values are ok.
        for (index = 0; index < NumberOfSubscriptions; index++) {
            if (Subcriptions[index].FirstMessageIdentifier > Subcriptions[index].LastMessageIdentifier ||
                Subcriptions[index].FirstDataCodingScheme > Subcriptions[index].LastDataCodingScheme) {
                // One of the "first" values is greater than its "last" value. Reject the subscription request.
                SMS_A_(SMS_LOG_E("cbmngr: MessageSubscribe: Invalid values: First MID %d, Last MID %d, First DCS %d, Last DCS %d",
                              Subcriptions[index].FirstMessageIdentifier,
                              Subcriptions[index].LastMessageIdentifier, Subcriptions[index].FirstDataCodingScheme, Subcriptions[index].LastDataCodingScheme));

                CBS_Error = CBS_ERROR_INVALID_PARAMETER;
                break;
            }
        }
    }

    if (CBS_Error == CBS_ERROR_NONE) {
        uint8_t ValidatedNumberOfSubscriptions;
        CBS_Subscription_t AllSubscriptions;
        const CBS_Subscription_t *ValidatedSubscriptions_p;

        if (SubscribeToAll) {
            // Subscribing to all CB Messages. This means that the Client may have
            // supplied garbage data in NumberOfSubscriptions and Subscriptions_p.
            // Therefore setup some valid entries.
            AllSubscriptions.FirstMessageIdentifier = CB_MESSAGE_DATA_MID_VALUE_MIN;
            AllSubscriptions.LastMessageIdentifier = CB_MESSAGE_DATA_MID_VALUE_MAX;
            AllSubscriptions.FirstDataCodingScheme = CB_MESSAGE_DATA_DCS_VALUE_MIN;
            AllSubscriptions.LastDataCodingScheme = CB_MESSAGE_DATA_DCS_VALUE_MAX;
            ValidatedNumberOfSubscriptions = 1;
            ValidatedSubscriptions_p = &AllSubscriptions;
        } else {
            ValidatedNumberOfSubscriptions = NumberOfSubscriptions;
            ValidatedSubscriptions_p = &Subcriptions[0];
        }

        // Call CBS_Search_AddSubscription() for each valid subscription.
        for (index = 0; index < ValidatedNumberOfSubscriptions; index++) {
            CBS_Error = CBS_Subscribe_AddSubscription(Client, ClientTag, SubscribeToAll, IsSim, &ValidatedSubscriptions_p[index]);

            if (CBS_Error == CBS_ERROR_NONE) {
                if (CachedMessageList_pp != NULL) {
                    // We have successfully added a new subscription to the CB Server. Now iterate through all
                    // the cached CB messages and determine whether any match the new subscription. If they do,
                    // add them to a list which will be dispatched to the client.
                    *CachedMessageList_pp = CBS_Retrieve_CachedMessages(ClientTag, SubscribeToAll, &ValidatedSubscriptions_p[index], *CachedMessageList_pp);
                }
            } else {
                SMS_A_(SMS_LOG_E("cbmngr: MessageSubscribe: Error %d: Index %d, All %d, IsSim %d, First MID %d, Last MID %d, First DCS %d, Last DCS %d",
                              CBS_Error,
                              index,
                              SubscribeToAll,
                              IsSim,
                              ValidatedSubscriptions_p[index].FirstMessageIdentifier,
                              ValidatedSubscriptions_p[index].LastMessageIdentifier, ValidatedSubscriptions_p[index].FirstDataCodingScheme, ValidatedSubscriptions_p[index].LastDataCodingScheme));
                break;
            }
        }
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit MessageSubscribe with Error %d", CBS_Error));

    return CBS_Error;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_MessageNumberOfSubscriptionsGet
 *
 * @description  Handle getting the number of subscriptions associated
 *               with the supplied Client and ClientTag.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   AllSubscriptions : TRUE if we are to count all subscriptions,
 *                                  or FALSE if we only count subscriptions made
 *                                  by this client.
 * @param [in]   IsSim : TRUE if subscriptions are for EFcbmid.
 *
 * @return       Number of subscriptions
 */
/********************************************************************/
uint8_t CBS_Manager_MessageNumberOfSubscriptionsGet(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t IsSim)
{
    uint8_t NumberOfSubscriptions;

    SMS_B_(SMS_LOG_D("cbmngr: Enter MessageNumberOfSubscriptionsGet: All %d", AllSubscriptions));

    NumberOfSubscriptions = CBS_Subscribe_CountSubscriptions(Client, AllSubscriptions, IsSim);

    SMS_B_(SMS_LOG_D("cbmngr: Exit MessageNumberOfSubscriptionsGet with count %d", NumberOfSubscriptions));
    return NumberOfSubscriptions;
}



/********************************************************************/
/**
 *
 * @function     CBS_Manager_Message_SubscriptionsGet
 *
 * @description  Get the subscriptions stored in the CB Server.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   AllSubscriptions : TRUE if we are to get all subscriptions,
 *                                  or FALSE if we only get subscriptions made
 *                                  by this client.
 * @param [in]   NumberOfSubscriptions : Maximum number of subscriptions to be
 *                                       returned.
 * @param [in]   IsSim : TRUE if subscriptions are for EFcbmid.
 * @param [out]  Subscription : Buffer which should be large enough to hold
 *                              NumberOfSubscriptions * sizeof(CBS_Subscription_t)
 *                              subscriptions.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Manager_Message_SubscriptionsGet(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t NumberOfSubscriptions,
    const uint8_t IsSim,
    CBS_Subscription_t Subscription[])
{
    SMS_B_(SMS_LOG_D("cbmngr: Enter SubscriptionsGet: All %d, Number %d.", AllSubscriptions, NumberOfSubscriptions));

    CBS_Subscribe_GetSubscriptions(Client, AllSubscriptions, NumberOfSubscriptions, IsSim, &Subscription[0]);

    SMS_B_(SMS_LOG_D("cbmngr: Exit SubscriptionsGet"));
}



/********************************************************************/
/**
 *
 * @function     CBS_Manager_MessageUnSubscribe
 *
 * @description  Handle the new unsubscription(s) which have been supplied
 *               by a client and add them to the CB Server.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   UnsubscribeFromAll : Flag indicating whether the client has
 *                                    unsubscribed from all CB messages, or not.
 * @param [in]   IsSim : TRUE if subscription is for EFcbmid.
 * @param [in]   NumberOfSubscriptions : Number of subscriptions supplied in
 *                                       Subscriptions_p parameter.
 * @param [in]   Subscriptions : Subscription data.
 *
 * @return       Error value.
 */
/********************************************************************/
CBS_Error_t CBS_Manager_MessageUnSubscribe(
    const SMS_PROCESS Client,
    const uint8_t UnsubscribeFromAll,
    const uint8_t IsSim,
    const uint8_t NumberOfSubscriptions,
    const CBS_Subscription_t Subscriptions[])
{
    uint8_t index;
    CBS_Error_t CBS_Error = CBS_ERROR_NONE;

    SMS_B_(SMS_LOG_D("cbmngr: Enter MessageUnSubscribe: All %d, IsSim %d, Num Subs %d", UnsubscribeFromAll, IsSim, NumberOfSubscriptions));
    // Iterate through supplied subscriptions and remove them from list.
    {
        uint8_t ValidatedNumberOfSubscriptions;
        CBS_Error_t TmpError;
        CBS_Subscription_t AllSubscriptions;
        const CBS_Subscription_t *ValidatedSubscriptions_p;

        if (UnsubscribeFromAll) {
            // Unsubscribing from all CB Messages. This means that the Client may have
            // supplied garbage data in NumberOfSubscriptions and Subscriptions_p.
            // Therefore setup some valid entries to ensure match with data used in
            // CBS_Manager_MessageSubscribe().
            AllSubscriptions.FirstMessageIdentifier = CB_MESSAGE_DATA_MID_VALUE_MIN;
            AllSubscriptions.LastMessageIdentifier = CB_MESSAGE_DATA_MID_VALUE_MAX;
            AllSubscriptions.FirstDataCodingScheme = CB_MESSAGE_DATA_DCS_VALUE_MIN;
            AllSubscriptions.LastDataCodingScheme = CB_MESSAGE_DATA_DCS_VALUE_MAX;
            ValidatedNumberOfSubscriptions = 1;
            ValidatedSubscriptions_p = &AllSubscriptions;
        } else {
            ValidatedNumberOfSubscriptions = NumberOfSubscriptions;
            ValidatedSubscriptions_p = &Subscriptions[0];
        }

        for (index = 0; index < ValidatedNumberOfSubscriptions; index++) {
            TmpError = CBS_Subscribe_RemoveSubscription(Client, UnsubscribeFromAll, IsSim, &ValidatedSubscriptions_p[index]);

            if (TmpError != CBS_ERROR_NONE) {
                SMS_A_(SMS_LOG_E("cbmngr: MessageUnSubscribe: Error %d: Index %d, All %d, IsSim %d, First MID %d, Last MID %d, First DCS %d, Last DCS %d",
                              TmpError,
                              index,
                              UnsubscribeFromAll,
                              IsSim,
                              ValidatedSubscriptions_p[index].FirstMessageIdentifier,
                              ValidatedSubscriptions_p[index].LastMessageIdentifier,
                              ValidatedSubscriptions_p[index].FirstDataCodingScheme,
                              ValidatedSubscriptions_p[index].LastDataCodingScheme));
                // Store the first "error" value detected, but keep going and try and remove any other
                // subscriptions supplied by the client. I.e. we do the best we can with the supplied
                // data.
                if (CBS_Error == CBS_ERROR_NONE) {
                    CBS_Error = TmpError;
                }
            }
        }
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit MessageUnSubscribe with Error %d", CBS_Error));
    return CBS_Error;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_CAT_DataDownload
 *
 * @description  Pack CB Message data into envelope which can be downloaded
 *               to CAT application on SIM.
 *
 * @param [in]   CBS_Message_p : CB Message data.
 *
 * @return       void
 */
/********************************************************************/
static void CBS_Manager_CAT_DataDownload(
    const CBS_Message_t * const CBS_Message_p)
{
    SMS_B_(SMS_LOG_D("cbmngr: Enter CAT_DataDownload"));

    if (SmUtil_UICC_ServiceEnabled(SIM_SERVICE_TYPE_SMS_CB)) {
        uint8_t *Envelope_p = SMS_HEAP_UNTYPED_ALLOC(CB_CAT_DATA_DOWNLOAD_MAX_LENGTH);

        if (Envelope_p != NULL) {
            // Build and send CB download envelope
            uint32_t EnvelopeSize;
            uint8_t *Length_p;
            uint8_t *Env_p = Envelope_p;

            // CB download Tag
            *Env_p++ = CB_CAT_CB_DOWNLOAD_TAG;

            // Set up a pointer to the Length byte to be filled in later.
            // Note that the length for the CB download will not exceed 127 and will therefore be coded on 1 byte.
            Length_p = Env_p;

            // point to start of Device identities octet
            Env_p++;

            // Build device identities TLV
            *Env_p++ = CB_CAT_DEVICE_IDENTITIES_TAG | CB_CAT_COMPREHENSION_REQUIRED_MASK;       // TAG
            *Env_p++ = CB_CAT_DEVICE_IDENTITIES_LENGTH; // LENGTH of VALUE (2)
            *Env_p++ = CB_CAT_DEVICE_NETWORK;   // VALUE (Source)
            *Env_p++ = CB_CAT_DEVICE_SIM;       // VALUE (Destination)
            *Length_p = CB_CAT_DEVICE_IDENTITIES_TOTAL_SIZE;    // Length of entire Device Identities TLV

            // Build cell broadcast message TLV
            *Env_p++ = CB_CAT_CELL_BROADCAST_MESSAGE_TAG | CB_CAT_COMPREHENSION_REQUIRED_MASK;     // TAG
            *Env_p++ = CB_CAT_SMS_CB_SIZE;      // LENGTH (Length is always 88 (decimal) for CB Message download - Ref. 31.111 section 8.5)

            *Env_p++ = CBS_Message_p->Header.SerialNumber >> 8;      /* Use first octet in word */
            *Env_p++ = CBS_Message_p->Header.SerialNumber;           /* Truncates to last octet in word */
            *Env_p++ = CBS_Message_p->Header.MessageIdentifier >> 8; /* Use first octet in word */
            *Env_p++ = CBS_Message_p->Header.MessageIdentifier;      /* Truncates to last octet in word */
            *Env_p++ = CBS_Message_p->Header.DataCodingScheme;
            *Env_p++ = CBS_Message_p->Header.PageParameter;
            (void) memcpy(Env_p, CBS_Message_p->Data, CBS_MESSAGE_MAX_PAGE_LENGTH);

            *Length_p += CB_CAT_SIZE_CELL_BROADCAST_MESSAGE_TLV;    // Increment with length of entire cell broadcast message TLV

            // Overall size of the entire envelope is the *Length_p +2 bytes for the CB Download tag tag and length and its length octet.
            EnvelopeSize = *Length_p + 2;

            // Perform envelope download
            {
                ste_sim_t *UICC_p = MessagingSystem_UICC_SimAsyncGet();

                if (UICC_p == NULL) {
                    SMS_A_(SMS_LOG_E("cbmngr: CAT_DataDownload: SIM UICC Server Object FAILED"));
                } else {
                    int UICC_Status;
                    uintptr_t UICC_Client_Tag;

                    // We need to distinguish, in the CAT callback, whether the envelope download came from here or the
                    // equivalent SMS data download. We use the Client Tag to check which is which.
                    UICC_Client_Tag = (uintptr_t) CB_CAT_CB_DOWNLOAD_TAG;

                    SMS_B_(SMS_LOG_D("cbmngr: CAT_DataDownload: calling ste_cat_envelope_command()"));
                    SMS_A_(SmUtil_HexPrint_Raw(Envelope_p, EnvelopeSize, "CBS: cbmngr.c: CB Envelope"));

                    UICC_Status = ste_cat_envelope_command(UICC_p, UICC_Client_Tag, (const char *) Envelope_p, EnvelopeSize);

                    SMS_B_(SMS_LOG_D("cbmngr: CAT_DataDownload: ste_cat_envelope_command returned %d", UICC_Status));
                    SMS_IDENTIFIER_NOT_USED(UICC_Status);       // Silence warnings if A prints not enabled.
                }
            }

            // Free envelope buffer memory
            SMS_HEAP_FREE(&Envelope_p);
        } else {
            // Memory allocation failed
            SMS_A_(SMS_LOG_E("cbmngr: CAT_DataDownload: MALLOC FAILED"));
        }
    } else {
        // UICC does not support CB Data Download
        SMS_A_(SMS_LOG_E("cbmngr: CAT_DataDownload: No UICC Support"));
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit CAT_DataDownload"));
}


/********************************************************************/
/**
 * @function     CBS_Manager_Dispatch
 *
 * @description  Dispatches the CB message PDU to a client.
 *
 * @param [in]   Client_p  : Subscribed client to which PDU should be dispatched.
 * @param [in]   PduLength : The PDU length in octets.
 * @param [in]   PduLength : The PDU.
 *
 * @return       CBS_Error_t
 */
/********************************************************************/
CBS_Error_t CBS_Manager_Dispatch(const CBS_Subscribe_ClientWithClientTagList_t * const Client_p,
                                 const uint16_t PduLength,
                                 const uint8_t * const Pdu_p)
{
    CBS_Error_t Result = CBS_ERROR_NONE;
    if (NULL == Client_p || NULL == Pdu_p) {
        SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_Dispatch: received NULL in parameters"));
        Result = CBS_ERROR_INVALID_PARAMETER;
        goto exit;
    } else {
        Event_CBS_CellBroadcastMessageReceived_t *SendMessage_p = SMS_SIGNAL_UNTYPED_ALLOC(sizeof(Event_CBS_CellBroadcastMessageReceived_t) + PduLength,
                                                                                           EVENT_CBS_CELLBROADCASTMESSAGERECEIVED);
        if (NULL == SendMessage_p) {
            SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_Dispatch: SendMessage_p is NULL"));
            Result = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
            goto exit;
        }
        (void) Do_SMS_ClientTag_Set(SendMessage_p, Client_p->ClientTag);
        SendMessage_p->PduLength = PduLength;
        (void) memmove(SendMessage_p->Pdu, Pdu_p, PduLength);
        SMS_SEND(&SendMessage_p, Client_p->Client);
    }

exit:
    return Result;
}




/********************************************************************/
/**
 * @function     CBS_Manager_DispatchUmtsBuffer
 *
 * @description  Handles the conversion and dispatching of a
 *               UMTS CB message.
 *
 * @param [in]   Client_p : Subscribed client to which PDU should be dispatched.
 * @param [in]   Buffer_p : Buffer containing the UMTS CB message.
 *
 * @return       CBS_Error_t
 */
/********************************************************************/
CBS_Error_t CBS_Manager_DispatchUmtsBuffer(const CBS_Subscribe_ClientWithClientTagList_t * const Client_p,
                                           CBS_MessagesList_t *Buffer_p)
{
    CBS_Error_t Result = CBS_ERROR_NONE;
    uint16_t PduLength = 0;
    uint8_t *Pdu_p = NULL;

    Result = CBS_Manager_ConvertUmtsMessagesToPdu(Buffer_p, &PduLength, &Pdu_p);

    if (CBS_ERROR_NONE == Result) {
        if (NULL != Pdu_p) {

            CBS_Manager_Dispatch(Client_p, PduLength, Pdu_p);

            SMS_HEAP_FREE(&Pdu_p);
        }
    }

    return Result;
}


/********************************************************************/
/**
 * @function     CBS_Manager_ConvertGsmMessageToPdu
 *
 * @description  Converts a GSM message block into a PDU. The GSM format
 *               specified in 3GPP TS 23.041.
 *
 * @param [in]   Message_p : CB Routing indication message block.
 *
 * @param [out]  PduLength_p : The length of the converted PDU
 * @param [out]  Pdu_pp      : The converted PDU
 *
 * @return       CBS_Error_t
 */
/********************************************************************/
CBS_Error_t CBS_Manager_ConvertGsmMessageToPdu(const CBS_Message_t * const Message_p, uint16_t *PduLength_p, uint8_t **Pdu_pp)
{
    CBS_Error_t Result = CBS_ERROR_NONE;
    uint16_t Length = 0;
    uint8_t *Pos_p = NULL;

    if (NULL == Message_p || NULL == Pdu_pp || NULL == PduLength_p) {
        SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_ConvertGsmMessageToPdu: received NULL in parameters"));
        Result = CBS_ERROR_INVALID_PARAMETER;
        goto exit;
    }

    /* Total size of: Header + CB page */
    Length = sizeof(CBS_MessageHeader_t) + CBS_MESSAGE_MAX_PAGE_LENGTH;

    *Pdu_pp = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(Length);

    if (NULL == *Pdu_pp) {
        Result = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
        goto exit;
    }

    *PduLength_p = Length;
    Pos_p = *Pdu_pp;

    /* Add GSM Header */
    *Pos_p++ = Message_p->Header.SerialNumber >> 8;      /* Use first octet in word */
    *Pos_p++ = Message_p->Header.SerialNumber;           /* Truncates to last octet in word */
    *Pos_p++ = Message_p->Header.MessageIdentifier >> 8; /* Use first octet in word */
    *Pos_p++ = Message_p->Header.MessageIdentifier;      /* Truncates to last octet in word */
    *Pos_p++ = Message_p->Header.DataCodingScheme;
    *Pos_p++ = Message_p->Header.PageParameter;

    /* Add GSM Message content*/
    (void) memcpy(Pos_p, Message_p->Data, CBS_MESSAGE_MAX_PAGE_LENGTH);

exit:
    return Result;
}

/********************************************************************/
/** ¤¤¤ TODO: The Temporary notes must be fixed when possible! ¤¤¤
 *
 * @function     CBS_Manager_ConvertEtwsMessageToPdu
 *
 * @description  Converts a ETWS message block into a PDU. The ETWS format
 *               specified in 3GPP TS 23.041.
 *
 * @param [in]   Message_p   : CB Routing indication message block.
 *
 * @param [out]  PduLength_p : The length of the converted PDU
 * @param [out]  Pdu_pp      : The converted PDU
 *
 * @return       CBS_Error_t
 */
/********************************************************************/
CBS_Error_t CBS_Manager_ConvertEtwsMessageToPdu(const CBS_Message_t * const Message_p, uint16_t *PduLength_p, uint8_t **Pdu_pp)
{
    CBS_Error_t Result = CBS_ERROR_NONE;
    uint16_t Length = 0;
    uint8_t *Pos_p = NULL;

    if (NULL == Message_p || NULL == Pdu_pp || NULL == PduLength_p) {
        SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_ConvertEtwsmMessageToPdu: received NULL in parameters"));
        Result = CBS_ERROR_INVALID_PARAMETER;
        goto exit;
    }

    /* Total size of: Serial Number + Message Identifier + Warning type + Warning information */
    Length = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t) + CBS_MESSAGE_MAX_ETWS_LENGTH;

    *Pdu_pp = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(Length);

    if (NULL == *Pdu_pp) {
        Result = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
        goto exit;
    }

    *PduLength_p = Length;
    Pos_p = *Pdu_pp;

    /* Copy Header */
    *Pos_p++ = Message_p->Header.SerialNumber >> 8;      /* Use first octet in word */
    *Pos_p++ = Message_p->Header.SerialNumber;           /* Truncates to last octet in word */
    *Pos_p++ = Message_p->Header.MessageIdentifier >> 8; /* Use first octet in word */
    *Pos_p++ = Message_p->Header.MessageIdentifier;      /* Truncates to last octet in word */
    /* Temporary: The modem must support Warning Type parameter */
    *Pos_p++ = 0x9;  /* 00001001: <= Other and activate emergency user alert */
    *Pos_p++ = 0x80; /* 10000000: <= Activation of warning notification in UI and padding */

    /* Copy ETWS Warning information */
    /* Temporary: It must be verified that GSM message contents and warning information are aligned! */
    (void) memmove(Pos_p, Message_p->Data, CBS_MESSAGE_MAX_ETWS_LENGTH);

exit:
    return Result;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_ConvertUmtsMessagesToPdu
 *
 * @description  Converts a UMTS message block into a PDU. The UMTS format
 *               specified in 3GPP TS 23.041.
 *
 * @param [in]   Message_p : CB Routing indication message blocks.
 *
 * @param [out]  PduLength_p : The length of the converted PDU
 * @param [out]  Pdu_pp      : The converted PDU
 *
 * @return       CBS_Error_t
 */
/********************************************************************/
CBS_Error_t CBS_Manager_ConvertUmtsMessagesToPdu(CBS_MessagesList_t * Messages_p, uint16_t *PduLength_p, uint8_t **Pdu_pp)
{
    CBS_Error_t Result = CBS_ERROR_NONE;
    CBS_MessagesList_t *Message_p = NULL;
    uint8_t *Pos_p = NULL;
    uint16_t Length = 0;

    if (NULL == Messages_p || NULL == Pdu_pp || NULL == PduLength_p) {
        SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_ConvertUmtsMessagesToPdu: received NULL in parameters"));
        Result = CBS_ERROR_INVALID_PARAMETER;
        goto exit;
    }

    /* Total length of: Message Type + Header + CB data [number_of_pages + n(page-info + page-length)] */
    Length = 1 + sizeof(CBS_MessageHeader_t) + Messages_p->Message.Header.PageParameter * (CBS_MESSAGE_MAX_PAGE_LENGTH + 1);

    *Pdu_pp = (uint8_t *) SMS_HEAP_UNTYPED_ALLOC(Length);

    if (NULL == *Pdu_pp) {
        Result = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
        goto exit;
    }

    *PduLength_p = Length;
    Pos_p = *Pdu_pp;

    /* Copy Header */
    *Pos_p++ = 0x01;                                              /* 0x01: CBS message */
    *Pos_p++ = Messages_p->Message.Header.SerialNumber >> 8;      /* Use first octet in word */
    *Pos_p++ = Messages_p->Message.Header.SerialNumber;           /* Truncates to last octet in word */
    *Pos_p++ = Messages_p->Message.Header.MessageIdentifier >> 8; /* Use first octet in word */
    *Pos_p++ = Messages_p->Message.Header.MessageIdentifier;      /* Truncates to last octet in word */
    *Pos_p++ = Messages_p->Message.Header.DataCodingScheme;

    /* Copy CB data */
    *Pos_p++ = Messages_p->Message.Header.PageParameter;
    for (Message_p = Messages_p; NULL != Message_p; Message_p = Message_p->Next_p, Pos_p += CBS_MESSAGE_MAX_PAGE_LENGTH + 1) {
        (void) memmove(Pos_p, Message_p->Message.Data, CBS_MESSAGE_MAX_PAGE_LENGTH);
        *(Pos_p + CBS_MESSAGE_MAX_PAGE_LENGTH) = Message_p->Message.UsefulData;
    }

exit:
    return Result;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_HandleMessage
 *
 * @description  Handles individual CB message blocks supplied in the CB Routing
 *               indication or retrieved from the message cache. A GSM or ETWS
 *               formatted CB message has a 1-1 relationship to a block and can
 *               therefore be converted and dispatched directly, while UMTS
 *               formatted CB messages can span several message blocks and needs
 *               to be collected before being converted and dispatched.
 *
 *               For GSM or ETWS CB messages the caller can optionally dispatch
 *               the CB message or have the converted PDU:s returned. The CB message blocks
 *               that are part of a UMTS CB message are collected into the supplied buffer.
 *
 * @param [in]   Client_p  : Subscribed client to which PDU should be dispatched.
 *               Message_p : CB Routing indication message block.
 *               Dispatch  : TRUE if message should be dispatched, FALSE if not.
 *
 * @param [out]  Buffer_p     : The UMTS CB message buffer
 * @param [out]  PduLength_p  : The length of the converted PDU
 * @param [out]  Pdu_pp       : The converted PDU
 *
 * @return       CBS_Error_t
 */
/********************************************************************/
CBS_Error_t CBS_Manager_HandleMessage(const CBS_Subscribe_ClientWithClientTagList_t * const Client_p,
                                      const CBS_Message_t * const Message_p,
                                      const bool Dispatch,
                                      CBS_MessagesList_t *Buffer_p,
                                      uint16_t *PduLength_p,
                                      uint8_t **Pdu_pp)
{
    CBS_Error_t Result = CBS_ERROR_NONE;

    if (CB_MESSAGE_ETWS_VALUE_MIN <= Message_p->Header.MessageIdentifier &&
        CB_MESSAGE_ETWS_VALUE_MAX >= Message_p->Header.MessageIdentifier) {
        uint16_t PduLength = 0;
        uint8_t *Pdu_p = NULL;

        Result = CBS_Manager_ConvertEtwsMessageToPdu(Message_p, &PduLength, &Pdu_p);

        if (CBS_ERROR_NONE == Result) {
            if (Dispatch) {
                CBS_Manager_Dispatch(Client_p, PduLength, Pdu_p);
                SMS_HEAP_FREE(&Pdu_p);
            } else {
                *PduLength_p = PduLength;
                *Pdu_pp = Pdu_p;
            }
        } else {
            SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_HandleMessage: Could not create ETWS PDU"));
        }
    } else if (0xf < Message_p->Header.PageParameter) {
        uint16_t PduLength = 0;
        uint8_t *Pdu_p = NULL;

        Result = CBS_Manager_ConvertGsmMessageToPdu(Message_p, &PduLength, &Pdu_p);

        if (CBS_ERROR_NONE == Result) {
            if (Dispatch) {
                CBS_Manager_Dispatch(Client_p, PduLength, Pdu_p);
                SMS_HEAP_FREE(&Pdu_p);
            } else {
                *PduLength_p = PduLength;
                *Pdu_pp = Pdu_p;
            }
        } else {
            SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_HandleMessage: Could not create GSM PDU"));
        }
    } else if (0xf >= Message_p->Header.PageParameter) {
        CBS_MessagesList_t *Node_p = (CBS_MessagesList_t *) SMS_HEAP_ALLOC(CBS_MessagesList_t);
        if (NULL == Node_p) {
            SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_HandleMessage: Node_p is NULL"));
            Result = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
            goto exit;
        }
        Node_p->Message = *Message_p;
        Node_p->Next_p = Buffer_p->Next_p;
        Buffer_p->Next_p = Node_p;
    } else {
        SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_HandleMessage: Could not determine message type"));
        Result = CBS_ERROR_CS_INTERNAL_ERROR;
    }

exit:
    return Result;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_HandleNewIndication
 *
 * @description  Handle the receipt of new CB indication from the modem. This
 *               function will cache its messages and get a list of any
 *               clients which have matching subscriptions. If there are
 *               any clients, then the CB messages are dispatched to them.
 *
 * @param [in]   CbRoutingIndData_p : CB Messages indication.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Manager_HandleNewCbIndication(
    const SMS_MAL_Callback_CB_Routing_Ind_Data_t * const CbRoutingIndData_p)
{
    CBS_Subscribe_ClientWithClientTagList_t *Clients_p = NULL;
    CBS_Subscribe_ClientWithClientTagList_t *Client_p = NULL;
    uint8_t i;
    uint8_t CB_Sent_to_AT = 0;
    uint8_t CB_Sent_to_AT_UMTS=0;

    SMS_B_(SMS_LOG_D("cbmngr: Enter CBS_Manager_HandleNewIndication"));

    if (NULL == CbRoutingIndData_p) {
        SMS_A_(SMS_LOG_E("cbmngr: CBS_Manager_HandleNewIndication: CbRoutingIndData_p is NULL"));
        return;
    }

    /* Add CB messages to cache */
    for (i = 0; i < CbRoutingIndData_p->number_of_messages; i++) {
        CBS_Msgs_StoreMessage((CBS_Message_t *) &CbRoutingIndData_p->messages[i]);
    }

    /* Send CB messages to the clients that are subscribed to it */
    Clients_p = CBS_Subscribe_GetClientList();

    for (Client_p = Clients_p; NULL != Client_p; Client_p = Client_p->Next_p) {
        CBS_MessagesList_t UmtsBuffer = { {{ 0, 0, 0, 0 }, 0, { 0 }}, NULL };
        uint8_t i;

        for (i = 0; i < CbRoutingIndData_p->number_of_messages; i++) {
            if (IsClientSubscribedToMessage(Client_p, &CbRoutingIndData_p->messages[i].header, CBS_StateInfo.CBS_Activated_Client)) {
                if (Client_p->IsSim) {
                    CBS_Manager_CAT_DataDownload((CBS_Message_t *) &CbRoutingIndData_p->messages[i]);
                } else {
                    if(CB_Sent_to_AT == 0) {
                    (void) CBS_Manager_HandleMessage(Client_p, (CBS_Message_t *) &CbRoutingIndData_p->messages[i], TRUE, &UmtsBuffer, NULL, NULL);
                    CB_Sent_to_AT = 1;
                    }
                }
            }
        }

        if (NULL != UmtsBuffer.Next_p && (CB_Sent_to_AT_UMTS == 0)) {
            CBS_MessagesList_t *Tmp_p = NULL;
            CB_Sent_to_AT_UMTS = 1;
            (void) CBS_Manager_DispatchUmtsBuffer(Client_p, UmtsBuffer.Next_p);
            while (NULL != UmtsBuffer.Next_p) {
                Tmp_p = UmtsBuffer.Next_p;
                UmtsBuffer.Next_p = Tmp_p->Next_p;
                SMS_HEAP_FREE(&Tmp_p);
            }
        }
    }

    while (NULL != Clients_p) {
        Client_p = Clients_p;
        Clients_p = Client_p->Next_p;
        SMS_HEAP_FREE(&Client_p);
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit CBS_Manager_HandleNewIndication"));
}

/********************************************************************/
/**
 *
 * @function     CBS_Manager_Init
 *
 * @description  Initialise the CB Server data structures and ensure
 *               it is ready to start processing requests from clients.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Manager_Init(
    void)
{
    SMS_A_(SMS_LOG_I("cbmngr.c: CBS_Manager_Init called"));
    // Initialise state / system information.
    CBS_StateInfo.CBS_Activated_Client = FALSE;
    CBS_StateInfo.CBS_Activated_CBMID = FALSE;
    CBS_StateInfo.Modem_Activated = FALSE;
#ifndef SMS_USE_MFL_MODEM
    CBS_StateInfo.MAL_SubscriptionNumberRxd = FALSE;
    CBS_StateInfo.MAL_SubscriptionNumber = 0;
#endif
}


#ifndef SMS_USE_MFL_MODEM
/********************************************************************/
/**
 *
 * @function     CBS_Manager_Init_MAL_Info
 *
 * @description  When a CB subscription to the MAL modem is successfully
 *               enabled, a callback is received which contains the
 *               Subscription Number. This function is used to store
 *               that value in the CBS data structures.
 *
 * @param [in]   SubscriptionNumber : Subscription Number returned by
 *                                    MAL Modem
 *
 * @return       void
 */
/********************************************************************/
void CBS_Manager_Init_MAL_Info(
    const uint8_t SubscriptionNumber)
{
    CBS_StateInfo.MAL_SubscriptionNumberRxd = TRUE;
    CBS_StateInfo.MAL_SubscriptionNumber = SubscriptionNumber;
}
#endif


/********************************************************************/
/**
 *
 * @function     CBS_Manager_CBMID_Size_Get
 *
 * @description  Gets the size of the EFcbmid file on the SIM card.
 *
 * @param [out]  FileSize_p : Size of EFcbmid file.
 *
 * @return       TRUE if size obtained successfully, else FALSE.
 */
/********************************************************************/
static uint8_t CBS_Manager_CBMID_Size_Get(
    int *const FileSize_p)
{
    uint8_t Result = FALSE;
    int sim_result = -1;
    ste_sim_t *sim_sync;
    uintptr_t client_tag;
    ste_uicc_sim_file_get_format_response_t file_format = {
        STE_UICC_STATUS_CODE_FAIL,
        STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
        {0x00, 0x00},
        SIM_FILE_STRUCTURE_UNKNOWN,
        0,
        0,
        0
    };

    SMS_B_(SMS_LOG_D("cbmngr: Enter CBMID_Size_Get"));

    sim_sync = MessagingSystem_UICC_SimSyncGet();
    client_tag = (uintptr_t) sim_sync;

    SMS_B_(SMS_LOG_D("calling: ste_uicc_sim_file_get_format"));

    sim_result = ste_uicc_sim_file_get_format_sync(sim_sync, client_tag, EF_CBMID_FILENAME, EF_FILEPATH, &file_format);

    if (sim_result == 0 && STE_UICC_STATUS_CODE_OK == file_format.uicc_status_code) {
        *FileSize_p = file_format.file_size;
        Result = TRUE;
        SMS_B_(SMS_LOG_D("Manager_CBMID_Size_Get file_format.file_size = %d ", file_format.file_size));
        SMS_B_(SMS_LOG_D("Manager_CBMID_Size_Get file_format.record_len=%d file_format.num_records=%d", file_format.record_len, file_format.num_records));
    } else {
        SMS_A_(SMS_LOG_E("Manager_CBMID_Size_Get Error sim_result = %d ", sim_result));
        SMS_A_(SMS_LOG_E("Manager_CBMID_Size_Get Error file_format.status_code_fail_details = %d ", file_format.uicc_status_code_fail_details));
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit CBMID_Size_Get: Result=%d", Result));
    return Result;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_CBMID_Read
 *
 * @description  Gets the size of the EFcbmid file on the SIM card.
 *
 * @param [in]   FileSize : Size of EFcbmid file to be read. This must
 *                          be the same as the size of the buffer.
 * @param [out]  CBMID_DataBuf_p : Buffer into which the the EFcbmid
 *                                 data is stored. Buffer must be at
 *                                 least FileSize bytes in size.
 *
 * @return       TRUE if file is read successfully, else FALSE.
 */
/********************************************************************/
static uint8_t CBS_Manager_CBMID_Read(
    const int FileSize,
    uint8_t * const CBMID_DataBuf_p)
{
    uintptr_t client_tag;
    ste_sim_t *sim_sync;
    int read_status;
    int file_name = EF_CBMID_FILENAME;
    uint8_t Result = FALSE;
    ste_uicc_sim_file_read_binary_response_t read_result = {
        STE_UICC_STATUS_CODE_FAIL,
        STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS,
        {0x00, 0x00},
        NULL,
        0
    };

    SMS_B_(SMS_LOG_D("cbmngr: Enter CBMID_Read"));

    sim_sync = MessagingSystem_UICC_SimSyncGet();
    client_tag = (uintptr_t) sim_sync;

    read_status = ste_uicc_sim_file_read_binary_sync(sim_sync, client_tag, file_name, EF_CBMID_FILE_READ_OFFSET, FileSize, EF_FILEPATH, &read_result);

    if (read_status == 0 && read_result.uicc_status_code == STE_UICC_STATUS_CODE_OK) {
        if (read_result.length == FileSize && read_result.data != NULL) {
            memcpy(CBMID_DataBuf_p, read_result.data, FileSize);
            Result = TRUE;
        } else {
            SMS_A_(SMS_LOG_E("cbmngr: CBMID_Read: ste_uicc_sim_file_read_binary_sync: ERROR: read_result.length %d, read_result.data %sNULL.",
                             read_result.length, read_result.data != NULL ? "NOT " : ""));
        }
        if (read_result.data != NULL) {
            free(read_result.data);
        }
    } else {
        SMS_A_(SMS_LOG_E("cbmngr: CBMID_Read: ste_uicc_sim_file_read_binary_sync: ERROR: read_status %d, uicc_status_code %d.",
                         read_status, read_result.uicc_status_code));
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit CBMID_Read: Result=%d", Result));
    return Result;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_Init_CBMID
 *
 * @description  Controls subscription to MID values defined in the
 *               EFcmbid file.
 *               1 Get size of EFcbmid file.
 *               2 Read EFcbmid file.
 *               3 Iterate through MID values in EFcbmid file and for each:
 *                 a Check MID is valid (not 0xFFFF).
 *                 b Build up data structure.
 *               4 Pass data structure from 3 into CBS_Manager_MessageSubscribe().
 *               5 Ensure that the modem is in the correct state to receive any CB
 *                 messages needed for EFcbmid subscriptions.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Manager_Init_CBMID(
    void)
{
    uint8_t CurrentCBS_Activated_CBMID_State;
    int CBMID_FileSize = 0;

    SMS_B_(SMS_LOG_D("cbmngr: Enter Init_CBMID"));

    // Set CBS_Activated_CBMID to FALSE because we don't yet know if we have any valid EFcbmid values on the
    // SIM card.
    CurrentCBS_Activated_CBMID_State = CBS_StateInfo.CBS_Activated_CBMID;
    CBS_StateInfo.CBS_Activated_CBMID = FALSE;

    if (CBS_Manager_CBMID_Size_Get(&CBMID_FileSize)) {
        uint8_t *CBMID_DataBuf_p = SMS_HEAP_UNTYPED_ALLOC(CBMID_FileSize);

        if (CBMID_DataBuf_p != NULL) {
            if (CBS_Manager_CBMID_Read(CBMID_FileSize, CBMID_DataBuf_p)) {
                // Convert the data in CBMID_Data_Buf_p into a structure which can be
                // passed to CBS_Manager_MessageSubscribe().
                CBS_Subscription_t *SubscriptionData_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(CBS_Subscription_t) * (CBMID_FileSize / EF_CBMID_OCTETS_PER_MID));

                if (SubscriptionData_p != NULL) {
                    int Index;
                    const uint8_t *CBMID_DataIndex_p = CBMID_DataBuf_p;
                    CBS_Subscription_t *SubscriptionIndex_p = SubscriptionData_p;
                    uint8_t NumberOfSubscriptions = 0;

                    for (Index = 0; Index < CBMID_FileSize / EF_CBMID_OCTETS_PER_MID; Index++) {
                        CBS_MessageIdentifier_t MID;
                        uint8_t HiByte = *CBMID_DataIndex_p++;
                        uint8_t LoByte = *CBMID_DataIndex_p++;

                        // Build MID from data in EFcbmid file.
                        MID = (CBS_MessageIdentifier_t) ((HiByte << 8) | LoByte);
                        SMS_B_(SMS_LOG_D("CBS_Manager_Init_CBMID: MID of 0x%04X on SIM", MID));

                        // Validate MID
                        if (MID != EF_CBMID_UNUSED_ENTRY) {
                            // Subscribe to single MID, not a range.
                            SubscriptionIndex_p->FirstMessageIdentifier = MID;
                            SubscriptionIndex_p->LastMessageIdentifier = MID;
                            // Subscribe to entire DCS range.
                            SubscriptionIndex_p->FirstDataCodingScheme = CB_MESSAGE_DATA_DCS_VALUE_MIN;
                            SubscriptionIndex_p->LastDataCodingScheme = CB_MESSAGE_DATA_DCS_VALUE_MAX;
                            SubscriptionIndex_p++;
                            NumberOfSubscriptions++;
                        } else {
                            // It is likely that there will be MIDs of 0XFFFF on the SIM card as this is what should
                            // be used to fill unused octets.
                            SMS_C_(SMS_LOG_V("CBS_Manager_Init_CBMID: MID of 0xFFFF on SIM ignored"));
                        }
                    }

                    // Now add subscription to CBS Server.
                    if (NumberOfSubscriptions > 0) {
                        CBS_Error_t Error = CBS_Manager_MessageSubscribe(SIM_CLIENT_PROCESS, SIM_CLIENT_TAG, FALSE, TRUE, NumberOfSubscriptions, SubscriptionData_p, NULL);

                        SMS_B_(SMS_LOG_D("CBS_Manager_Init_CBMID: CBS_Manager_MessageSubscribe() returned %d.", Error));
                        if (Error == CBS_ERROR_NONE) {
                            // SIM contains valid CBMID values so we need to enable the modem to receive the messages.
                            // Set the CBS State flag here which is picked up at the end of this function.
                            CBS_StateInfo.CBS_Activated_CBMID = TRUE;
                        }
                    }

                    SMS_HEAP_FREE(&SubscriptionData_p);
                } else {
                    SMS_A_(SMS_LOG_E("CBS_Manager_Init_CBMID: Malloc SubscriptionData_p Failed"));
                }
            } else {
                SMS_A_(SMS_LOG_E("CBS_Manager_Init_CBMID: Read Failed"));
            }

            SMS_HEAP_FREE(&CBMID_DataBuf_p);
        } else {
            SMS_A_(SMS_LOG_E("CBS_Manager_Init_CBMID: Malloc CBMID_DataBuf_p Failed"));
        }
    } else {
        // If the SIM does not have a EFcbmid file, then it is impossible to get its size.
        SMS_B_(SMS_LOG_E("CBS_Manager_Init_CBMID: FileSize Get Failed"));
    }

    // Activate the modem, if necessary
    SMS_B_(SMS_LOG_D("CBS_Manager_Init_CBMID: Current_Activated_CBMID=%d, New Activated_CBMID=%d.", CurrentCBS_Activated_CBMID_State, CBS_StateInfo.CBS_Activated_CBMID));
    if (CurrentCBS_Activated_CBMID_State != CBS_StateInfo.CBS_Activated_CBMID) {
        // The CBS state flags have changed, so check the activation state.
        CBS_Manager_CheckActivation();
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit Init_CBMID"));
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_EnableModem
 *
 * @description  Enable the modem so that CB messages are received. (If
 *               modem is already enabled, just return TRUE and do
 *               nothing else.)
 *
 * @return       TRUE modem is enabled, FALSE modem is not enabled.
 */
/********************************************************************/
static uint8_t CBS_Manager_EnableModem(
    void)
{
    uint8_t ModemActivated = FALSE;

    SMS_B_(SMS_LOG_D("cbmngr: Enter EnableModem"));
    SMS_A_(SMS_LOG_I("smsmain.c: call CBS_Manager_DisableModem()"));
    if (CBS_StateInfo.Modem_Activated) {
        // Modem already activated
        ModemActivated = TRUE;
        SMS_A_(SMS_LOG_I("cbmngr: EnableModem: Already Activated"));
    } else {
#ifdef SMS_USE_MFL_MODEM
        // Support for MFL Modem
        SMS_A_(SMS_LOG_I("cbmngr: ****** MFL Modem not supported ******"));
#else
        // Support for MAL Modem.
#define MAL_CBS_REQUEST_ROUTING_SET_SUBSCRIBE_TO_ALL   1
#define MAL_CBS_REQUEST_ROUTING_SET_NEW_SUBSCRIPTION   0
#define MAL_CBS_REQUEST_ROUTING_SET_NOT_SIM            0

        int ret;
        MAL_CBS_MessageIdentifier_t CBMI_List = 0;      // Stub value to pass in to function. Ignored as we're subscribing to all.
        MAL_CBS_DataCodingScheme_t DCS_List = 0;        // Stub value to pass in to function. Ignored as we're subscribing to all.

        ret = mal_cbs_request_routing_set(MAL_CBS_REQUEST_ROUTING_SET_SUBSCRIBE_TO_ALL, MAL_CBS_REQUEST_ROUTING_SET_NEW_SUBSCRIPTION, MAL_CBS_REQUEST_ROUTING_SET_NOT_SIM, &CBMI_List, 0,       // CBMI List Length
                                          &DCS_List, 0);        // DCS List Length

        if (ret == MAL_SMS_CBS_SUCCESS) {
            ModemActivated = TRUE;
            SMS_A_(SMS_LOG_I("cbmngr: EnableModem: Activated"));
        } else {
            SMS_A_(SMS_LOG_E("cbmngr: EnableModem: ERROR mal returned %d", ret));
        }
#endif
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit EnableModem: ModemActivated=%d", ModemActivated));

    return ModemActivated;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_ToggleModemActivationState
 *
 * @description  Toggles the modem activation flag.
 *
 * @return       -
 */
/********************************************************************/

void CBS_Manager_ToggleModemActivationState(void)
{
    CBS_StateInfo.Modem_Activated = !CBS_StateInfo.Modem_Activated;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_DisableModem
 *
 * @description  Disable the modem so that CB messages are note received.
 *               (If modem is already disabled, just return TRUE and
 *               do nothing else.)
 *
 * @return       TRUE modem is disabled, FALSE modem is not disabled.
 */
/********************************************************************/
static uint8_t CBS_Manager_DisableModem(
    void)
{
    uint8_t ModemDisabled = FALSE;

    SMS_B_(SMS_LOG_D("cbmngr: Enter DisableModem"));

    if (!CBS_StateInfo.Modem_Activated) {
        // Modem already deactivated
        ModemDisabled = TRUE;
        SMS_A_(SMS_LOG_I("cbmngr: DisableModem: Already Deactivated"));
    } else {
#ifdef SMS_USE_MFL_MODEM
        // Support for MFL Modem
        SMS_A_(SMS_LOG_I("cbmngr: ****** MFL Modem not supported ******"));
#else
        // Support for MAL Modem.
        if (CBS_StateInfo.MAL_SubscriptionNumberRxd) {
            int ret = mal_cbs_request_routing_remove(CBS_StateInfo.MAL_SubscriptionNumber);

            if (ret == MAL_SMS_CBS_SUCCESS) {
                ModemDisabled = TRUE;
                CBS_StateInfo.MAL_SubscriptionNumberRxd = FALSE;
                SMS_A_(SMS_LOG_I("cbmngr: DisableModem: Deactivated"));
                // Clear cache of CB Messages previously received.
                CBS_Msgs_ClearCachedMessages();
            } else {
                SMS_A_(SMS_LOG_E("cbmngr: DisableModem: ERROR mal returned %d", ret));
            }
        } else {
            SMS_A_(SMS_LOG_E("cbmngr: DisableModem: ERROR no subscription number"));
        }
#endif
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit DisableModem: ModemDisabled=%d", ModemDisabled));

    return ModemDisabled;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_ActivateModem
 *
 * @description  Checks if any clients have supplied subscriptions. If
 *               they have, try and enable the modem to receive CB
 *               Messages.
 *
 * @return       Error value.
 */
/********************************************************************/
static CBS_Error_t CBS_Manager_ActivateModem(
    void)
{
    CBS_Error_t Error = CBS_ERROR_NONE;

    SMS_B_(SMS_LOG_D("cbmngr: Enter ActivateModem"));
    // Although CB Server has been "activated" by a client. We only enable the modem if we
    // also have some subscriptions.
    if (CBS_Subscribe_CheckForSubscriptions()) {
        // Subscriptions exist
        CBS_StateInfo.Modem_Activated = CBS_Manager_EnableModem();

        if (!CBS_StateInfo.Modem_Activated) {
            Error = CBS_ERROR_RADIO_INACTIVE;
        }
    } else {
        // No subscriptions exist
        SMS_B_(SMS_LOG_D("cbmngr.c: CBS_Manager_ActivateModem: No Subscriptions"));
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit DisableModem: Error=%d", Error));

    return Error;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_DeactivateModem
 *
 * @description  Control the deactivation of the modem.
 *
 * @return       Error value.
 */
/********************************************************************/
static CBS_Error_t CBS_Manager_DeactivateModem(
    void)
{
    CBS_Error_t Error;

    SMS_B_(SMS_LOG_D("cbmngr: Enter DeactivateModem"));

    CBS_StateInfo.Modem_Activated = !CBS_Manager_DisableModem();

    if (!CBS_StateInfo.Modem_Activated) {
        Error = CBS_ERROR_NONE;
    } else {
        // Error could occur if a deactivate is attempted before the modem callback for
        // a successful activation had been processed.
        Error = CBS_ERROR_TEMPORARY_FAILURE;
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit DeactivateModem: Error=%d", Error));

    return Error;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_Activate
 *
 * @description  Control the activation of the CB Server.
 *
 * @return       Error value.
 */
/********************************************************************/
CBS_Error_t CBS_Manager_Activate(
    void)
{
    CBS_Error_t Error = CBS_ERROR_NONE;

    SMS_B_(SMS_LOG_D("cbmngr: Enter Activate"));

    if (CBS_StateInfo.CBS_Activated_Client) {
        SMS_B_(SMS_LOG_D("cbmngr.c: CBS_Manager_Activate: Already Activated"));
    } else {
        Error = CBS_Manager_ActivateModem();

        if (Error == CBS_ERROR_NONE) {
            CBS_StateInfo.CBS_Activated_Client = TRUE;
        }
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit Activate: Error=%d", Error));

    return Error;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_Deactivate
 *
 * @description  Control the deactivation of the CB Server.
 *
 * @return       Error value.
 */
/********************************************************************/
CBS_Error_t CBS_Manager_Deactivate(
    void)
{
    CBS_Error_t Error = CBS_ERROR_NONE;

    SMS_B_(SMS_LOG_D("cbmngr: Enter Deactivate"));

    if (!CBS_StateInfo.CBS_Activated_Client) {
        SMS_B_(SMS_LOG_D("cbmngr.c: CBS_Manager_Deactivate: Already Deactivated"));
    } else {
        CBS_StateInfo.CBS_Activated_Client = FALSE;

        // We must not deactivate the modem if there are any active subscriptions from the
        // EFcbmid file.
        if (!CBS_StateInfo.CBS_Activated_CBMID) {
            Error = CBS_Manager_DeactivateModem();
        } else {
            SMS_B_(SMS_LOG_D("cbmngr.c: CBS_Manager_Deactivate: CBMID subscriptions exist"));
            // Returning CBS_ERROR_NONE as there is no other suitable, or more meaningful, value.
        }
    }

    SMS_B_(SMS_LOG_D("cbmngr: Exit Deactivate: Error=%d", Error));

    return Error;
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_CheckActivation
 *
 * @description  This function checks the enabled / disabled state of
 *               the modem after a subscribe / unsubscribe request from
 *               a client.  This is because the modem is not actually
 *               enabled unless there is at least one subscription
 *               from a client.  Similarly, if the modem is enabled,
 *               but the last subscription is removed by a client, the
 *               modem is then disabled. This is to ensure that we don't
 *               handle CB messages when there is no client to
 *               receive them.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Manager_CheckActivation(
    void)
{
    SMS_B_(SMS_LOG_D("cbmngr.c: Enter CheckActivation"));
    SMS_A_(SMS_LOG_I("smsmain.c: CBS_Manager_CheckActivation()"));
    // Check if we have any valid subscriptions
    if (CBS_Subscribe_CheckForSubscriptions()) {
        // Subscriptions exist, ensure modem is activated if CBS has already been "activated".
        // N.B. CBS could be activated by a client or by valid subscriptions from the EFcbmid file.
        if (CBS_StateInfo.CBS_Activated_CBMID || CBS_StateInfo.CBS_Activated_Client) {
            CBS_StateInfo.Modem_Activated = CBS_Manager_EnableModem();
        }
    } else {
        // No subscriptions exist, ensure modem is deactivated.
        CBS_StateInfo.Modem_Activated = !CBS_Manager_DisableModem();
    }

    SMS_B_(SMS_LOG_D("cbmngr.c: Exit CheckActivation"));
}


/********************************************************************/
/**
 *
 * @function     CBS_Manager_CAT_Refresh
 *
 * @description  Function called when a CAT Refresh has occurred which
 *               could have changed the contents of the EFcbmid file.
 *               Controls the removal of any existing EFcbmid subscriptions
 *               and then re-subscribes to any MID values in the EFcbmid
 *               file.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Manager_CAT_Refresh(
    void)
{
    SMS_B_(SMS_LOG_D("cbmngr.c: Enter CAT_Refresh"));

    // Remove all current EFcbmid subscriptions. This will not update the modem "activated" state.
    CBS_Subscribe_RemoveAllSimSubscriptions();

    // Re-add all EFcbmid subscriptions. This will ensure that the modem "activated" state is updated, if necessary.
    CBS_Manager_Init_CBMID();

    SMS_B_(SMS_LOG_D("cbmngr.c: Exit CAT_Refresh"));
}
