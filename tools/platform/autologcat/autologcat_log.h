/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef AUTOLOGCAT_LOG_H_
#define AUTOLOGCAT_LOG_H_

#include <stdio.h>

#undef LOG_TAG
#define LOG_TAG "ALC"

#ifdef ANDROID_OS

#include <utils/Log.h>

#else

# define QUOTEME_(x) #x
# define QUOTEME(x) QUOTEME_(x)
# define ALOGV(...) printf("V/" LOG_TAG " (" __FILE__ ":" QUOTEME(__LINE__) "): " __VA_ARGS__)
# define ALOGD(...) printf("D/" LOG_TAG " (" __FILE__ ":" QUOTEME(__LINE__) "): " __VA_ARGS__)
# define ALOGI(...) printf("I/" LOG_TAG " (" __FILE__ ":" QUOTEME(__LINE__) "): " __VA_ARGS__)
# define ALOGW(...) printf("W/" LOG_TAG "(" ")" __VA_ARGS__)
# define ALOGE(...) printf("E/" LOG_TAG "(" ")" __VA_ARGS__)
#endif

#endif /* AUTOLOGCAT_LOG_H_ */
