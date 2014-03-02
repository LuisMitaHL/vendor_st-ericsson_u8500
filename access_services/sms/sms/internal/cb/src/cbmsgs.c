/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
*
* DESCRIPTION:
*     Data and functions for the CB message storage
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

#include "r_cbslinuxporting.h"

#include "t_cbs.h"

#include "cbmsgs.h"

/*
**========================================================================
** Defines.
**========================================================================
*/


// Define the number of CB Messages that can be cached in RAM.
// Value can be overridden using a compile switch.
#ifndef MAX_NBR_OF_STORED_MESSAGES
#define MAX_NBR_OF_STORED_MESSAGES (100)
#endif



/*
**========================================================================
**    Local declarations
**========================================================================
*/

/*
**========================================================================
**    Local function declarations
**========================================================================
*/

static uint8_t DetermineIfExistingMessage(
    const CBS_Message_t * const MessageIn_p,
    CBS_MessagesList_t ** MatchesOut_pp);

static CBS_MessagesList_t *AddNewMessage(
    const CBS_Message_t * const Message_p);
/*
**========================================================================
**    Local variable definitions.
**========================================================================
*/


static CBS_MessagesList_t *CachedMessageList_p = NULL;  // Head of linked list of cached CB Messages.
static uint16_t NumberOfStoredMessages = 0;            // Number of stored messages in linked list.

/*
**========================================================================
**    External Functions
**========================================================================
*/



/********************************************************************/
/**
 *
 * @function     CBS_Msgs_CachedMessageList_Get
 *
 * @description  Return const pointer to cached messages linked list head.
 *
 *
 * @return       Cached messages linked list head
 */
/********************************************************************/
const CBS_MessagesList_t *CBS_Msgs_CachedMessageList_Get(
    void)
{
    return CachedMessageList_p;
}



/********************************************************************/
/**
 *
 * @function     CBS_Msgs_StoreMessage
 *
 * @description  Determine if the supplied message is already in the cache.
 *               If it is not, add it to the cache.
 *
 * @param [in]   Message_p : CB message header and data information.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Msgs_StoreMessage(
    const CBS_Message_t * const Message_p)
{
    if (NULL == Message_p) {
        SMS_B_(SMS_LOG_E("cbmsgs.c: Message_p is NULL"));
        return;
    }

    if (!DetermineIfExistingMessage(Message_p, NULL)) {
        CBS_MessagesList_t *Tmp_p = NULL;
        // CB Message is not in the cache.
        SMS_B_(SMS_LOG_D("cbmsgs.c: CB *NOT* in Cache: SN 0x%04X, MID 0x%04x, DCS 0x%02X, PP 0x%02X.",
                Message_p->Header.SerialNumber, Message_p->Header.MessageIdentifier, Message_p->Header.DataCodingScheme, Message_p->Header.PageParameter));
        // Add new message to cache.
        Tmp_p = AddNewMessage(Message_p);
        SMS_B_ASSERT_(Tmp_p != NULL);
        SMS_IDENTIFIER_NOT_USED(Tmp_p);
    } else {
        SMS_C_(SMS_LOG_V("cbmsgs.c: CB is in Cache: SN 0x%04X, MID 0x%04x, DCS 0x%02X, PP 0x%02X.",
                Message_p->Header.SerialNumber, Message_p->Header.MessageIdentifier, Message_p->Header.DataCodingScheme, Message_p->Header.PageParameter));
    }
}


/********************************************************************/
/**
 *
 * @function     CBS_Msgs_RetrieveMessage
 *
 * @description  Determine if the requested message is in the cache.
 *               If it is, return it.
 *
 * @param [in]   MessageHeader_p : CB Message header information of CB message
 *                                 being requested.
 * @param [out]  Messages_pp : Pointer to list of requested CB messages
 *                             that matches MessageHeader_p.
 *
 * @return       Error value.
 */
