######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : Android style makefile
## 
## Author          : Haiyuan Bu <haiyuan.bu@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)
SIM_PATH := $(LOCAL_PATH)/..
CN_PATH := $(ACCESS_SERVICES_PATH)/call_network/call_network

include $(CLEAR_VARS)

LOCAL_MODULE := libpc
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := pc_provide_local_info.c pc_more_time.c pc_polling_off.c pc_poll_interval.c \
	pc_send_dtmf.c pc_send_sms.c pc_send_ss.c pc_send_ussd.c pc_setup_call.c \
	pc_refresh.c pc_setup_event_list.c pc_timer_management.c pc_utilities.c pc_language_notification.c

LOCAL_C_INCLUDES := \
	$(CN_PATH)/libcn/include \
	$(SIM_PATH)/include \
	$(SIM_PATH)/libsimcom \
	$(SIM_PATH)/libapdu \
	$(SIM_PATH)/libsim \
	$(SIM_PATH)/cat \
	$(SIM_PATH)/simd \
	$(SIM_PATH)/uiccd/include \
        $(ACCESS_SERVICES_PATH)/sms/sms/include \
        $(PROCESSING_PATH)/security_framework/cops/cops-api/include

include $(BUILD_STATIC_LIBRARY)
