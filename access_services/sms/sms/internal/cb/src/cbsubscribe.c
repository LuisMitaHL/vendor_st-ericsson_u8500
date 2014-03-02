/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     Data and functions for the subscriber management
*
********************************************************************************************
*/

/*
**========================================================================
**    Global declarations
**========================================================================
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "t_cbs.h"
#include "cbsubscribe.h"

#include "r_cbslinuxporting.h"
#include "smutil.h"

/*
**========================================================================
** Defines.
**========================================================================
*/

//CB messages with DCS equals 15
#define CB_DATA_DCS_UNSPECIFIED_LANGUAGE 15

/*
**========================================================================
**    Local declarations
**========================================================================
*/

// Data structure used to hold linked list of subscriptions.
typedef struct CBS_SubscriberList_tag {
    struct CBS_SubscriberList_tag *Next_p;
    SMS_PROCESS Client;
    SMS_ClientTag_t ClientTag;
    uint8_t IsSim;              // For subscriptions from EFcbmid
    uint8_t SubscribeToAll;
    CBS_Subscription_t Subscription;
} CBS_SubscriberList_t;




/*
**========================================================================
**    Local function declarations
**========================================================================
*/


static CBS_SubscriberList_t *Find_Matching_Subscription(
    const SMS_PROCESS Client,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p,
    CBS_SubscriberList_t ** ListParent_pp);

static CBS_SubscriberList_t *Find_Sim_Subscription(
    CBS_SubscriberList_t ** ListParent_pp);

static CBS_SubscriberList_t *SubscriberList_Add(
    const SMS_PROCESS Client,
    const SMS_ClientTag_t ClientTag,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p);

static uint8_t SubscriberList_Remove(
    const SMS_PROCESS Client,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p);

static uint8_t CBS_Subscribe_FindClientInClientList(
    const SMS_PROCESS Client,
    const SMS_ClientTag_t ClientTag,
    const uint8_t IsSim,
    const CBS_Subscribe_ClientWithClientTagList_t * ClientList_p);

static void CBS_NumberOfSubscriptions_Incr(
    void);

static void CBS_NumberOfSubscriptions_Decr(
    void);

/*
**========================================================================
**    Local variable definitions.
**========================================================================
*/

static CBS_SubscriberList_t *CBS_SubscriberList_p = NULL;       // Head of linked list of subscriptions.
static uint32_t CBS_NumberOfSubscriptions = 0;  // Number of subscriptions.

/*
**========================================================================
**    External Functions
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     CBS_Subscribe_AddSubscription
 *
 * @description  Adds the supplied subscription to the list of subscriptions.
 *               Function checks whether we have reached the maximum number
 *               of permitted subscriptions and whether we already have a
 *               matching subscription; if we do we return an error code.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   SubscribeToAll : Flag indicating whether the client has
 *                                subscribed to all CB pages, or not.
 * @param [in]   IsSim : Flag indicating whether the subscription is
 *                       derived from EFcbmid records.
 * @param [in]   Subscription_p : Subscription data.
 *
 * @return       Error value.
 */
/********************************************************************/
CBS_Error_t CBS_Subscribe_AddSubscription(
    SMS_PROCESS Client,
    SMS_ClientTag_t ClientTag,
    uint8_t SubscribeToAll,
    uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p)
{
    CBS_Error_t Result;

    SMS_B_(SMS_LOG_D("cbsubscribe.c: AddSubscription: BEGIN..."));

    if (CBS_NumberOfSubscriptions >= CBS_MAX_NUMBER_SUBSCRIPTIONS) {
        Result = CBS_ERROR_TOO_MANY_SUBSCRIBERS;
    } else if (Find_Matching_Subscription(Client, SubscribeToAll, IsSim, Subscription_p, NULL) != NULL) {
        // Supplied subscription is a duplicate of an existing subscription
        Result = CBS_ERROR_ALREADY_SUBSCRIBED;
    } else {
        // Add new subscription to list CBS_SubscriberList_t
        if (SubscriberList_Add(Client, ClientTag, SubscribeToAll, IsSim, Subscription_p) != NULL) {
            Result = CBS_ERROR_NONE;
        } else {
            Result = CBS_ERROR_MEMORY_ALLOCATION_FAILED;
        }
    }

    SMS_B_(SMS_LOG_D("cbsubscribe.c: AddSubscription: END, Result = %d", Result));
    return Result;
}


