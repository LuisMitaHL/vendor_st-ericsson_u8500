#
# Copyright (C) ST-Ericsson SA 2010
#

LOCAL_PATH:= $(call my-dir)

# Generate product info
LOCAL_MODULE:= at_core
LOCAL_MODULE_CLASS := DATA

include $(CLEAR_VARS)

ifeq ($(ATC_ENABLE_FEATURE_ATC),true)

###################################################################
#
# Config
#
###################################################################

include $(LOCAL_PATH)/Config.mk

# Make AT responses for ITU V.250 commands easier to read by adding
# an extra pair of S3+S4 before the final response code.
ATC_ENABLE_FEATURE_READABILITY ?= true

EXE_PATH := $(LOCAL_PATH)/internal/executor

###################################################################
#
# Include Paths
#
###################################################################
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
	$(LOCAL_PATH)/config/include \
	$(LOCAL_PATH)/internal/main/include \
	$(LOCAL_PATH)/internal/anchor/include \
	$(LOCAL_PATH)/internal/executor/include \
	$(LOCAL_PATH)/internal/parser/include \
	$(LOCAL_PATH)/internal/predial/include \
	$(LOCAL_PATH)/internal/selector/include \
	$(TOP)/hardware/ril/libril \
	$(ACCESS_SERVICES_PATH)/common/common_functionality/include

