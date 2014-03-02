LOCAL_PATH := $(call my-dir)

# Build thermald
include $(CLEAR_VARS)

LOCAL_MODULE:= \
	thermald

LOCAL_SRC_FILES = \
	log.c \
	actions.c \
	config.c \
	hwmon.c \
	main.c \
	sensors.c \
	mitigation_actions.c \
	socket.c

LOCAL_SHARED_LIBRARIES += \
	libcutils

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libthermal

LOCAL_CFLAGS += \
	-DANDROID_BUILD

ifeq ($(THERMALSENSOR_SET_PLATFORM), u5500)
LOCAL_CFLAGS += \
	-DTHERMAL_SERVICE_U5500
else
LOCAL_CFLAGS += \
	-DTHERMAL_SERVICE_U8500
endif

LOCAL_MODULE_TAGS := \
	eng

#################################################
# ThermalService optional extras...
#################################################

# STE modem thermal mitigations
ifeq ($(CN_ENABLE_FEATURE_MAD),true)
LOCAL_CFLAGS += \
	-DSTE_MODEM_THERMAL_MITIGATIONS

LOCAL_SRC_FILES += \
	modem_mitigations.c

LOCAL_C_INCLUDES += \
	$(call include-path-for, dbus)

LOCAL_SHARED_LIBRARIES += \
	libdbus
endif
#################################################

include $(BUILD_EXECUTABLE)

# Copy thermal.conf over to /system/etc
include $(CLEAR_VARS)

LOCAL_MODULE := \
	thermal.conf

LOCAL_MODULE_CLASS := \
	ETC

LOCAL_MODULE_PATH := \
	$(TARGET_OUT_ETC)

ifeq ($(THERMALSENSOR_SET_PLATFORM), u5500)
LOCAL_SRC_FILES := \
	u5500_thermal.conf
else
LOCAL_SRC_FILES := \
	u8500_thermal.conf
endif

LOCAL_MODULE_TAGS := \
	eng

include $(BUILD_PREBUILT)