/********************************************************************/
/**
 *
 * @function     CBS_Subscribe_RemoveSubscription
 *
 * @description  Removes the supplied subscription from the list of subscriptions.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   SubscribeToAll : Flag indicating whether the client has
 *                                subscribed to all CB pages, or not.
 * @param [in]   IsSim : Flag indicating whether the subscription is
 *                       derived from EFcbmid records.
 * @param [in]   Subscription_p : Subscription data.
 *
 * @return       Error value.
 */
/********************************************************************/
CBS_Error_t CBS_Subscribe_RemoveSubscription(
    SMS_PROCESS Client,
    uint8_t SubscribeToAll,
    uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p)
{
    CBS_Error_t Result;

    SMS_B_(SMS_LOG_D("cbsubscribe.c: RemoveSubscription: BEGIN..."));

    if (SubscriberList_Remove(Client, SubscribeToAll, IsSim, Subscription_p)) {
        Result = CBS_ERROR_NONE;
    } else {
        Result = CBS_ERROR_INVALID_SUBSCRIPTION;
    }

    SMS_B_(SMS_LOG_D("cbsubscribe.c: RemoveSubscription: END, Result = %d", Result));
    return Result;
}



/********************************************************************/
/**
 *
 * @function     CBS_Subscribe_RemoveAllSimSubscriptions
 *
 * @description  Iterate through all subscriptions and remove any which
 *               are from the SIM.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Subscribe_RemoveAllSimSubscriptions(
    void)
{
    uint8_t Removed;

    do {
        CBS_SubscriberList_t *PrevSubscriber_p = NULL;
        CBS_SubscriberList_t *MatchSubscriber_p;

        MatchSubscriber_p = Find_Sim_Subscription(&PrevSubscriber_p);

        if (MatchSubscriber_p != NULL) {
            // Match found, now remove it.
            if (PrevSubscriber_p == NULL) {
                // Match is head of linked list.
                CBS_SubscriberList_p = MatchSubscriber_p->Next_p;
            } else {
                // Match is somewhere in linked list other than head
                PrevSubscriber_p->Next_p = MatchSubscriber_p->Next_p;
            }

            Removed = TRUE;
            CBS_NumberOfSubscriptions_Decr();
            SMS_HEAP_FREE(&MatchSubscriber_p);
        } else {
            // No more matches found, so force exit of do...while loop.
            Removed = FALSE;
        }
    } while (Removed);
}


/********************************************************************/
/**
 *
 * @function     CBS_Subscribe_CheckForSubscriptions
 *
 * @description  Check if there are any subscriptions.
 *
 * @return       TRUE if there are subscriptions, else FALSE.
 */
/********************************************************************/
uint8_t CBS_Subscribe_CheckForSubscriptions(
    void)
{
    if (CBS_SubscriberList_p != NULL) {
        SMS_C_ASSERT_(CBS_NumberOfSubscriptions > 0);
        return TRUE;
    } else {
        SMS_C_ASSERT_(CBS_NumberOfSubscriptions == 0);
        return FALSE;
    }
}


/********************************************************************/
/**
 *
 * @function     CBS_Subscribe_CountSubscriptions
 *
 * @description  Count the number of subscriptions. Clients can specify
 *               their own subscriptions, or all subscriptions.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   AllSubscriptions : Flag indicating that we are to count
 *                                  all subscriptions from non-SIM clients,
 *                                  or just "this" client's.
 * @param [in]   IsSim : Flag indicating we are to count the subscriptions
 *                       derived from EFcbmid records.
 *
 * @return       Error value.
 */