/********************************************************************/
CBS_Error_t CBS_Msgs_RetrieveMessage(
    const CBS_MessageHeader_t * const MessageHeader_p,
    CBS_MessagesList_t ** const Messages_pp)
{
    CBS_Error_t Error;
    CBS_Message_t Message;

    SMS_B_ASSERT_(MessageHeader_p != NULL);

    Message.Header = *MessageHeader_p;
    memset(Message.Data, 0, sizeof(CBS_MessageData_t));
    Message.UsefulData = 0;

    if (DetermineIfExistingMessage(&Message, Messages_pp)) {
        // Message exists in cache
        Error = CBS_ERROR_NONE;
    } else {
        // Message does not exist in cache
        Error = CBS_ERROR_REQUESTED_MESSAGE_NO_LONGER_EXISTS;
    }

    return Error;
}


/********************************************************************/
/**
 *
 * @function     CBS_Msgs_ClearCachedMessages
 *
 * @description  Iterate through cached CB messages and remove them.
 *
 * @return       void
 */
/********************************************************************/
void CBS_Msgs_ClearCachedMessages(
    void)
{
    SMS_C_(uint32_t DestroyedMessages = 0);
    SMS_B_(SMS_LOG_D("CBS_Msgs_ClearCachedMessages called"));

    while (CachedMessageList_p != NULL) {
        CBS_MessagesList_t *Curr_p = CachedMessageList_p;
        CachedMessageList_p = CachedMessageList_p->Next_p;
        SMS_HEAP_FREE(&Curr_p);
        SMS_C_(DestroyedMessages++);
    }

    CachedMessageList_p = NULL;
    NumberOfStoredMessages = 0;
    SMS_C_(SMS_LOG_V("CBS_Msgs_ClearCachedMessages destroyed %d messages", DestroyedMessages));
}


/*
**========================================================================
**    Local functions
**========================================================================
*/

/********************************************************************/
/**
 *
 * @function     DetermineIfExistingMessage
 *
 * @description  Iterate through cached messages and look for the ones which
 *               has same data as supplied. If matching entries are found
 *               the method returns TRUE, otherwise FALSE. Also if
 *               MatchesOut_pp is not NULL and there are matching entries, they are
 *               returned as a linked list which the caller must free.
 *
 * @param [in]   MessageIn_p : CB Message to search for.
 * @param [out]  MatchesOut_pp : Pointer to linked list with matching
 *                               entries to the supplied
 *                               Header data. (Must be freed by caller).
 *                               Set to NULL if not to be used.
 *
 * @return       TRUE - match found, FALSE no match found.
 */
/********************************************************************/
static uint8_t DetermineIfExistingMessage(
    const CBS_Message_t * const MessageIn_p,
    CBS_MessagesList_t ** MatchesOut_pp)
{
    CBS_MessagesList_t *Matches_p = NULL;
    CBS_MessagesList_t *Message_p = NULL;
    uint8_t MatchFound = FALSE;

    if (NULL == MessageIn_p) {
        SMS_A_(SMS_LOG_E("cbmsgs: DetermineIfExistingMessage: MessageIn_p is NULL"));
        goto error;
    }

    for (Message_p = CachedMessageList_p; NULL != Message_p; Message_p = Message_p->Next_p) {
        if (Message_p->Message.Header.MessageIdentifier == MessageIn_p->Header.MessageIdentifier &&
            Message_p->Message.Header.SerialNumber == MessageIn_p->Header.SerialNumber &&
            Message_p->Message.Header.DataCodingScheme == MessageIn_p->Header.DataCodingScheme &&
            Message_p->Message.Header.PageParameter == MessageIn_p->Header.PageParameter) {

            if (Message_p->Message.Header.PageParameter <= 0xf) {
                if (MessageIn_p->UsefulData != Message_p->Message.UsefulData) {
                    continue;
                }
                if (0 != memcmp(MessageIn_p->Data, Message_p->Message.Data, sizeof(CBS_MessageData_t))) {
                    continue;
                }
            }

            if (NULL != MatchesOut_pp) {
                CBS_MessagesList_t *Match_p = (CBS_MessagesList_t *) SMS_HEAP_ALLOC(CBS_MessagesList_t);
                if (NULL == Match_p) {
                    SMS_A_(SMS_LOG_E("cbmngr: DetermineIfExistingMessage: Match_p is NULL"));
                    goto error;
                }
                Match_p->Message = Message_p->Message;
                Match_p->Next_p = Matches_p;
                Matches_p = Match_p;
            }
            MatchFound = TRUE;
        }
    }

    if (NULL != MatchesOut_pp) {
        *MatchesOut_pp = Matches_p;
    }

exit:
    return MatchFound;

error:

    while (NULL != Matches_p) {
        CBS_MessagesList_t *Messages_p = Matches_p;
        Matches_p = Matches_p->Next_p;
        SMS_HEAP_FREE(&Messages_p);
    }

    goto exit;
}


