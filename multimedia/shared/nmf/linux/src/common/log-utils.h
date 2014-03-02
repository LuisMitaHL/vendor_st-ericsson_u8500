/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef LOG_UTILS_H
#define LOG_UTILS_H

#ifndef LOG_TAG
#error "Define LOG_TAG !"
#endif

#ifdef ANDROID

#include <cutils/log.h>
#include <cutils/properties.h>

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LOG_NDEBUG
  #define LOG_NDEBUG 1
#endif

#define LOGE(...) printf("E/" LOG_TAG ": " __VA_ARGS__)
#define ALOGE(...) printf("E/" LOG_TAG ": " __VA_ARGS__)
#define ALOGW(...) printf("W/" LOG_TAG ": " __VA_ARGS__)
#define ALOGI(...) printf("I/" LOG_TAG ": " __VA_ARGS__)
#define ALOGD(...) printf("D/" LOG_TAG ": " __VA_ARGS__)

#if LOG_NDEBUG
  #define ALOGV(...) ((void)0)
#else
  #define ALOGV(...) printf("V/" LOG_TAG ": " __VA_ARGS__)
#endif

#define property_get(key, value, default_value)       \
	do {                                          \
		char *val = getenv(key);	      \
		if (val == NULL)		      \
			strcpy(value, default_value); \
		else                                  \
			strcpy(value, val);           \
	} while (0)

#endif // ANDROID

#endif //LOG_UTILS_H