/********************************************************************/
uint8_t CBS_Subscribe_CountSubscriptions(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t IsSim)
{
    uint8_t Count = 0;
    const CBS_SubscriberList_t *CurrSubscriber_p = CBS_SubscriberList_p;

    while (CurrSubscriber_p != NULL) {
        if (IsSim == CurrSubscriber_p->IsSim) {
            // Only count subscriptions which have come from the same source; i.e. if we
            // are counting subscriptions from a client, we ignore those which have
            // come from EFcbmid, or vice versa.
            // If we are counting all subscriptions, or those from the EFcbmid, then we don't care whether the
            // Client and ClientTag matches.

            if (AllSubscriptions || IsSim || CurrSubscriber_p->Client == Client) {
                SMS_C_ASSERT_(Count < 0xFF);
                Count++;
            }
        }

        CurrSubscriber_p = CurrSubscriber_p->Next_p;
    }

    return Count;
}


/********************************************************************/
/**
 *
 * @function     CBS_Subscribe_GetSubscriptions
 *
 * @description  Gets the subscriptions which match the supplied
 *               input parameters. Matching subscriptions are copied
 *               into the supplied buffer.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   AllSubscriptions : Flag indicating that we are to count
 *                                  all subscriptions from non-SIM clients,
 *                                  or just "this" client's.
 * @param [in]   NumberOfSubscriptions : The number of subscriptions which
 *                                       can be stored in the Subscription_p
 *                                       buffer.
 * @param [in]   IsSim : Flag indicating we are to count the subscriptions
 *                       derived from EFcbmid records.
 * @param [out]  Subscription_p : Output buffer for copying subscription data
 *                                into. Size of the buffer is large enough to
 *                                hold NumberOfSubscriptions * sizeof (CBS_Subscription_t)
 *                                entries.  N.B. this buffer must already have
 *                                been malloc'd.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Subscribe_GetSubscriptions(
    const SMS_PROCESS Client,
    const uint8_t AllSubscriptions,
    const uint8_t NumberOfSubscriptions,
    const uint8_t IsSim,
    CBS_Subscription_t * const Subscription_p)
{
    uint8_t SubscriptionCount = 0;
    const CBS_SubscriberList_t *CurrSubscriber_p = CBS_SubscriberList_p;
    CBS_Subscription_t *CurrSubscription_p = Subscription_p;

    while (CurrSubscriber_p != NULL && SubscriptionCount < NumberOfSubscriptions) {
        if (IsSim == CurrSubscriber_p->IsSim) {
            // Only get subscriptions which have come from the same source; i.e. if we
            // are getting subscriptions from a client, we ignore those which have
            // come from EFcbmid, or vice versa.

            // If we are getting all subscriptions, or those from the EFcbmid, then we don't care whether the
            // Client and ClientTag matches.
            if (AllSubscriptions || IsSim || CurrSubscriber_p->Client == Client) {
                // Match found, so copy the subscription information into the output buffer.
                CurrSubscription_p->FirstMessageIdentifier = CurrSubscriber_p->Subscription.FirstMessageIdentifier;
                CurrSubscription_p->LastMessageIdentifier = CurrSubscriber_p->Subscription.LastMessageIdentifier;
                CurrSubscription_p->FirstDataCodingScheme = CurrSubscriber_p->Subscription.FirstDataCodingScheme;
                CurrSubscription_p->LastDataCodingScheme = CurrSubscriber_p->Subscription.LastDataCodingScheme;
                CurrSubscription_p++;
                SubscriptionCount++;
            }
        }

        CurrSubscriber_p = CurrSubscriber_p->Next_p;
    }
}



/********************************************************************/
/**
 *
 * @function     CBS_Subscribe_GetClientList
 *
 * @description  Iterates through all the subscriptions looking for
 *               any which match the supplied MID and DCS values. If
 *               a match is found, an entry is added to a linked list
 *               of clients.
 *
 * @param [in]   MessageIdentifier : Message ID (MID) of CB Page.
 * @param [in]   DataCodingScheme : Data Coding Scheme (DCS) of CB Page.
 * @param [in]   IsClientActivated : TRUE if a client has activated the
 *                                   CB Server. (It is possible that the
 *                                   CB Server is activate as a result of
 *                                   EFcbmid subscriptions but we don't
 *                                   want to send any CB pages to clients
 *                                   if they haven't activated CBS.
 *
 * @return       Linked list of clients with subscriptions which match
 *               supplied data.
 */
