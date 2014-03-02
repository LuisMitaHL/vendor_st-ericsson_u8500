LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := \
	libthermal

LOCAL_SRC_FILES := \
	thermal.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include

LOCAL_MODULE_TAGS := \
	eng

LOCAL_PRELINK_MODULE := \
	false

include $(BUILD_SHARED_LIBRARY)
