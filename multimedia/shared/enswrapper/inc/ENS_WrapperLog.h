/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ENS_WRAPPERLOG_H_
#define _ENS_WRAPPERLOG_H_

#define LOG_TAG "ENSWrapper"
#define LOG_NDEBUG 0

#include "linux_utils.h"

#ifdef  __cplusplus
#include "ENS_Wrapper.h"

#define OMX_LOG_LEVEL_BUFFER 3
#define WRAPPER_DEFAULT_STR_SIZE 128
#define WRAPPER_MAX_DEBUG_BUFFER 2048
#define OMX_BUFFERDATA_NB_END_BYTES 16
#define OMX_BUFFERDATA_NB_BYTES_PER_LINES 16

#define OMX_WRAPPER_LOG_LEVEL ENS_Wrapper::OMX_trace_level
#define OMX_WRAPPER_DEBUG     ENS_Wrapper::DEBUG_trace_enabled

#define WRAPPER_DEBUG_LOG(...)			\
    if (ENS_Wrapper::DEBUG_trace_enabled) {	\
	ALOGV(__VA_ARGS__);			\
    }

#define WRAPPER_OMX_TRACE(comp)                                         \
    (OMX_WRAPPER_LOG_LEVEL &&                                           \
     ((comp == NULL) || (OMX_HANDLETYPE_TO_WRAPPER(comp)->traceActivity)))

#define WRAPPER_OMX_LOG(comp, ...)                                      \
    if (WRAPPER_OMX_TRACE(comp)) {                                      \
	ALOGD(__VA_ARGS__);                                              \
    }

#define WRAPPER_OMX_LOG_BUFFER(comp, ...)                               \
    if ((OMX_WRAPPER_LOG_LEVEL >= OMX_LOG_LEVEL_BUFFER) &&              \
        WRAPPER_OMX_TRACE(comp)) {                                      \
	ALOGD(__VA_ARGS__);                                              \
    }

#define WRAPPER_OMX_LOG_RETURN_STR(comp, str, retval , what, noErrorLog, extra, ...) \
    do {                                                                \
        char result_str[WRAPPER_DEFAULT_STR_SIZE];                      \
        if ((retval != OMX_ErrorNone) && (!noErrorLog)) {   \
                ALOGE("return %s - " what " " OMX_COMPONENT_DEF_STR " " extra, \
                    strOMX_ERRORTYPE(retval, result_str, WRAPPER_DEFAULT_STR_SIZE), \
                    OMX_COMPONENT_DEF_ARG_STR(str, comp), ##__VA_ARGS__);  \
        } else if (WRAPPER_OMX_TRACE(comp)) {                           \
            ALOGD("return %s - " what " " OMX_COMPONENT_DEF_STR extra,   \
                 strOMX_ERRORTYPE(retval, result_str, WRAPPER_DEFAULT_STR_SIZE), \
                 OMX_COMPONENT_DEF_ARG_STR(str, comp), ##__VA_ARGS__);  \
        }                                                               \
    } while (0)

#define WRAPPER_OMX_LOG_RETURN(comp, retval , what, noErrorLog, extra, ...)         \
    WRAPPER_OMX_LOG_RETURN_STR(comp, OMX_HANDLETYPE_GET_NAME(hComponent), \
                               retval , what, noErrorLog, extra, ##__VA_ARGS__)

#define DEBUG_RME_PRINTF(...)			\
    if (ENS_Wrapper::RME_trace_enabled) {	\
	ALOGD(__VA_ARGS__);			\
    }

#define IS_WRAPPER_OMX_LOG_LEVEL(level) \
    (OMX_WRAPPER_LOG_LEVEL >= (level))

#define OMX_COMPONENT_DEF_STR "%s h=0x%08x"

#define OMX_COMPONENT_DEF_ARG_STR(str, hComponent) \
  (str) , (unsigned int) hComponent

#define OMX_COMPONENT_DEF_ARG(hComponent) \
  OMX_COMPONENT_DEF_ARG_STR((OMX_HANDLETYPE_IS_WRAPPER_HANDLE_ID(hComponent) ? " WRAPPER_HANDLE " : \
                             OMX_HANDLETYPE_GET_NAME(hComponent)) , (unsigned int) hComponent)

void sprintCBMessage(char *buffer, CB_Message *message, size_t size);

#endif  /* #ifdef  __cplusplus */

#endif /* _ENS_WRAPPERLOG_H_ */


