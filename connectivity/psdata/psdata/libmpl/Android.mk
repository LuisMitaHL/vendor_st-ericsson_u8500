LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmpl
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := mpl_list.c mpl_param.c mpl_msg.c mpl_config.c

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
