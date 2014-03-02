######################### -*- Mode: Makefile-Gmake -*- ########################
## Copyright (C) 2010, ST-Ericsson
##
## File name       : Android.mk
## Description     : Top level Android make file for call_network system
##
## Author          : Mats Bengtsson <mats.m.bengtsson@stericsson.com>
## Author          : Johan Norberg  <johan.xx.norberg@stericsson.com>
##
###############################################################################

# Top-level filter
ifeq ($(CN_ENABLE_FEATURE_CN),true)

# Global definitions
LOCAL_PATH:= $(call my-dir)
SIM := $(ACCESS_SERVICES_PATH)/sim/sim

CN_ENABLE_DEBUG := true

# Enable RAT balancing only if the modem is not doing some RAT balancing by itself
CN_ENABLE_FEATURE_RAT_BALANCING := false

# MAL backend specific settings
ifeq ($(CN_SET_FEATURE_BACKEND),MAL)
$(info CALL_NETWORK: Building for MAL backend)
CN_ENABLE_FEATURE_MAL := true
CN_ENABLE_FEATURE_MFL := false

CN_ENABLE_FEATURE_MAL_CS := true
CN_ENABLE_FEATURE_MAL_GSS := true
CN_ENABLE_FEATURE_MAL_MCE := true
CN_ENABLE_FEATURE_MAL_MIS := true
CN_ENABLE_FEATURE_MAL_NET := true
CN_ENABLE_FEATURE_MAL_MIS := true
CN_ENABLE_FEATURE_MAL_RF := true
CN_ENABLE_FEATURE_MAL_MTE := true
CN_ENABLE_FEATURE_MAL_NETLNK := true
CN_ENABLE_FEATURE_MAL_NVD := true
ifeq ($(CN_ENABLE_FEATURE_FTD),true)
CN_ENABLE_FEATURE_MAL_FTD := true
endif
MAL := $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal

else

# MFL backend specific settings
ifeq ($(CN_SET_FEATURE_BACKEND),MFL)
$(info CALL_NETWORK: Building for MFL backend)
CN_ENABLE_FEATURE_MAL := false
CN_ENABLE_FEATURE_MFL := true
MFL := $(ACCESS_SERVICES_PATH)/mfl

else

$(error CALL_NETWORK: No backend selected!)

endif
endif

################## Server Executable ##################
include $(CLEAR_VARS)

### Include directories ###
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/libcn/include \
	$(LOCAL_PATH)/internal/cnserver/include \
	$(LOCAL_PATH)/internal/main/include \
	$(SIM)/include \
	$(SIM)/cat/libcat \
	external/icu4c/common \
	$(ACCESS_SERVICES_PATH)/common/common_functionality/include \
	$(ACCESS_SERVICES_PATH)/modem_adaptations/modem-supervisor

ifeq ($(CN_ENABLE_FEATURE_MAL),true)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/internal/mal/include \
	$(MAL)/mal_util/include \
	$(MAL)/libmalcs/include \
	$(MAL)/libmalgss/include \
	$(MAL)/libmalmce/include \
	$(MAL)/libmalmis/include \
	$(MAL)/libmalnet/include \
	$(MAL)/libmalmte/include \
	$(MAL)/libmalrf/include \
	$(MAL)/libshmnetlnk/include \
	$(MAL)/libmalnvd/include \
	$(MAL)/libmalftd/include \
	$(MAL)/nwm_include
endif

ifeq ($(CN_ENABLE_FEATURE_MFL),true)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/internal/mfl/include
endif

### Source files ###
LOCAL_SRC_FILES := \
	libcn/src/cn_log.c \
	internal/main/src/main.c \
	internal/main/src/fdmon.c \
	internal/main/src/sockserv.c \
	internal/cnserver/src/message_handler.c \
	internal/cnserver/src/cnserver.c \
	internal/cnserver/src/cn_timer.c \
	internal/cnserver/src/cn_utils.c \
	internal/cnserver/src/cn_ss_command_handling.c \
	internal/cnserver/src/cn_pdc_ecc_list_handling.c \
	internal/cnserver/src/cn_pdc_main.c \
	internal/cnserver/src/sim_client.c \
	internal/cnserver/src/cn_pdc_msg_translation.c \
	internal/cnserver/src/cn_pdc_ecc_check.c \
	internal/cnserver/src/cn_pdc_fdn_check.c \
	internal/cnserver/src/cn_pdc_sat_cc.c \
	internal/cnserver/src/cn_pdc_utils.c \
	internal/cnserver/src/str_convert.c \
	internal/cnserver/src/plmn_main.c \
	internal/cnserver/src/plmn_list.c \
	internal/cnserver/src/plmn_file.c \
	internal/cnserver/src/plmn_sim.c \
	internal/cnserver/src/cn_battery_client.c \
	internal/cnserver/src/cn_battery_main.c \
	internal/cnserver/src/cn_rat_control.c \
	internal/cnserver/src/cn_log_client.c \
	internal/cnserver/src/cn_message_strings.c \
	internal/cnserver/src/cn_event_trigger_level.c

