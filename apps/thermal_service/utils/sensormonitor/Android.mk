LOCAL_PATH := $(call my-dir)

# Build sensormonitor
include $(CLEAR_VARS)

LOCAL_MODULE:= \
	sensormonitor

LOCAL_SRC_FILES = \
	sensormonitor.c

LOCAL_SHARED_LIBRARIES += \
	libcutils

LOCAL_MODULE_TAGS := \
	eng

include $(BUILD_EXECUTABLE)

