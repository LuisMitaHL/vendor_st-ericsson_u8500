######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : builds Modem Simulation Library
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SIM_DIR := $(ACCESS_SERVICES_PATH)/sim/sim
MODEM_DIR := $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal

LOCAL_MODULE := libmsl

LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES := \
    $(MODEM_DIR)/libshmnetlnk/include \
    $(SIM_DIR)/include \
    $(SIM_DIR)/libsim \
    $(SIM_DIR)/libcat \
    $(SIM_DIR)/libapdu \
    $(SIM_DIR)/cat \
    $(SIM_DIR)/simd \
    $(SIM_DIR)/libsimcom \
    $(SIM_DIR)/uiccd/include \
    $(PROCESSING_PATH)/security_framework/cops/cops-api/include \
    $(ACCESS_SERVICES_PATH)/sms/sms/include

LOCAL_SRC_FILES := sim_stub.c sim_stub_socket.c

LOCAL_SHARED_LIBRARIES := libshmnetlnk

include $(BUILD_STATIC_LIBRARY)
