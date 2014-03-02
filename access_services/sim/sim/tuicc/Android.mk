######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : build tuicc executable
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SIM_PATH := $(LOCAL_PATH)/..

LOCAL_MODULE := tuicc
LOCAL_MODULE_TAGS := eng tests debug

ifeq ($(SIM_ENABLE_FEATURE_STRESS_TEST),true)
LOCAL_CFLAGS += -DCFG_ENABLE_SIM_STRESS_TEST
endif

LOCAL_SRC_FILES := tuicc.c

LOCAL_C_INCLUDES := \
	$(SIM_PATH)/include \
	$(SIM_PATH)/libsim \
	$(SIM_PATH)/libsimcom \
	$(SIM_PATH)/simd \
	$(SIM_PATH)/../../common/common_functionality/include

LOCAL_STATIC_LIBRARIES := libsim libsimcom

include $(BUILD_EXECUTABLE)
