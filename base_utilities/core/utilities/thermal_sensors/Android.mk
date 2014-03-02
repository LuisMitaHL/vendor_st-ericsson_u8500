LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS += -Wall -Wextra -Werror

LOCAL_C_INCLUDES += $(LOCAL_PATH)/..
LOCAL_C_INCLUDES += external/expat/lib

LOCAL_MODULE:= libthermalutils

LOCAL_SRC_FILES:= \
	thermalutils.c

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := libexpat liblog

include $(BUILD_STATIC_LIBRARY)

# Copy thermal_sensors_ux500.xml over to /system/etc
ifeq ($(THERMALSENSOR_SET_PLATFORM), u5500)
include $(CLEAR_VARS)

LOCAL_MODULE := \
	thermal_sensors.xml

LOCAL_MODULE_CLASS := \
	ETC

LOCAL_MODULE_PATH := \
	$(TARGET_OUT_ETC)

LOCAL_SRC_FILES := \
	thermal_sensors_u5500.xml

LOCAL_MODULE_TAGS := \
	optional

include $(BUILD_PREBUILT)
endif