/********************************************************************/
/**
 *
 * @function     AddNewMessage
 *
 * @description  Adds the supplied page to the cache.
 *               If the cache is not yet full, the new entry is added
 *               to the end (tail) of the linked list.  However if the
 *               cache is full the oldest entry (head) of the linked
 *               list is removed and the new entry added to the end
 *               (tail) of the linked list.  This ensures that the
 *               oldest entry is at the head of the list and the newest
 *               is at the tail.
 *
 * @param [in]   MessageHeader_p : CB Message header information of CB message
 *                              being stored in cache.
 * @param [in]   MessageData_p : CB Message data to be stored in cache.
 *
 * @return       New message entry in linked list, or NULL if malloc
 *               failed.
 */
/********************************************************************/
static CBS_MessagesList_t *AddNewMessage(
    const CBS_Message_t * const Message_p)
{
    CBS_MessagesList_t *MessageToAdd_p;

    SMS_B_(SMS_LOG_D("cbmsgs: Add new message: (Currently %d of %d messages stored)", NumberOfStoredMessages, MAX_NBR_OF_STORED_MESSAGES));

    if (NumberOfStoredMessages < MAX_NBR_OF_STORED_MESSAGES) {
        // Cache not full, so add a new node into linked list.
        MessageToAdd_p = SMS_HEAP_UNTYPED_ALLOC(sizeof(CBS_MessagesList_t));
        if (MessageToAdd_p != NULL) {
            NumberOfStoredMessages++;
        } else {
            SMS_A_(SMS_LOG_E("cbmsgs: AddNewMessage: MALLOC FAILED"));
        }
    } else {
        // Cache full, remove oldest entry which will be at the head of the linked list.
        // We will re-use this block of RAM but it will be added to the end of the linked list.
        MessageToAdd_p = CachedMessageList_p;
        CachedMessageList_p = MessageToAdd_p->Next_p;
        SMS_C_(SMS_LOG_V("cbmsgs: Dropping Message from full cache. SN=0x%04X, MID=0x%04X, DCS=0x%02X, PP=0x%02X.",
                      MessageToAdd_p->Message.Header.SerialNumber, MessageToAdd_p->Message.Header.MessageIdentifier, MessageToAdd_p->Message.Header.DataCodingScheme, MessageToAdd_p->Message.Header.PageParameter));
    }

    // Now, assuming we have something, initialise the data and add it to the linked list.
    if (NULL != MessageToAdd_p) {
        // Write new CB data to node which is being added to end of linked list.
        MessageToAdd_p->Message = *Message_p;
        MessageToAdd_p->Next_p = NULL;

        if (CachedMessageList_p == NULL) {
            // Linked list is empty, so adding to head.
            CachedMessageList_p = MessageToAdd_p;
        } else {
            // Add to end of linked list.
            CBS_MessagesList_t *CurrNode_p = CachedMessageList_p;

            while (CurrNode_p->Next_p != NULL) {
                CurrNode_p = CurrNode_p->Next_p;
            }

            CurrNode_p->Next_p = MessageToAdd_p;
        }
    }
    SMS_B_(SMS_LOG_D("cbmsgs: Added new message: (Currently %d of %d messages stored)", NumberOfStoredMessages, MAX_NBR_OF_STORED_MESSAGES));

    return MessageToAdd_p;
}


