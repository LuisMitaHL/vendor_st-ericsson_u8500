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

#ifndef SIM_CLIENT_COMMON__H
#define SIM_CLIENT_COMMON__H


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

#include "sim.h"
#include "gnsSUPL_Api.h"
#include "agpsosa.h"

#undef  LOG_TAG
#define LOG_TAG "AMIL_SIM"

#define INF       ALOGI
#define ERR       ALOGE
#define DBG       ALOGD

//Exported API's

void simClient1_1SimClientInit(void);

#endif //SIM_CLIENT_COMMON__H


bool simClient1_1Init(int8_t vp_handle);
void simClient1_6DeInit(int8_t vp_handle);





