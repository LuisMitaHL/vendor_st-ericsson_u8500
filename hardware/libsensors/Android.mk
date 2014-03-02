LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# Include kerne headers in a proper way later on
#LOCAL_C_INCLUDES:= $(LOCAL_PATH)/../../../../kernel/include

LOCAL_MODULE := sensors.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_SHARED_LIBRARIES := liblog libcutils
# include any shared library dependencies

LOCAL_SRC_FILES := sensors.c

ifeq ($(LIBSENSORS_SET_PLATFORM), u8500)
LOCAL_CFLAGS := -DSENSORS_U8500 
LOCAL_CFLAGS += -DTSL27713_SENSOR
else ifeq ($(LIBSENSORS_SET_PLATFORM), u5500)
LOCAL_CFLAGS := -DSENSORS_U5500
else ifeq ($(LIBSENSORS_SET_PLATFORM), u9540)
LOCAL_CFLAGS := -DSENSORS_U9540
endif

include $(BUILD_SHARED_LIBRARY)
