#/****************************************************************************
# *                                                                          *
# *  Copyright (C) 2011 ST-Ericsson                                          *
# *                                                                          *
# *  Author: Par-Gunnar HJALMDAHL <par-gunnar.p.hjalmdahl AT stericsson.com> *
# *                                                                          *
# ****************************************************************************/

# Make sure that statements below are included if only ste-cg29xx_ctrl is being built
ifneq ($(ONE_SHOT_MAKEFILE),)
STE_CG29XX_CTRL_ENABLE_FEATURE_STE_CG29XX_CTRL := true
endif

# Only include ste-cg29xx_ctrl if it was enabled
ifeq ($(STE_CG29XX_CTRL_ENABLE_FEATURE_STE_CG29XX_CTRL),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := ste-cg29xx_ctrl
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

endif
