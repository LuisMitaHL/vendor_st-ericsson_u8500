/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************************
* DESCRIPTION:
*     Include file for the message page storage
*
********************************************************************************************
*/

#ifndef INCLUSION_GUARD_CBS_MSGS_H
#define INCLUSION_GUARD_CBS_MSGS_H

#include "t_cbs.h"

/*
**========================================================================
**    External function declarations
**========================================================================
*/

void CBS_Msgs_StoreMessage(
    const CBS_Message_t * const Message_p);

CBS_Error_t CBS_Msgs_RetrieveMessage(
    const CBS_MessageHeader_t * const MessageHeader_p,
    CBS_MessagesList_t ** const Messages_pp);

const CBS_MessagesList_t *CBS_Msgs_CachedMessageList_Get(
    void);

void CBS_Msgs_ClearCachedMessages(
    void);

CBS_MessagesList_t *CBS_Msgs_GetNextUmtsMessage(CBS_MessagesList_t **Messages_pp);

#endif                          // INCLUSION_GUARD_CBS_MSGS_H