/********************************************************************/

CBS_Subscribe_ClientWithClientTagList_t *CBS_Subscribe_GetClientList(void)
{
    CBS_Subscribe_ClientWithClientTagList_t *ClientListHead_p = NULL;
    const CBS_SubscriberList_t *SubscriberList_p = CBS_SubscriberList_p;

    while (SubscriberList_p != NULL) {
        CBS_Subscribe_ClientWithClientTagList_t *New_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(CBS_Subscribe_ClientWithClientTagList_t));

        if (New_p != NULL) {
            // New node created. Add it to head of linked list.
            SMS_B_(SMS_LOG_D("cbsubscribe: GetClientList: Adding Client %d with Client Tag %d, IsSim %d.", SubscriberList_p->Client, SubscriberList_p->ClientTag, SubscriberList_p->IsSim));
            New_p->Next_p = ClientListHead_p;
            New_p->Client = SubscriberList_p->Client;
            New_p->ClientTag = SubscriberList_p->ClientTag;
            New_p->IsSim = SubscriberList_p->IsSim;
            ClientListHead_p = New_p;
         } else {
             SMS_A_(SMS_LOG_E("cbsubscribe: GetClientList: ERROR Malloc failed"));
         }
         SubscriberList_p = SubscriberList_p->Next_p;
    }
    return ClientListHead_p;
}


uint8_t IsClientSubscribedToMessage(CBS_Subscribe_ClientWithClientTagList_t *Client_p,
                                    const CBS_MessageHeader_t * const Header_p,
                                    const uint8_t IsClientActivated)
{
    uint8_t MatchFound = FALSE;
    const CBS_SubscriberList_t *SubscriberList_p = CBS_SubscriberList_p;

    while (SubscriberList_p != NULL) {
        if (Client_p->Client == SubscriberList_p->Client &&
            Client_p->ClientTag == SubscriberList_p->ClientTag) {
            if (IsClientActivated || SubscriberList_p->IsSim) {
                // Check if the supplied MID and DCS match the subscription currently be processed,
                // or the DCS equals CB_DATA_DCS_UNSPECIFIED_LANGUAGE, DCS match!.
                if (SubscriberList_p->SubscribeToAll ||
                    (SubscriberList_p->Subscription.FirstMessageIdentifier <= Header_p->MessageIdentifier &&
                    SubscriberList_p->Subscription.LastMessageIdentifier >= Header_p->MessageIdentifier &&
                    ((SubscriberList_p->Subscription.FirstDataCodingScheme <= Header_p->DataCodingScheme && SubscriberList_p->Subscription.LastDataCodingScheme >= Header_p->DataCodingScheme) ||
                    (SubscriberList_p->Subscription.FirstDataCodingScheme <= CB_DATA_DCS_UNSPECIFIED_LANGUAGE && SubscriberList_p->Subscription.LastDataCodingScheme >= CB_DATA_DCS_UNSPECIFIED_LANGUAGE)))) {
                    MatchFound = TRUE;
                    break;
                }
            }
        }
        SubscriberList_p = SubscriberList_p->Next_p;
    }

    return MatchFound;
}


/*
**========================================================================
**    Local Functions
**========================================================================
*/


