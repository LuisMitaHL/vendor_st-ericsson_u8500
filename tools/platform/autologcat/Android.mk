LOCAL_PATH:= $(call my-dir)

ifeq ($(TARGET_ARCH),arm)
common_cflags := -Wall -ansi -std=gnu99 -march=armv5te -mtune=arm9e -Os
else
common_cflags := -Wall -ansi -std=gnu99 -O2
endif

# Build autologcat
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DANDROID_OS $(common_cflags)

LOCAL_SHARED_LIBRARIES := libcutils libutils

LOCAL_SRC_FILES := autologcat.c

LOCAL_MODULE := autologcat
LOCAL_MODULE_TAGS := eng development

include $(BUILD_EXECUTABLE)

# autologcat.conf
include $(CLEAR_VARS)

LOCAL_MODULE := autologcat.conf
LOCAL_MODULE_TAGS := eng development
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := ETC

include $(BUILD_PREBUILT)