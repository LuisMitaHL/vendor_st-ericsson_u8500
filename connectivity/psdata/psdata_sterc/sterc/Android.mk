LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(PSDATA_STEPSCC_SET_MODEM_BEARER), CAIF)
LOCAL_C_INCLUDES += \
        $(TOP)/external/dbus/
LOCAL_MODULE := stercd
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := stercd_caif.c sterc_netif.c sterc_misc.c atchannel.c at_tok.c
LOCAL_SHARED_LIBRARIES := liblog libdbus
LOCAL_CFLAGS := -DSTERC_SW_VARIANT_ANDROID

else
LOCAL_MODULE := stercd
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := stercd.c sterc_handler.c sterc_runscript.c sterc_pscc.c sterc_if_ppp.c sterc_log_client.c
LOCAL_SHARED_LIBRARIES := libstecom libmpl libsterc liblog libpscc libutils
LOCAL_STATIC_LIBRARIES := libaccsutil_security
LOCAL_CFLAGS := -DSTERC_SW_VARIANT_ANDROID -DCFG_USE_ANDROID_LOG
LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../../../common/libstecom \
  $(LOCAL_PATH)/../../psdata/libmpl \
  $(LOCAL_PATH)/../../psdata/libnlcom \
  $(LOCAL_PATH)/../../psdata_stepscc/libpscc \
  $(LOCAL_PATH)/../libsterc \
  $(ACCESS_SERVICES_PATH)/common/common_functionality/include
endif

include $(BUILD_EXECUTABLE)