/********************************************************************/
/**
 *
 * @function     CBS_NumberOfSubscriptions_Incr
 *
 * @description  Increment the "Number Of Subscriptions" counter.
 *
 * @return       void
 */
/********************************************************************/
static void CBS_NumberOfSubscriptions_Incr(
    void)
{
    SMS_C_ASSERT_(CBS_NumberOfSubscriptions < 0xFFFFFFFF);

    CBS_NumberOfSubscriptions++;
}


/********************************************************************/
/**
 *
 * @function     CBS_NumberOfSubscriptions_Decr
 *
 * @description  Decrement the "Number Of Subscriptions" counter.
 *
 * @return       void
 */
/********************************************************************/
static void CBS_NumberOfSubscriptions_Decr(
    void)
{
    if (CBS_NumberOfSubscriptions > 0) {
        CBS_NumberOfSubscriptions--;
    } else {
        SMS_A_(SMS_LOG_E("CBS: cbsubscribe: NumberOfSubscriptions_Decr: ERROR Counter is zero"));
    }
}


/********************************************************************/
/**
 *
 * @function     Find_Matching_Subscription
 *
 * @description  Iterate through list of subscriptions and look for a
 *               match. If it is found, return its entry in the linked
 *               list.  Optionally, it is also possible to return its
 *               parent in the linked list if ListParent_pp != NULL.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   SubscribeToAll : Flag indicating whether the client has
 *                                subscribed to all CB pages, or not.
 * @param [in]   IsSim : Flag indicating whether the subscription is
 *                       derived from EFcbmid records.
 * @param [in]   Subscription_p : Subscription data to match against.
 * @param [out]  ListParent_pp : Address of matching node's parent in
 *                               linked list.
 *
 * @return       Node in linked list which matches supplied subscription
 *               data.
 */
/********************************************************************/
static CBS_SubscriberList_t *Find_Matching_Subscription(
    const SMS_PROCESS Client,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p,
    CBS_SubscriberList_t ** ListParent_pp)
{
    CBS_SubscriberList_t *Match_p = NULL;
    CBS_SubscriberList_t *CurrSubscriber_p = CBS_SubscriberList_p;

    if (ListParent_pp != NULL) {
        *ListParent_pp = NULL;
    }

    while (CurrSubscriber_p != NULL) {
        if (CurrSubscriber_p->Client == Client &&
            CurrSubscriber_p->SubscribeToAll == SubscribeToAll &&
            CurrSubscriber_p->IsSim == IsSim &&
            CurrSubscriber_p->Subscription.FirstMessageIdentifier == Subscription_p->FirstMessageIdentifier &&
            CurrSubscriber_p->Subscription.LastMessageIdentifier == Subscription_p->LastMessageIdentifier &&
            CurrSubscriber_p->Subscription.FirstDataCodingScheme == Subscription_p->FirstDataCodingScheme &&
            CurrSubscriber_p->Subscription.LastDataCodingScheme == Subscription_p->LastDataCodingScheme) {
            Match_p = CurrSubscriber_p;
            break;
        }

        if (ListParent_pp != NULL) {
            *ListParent_pp = CurrSubscriber_p;
        }
        CurrSubscriber_p = CurrSubscriber_p->Next_p;
    }

    return Match_p;
}


/********************************************************************/
/**
 *
 * @function     Find_Sim_Subscription
 *
 * @description  Iterate through list of subscriptions and look for one
 *               which is a due to the SIM EFcbmid contents.
 *               If it is found, return its entry in the linked
 *               list and its parent in the linked list.
 *
 * @param [out]  ListParent_pp : Address of matching node's parent in
 *                               linked list.
 *
 * @return       Node in linked list which is due to SIM EFcbmid.
 */