ifeq ($(EXE_USE_AUDIO_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/audio/include \
					$(AUDIO)/include
endif

ifeq ($(EXE_USE_CN_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/cn/include \
					$(CN)/libcn/include
endif

ifeq ($(EXE_USE_COPS_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/cops/include \
					$(COPS)/cops-api/include
endif

ifeq ($(EXE_USE_PSCC_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/pscc/include \
					$(PSCC)/common/libstecom \
					$(PSCC)/psdata/psdata/libmpl \
					$(PSCC)/psdata/psdata_stepscc/libpscc \
					$(PSCC)/psdata/psdata/libpscc \
					$(PSCC)/psdata/psdata_sterc/libsterc \
					$(TOP)/system/core/libnetutils
endif

ifeq ($(EXE_USE_SIM_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/sim/include \
					$(SIM)/cat/libcat \
					$(SIM)/include
endif

ifeq ($(EXE_USE_SIMPB_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/simpb/include \
                                        $(SIMPB)/include
endif


ifeq ($(EXE_USE_SMS_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/sms/include \
					$(SMS)/include \
					$(SMS)/internal/porting/include

endif

ifeq ($(EXE_USE_CSPSA_SERVICE),true)
LOCAL_C_INCLUDES += $(EXE_PATH)/storage/include \
					$(CSPSA)
endif

###################################################################
#
# Source Files
#
###################################################################
LOCAL_SRC_FILES:= \
	internal/anchor/src/atc_anchor.c \
	internal/executor/src/exe.c \
	internal/executor/src/exe_start.c \
	internal/executor/src/exe_request_record.c \
	internal/executor/src/exe_last_fail_cause.c \
	internal/executor/src/exe_util.c \
	internal/main/src/atc.c \
	internal/main/src/atc_context.c \
	internal/main/src/atc_debug.c \
	internal/main/src/atc_sockets.c \
	internal/main/src/atc_log_client.c \
	internal/parser/src/atc_command_table.c \
	internal/parser/src/atc_exe_glue.c \
	internal/parser/src/atc_hand_at_control.c \
	internal/parser/src/atc_hand_audio.c \
	internal/parser/src/atc_hand_call.c \
	internal/parser/src/atc_hand_clock.c \
	internal/parser/src/atc_hand_debug.c \
	internal/parser/src/atc_hand_flags.c \
	internal/parser/src/atc_hand_me_control.c \
	internal/parser/src/atc_hand_net.c \
	internal/parser/src/atc_hand_ps.c \
	internal/parser/src/atc_hand_sat.c \
	internal/parser/src/atc_hand_security.c \
	internal/parser/src/atc_hand_sim.c \
	internal/parser/src/atc_hand_sms.c \
	internal/parser/src/atc_hand_ss.c \
	internal/parser/src/atc_string.c \
	internal/parser/src/atc_hand_storage.c \
	internal/parser/src/atc_parser_util.c \
	internal/parser/src/atc_parser.c \
	internal/parser/src/c_type.c \
	internal/predial/src/predial_check.c \
	internal/selector/src/atc_selector.c

ifeq ($(EXE_USE_AUDIO_SERVICE),true)
LOCAL_SRC_FILES +=  internal/executor/audio/src/exe_audio_requests.c
endif

ifeq ($(EXE_USE_COPS_SERVICE),true)
LOCAL_SRC_FILES +=	internal/executor/cops/src/exe_cops_client.c \
					internal/executor/cops/src/exe_cops_requests.c
endif

ifeq ($(EXE_USE_CN_SERVICE),true)
LOCAL_SRC_FILES +=	internal/executor/cn/src/exe_cn_client.c \
					internal/executor/cn/src/exe_cn_events.c \
					internal/executor/cn/src/exe_cn_requests.c \
					internal/executor/cn/src/exe_cn_utils.c
endif

ifeq ($(EXE_USE_PSCC_SERVICE),true)
LOCAL_SRC_FILES += internal/executor/pscc/src/exe_pscc_client.c \
					internal/executor/pscc/src/exe_pscc_requests.c \
					internal/executor/pscc/src/exe_pscc_iadb.c \
					internal/executor/pscc/src/exe_pscc_events.c \
					internal/executor/pscc/src/exe_pscc_requests_data_call_list.c \
					internal/executor/pscc/src/exe_pscc_requests_deactivate_data_call.c \
					internal/executor/pscc/src/exe_pscc_requests_modify_data_call.c \
					internal/executor/pscc/src/exe_pscc_requests_nap.c \
					internal/executor/pscc/src/exe_pscc_requests_dun.c \
					internal/executor/pscc/src/exe_pscc_requests_send_data_uplink_call.c \
					internal/executor/pscc/src/exe_pscc_requests_setup_data_call.c \
					internal/executor/pscc/src/exe_pscc_util.c
endif

ifeq ($(EXE_USE_SIM_SERVICE),true)
LOCAL_SRC_FILES += internal/executor/sim/src/exe_sim_client.c \
					internal/executor/sim/src/exe_sim_requests.c
endif

ifeq ($(EXE_USE_SIMPB_SERVICE),true)
LOCAL_SRC_FILES += internal/executor/simpb/src/exe_simpb_client.c
endif

ifeq ($(EXE_USE_SMS_SERVICE),true)
LOCAL_SRC_FILES  += internal/executor/sms/src/exe_sms_client.c \
					internal/executor/sms/src/exe_sms_requests.c \
					internal/executor/sms/src/exe_sms_requests_cbs.c \
					internal/executor/sms/src/exe_sms_utils.c
endif

ifeq ($(EXE_USE_CSPSA_SERVICE),true)
LOCAL_SRC_FILES  += internal/executor/storage/src/exe_cspsa_requests.c
endif

LOCAL_SRC_FILES  += internal/executor/src/exe_requests.c

###################################################################
#
# Shared Libraries
#
###################################################################
LOCAL_SHARED_LIBRARIES := libcutils liblog libnetutils libril libdl

ifeq ($(EXE_USE_CN_SERVICE),true)
LOCAL_SHARED_LIBRARIES += libcn
endif

ifeq ($(EXE_USE_PSCC_SERVICE),true)
LOCAL_SHARED_LIBRARIES += libstecom libmpl libpscc libsterc
endif

ifeq ($(EXE_USE_SIM_SERVICE),true)
endif

ifeq ($(EXE_USE_SMS_SERVICE),true)
LOCAL_SHARED_LIBRARIES += libsms_server
endif

ifeq ($(EXE_USE_COPS_SERVICE),true)
LOCAL_SHARED_LIBRARIES += libcops
endif

ifeq ($(EXE_USE_CSPSA_SERVICE),true)
LOCAL_SHARED_LIBRARIES += libcspsa
endif

LOCAL_LDLIBS += -lpthread -lm

###################################################################
#
# Static Libraries
#
###################################################################
LOCAL_STATIC_LIBRARIES :=	libaccsutil_security

ifeq ($(EXE_USE_AUDIO_SERVICE),true)

ifeq ($(EXE_USE_AUDIO_SERVICE_LEGACY),true)
LOCAL_STATIC_LIBRARIES +=	libste_adm
else
LOCAL_SHARED_LIBRARIES +=       libste_adm
endif
endif

ifeq ($(EXE_USE_SIM_SERVICE),true)
LOCAL_STATIC_LIBRARIES +=	libsim libsimcom
endif

ifeq ($(EXE_USE_SIMPB_SERVICE),true)
LOCAL_STATIC_LIBRARIES +=       libsimpb
endif
# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

###################################################################
#
# Compile Flags
#
###################################################################

include $(LOCAL_PATH)/Compile_flags.mk

ifeq ($(ATC_ENABLE_FEATURE_ATC_CUSTOMER_EXTENSIONS),true)
# This file describes how to build the at_core_extension so that,
# those files are built together with at_core_common
$(info AT-Core: Building with AT_CORE_EXTENSION)
include $(ACCESS_SERVICES_PATH)/at/at_core_extension/inc_Android.mk
else
$(info AT-Core: Building without AT_CORE_EXTENSION)
endif

LOCAL_MODULE:= at_core
LOCAL_MODULE_TAGS := optional

ifeq ($(ATC_ENABLE_FEATURE_FTD),true)
LOCAL_CFLAGS += -DENABLE_FTD
endif
include $(BUILD_EXECUTABLE)


###################################################################
#
# Runtime Configuration Files
#
###################################################################

LOCAL_MODULE:= at_core
LOCAL_MODULE_CLASS := ETC
AT_CORE_VENDOR := "ST-Ericsson"
AT_CORE_INTERMEDIATES := $(call local-intermediates-dir)

ifneq ($(VENDOR),)
ifneq ($(VENDOR),unknown)
AT_CORE_VENDOR := $(VENDOR)
endif
endif

$(shell mkdir -p $(AT_CORE_INTERMEDIATES))
$(shell sh $(LOCAL_PATH)/internal/scripts/create_system_propfile.sh $(AT_CORE_INTERMEDIATES) $(AT_CORE_VENDOR) $(subst ste_,,$(TARGET_PRODUCT)))

# system_id.cfg

include $(CLEAR_VARS)

LOCAL_PATH := $(AT_CORE_INTERMEDIATES)
LOCAL_SRC_FILES := system_id.cfg
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/AT

include $(BUILD_PREBUILT)

# manuf_id.cfg

include $(CLEAR_VARS)

LOCAL_PATH := $(AT_CORE_INTERMEDIATES)
LOCAL_SRC_FILES := manuf_id.cfg
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/AT

include $(BUILD_PREBUILT)

# model_id.cfg

include $(CLEAR_VARS)

LOCAL_PATH := $(AT_CORE_INTERMEDIATES)
LOCAL_SRC_FILES := model_id.cfg
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc/AT

include $(BUILD_PREBUILT)

endif
