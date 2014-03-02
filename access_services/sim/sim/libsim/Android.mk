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

LOCAL_MODULE := libsim
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libsimcom \
	$(LOCAL_PATH)/../simd \

LOCAL_SRC_FILES := cat.c sync.c uicc.c sim.c



include $(BUILD_STATIC_LIBRARY)
