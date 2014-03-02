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

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# No source code, treat binary files as prebuilt

LOCAL_MODULE := msa
LOCAL_MODULE_TAGS := optional

# This is the file that will be copied
LOCAL_PREBUILT_EXECUTABLES := msa

include $(BUILD_MULTI_PREBUILT)

endif
