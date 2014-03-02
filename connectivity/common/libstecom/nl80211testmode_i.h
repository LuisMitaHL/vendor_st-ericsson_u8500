/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _NL80211TESTMODE_I_H_
#define _NL80211TESTMODE_I_H_

#ifndef CONFIG_LIBNL20
#  define nl_sock nl_handle
#endif

#define STEC_NL_IFNAME_SIZE 200

struct stec_nl * _stec_nl80211testmode_init();
void _stec_nl80211testmode_deinit(struct stec_nl * const nl);

int _stec_nl80211testmode_open(struct stec_nl * const nl, const struct sockaddr *addr, socklen_t addr_len);
int _stec_nl80211testmode_close(struct stec_nl * const nl);
int _stec_nl80211testmode_send(struct stec_nl * const nl, const char * const buf, size_t buf_len);
int _stec_nl80211testmode_recv(struct stec_nl * const nl, char * const buf, size_t buf_len);

int _stec_nl80211testmode_event_subscribe(struct stec_nl * const nl);
int _stec_nl80211testmode_event_unsubscribe(struct stec_nl * const nl);
int _stec_nl80211testmode_event_recv(struct stec_nl * const nl, char * const buf, size_t buf_len);

#endif

