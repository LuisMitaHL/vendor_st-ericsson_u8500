# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

LOCAL_PATH:= $(call my-dir)

LOCAL_MODULE:= simpbd
LOCAL_MODULE_CLASS := DATA

include $(CLEAR_VARS)

ifeq ($(SIM_ENABLE_FEATURE_SIMPB),true)

###################################################################
#
# Config
#
###################################################################

SIM := $(ACCESS_SERVICES_PATH)/sim/sim
SIMPB := $(ACCESS_SERVICES_PATH)/sim/sim_phonebook
SIMPBD := $(SIMPB)/simpbd
COMMON := $(ACCESS_SERVICES_PATH)/common/common_functionality

LOCAL_CFLAGS := \
	-DCFG_USE_ANDROID_LOG \
	-DCFG_ANDROID_LOG_TAG='"RIL SIMPBD"'

###################################################################
#
# Include Paths
#
###################################################################
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/libsimpb \
	$(SIMPB)/include

LOCAL_C_INCLUDES += \
	$(SIM)/cat/libcat \
	$(SIM)/include \
	$(COMMON)/include


###################################################################
#
# Source Files
#
###################################################################
LOCAL_SRC_FILES:= \
	main/log.c \
	main/main.c \
	main/fdmon.c \
	main/sockserv.c \
	main/simpbd.c \
	main/simpbd_record.c \
	cache/simpbd_cache.c \
	handlers/simpbd_handlers.c \
	handlers/simpbd_handler_error.c

###################################################################
#
# Shared Libraries
#
##################################################################
LOCAL_SHARED_LIBRARIES := libcutils

###################################################################
#
# Static Libraries
#
###################################################################

LOCAL_STATIC_LIBRARIES +=	libsim libsimcom libaccsutil_security

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
###################################################################
#
# Compile Flags
#
###################################################################

#include $(LOCAL_PATH)/Compile_flags.mk

LOCAL_MODULE:= simpbd
include $(BUILD_EXECUTABLE)
endif

