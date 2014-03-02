######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : builds test program for apdu handling
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)
CN_PATH := $(ACCESS_SERVICES_PATH)/call_network/call_network

include $(CLEAR_VARS)
SIM_PATH := $(LOCAL_PATH)/..
LOCAL_MODULE := tapdu
LOCAL_MODULE_TAGS := eng tests debug

LOCAL_SRC_FILES := apdu_test.c

LOCAL_C_INCLUDES := \
	$(SIM_PATH)/libapdu \
	$(SIM_PATH)/libsimcom \
	$(SIM_PATH)/include \
	$(CN_PATH)/libcn/include


LOCAL_STATIC_LIBRARIES := libapdu libsimcom
LOCAL_SHARED_LIBRARIES := libdl

include $(BUILD_EXECUTABLE)
