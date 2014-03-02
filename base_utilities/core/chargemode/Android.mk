LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	chargemode.c

LOCAL_CFLAGS += -Wall -Wextra -DANDROID

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= chargemode
include $(BUILD_EXECUTABLE)
