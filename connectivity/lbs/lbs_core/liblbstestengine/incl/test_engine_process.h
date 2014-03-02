/*
 * Positioning Manager
 *
 * test_engine.h
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef __TEST_ENGINE_H__
#define __TEST_ENGINE_H__
#ifndef ARM_LINUX_AGPS_FTR

#include "android_log.h"
#endif

void TSTENG_close_down(void);
void TSTENG_set_server_port(const int port);
void TSTENG_init(void);

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "Stelp"
#ifndef ARM_LINUX_AGPS_FTR
#define INF(fmt, arg...) ALOGI("[%s] " fmt, __FUNCTION__, ## arg)
#define DBG(fmt, arg...) ALOGI("[%s] " fmt, __FUNCTION__, ## arg)
#define ERR(fmt, arg...) ALOGE("[%s] " fmt, __FUNCTION__, ## arg)
#define CRI(fmt, arg...) ALOGE("[%s] " fmt, __FUNCTION__, ## arg)
#define API(fmt, arg...) ALOGI("API %s " fmt, __FUNCTION__, ##arg);
#define API_NAMED(name,fmt,arg...) ALOGI("API " name " - " fmt, ##arg);
#define INF_BLANK(fmt, arg...) ALOGI(fmt, ## arg)
#else
#define INF(fmt, arg...) printf("[%s] " fmt, __FUNCTION__, ## arg)
#define DBG(fmt, arg...) printf("[%s] " fmt, __FUNCTION__, ## arg)
#define ERR(fmt, arg...) printf("[%s] " fmt, __FUNCTION__, ## arg)
#define CRI(fmt, arg...) printf("[%s] " fmt, __FUNCTION__, ## arg)
#define API(fmt, arg...) printf("API %s " fmt, __FUNCTION__, ##arg);
#define API_NAMED(name,fmt,arg...) printf("API " name " - " fmt, ##arg);
#define INF_BLANK(fmt, arg...) printf(fmt, ## arg)
#endif

#endif
