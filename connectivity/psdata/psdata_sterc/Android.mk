BASE_PATH := $(call my-dir)

ifeq ($(PSDATA_STEPSCC_SET_MODEM_BEARER), CAIF)
DIRS := \
    $(BASE_PATH)/sterc/Android.mk
else
DIRS := \
    $(BASE_PATH)/libsterc/Android.mk \
    $(BASE_PATH)/plugin/Android.mk \
    $(BASE_PATH)/sterc/Android.mk
endif

include $(DIRS)


################### Static files ###################

ifeq ($(PSDATA_ENABLE_FEATURE_STERC_CONF),true)

STERC_EXAMPLES_PATH := $(LOCAL_PATH)/examples

# sterc_script_connect

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := sterc_script_connect
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

# sterc_script_disconnect

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := sterc_script_disconnect
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

# sterc_conf

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := sterc_conf
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

# sterc_script_connect_alt

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := sterc_script_connect_alt
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

# sterc_conf_alt

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := sterc_conf_alt
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

# sterc_script_connect_dun

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := sterc_script_connect_dun
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

# sterc_script_disconnect_dun

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := sterc_script_disconnect_dun
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/psdata

include $(BUILD_PREBUILT)

# rt_tables

include $(CLEAR_VARS)

LOCAL_PATH := $(STERC_EXAMPLES_PATH)
LOCAL_SRC_FILES := rt_tables
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/iproute2

include $(BUILD_PREBUILT)

endif
