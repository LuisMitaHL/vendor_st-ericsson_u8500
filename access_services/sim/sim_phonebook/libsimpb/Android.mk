# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libsimpb

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../simpbd/include

LOCAL_SRC_FILES := \
	../simpbd/main/log.c \
	libsimpb.c

LOCAL_CFLAGS := \
	-DCFG_USE_ANDROID_LOG \
	-DCFG_ANDROID_LOG_TAG='"RIL SIMPBCL"'

include $(BUILD_STATIC_LIBRARY)