ifeq ($(CN_ENABLE_FEATURE_MAL),true)
LOCAL_SRC_FILES += \
	internal/mal/src/mal_client.c \
	internal/mal/src/request_handlers.c \
	internal/mal/src/event_handlers.c \
	internal/mal/src/utils.c \
	internal/mal/src/dial_dtmf_handler.c
endif

ifeq ($(CN_ENABLE_FEATURE_MFL),true)
LOCAL_SRC_FILES += \
	internal/mfl/src/mfl_client.c \
	internal/mfl/src/request_handlers.c \
	internal/mfl/src/event_handlers.c
endif

### Compiler flags and linking directives ###
LOCAL_CFLAGS := -fno-short-enums -Wall -DCFG_USE_ANDROID_LOG -DUSE_UCNV
LOCAL_LDLIBS := -lm
LOCAL_SHARED_LIBRARIES := libcutils libutils libicuuc
LOCAL_STATIC_LIBRARIES := libsim libsimcom libaccsutil_security libmsup

ifeq ($(CN_ENABLE_FEATURE_MAL),true)
LOCAL_CFLAGS += -DUSE_MAL_BACKEND
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_NETLNK),true)
LOCAL_SHARED_LIBRARIES += libshmnetlnk
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_CS),true)
LOCAL_CFLAGS += -DUSE_MAL_CS
LOCAL_SHARED_LIBRARIES += libmalcs \
  libphonet libisimessage # These dependencies are added ONLY to fix a build problem when building Moderated Modem package
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_GSS),true)
LOCAL_CFLAGS += -DUSE_MAL_GSS
LOCAL_SHARED_LIBRARIES += libmalgss
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_MCE),true)
LOCAL_CFLAGS += -DUSE_MAL_MCE
LOCAL_SHARED_LIBRARIES += libmalmce
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_MIS),true)
LOCAL_CFLAGS += -DUSE_MAL_MIS
LOCAL_SHARED_LIBRARIES += libmalmis
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_NET),true)
LOCAL_CFLAGS += -DUSE_MAL_NET
LOCAL_SHARED_LIBRARIES += libmalnet
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_MTE),true)
LOCAL_CFLAGS += -DUSE_MAL_MTE
LOCAL_SHARED_LIBRARIES += libmalmte
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_FTD),true)
LOCAL_CFLAGS += -DUSE_MAL_FTD
LOCAL_SHARED_LIBRARIES += libmalftd
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_NVD),true)
LOCAL_CFLAGS += -DUSE_MAL_NVD
LOCAL_SHARED_LIBRARIES += libmalnvd
endif

ifeq ($(CN_ENABLE_FEATURE_MAL_RF),true)
LOCAL_CFLAGS += -DUSE_MAL_RF
LOCAL_SHARED_LIBRARIES += libmalrf
endif

ifeq ($(CN_ENABLE_FEATURE_MFL),true)
LOCAL_CFLAGS += -DUSE_MFL_BACKEND
endif

ifeq ($(CN_ENABLE_FEATURE_RAT_BALANCING),true)
LOCAL_CFLAGS += -DENABLE_RAT_BALANCING
endif

ifeq ($(CN_REMOVE_PLUS_IN_DIAL_STRING),true)
LOCAL_CFLAGS += -DREMOVE_PLUS_IN_DIAL_STRING
endif

ifeq ($(CN_ENABLE_DEBUG),true)
LOCAL_CFLAGS += -DCN_DEBUG_ENABLED
endif

### Server executable ###
LOCAL_MODULE:=cn_server
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


################## Client library ##################
include $(CLEAR_VARS)

LOCAL_C_INCLUDES+= \
	$(LOCAL_PATH)/libcn/include \
	$(LOCAL_PATH)/internal/cnserver/include \
	$(ACCESS_SERVICES_PATH)/common/common_functionality/include

LOCAL_CFLAGS := -fno-short-enums -Wall -DCFG_USE_ANDROID_LOG
LOCAL_LDLIBS := -lm

LOCAL_SRC_FILES:= \
	libcn/src/cn_client.c \
	libcn/src/cn_log.c

LOCAL_SHARED_LIBRARIES := libutils
LOCAL_STATIC_LIBRARIES := libaccsutil_security

LOCAL_MODULE:=libcn
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)


################### Static files ###################

CN_PLMN_PATH := $(LOCAL_PATH)/internal

# plmn.operator.list

include $(CLEAR_VARS)

LOCAL_PATH := $(CN_PLMN_PATH)
LOCAL_SRC_FILES := plmn.operator.list
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc

include $(BUILD_PREBUILT)

# plmn.latam.list

include $(CLEAR_VARS)

LOCAL_PATH := $(CN_PLMN_PATH)
LOCAL_SRC_FILES := plmn.latam.list
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc

include $(BUILD_PREBUILT)

endif # ifeq CN_ENABLE_FEATURE_CN,true
