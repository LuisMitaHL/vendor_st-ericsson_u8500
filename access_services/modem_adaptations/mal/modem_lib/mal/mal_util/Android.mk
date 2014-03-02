# Copyright (c) ST-Ericsson AB 2009-2010
#
# Author: Rohan Wadageri<rohanmallikarjun.wadageri@stericsson.com>
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LIBMALUTIL_LIBPATH := $(LOCAL_PATH)/pre_built
LOCAL_PATH := $(LIBMALUTIL_LIBPATH)
LOCAL_MODULE_TAGS := eng
LOCAL_PREBUILT_LIBS := libmalutil.so
include $(BUILD_MULTI_PREBUILT)
