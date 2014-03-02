LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ANDROID_VER_GINGERBREAD := true
ifeq ($(LBS_SET_NDK_BUILD),true)
LOCAL_SET_NDK_BUILD := true
endif

LOCAL_SHARED_LIBRARIES := libclientgps libcutils


LOCAL_CFLAGS += -DAGPS_ANDR_ECLAIR_FTR -DAGPS_ANDROID_HEX_NOTIFICATION_FTR
LOCAL_CFLAGS += -DDEBUG_LOG_LEV2

LOCAL_SRC_FILES += srce/gpsinterface.c

ifeq ($(ANDROID_VER_GINGERBREAD),true)
LOCAL_CFLAGS += -DAGPS_ANDR_GINGER_FTR
LOCAL_SRC_FILES += srce/gps_hal_interface.c
LOCAL_C_INCLUDES += $(LOCAL_PATH)/incl
LOCAL_MODULE_TAGS := optional
endif

LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/../lbs_core/lbsclient/incl  \
			$(LOCAL_PATH)/../lbs_core/include         \
			$(LOCAL_PATH)/../lbs_core/lbsd/libagpsosa/incl

LOCAL_PRELINK_MODULE := false

ifeq ($(LOCAL_SET_NDK_BUILD),true)
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/../lbs_external/utils_lib
LOCAL_LDLIBS := -llog
endif

ifeq ($(ANDROID_VER_GINGERBREAD),true)
LOCAL_MODULE:= gps.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
else
LOCAL_MODULE:= libgps
endif

include $(BUILD_SHARED_LIBRARY)



