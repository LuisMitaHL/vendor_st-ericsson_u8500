/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#ifndef _IMG_ENSWRAPPER_LOG_H_
#define _IMG_ENSWRAPPER_LOG_H_


#ifndef LOG_TAG
	#define LOG_TAG "?"
#endif

#ifdef ANDROID
	#include <cutils/log.h>
#elif !defined(_LINUX_UTILS_H_)
	#define LOGE(...) printf("\nError  /" LOG_TAG ": " __VA_ARGS__)
	#define LOGI(...) printf("\nInfo   /" LOG_TAG ": " __VA_ARGS__)
	#define LOGD(...) printf("\nDebug  /" LOG_TAG ": " __VA_ARGS__)
	#define LOGV(...) printf("\nV....../" LOG_TAG ": " __VA_ARGS__)
	#define LOGW(...) printf("\nWarning/" LOG_TAG ": " __VA_ARGS__)
#endif // ANDROID


#define IMGWRAPPER_DEBUG_LOG(...)                              \
	if (GetComponentManager().GetTraceLevel(eTrace_Debug)) {   \
	LOGD(__VA_ARGS__);                                         \
	}

#define IMGWRAPPER_OMX_LOG(...)                            \
	if (GetComponentManager().GetTraceLevel(eTrace_Omx)) { \
	LOGD(__VA_ARGS__);                                     \
	}

#define IMGWRAPPER_OMX_LOGV(...)                           \
	if (GetComponentManager().GetTraceLevel(eTrace_Omx)) { \
	LOGV(__VA_ARGS__);                                     \
	}

#define IMGWRAPPER_OMX_LOGI(...)                            \
	if (GetComponentManager().GetTraceLevel(eTrace_Omx)) {  \
	LOGI(__VA_ARGS__);                                      \
	}

#define IMGWRAPPER_OMX_LOGE(...)                           \
	if (GetComponentManager().GetTraceLevel(eTrace_Omx)) { \
	LOGE(__VA_ARGS__);                                     \
	}

#define IMGWRAPPER_OMX_LOG_LEVEL(level, ...)                        \
	if (GetComponentManager().GetTraceLevel(eTrace_Omx) >= level) { \
	LOGD(__VA_ARGS__);                                              \
	}

/** Function generated by a Perl script */
void displayParameterAndConfig(unsigned int index, void *config);

#endif /* _IMG_ENSWRAPPER_LOG_H_ */


