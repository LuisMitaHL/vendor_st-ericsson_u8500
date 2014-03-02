/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ATS_LOG_H
#define ATS_LOG_H

/*
 * Add Logcat support if available
 *
 *
 */

#ifdef ANDROID
#define LOG_TAG "AudioTuningServer"
#define LOG_NDEBUG 0
#include "cutils/log.h"
#else
#define ALOGI printf
#define ALOGD printf
#define ALOGE printf
#define ALOGW printf
#define ALOGV printf
#endif

#endif // LOG_H

