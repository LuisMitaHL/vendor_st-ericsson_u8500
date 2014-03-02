/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef LSIM_H
#define LSIM_H

#ifndef __RTK_E__
#include <string.h>
#endif

#ifndef ARM_LINUX_AGPS_FTR
#define INF(fmt, arg...) ALOGI("[%s] " fmt, __FUNCTION__, ## arg)
#define ERR(fmt, arg...) ALOGE("[%s] " fmt, __FUNCTION__, ## arg)
#ifdef  DEBUG_LOG_PRINT
#define DBG(fmt, arg...) ALOGD("[%s] " fmt, __FUNCTION__, ## arg)
#else
#define DBG
#endif

#else /*ARM_LINUX_AGPS_FTR*/

#define INF       printf
#define ERR       printf
#ifdef  DEBUG_LOG_PRINT
#define DBG       printf
#else
#define DBG
#endif

#endif /*ARM_LINUX_AGPS_FTR*/


#define FIRST_LSIMUP_MSG_ENUM       0
#define FIRST_LSIMCP_MSG_ENUM      20
#define FIRST_LSIMCCM_MSG_ENUM     60
#define FIRST_LSIMEE_MSG_ENUM      80

#endif /* LSIM_H */
