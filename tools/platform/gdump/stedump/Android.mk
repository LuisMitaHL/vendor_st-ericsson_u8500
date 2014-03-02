LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := stedump.c mm_stedump.c
LOCAL_MODULE := stedump
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES +=  libcutils

LOCAL_C_INCLUDES := frameworks/native/cmds/dumpstate


include $(BUILD_EXECUTABLE)
