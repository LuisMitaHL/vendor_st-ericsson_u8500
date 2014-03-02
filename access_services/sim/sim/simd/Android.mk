######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : builds cat
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
SIM_PATH := $(ACCESS_SERVICES_PATH)/sim/sim
CN_PATH := $(ACCESS_SERVICES_PATH)/call_network/call_network
SMS_PATH := $(ACCESS_SERVICES_PATH)/sms/sms


LOCAL_MODULE := simd
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES := \
	$(SIM_PATH)/include \
	$(SIM_PATH)/libsim \
	$(SIM_PATH)/libsimcom \
	$(SIM_PATH)/cat \
	$(SIM_PATH)/libapdu \
	$(SIM_PATH)/libpc \
	$(SIM_PATH)/uiccd/include \
	$(SMS_PATH)/include \
        $(CN_PATH)/libcn/include \
	$(ACCESS_SERVICES_PATH)/common/common_functionality/include

LOCAL_CFLAGS += -DCFG_USE_ANDROID_LOG

ifeq ($(SIM_ENABLE_PIN_CACHE_FOR_MSR),true)
	LOCAL_CFLAGS += -DSIMD_SUPPORT_PIN_CACHE_FOR_MSR
endif

LOCAL_SRC_FILES := simd.c catd_reader.c catd_cn_client.c simd_timer.c sim_log_client.c

LOCAL_STATIC_LIBRARIES := libaccsutil_security cat uicc libapdu libpc libsimcom

# FIXME: Only link against libsms_server for MAL, not for MSL (where we stub what we use)
# If libsms_server can be built and used on HOST, only then may we use it for MSL.

LOCAL_SHARED_LIBRARIES := liblog libcops libcn libshmnetlnk libutils libtee

ifeq ($(SIM_SET_FEATURE_BACKEND),MAL)
$(info SIMD: Building for MAL backend)
LOCAL_STATIC_LIBRARIES += libmal
LOCAL_SHARED_LIBRARIES += libmaluicc
LOCAL_SHARED_LIBRARIES += libsms_server
else 
ifeq ($(SIM_SET_FEATURE_BACKEND),MSL)
$(info SIMD: Building for MSL backend)
LOCAL_STATIC_LIBRARIES += libmsl
else
$(warning SIMD: backend not configured)
endif
endif

include $(BUILD_EXECUTABLE)
