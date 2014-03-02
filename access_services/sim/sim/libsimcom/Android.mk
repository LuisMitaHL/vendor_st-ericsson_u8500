######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : builds libsimcom library
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
SIM_PATH := $(LOCAL_PATH)/..
LOCAL_MODULE := libsimcom
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := \
	$(SIM_PATH)/include \
	$(SIM_PATH)/libsim

LOCAL_SRC_FILES := cat_internal.c sim_internal.c event_stream.c cat_barrier.c msgq.c state_machine.c client_data.c func_trace.c

include $(BUILD_STATIC_LIBRARY)
