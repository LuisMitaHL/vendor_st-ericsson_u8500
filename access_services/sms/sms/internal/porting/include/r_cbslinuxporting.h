/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _R_CBSLINUXPORTING_H
#define _R_CBSLINUXPORTING_H

/* Set log tag */
#ifdef PLATFORM_ANDROID
#define LOG_TAG "RIL_CBS"
#else
#define LOG_TAG "CBS"
#endif

#include "r_smslinuxporting.h"

#endif /* _R_CBSLINUXPORTING_H */
