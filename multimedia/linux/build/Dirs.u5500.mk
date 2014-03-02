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
BUILD_IMAGING=$(BUILD_ALL)

BUILD_VALID=false

ifeq ($(STE_SOFTWARE),android)
  BUILD_MALI=$(BUILD_ALL)
else
  ## Does not build in lbp
  BUILD_MALI=false
endif
BUILD_KHRONOS_CTS=false

################################################################################

DIRECTORIES_TO_SETUP := shared/mmenv shared/makefiles audio/makefiles video/makefiles

## Move below 3 lines to Deps.u5500.mk upon creation
shared/makefiles-$(SETUP_TARGET): shared/mmenv-$(SETUP_TARGET)
audio/makefiles-$(SETUP_TARGET): shared/makefiles-$(SETUP_TARGET)
video/makefiles-$(SETUP_TARGET): shared/makefiles-$(SETUP_TARGET)

################################################################################
# shared

ifeq ($(BUILD_SHARED),true)

REF_DIRS_TO_BUILD += \
shared/ste_shai \
shared/ostgrp \
shared/osttools/osttc \
linux/trace \
shared/nmf/linux \
shared/mmhwbuffer_api \
shared/omxil \
shared/utils \
shared/host_trace \
shared/cli \
shared/mmtrace \
shared/omxilosalservices_api \
shared/omxilosalservices_los \
shared/osttrace \
shared/rme \
shared/ens_interface \
shared/ens \
shared/hash \
shared/mmhwbuffer_osi \
shared/enswrapper \
linux/bellagio_omxcore \
linux/bellagio_ste_omxloader
endif #ifeq ($(BUILD_SHARED),true)

## Build mmte 1 time for dep video is having on it
ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
shared/mmte_api 
endif

################################################################################
# audio
ifeq ($(BUILD_AUDIO),true)
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
REF_DIRS_TO_BUILD += \
audio/mmprobe
endif ##ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
endif ##ifeq ($(BUILD_AUDIO),true)

ifeq ($(BUILD_AUDIO),true)
REF_DIRS_TO_BUILD += \
audio/audio_chipset_apis \
audio/audiolibs \
audio/afm \
audio/mp3 \
audio/updownmix \
audio/volctrl \
audio/amr \
audio/samplerateconv \
audio/mixer \
audio/aacenc \
audio/amrwb \
audio/g711 \
audio/gsmfr \
audio/gsmhr \
audio/speech_proc_itf \
audio/drc \
audio/speech_enhancement \
audio/tty \
audio/comfortnoise \
audio/speech_proc_vcs \
audio/timescale \
audio/spectrum_analyzer \
audio/audio_visualization \
audio/virtual_surround \
audio/channel_synthesis \
audio/noise_reduction \
audio/audio_hwctrl \
audio/flac \
audio/g729 \
audio/dtsdec \
audio/aac_arm \
audio/chorus \
audio/binary_splitter \
audio/alsasink \
audio/speech_proc \
audio/cscall \
audio/pcm_splitter \
audio/alsasource \
audio/wmapro_v10

ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
audio/ate_ext
endif
endif #ifeq ($(BUILD_AUDIO),true)

################################################################################
# video

ifeq ($(BUILD_VIDEO),true)

REF_DIRS_TO_BUILD += \
video/video_chipset_apis \
video/videotools \
video/components/common  \
video/components/registers_def \
video/components/hardware \
video/components/scheduler \
video/components/hva_drv \
video/vfm \
video/components/jpegenc \
video/components/mpeg4dec \
video/components/mpeg4enc \
video/components/h264dec \
video/components/h264enc \
video/components/vp6dec \
video/components/exif_mixer

ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
video/components/val_source_sink \
video/video_valid_common \
video/video_valid_jpegdec \
video/video_valid_jpegenc \
video/video_valid_mpeg4dec \
video/video_valid_mpeg4enc \
video/video_valid_exifmixer \
video/video_valid_multicomp \
video/video_valid_vp6dec \
video/rotateresize
endif #ifeq ($(BUILD_VALID),true)
endif #ifeq ($(BUILD_VIDEO),true)

ifeq ($(BUILD_MALI),true)
REF_DIRS_TO_BUILD += linux/mali400ko
REF_DIRS_TO_BUILD += linux/mali400
  ifeq ($(BUILD_KHRONOS_CTS),true)
    REF_DIRS_TO_BUILD += linux/khronos_cts
  endif
endif

################################################################################
# imaging

ifeq ($(BUILD_IMAGING),true)
REF_DIRS_TO_BUILD += \
imaging/mmio_api \
linux/i2c_lib \
linux/mmio \
imaging/stab \
linux/linux_sralib \
imaging/imaging_platform_configuration \
imaging/ext_sia_hw_components \
imaging/ifm \
imaging/flash_api \
linux/linux_user_irqlib \
imaging/asyncflash \
imaging/fileio \
linux/camera_flash \
linux/camera_flashlibcc \
imaging/ext_imgcommon \
imaging/ext_hsmcamera \
imaging/wrapper_openmax \
imaging/armivproc

ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
imaging/ite \
imaging/ite_omx_use_cases
endif #ifeq ($(BUILD_VALID),true)
endif #ifeq ($(BUILD_IMAGING),true)

ifeq ($(SW_VARIANT),lbp)
REF_DIRS_TO_BUILD += \
linux/b2r2lib 
endif

REF_DIRS_TO_BUILD += \
linux/ste_omxloader_components

ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
shared/mmte \
audio/audio_valid 
endif #ifeq ($(BUILD_VALID),true)