/********************************************************************/
static CBS_SubscriberList_t *Find_Sim_Subscription(
    CBS_SubscriberList_t ** ListParent_pp)
{
    CBS_SubscriberList_t *Match_p = NULL;
    CBS_SubscriberList_t *CurrSubscriber_p = CBS_SubscriberList_p;

    *ListParent_pp = NULL;

    while (CurrSubscriber_p != NULL) {
        if (CurrSubscriber_p->IsSim) {
            Match_p = CurrSubscriber_p;
            break;
        }

        *ListParent_pp = CurrSubscriber_p;
        CurrSubscriber_p = CurrSubscriber_p->Next_p;
    }

    return Match_p;
}


/********************************************************************/
/**
 *
 * @function     SubscriberList_Remove
 *
 * @description  Determine if there is a matching subscription in the
 *               CB server. If there is, remove it.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   SubscribeToAll : Flag indicating whether the client has
 *                                subscribed to all CB pages, or not.
 * @param [in]   IsSim : Flag indicating whether the subscription is
 *                       derived from EFcbmid records.
 * @param [in]   Subscription_p : Subscription data to match against.
 *
 * @return       TRUE if a matching subscriber has been removed from
 *               linked list, else FALSE.
 */
/********************************************************************/
static uint8_t SubscriberList_Remove(
    const SMS_PROCESS Client,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p)
{
    uint8_t Removed = FALSE;
    CBS_SubscriberList_t *PrevSubscriber_p = NULL;
    CBS_SubscriberList_t *MatchSubscriber_p;

    MatchSubscriber_p = Find_Matching_Subscription(Client, SubscribeToAll, IsSim, Subscription_p, &PrevSubscriber_p);

    if (MatchSubscriber_p != NULL) {
        // Match found, now remove it.
        if (PrevSubscriber_p == NULL) {
            // Match is head of linked list.
            CBS_SubscriberList_p = MatchSubscriber_p->Next_p;
        } else {
            // Match is somewhere in linked list other than head
            PrevSubscriber_p->Next_p = MatchSubscriber_p->Next_p;
        }

        Removed = TRUE;
        CBS_NumberOfSubscriptions_Decr();
        SMS_HEAP_FREE(&MatchSubscriber_p);
    }

    return Removed;
}


/********************************************************************/
/**
 *
 * @function     SubscriberList_Add
 *
 * @description  Add a new subscription to the linked list of subscriptions.
 *
 * @param [in]   Client : ID of calling client. (Its event socket value.)
 * @param [in]   ClientTag : ClientTag supplied by the calling client.
 * @param [in]   SubscribeToAll : Flag indicating whether the client has
 *                                subscribed to all CB pages, or not.
 * @param [in]   IsSim : Flag indicating whether the subscription is
 *                       derived from EFcbmid records.
 * @param [in]   Subscription_p : Subscription data to match against.
 *
 * @return       Newly created subscription node.
 */
/********************************************************************/
static CBS_SubscriberList_t *SubscriberList_Add(
    const SMS_PROCESS Client,
    const SMS_ClientTag_t ClientTag,
    const uint8_t SubscribeToAll,
    const uint8_t IsSim,
    const CBS_Subscription_t * Subscription_p)
{
    CBS_SubscriberList_t *New_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(CBS_SubscriberList_t));

    if (New_p != NULL) {
        New_p->Client = Client;
        New_p->ClientTag = ClientTag;
        New_p->SubscribeToAll = SubscribeToAll;
        New_p->IsSim = IsSim;
        New_p->Subscription = *Subscription_p;

        // Add new subscriber to top of linked list.
        if (CBS_SubscriberList_p == NULL) {
            New_p->Next_p = NULL;
            CBS_SubscriberList_p = New_p;
        } else {
            New_p->Next_p = CBS_SubscriberList_p;
            CBS_SubscriberList_p = New_p;
        }

        CBS_NumberOfSubscriptions_Incr();
    } else {
        SMS_A_(SMS_LOG_E("cbsubscribe: SubscriberList_Add: ERROR Malloc failed"));
    }

    return New_p;
}
