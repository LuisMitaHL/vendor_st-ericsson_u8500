######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : Android style makefile
## 
## Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)
SIM_PATH := $(LOCAL_PATH)/..
CN_PATH := $(ACCESS_SERVICES_PATH)/call_network/call_network

include $(CLEAR_VARS)

LOCAL_MODULE := libapdu
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := apdu.c apdu_utilities.c
LOCAL_C_INCLUDES := \
	$(CN_PATH)/libcn/include \
	$(SIM_PATH)/include \
	$(SIM_PATH)/libsimcom

include $(BUILD_STATIC_LIBRARY)
