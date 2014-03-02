# Copyright (C) ST-Ericsson AB 2008-2009
# Copyright 2006 The Android Open Source Project
#
# Based on reference-ril
# Modified for ST-Ericsson U300 modems.
# Author: Christian Bejram <christian.bejram@stericsson.com>
#
# XXX using libutils for simulator build only...
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


# Build only if configured to be built
ifeq ($(RIL_ENABLE_FEATURE_RIL), true)

LOCAL_SRC_FILES:= \
	u300-ril.c \
	u300-ril-callhandling.c \
	u300-ril-messaging.c \
	u300-ril-network.c \
	u300-ril-pdp.c \
	u300-ril-requestdatahandler.c \
	u300-ril-services.c \
	u300-ril-sim.c \
	u300-ril-stk.c \
	u300-ril-oem.c \
	u300-ril-netif.c \
	atchannel.c \
	misc.c \
	fcp_parser.c \
	at_tok.c

LOCAL_SHARED_LIBRARIES := libcutils libutils libril libnetutils

# For asprinf
LOCAL_CFLAGS := -D_GNU_SOURCE

# CAIF modem communication
ifeq ($(RIL_ENABLE_FEATURE_CAIF), true)
LOCAL_CFLAGS += -DU300_RIL_CAIF
endif

# Dual channels, one channel for prioritized and one channel
# for non-prioritized AT commands
ifeq ($(RIL_ENABLE_FEATURE_DUAL_CHANNELS), true)
LOCAL_CFLAGS += -DDUAL_CHANNELS
endif

# GPRS auto attach
ifeq ($(RIL_ENABLE_FEATURE_GPRS_AUTO_ATTACH), false)
LOCAL_CFLAGS += -DNO_AUTO_GPRS_ATTACH
endif

# SMS Phase 2+ service
ifeq ($(RIL_ENABLE_FEATURE_SMS_PHASE_2_PLUS), true)
LOCAL_CFLAGS += -DSMS_PHASE_2_PLUS
endif

# FDN service availability
ifeq ($(RIL_ENABLE_FEATURE_FDN_SERVICE_AVAILABILITY), true)
LOCAL_CFLAGS += -DFDN_SERVICE_AVAILABILITY
endif

# U8500 modem, FIXME: to be removed when functionality merged
ifeq ($(RIL_ENABLE_FEATURE_U8500), true)
LOCAL_CFLAGS += -DUSE_U8500_RIL
endif

# SAT ENVELOPE no interleaving support
ifeq ($(SIM_ENABLE_SAT_ENV_NO_INTERLEAVING_SUPPORT),true)
LOCAL_CFLAGS += -DSAT_ENV_NO_INTERLEAVING_SUPPORT
endif

LOCAL_C_INCLUDES := \
	$(KERNEL_HEADERS) \
	$(TOP)/hardware/ril/libril/ \
	$(TOP)/system/core/libnetutils/

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

LOCAL_LDLIBS += -lpthread

LOCAL_CFLAGS += -DRIL_SHLIB

LOCAL_MODULE := libu300-ril
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

##########

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        atchannel.c \
        misc.c \
        at_tok.c

LOCAL_C_INCLUDES := \
	$(TOP)/system/core/libnetutils/

LOCAL_SHARED_LIBRARIES := libcutils libnetutils

# Disable prelink, or add to build/core/prelink-linux-arm.map
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libu300-parser
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

################### Static files ###################

# ril_config

include $(CLEAR_VARS)

LOCAL_PATH := $(LOCAL_PATH)
LOCAL_SRC_FILES := ril_config
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(PRODUCT_OUT)/system/etc

include $(BUILD_PREBUILT)

endif
