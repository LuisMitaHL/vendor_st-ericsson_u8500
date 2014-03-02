LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	startup.c

LOCAL_CFLAGS += -Wall -Wextra -Werror

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= startup
include $(BUILD_EXECUTABLE)
