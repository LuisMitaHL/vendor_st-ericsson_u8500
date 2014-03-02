LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libstecom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := stecom.c subscribe.c
LOCAL_CFLAGS := -DSW_VARIANT_ANDROID

LOCAL_SHARED_LIBRARIES := libutils

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
