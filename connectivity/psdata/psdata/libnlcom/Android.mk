LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libnlcom
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := nlcom.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../libmpl

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
