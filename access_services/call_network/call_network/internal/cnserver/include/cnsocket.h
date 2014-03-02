/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __cnsocket_h__
#define __cnsocket_h__ (1)

#define CN_NUM_SOCKET_SERVERS 2
#define CN_REQUEST_SOCKET_MAX_CLIENTS 10
#define CN_EVENT_SOCKET_MAX_CLIENTS 10

#ifndef HAVE_ANDROID_OS
static const char cn_request_socket_name[] = "./tmp/socket/cn_request";
static const char cn_event_socket_name[]   = "./tmp/socket/cn_event";
#else
static const char cn_request_socket_name[] = "/dev/socket/cn_request";
static const char cn_event_socket_name[]   = "/dev/socket/cn_event";
#endif

#endif /* __cnsocket_h__ */
