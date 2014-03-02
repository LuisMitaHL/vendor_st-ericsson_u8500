/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Balaji Natakala
 *  Email : balaji.natakala@stericsson.com
 *****************************************************************************/

#ifndef NET_CLIENT_COMMON__H
#define NET_CLIENT_COMMON__H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "gnsSUPL_Api.h"
#include "agpsosa.h"

#include "cn_client.h"
#include "amil.h"



#undef  LOG_TAG
#define LOG_TAG "AMIL_CN"

#define INF       ALOGI
#define ERR       ALOGE
#define DBG       ALOGD

#endif  //NET_CLIENT_COMMON__H