/********************************************************************/
/**
 *
 * @function     CBS_Msgs_ContainsEqualMessages
 *
 * @description Compares if two message list nodes contain messages
 *              with the same message identifier and the same data
 *              coding scheme.
 *
 * @param [in]   Message1_p : First message argument in comparison.
 * @param [in]   Message2_p : Second message argument in comparison.
 *
 * @return       TRUE if mid and dcs matches, otherwise FALSE.
 */
/********************************************************************/
static int CBS_Msgs_ContainsEqualMessages(CBS_MessagesList_t *Message1_p,
                                              CBS_MessagesList_t *Message2_p)
{
    if (Message1_p && Message2_p) {
        return (Message1_p->Message.Header.MessageIdentifier == Message2_p->Message.Header.MessageIdentifier &&
                Message1_p->Message.Header.DataCodingScheme == Message2_p->Message.Header.DataCodingScheme &&
                Message1_p->Message.Header.SerialNumber == Message2_p->Message.Header.SerialNumber);
    } else {
        return 0;
    }
}

/********************************************************************/
/**
 *
 * @function     CBS_Msgs_GetNextUmtsMessage
 *
 * @description Detaches the first CB UMTS message sub list from the
 *              rest of the CB UMTS messages buffer.
 *
 * @param [in/out] Messages_pp : The buffer with UMTS messages.
 *
 * @return       Sub list with the next UMTS message.
 */
/********************************************************************/
CBS_MessagesList_t *CBS_Msgs_GetNextUmtsMessage(CBS_MessagesList_t **Messages_pp)
{
    CBS_MessagesList_t *Tail_p = NULL;
    CBS_MessagesList_t *Head_p = NULL;
    CBS_MessagesList_t *PrevMesg_p = NULL;
    CBS_MessagesList_t *PrevTail_p = NULL;
    CBS_MessagesList_t *Pos_p = NULL;

    if (NULL == Messages_pp || NULL == *Messages_pp) {
        SMS_A_(SMS_LOG_E("cbmngr: CBS_Msgs_UnlinkNextUmtsMessage: received NULL parameters"));
        goto exit;
    }

    /*
     *   Objective: Get first UMTS CB Message MA(x) from Buffer.
     *   Receive:   Buffer(in):  MA(1)-->MA(2)-->MB(1)-->MB(2)-->MB(3)-->MC(1)-->MC(2)-->(null)
     *   Update:    Buffer(out): MB(1)-->MB(2)-->MB(3)-->MC(1)-->MC(2)-->(null)
     *   Return:    Sub list:    MA(1)-->MA(2)-->(null)
     */
    Head_p = PrevMesg_p = *Messages_pp;

    for (Pos_p = Head_p->Next_p; NULL != Pos_p; Pos_p = Pos_p->Next_p) {
        if (CBS_Msgs_ContainsEqualMessages(Head_p, Pos_p)) {
            PrevMesg_p->Next_p = Pos_p;
            PrevMesg_p = Pos_p;

            if (NULL != PrevTail_p && NULL == Pos_p->Next_p) {
                PrevTail_p->Next_p = NULL;
            }

        } else {

            if (NULL == Tail_p) {
                Tail_p = Pos_p;
                PrevTail_p = Pos_p;
            } else {
                PrevTail_p->Next_p = Pos_p;
                PrevTail_p = Pos_p;
            }

            if (NULL == Pos_p->Next_p) {
                PrevMesg_p->Next_p = NULL;
            }

        }
    }

    *Messages_pp = Tail_p;

exit:
    return Head_p;
}
