#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson                                      *
# *                                                                      *
# *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
# *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
# *                                                                      *
# ************************************************************************/

# Make sure that statements below are included if only MSA is being built
ifneq ($(ONE_SHOT_MAKEFILE),)
MSA_ENABLE_FEATURE_MSA := true
endif

# Only include MSA if it was enabled
ifeq ($(MSA_ENABLE_FEATURE_MSA),true)

LOCAL_PATH:= $(call my-dir)

#-ffunction-sections -fdata-sections will section the functions and data during compilation
#-Wl,--gc-sections will remove the sections not used during linking
common_cflags := -Wall -Werror -std=gnu99 -ffunction-sections -fdata-sections -Wl,--gc-sections

# Build msa
include $(CLEAR_VARS)

LOCAL_CFLAGS += $(common_cflags)
LOCAL_C_INCLUDES += $(ACCESS_SERVICES_PATH)/modem_adaptations/mal/modem_lib/mal/libshmnetlnk/include

LOCAL_SHARED_LIBRARIES += libshmnetlnk

LOCAL_SRC_FILES := \
	main.c \
	config.c \
	log.c \
	fsa.c \
	posix.c \
	convenience.c \
	process.c \
	state.c \
	wakelock.c

LOCAL_MODULE := msa
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

endif

