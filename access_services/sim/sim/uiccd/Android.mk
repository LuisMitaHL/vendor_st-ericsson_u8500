######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : Android.mk
## Description     : builds uicc
## 
## Author          : Stefan Svenberg <stefan.svenberg@stericsson.com>
## 
###############################################################################

LOCAL_PATH := $(call my-dir)
CN_PATH := $(ACCESS_SERVICES_PATH)/call_network/call_network

include $(CLEAR_VARS)
SIM_PATH := $(LOCAL_PATH)/..
LOCAL_MODULE := uicc
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE:= false

ifeq ($(SIM_ENABLE_MNC_LENGTH_CHECK_SUPPORT),true)
	LOCAL_CFLAGS += -DSIM_MNC_LENGTH_CHECK
endif

ifeq ($(SIM_ENABLE_PIN_CACHE_FOR_MSR),true)
	LOCAL_CFLAGS += -DSIMD_SUPPORT_PIN_CACHE_FOR_MSR
endif

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/../libsimcom \
	$(LOCAL_PATH)/../libapdu \
	$(LOCAL_PATH)/../libsim \
	$(LOCAL_PATH)/../simd \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../cat \
	$(CN_PATH)/libcn/include \
	$(ACCESS_SERVICES_PATH)/common/common_functionality/include \
	$(PROCESSING_PATH)/security_framework/bass_app/teec/include

LOCAL_WHOLE_STATIC_LIBRARIES := libaccsutil_security
LOCAL_SRC_FILES := \
  src/uiccd.c \
  src/uiccd_msg.c \
  src/uicc_sig.c \
  src/uicc_event_state_change_handler.c \
  src/uicc_channel_close_handler.c \
  src/uicc_channel_open_handler.c \
  src/uicc_channel_send_handler.c \
  src/uicc_file_ecc_handler.c \
  src/uicc_file_fdn_read_handler.c \
  src/uicc_file_icon_read_handler.c \
  src/uicc_file_plmn_common.c \
  src/uicc_file_plmn_read_handler.c \
  src/uicc_file_plmn_update_handler.c \
  src/uicc_file_rat_read_handler.c \
  src/uicc_other_sap_handler.c \
  src/uicc_other_sim_reset_handler.c \
  src/uicc_other_deprecated.c \
  src/uicc_other_app_status_handler.c \
  src/uicc_other_card_status_handler.c \
  src/uicc_other_get_sim_state_handler.c \
  src/uicc_other_app_info_handler.c \
  src/uicc_other_sim_status_handler.c \
  src/uicc_other_sim_power_on_off_handler.c \
  src/uicc_other_modem_silent_reset.c \
  src/uicc_file_sim_file_read_generic_handler.c \
  src/uicc_file_smsc_common.c \
  src/uicc_file_smsc_get_record_max_handler.c \
  src/uicc_file_smsc_read_handler.c \
  src/uicc_file_smsc_restore_from_record_handler.c \
  src/uicc_file_smsc_save_to_record_handler.c \
  src/uicc_file_smsc_update_handler.c \
  src/uicc_file_subscriber_number_read_handler.c \
  src/uicc_file_read_sim_file_record_handler.c \
  src/uicc_file_sim_file_get_format_handler.c \
  src/uicc_file_get_file_information_handler.c \
  src/uicc_file_read_sim_file_binary_handler.c \
  src/uicc_file_update_sim_file_record_handler.c \
  src/uicc_file_update_sim_file_binary_handler.c \
  src/uicc_file_service_table_get_handler.c \
  src/uicc_file_service_table_update_handler.c \
  src/uicc_file_service_availability_handler.c \
  src/uicc_file_service_table_common.c \
  src/uicc_pin_verify_handler.c \
  src/uicc_pin_disable_handler.c \
  src/uicc_pin_enable_handler.c \
  src/uicc_pin_info_handler.c \
  src/uicc_pin_change_handler.c \
  src/uicc_pin_unblock_handler.c \
  src/uicc_pin_caching_handler.c \
  src/uicc_state_machine.c
  
include $(BUILD_STATIC_LIBRARY)
