/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _LIBSTECOM_NL80211TESTMODE_H_
#define _LIBSTECOM_NL80211TESTMODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/sockaddr.h>

struct sockaddr_nl80211tm
{
  __SOCKADDR_COMMON (nltm_);
  char nltm_path[120]; /* Path name */
};

struct stec_ctrl;

/**
 * stec_nl80211testmode_event_recv - Receive a pending event message from nl80211
 * @ctrl:        Control interface data from stec_open()
 * @buf:         Buffer for the message data
 * @buf_len:     Length of the reply buffer
 * Returns:      Length of data received on success, -1 on failure
 *
 * This function will receive a pending message. This function will block
 * if no messages are available. The received message will
 * be written to buf and the actual length of the message is returned.
 */
int stec_nl80211testmode_event_recv(struct stec_ctrl *ctrl, char *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif

#endif

