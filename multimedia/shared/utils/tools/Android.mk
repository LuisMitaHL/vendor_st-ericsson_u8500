LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := client.c
LOCAL_MODULE := traceclient
LOCAL_MODULE_TAGS := optional

include $(BUILD_HOST_EXECUTABLE)

