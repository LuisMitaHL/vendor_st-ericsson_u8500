LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ppp_sterc
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ppp_sterc.c
LOCAL_SHARED_LIBRARIES := libmpl libsterc libstecom
LOCAL_C_INCLUDES := \
  external/ppp/pppd \
  external/ppp/pppd/include \
  $(LOCAL_PATH)/../../../common/libstecom \
  $(LOCAL_PATH)/../../psdata/libmpl \
  $(LOCAL_PATH)/../libsterc

LOCAL_ALLOW_UNDEFINED_SYMBOLS:=true

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

