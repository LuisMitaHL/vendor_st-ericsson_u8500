/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_SMS_H
#define EXE_SMS_H 1

/* Add client specific requester states here.
 * Do use client specific ids.
 */
#define EXE_STATE_MSG_STORAGE_MEM1_RESPONSE               10
#define EXE_STATE_MSG_STORAGE_MEM2_RESPONSE               11
#define EXE_STATE_MSG_STORAGE_MEM3_RESPONSE               12
#define EXE_STATE_MSG_FIND                                13
#define EXE_STATE_MSG_DELETE                              14
#define EXE_STATE_MSG_DELETE_RESPONSE                     15
#define EXE_STATE_MSG_SMS_EVENT_SUBSCRIBE_RESPONSE        16
#define EXE_STATE_MSG_SMS_EVENT_UNSUBSCRIBE_RESPONSE      17
#define EXE_STATE_MSG_CBS_SUBSCRIBE_RESPONSE              18
#define EXE_STATE_MSG_CBS_UNSUBSCRIBE_RESPONSE            19
#define EXE_STATE_MSG_CBS_ACTIVATE_RESPONSE               20
#define EXE_STATE_MSG_CBS_DEACTIVATE_RESPONSE             21
#define EXE_STATE_MSG_CBS_NUMBER_OF_SUBSRIPTIONS_RESPONSE 22
#define EXE_STATE_MSG_CBS_GET_SUBSRIPTIONS_RESPONSE       23
#define EXE_STATE_MSG_READ_SMS                            24
#define EXE_STATE_MSG_WRITE_SMS                           25
#define EXE_STATE_MSG_SEND_SMS                            26
#define EXE_STATE_MSG_SET_STATUS                          27
#define EXE_STATE_MSG_SET_STATUS_RESPONSE                 28


#endif

