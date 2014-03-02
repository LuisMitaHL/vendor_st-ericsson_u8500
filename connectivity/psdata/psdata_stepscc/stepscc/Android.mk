LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


ifeq ($(PSDATA_STEPSCC_SET_MODEM_BEARER), MAL)
  $(info PSDATA_STEPSCC: Building for MAL backend)
  MAL := $(LOCAL_PATH)/../../../../access_services/modem_adaptations/mal/modem_lib/mal
  LOCAL_SRC_FILES_MODEM := pscc_qpc_mal.c pscc_bearer_mal.c
  LOCAL_SHARED_LIBRARIES_MODEM := libmalgpds \
    libshmnetlnk \
    libmalpipe # This dependency is added ONLY to fix a build problem when building Moderated Modem package
  LOCAL_C_INCLUDES_MODEM := \
    $(MAL)/libmalgpds/include \
    $(MAL)/libshmnetlnk/include
else
  ifeq ($(PSDATA_STEPSCC_SET_MODEM_BEARER), MFL)
    $(info PSDATA_STEPSCC: Building for MFL backend)
    LOCAL_SRC_FILES_MODEM := pscc_bearer_mfl.c pscc_netdev_caif.c pscc_pco.c
    LOCAL_SHARED_LIBRARIES_MODEM := libmfl libmfl_cas libcrypto libnlcom
    LOCAL_C_INCLUDES_MODEM := \
      $(LOCAL_PATH)/../../../../access_services/modem_adaptations/mfl/mfl_proxy_cas/include \
      $(LOCAL_PATH)/../../../../access_services/modem_adaptations/mfl/mfl_framework/include
  endif
endif




LOCAL_MODULE := psccd
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := pscc_handler.c psccd.c pscc_object.c pscc_runscript.c pscc_utils.c pscc_nwifcfg.c pscc_sim.c pscc_log_client.c $(LOCAL_SRC_FILES_MODEM)
LOCAL_SHARED_LIBRARIES := liblog libstecom libmpl libpscc libutils $(LOCAL_SHARED_LIBRARIES_MODEM)
LOCAL_STATIC_LIBRARIES := libsim libsimcom libaccsutil_security
LOCAL_CFLAGS := -DPSCC_SW_VARIANT_ANDROID -DCFG_USE_ANDROID_LOG
LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/../../../common/libstecom \
  $(LOCAL_PATH)/../../psdata/libmpl \
  $(LOCAL_PATH)/../libpscc \
  $(LOCAL_PATH)/../../psdata/libnlcom \
  $(ACCESS_SERVICES_PATH)/common/common_functionality/include \
  $(LOCAL_C_INCLUDES_MODEM) \
  external/openssl/include/ \
  $(LOCAL_PATH)/../../../../access_services/sim/sim/include
LOCAL_LDLIBS := -lm

include $(BUILD_EXECUTABLE)


################### Static files ###################

ifeq ($(PSDATA_ENABLE_FEATURE_PSCC_CONF),true)

STEPSCC_EXAMPLES_PATH := $(LOCAL_PATH)/examples

# stepscc_conf

include $(CLEAR_VARS)

LOCAL_PATH := $(STEPSCC_EXAMPLES_PATH)
LOCAL_SRC_FILES := stepscc_conf
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

endif
