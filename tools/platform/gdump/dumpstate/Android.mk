
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := frameworks/native/cmds/dumpstate

LOCAL_SRC_FILES := dumpstate.c

LOCAL_MODULE := libdumpstate

LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)
