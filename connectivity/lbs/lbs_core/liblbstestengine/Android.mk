# Android.mk file for liblbstestengine
# liblbstestengine
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	srce/test_engine_main.c \
	srce/test_engine_process.c \
        srce/test_engine.c \
	srce/test_engine_utils.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/incl \
	$(LOCAL_PATH)/../incl \
	$(LOCAL_PATH)/../lbsd/libagpsosa/incl

LOCAL_MODULE := liblbstestengine
LOCAL_MODULE_TAGS := tests

LOCAL_PRELINK_MODULE := \
        false

LOCAL_CFLAGS += \
	-DAGPS_LINUX_FTR

LOCAL_STATIC_LIBRARIES := liblog

LOCAL_SHARED_LIBRARIES := \
        libc \
        libutils

include $(BUILD_SHARED_LIBRARY)
