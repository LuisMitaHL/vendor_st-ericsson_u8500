#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

# Allows to select one media - for ex.: make BUILD_ALL=false BUILD_AUDIO=true
BUILD_ALL=true

BUILD_SHARED=$(BUILD_ALL)
BUILD_AUDIO=$(BUILD_ALL)
BUILD_VIDEO=$(BUILD_ALL)
BUILD_IMAGING=false
CAMERA_ENABLE_FEATURE_RAW_SENSOR?=$(BUILD_ALL)
CAMERA_ENABLE_FEATURE_YUV_CAMERA?=$(BUILD_ALL)

BUILD_MALI=false
BUILD_KHRONOS_CTS=false

BUILD_VALID=$(BUILD_ALL)
BUILD_ITE_OMX_USE_CASES=false

################################################################################

DIRECTORIES_TO_SETUP := shared/mmenv shared/makefiles audio/makefiles

################################################################################
# shared

ifeq ($(BUILD_SHARED),true)

REF_DIRS_TO_BUILD += \
shared/ste_shai \
shared/ostgrp \
shared/osttools/osttc \
shared/host_trace \
shared/utils \
shared/nmf/tools \
shared/omxil

endif #ifeq ($(BUILD_SHARED),true)

################################################################################
# audio

ifeq ($(BUILD_AUDIO),true)

# Build mmprobe if feature enabled in BoardConfig.mk
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
REF_DIRS_TO_BUILD += \
audio/mmprobe
endif

REF_DIRS_TO_BUILD += \
audio/tools \
audio/audio_chipset_apis \
audio/aacenc \
audio/aac_arm \
audio/audiolibs \
audio/audio_3d_mixer_arm \
audio/amr \
audio/amrwb \
audio/gsmfr \
audio/gsmhr \
audio/mp3 \
audio/wmapro_v10 \
audio/mixer \
audio/samplerateconv \
audio/volctrl \
audio/updownmix \
audio/mdrc
endif #ifeq ($(BUILD_AUDIO),true)

################################################################################
# video

ifeq ($(BUILD_VIDEO),true)

REF_DIRS_TO_BUILD += \
video/g1dec

endif #ifeq ($(BUILD_VIDEO),true)

################################################################################
# imaging

ifeq ($(BUILD_IMAGING),true)
REF_DIRS_TO_BUILD +=
endif

