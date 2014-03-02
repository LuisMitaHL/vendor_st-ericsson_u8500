LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ARCH:=arm

# include $(LOCAL_PATH)/Makefile

LOCAL_SRC_FILES:=battery_params.c

srcdir := $(LOCAL_PATH)

LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := battery_params

LOCAL_REQUIRED_MODULES := liblog

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES += libcspsa
LOCAL_C_INCLUDES += $(STORAGE_PATH)/parameter_storage/cspsa

LOCAL_CFLAGS = -O2 -Wall

include $(BUILD_EXECUTABLE)
