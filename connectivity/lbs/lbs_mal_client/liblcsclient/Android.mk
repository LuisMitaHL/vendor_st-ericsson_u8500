#lcs make file

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PREBUILT_LIBS := /pre_built/liblcsclient.a

LOCAL_MODULE:= liblcsclient
LOCAL_MODULE_TAGS := optional

include $(BUILD_MULTI_PREBUILT)
