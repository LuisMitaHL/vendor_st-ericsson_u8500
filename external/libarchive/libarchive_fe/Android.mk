#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson                                      *
# *                                                                      *
# *  Author: Patrice CHOTARD <patrice.chotard AT stericsson.com>         *
# *                                                                      *
# ************************************************************************/

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -DHAVE_CONFIG_H

LOCAL_C_INCLUDES += $(LOCAL_PATH)/..
#LOCAL_C_INCLUDES += external/openssl/include
#LOCAL_C_INCLUDES += external/bionic/libc/include

LOCAL_MODULE    := libarchive_fe

LOCAL_SRC_FILES := \
	err.c \
	line_reader.c \
	matching.c \
	pathmatch.c \

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)



