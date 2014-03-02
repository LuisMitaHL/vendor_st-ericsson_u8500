LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libpscc
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := pscc_msg.c
LOCAL_SHARED_LIBRARIES := libmpl
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../psdata/libmpl

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
