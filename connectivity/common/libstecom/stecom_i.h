/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STECOM_I_H_
#define _STECOM_I_H_

struct stec_ctrl {
  int sock;
  int serverside;

  struct sockaddr *local_addr;
  socklen_t local_addr_len;

  struct sockaddr *remote_addr;
  socklen_t remote_addr_len;

  struct sockaddr *last_client_addr;
  socklen_t last_client_addr_len;

  struct subscribe_t *subscribe;

#ifdef CONFIG_NL80211_TESTMODE
  struct stec_nl *nl;
#endif
};

#endif

