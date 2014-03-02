######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : Builds Modem Access Library
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)

# Check our configuration, because we will not build without uiccmal

SIM_DIR := $(ACCESS_SERVICES_PATH)/sim/sim
MODEM_DIR := $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal
CN_PATH := $(ACCESS_SERVICES_PATH)/call_network/call_network
include $(CLEAR_VARS)

LOCAL_MODULE := libmal
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := \
	$(MODEM_DIR)/libmaluicc/include \
	$(MODEM_DIR)/mal_util/include \
	$(MODEM_DIR)/libmalmce/include \
	$(MODEM_DIR)/libshmnetlnk/include \
        $(SIM_DIR)/include \
	$(SIM_DIR)/libsimcom \
	$(SIM_DIR)/libapdu \
        $(SIM_DIR)/cat \
        $(SIM_DIR)/simd \
        $(SIM_DIR)/libsim \
        $(SIM_DIR)/uiccd/include \
	$(CN_PATH)/libcn/include \
        $(ACCESS_SERVICES_PATH)/common/common_functionality/include

LOCAL_CFLAGS += -Wall

LOCAL_SRC_FILES := \
  sim_mal_modem.c \
  sim_file_path.c \
  sim_malmce_rsp_trace.c \
  sim_maluicc_req_trace.c \
  sim_maluicc_rsp_trace.c \
  sim_trace_util.c

LOCAL_SHARED_LIBRARIES := libcn

ifneq ($(SIM_DETECT_PATH),)
LOCAL_CFLAGS += -DSET_DETECT_PATH=$(SIM_DETECT_PATH)
endif

include $(BUILD_STATIC_LIBRARY)
