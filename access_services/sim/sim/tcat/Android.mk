######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : build tcat executable
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)
CN_PATH := $(ACCESS_SERVICES_PATH)/call_network/call_network

include $(CLEAR_VARS)

LOCAL_MODULE := tcat
LOCAL_MODULE_TAGS := eng tests debug

LOCAL_SRC_FILES := tcat.c

LOCAL_C_INCLUDES := \
	$(SIM_PATH)/include \
	$(SIM_PATH)/libsim \
	$(SIM_PATH)/libapdu \
	$(SIM_PATH)/libsimcom \
	$(CN_PATH)/libcn/include

LOCAL_STATIC_LIBRARIES := libsim libapdu libsimcom

include $(BUILD_EXECUTABLE)
