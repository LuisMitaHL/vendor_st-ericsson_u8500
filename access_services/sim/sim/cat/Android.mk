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

LOCAL_MODULE := cat
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE:= false

LOCAL_C_INCLUDES := \
	$(CN_PATH)/libcn/include \
	$(SIM_PATH)/include \
	$(SIM_PATH)/libsimcom \
	$(SIM_PATH)/libapdu \
	$(SIM_PATH)/libsim \
	$(SIM_PATH)/libpc \
	$(SIM_PATH)/simd \
	$(SIM_PATH)/uiccd/include \
	$(SMS_PATH)/include \
	$(ACCESS_SERVICES_PATH)/common/common_functionality/include

LOCAL_SRC_FILES :=  catd.c catd_msg.c catd_tpl.c

LOCAL_WHOLE_STATIC_LIBRARIES := libaccsutil_security

LOCAL_SHARED_LIBRARIES := libcn

LOCAL_CFLAGS := \
   -DPLATFORM_ANDROID \
   $(CFLAGS_CUSTOM)

ifeq ($(SIM_ENABLE_SAT_BIP_SUPPORT),true)
	LOCAL_CFLAGS += -DSAT_BIP_SUPPORT
endif

ifdef SIM_SET_POLL_INTERVALL
	LOCAL_CFLAGS += -DSET_POLL_INTERVALL=$(SIM_SET_POLL_INTERVALL)
endif

ifdef SIM_SET_TERMINAL_PROFILE
	LOCAL_CFLAGS += -DSET_TERMINAL_PROFILE=$(SIM_SET_TERMINAL_PROFILE)
endif

ifdef SIM_SET_ADAPT_TERMINAL_SUPPORT_TABLE
	LOCAL_CFLAGS += -DSET_ADAPT_TERMINAL_SUPPORT_TABLE=$(SIM_SET_ADAPT_TERMINAL_SUPPORT_TABLE)
endif

ifeq ($(SIM_ENABLE_SAT_PLI_LANGUAGE_SUPPORT),true)
	LOCAL_CFLAGS += -DSAT_PLI_LANGUAGE_SUPPORT
endif

ifeq ($(SIM_ENABLE_SAT_EVENT_LANGUAGE_SUPPORT),true)
	LOCAL_CFLAGS += -DSAT_EVENT_LANGUAGE_SUPPORT
endif

ifeq ($(SIM_ENABLE_SAT_ENV_NO_INTERLEAVING_SUPPORT),true)
	LOCAL_CFLAGS += -DSAT_ENV_NO_INTERLEAVING_SUPPORT
endif

include $(BUILD_STATIC_LIBRARY)
