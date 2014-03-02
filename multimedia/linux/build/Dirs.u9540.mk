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

ifeq ($(STE_SOFTWARE),android)
  BUILD_MALI=$(BUILD_ALL)
else
  ## Does not build in lbp
  BUILD_MALI=false
endif
BUILD_KHRONOS_CTS=false

BUILD_VALID=$(BUILD_ALL)
RAMDISK_COPY_MMVALID_SCRIPTS_TO_ANDROID=false
INSTALL_AUDIO_SCRIPTS=false

BUILD_ITE_OMX_USE_CASES=false
ifeq ($(ENABLE_FEATURE_BUILD_HATS),true)
  RAMDISK_COPY_MMVALID_SCRIPTS_TO_ANDROID=true
  BUILD_ITE_OMX_USE_CASES=true
  INSTALL_AUDIO_SCRIPTS=true
endif

export RAMDISK_COPY_MMVALID_SCRIPTS_TO_ANDROID
export INSTALL_AUDIO_SCRIPTS

################################################################################

DIRECTORIES_TO_SETUP := shared/mmenv shared/makefiles audio/makefiles video/makefiles

################################################################################
# shared

ifeq ($(BUILD_SHARED),true)

REF_DIRS_TO_BUILD += \
shared/nmf/tools \
shared/nmf/mpcee \
shared/nmf/tests \
shared/ste_shai \
shared/ostgrp \
shared/osttools/osttc \
linux/trace \
shared/nmf/linux \
shared/mmhwbuffer_api \
shared/omxil \
shared/host_trace \
shared/utils \
shared/omxilosalservices_api \
shared/omxilosalservices_los \
shared/osttrace \
shared/rme \
shared/ens_interface \
shared/ens \
shared/mmhwbuffer_osi \
shared/enswrapper \
linux/bellagio_omxcore \
linux/bellagio_ste_omxloader

## Build mmte 1 time for dep video is having on it
ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
shared/cli \
shared/hash \
shared/mmte_nmf \
shared/mmte_api 
endif

endif #ifeq ($(BUILD_SHARED),true)

################################################################################
# audio

ifeq ($(BUILD_AUDIO),true)

REF_DIRS_TO_BUILD += \
audio/tools \
audio/audio_chipset_apis \
audio/audiolibs \
audio/afm \
audio/mp3 \
audio/amr \
audio/updownmix \
audio/samplerateconv \
audio/volctrl \
audio/mixer \
audio/amrwb \
audio/gsmfr \
audio/gsmhr \
audio/pcm_splitter \
audio/mdrc \
audio/speech_proc_itf \
audio/drc \
audio/speech_enhancement \
audio/spl \
audio/comfortnoise \
audio/speech_proc_vcs \
audio/speech_proc \
audio/audio_hwctrl \
audio/transducer_equalizer \
audio/virtual_surround \
audio/aac_arm \
audio/noise_reduction \
audio/audiocodec \
audio/alsasink \
audio/dtsdec \
audio/wmapro_v10 \
audio/alsasource \
audio/cscall \
audio/cscall_mpc \
audio/libeffects

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
video/components/common \
video/components/registers_def \
video/components/resource_manager \
video/components/vpp \
video/components/hardware \
video/components/scheduler \
video/vfm \
video/components/jpegdec \
video/components/jpegenc \
video/components/mpeg4dec \
video/components/mpeg2dec \
video/components/vc1dec	\
video/components/mpeg4enc \
video/components/debug \
video/components/h264dec \
video/components/h264enc \
video/components/vp6dec \
video/components/exif_mixer \
video/g1dec \
video/components/hva

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

REF_DIRS_TO_BUILD += \
linux/ste_omxloader_components

endif #ifeq ($(BUILD_VIDEO),true)

################################################################################
# imaging

ifeq ($(BUILD_IMAGING),true)
REF_DIRS_TO_BUILD += \
imaging/mmio_api \
linux/i2c_lib \
linux/mmio \
imaging/stab \
imaging/sia_hw_components_8500 \
imaging/imaging_platform_configuration \
imaging/ifm \
imaging/flash_api \
linux/camera_flash

ifeq ($(CAMERA_ENABLE_FEATURE_RAW_SENSOR),true)
REF_DIRS_TO_BUILD += \
imaging/isp8500_firmware_api \
imaging/omx3a_camera_extension \
$(patsubst $(MMROOT)/%,%, $(foreach dir, $(MMIMAGING)/isp_firmware,$(wildcard $(dir)/isp8500*)))  \
$(patsubst $(MMROOT)/%,%, $(foreach dir, $(MMIMAGING)/isp_tuning,$(wildcard $(dir)/tuning_*))) \
imaging/asyncflash \
linux/camera_flashlibcc \
linux/sra \
linux/uirq \
imaging/fileio \
imaging/aiq_common \
imaging/aiq_tools \
imaging/wrapper_openmax \
imaging/armivproc \
imaging/redeye_detector \
imaging/redeye_corrector \
imaging/splitter \
imaging/face_detector \
imaging/lightsensor_api \
linux/camera_lightsensor \
imaging/tuning \
imaging/nmx \
imaging/tuningdatabase \
imaging/damper \
imaging/tuningloader_api \
imaging/ambr \
imaging/ste3a \
imaging/sw3A \
imaging/norcos \
imaging/tuningloader \
imaging/imgcommon \
imaging/ispproc \
imaging/hsmcamera
endif

ifeq ($(CAMERA_ENABLE_FEATURE_YUV_CAMERA),true)
REF_DIRS_TO_BUILD += \
imaging/ext_sia_hw_components_8500 \
imaging/ext_imgcommon \
imaging/ext_hsmcamera
endif

ifeq ($(CAMERA_ENABLE_FEATURE_RAW_SENSOR),true)
ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
imaging/fakesource \
shared/fakesink
REF_DIRS_TO_BUILD += \
imaging/bridge \
imaging/ite \
imaging/ite_nmf
ifeq ($(BUILD_ITE_OMX_USE_CASES),true)
REF_DIRS_TO_BUILD += \
imaging/ite_omx_use_cases
endif #ifeq ($(BUILD_ITE_OMX_USE_CASES),true)
endif #ifeq ($(BUILD_VALID),true)
endif #ifeq ($(CAMERA_ENABLE_FEATURE_RAW_SENSOR),true)
endif #ifeq ($(BUILD_IMAGING),true)

ifeq ($(STE_SOFTWARE),lbp)
REF_DIRS_TO_BUILD += \
linux/b2r2lib
endif

ifeq ($(BUILD_SHARED),true)
ifeq ($(BUILD_VALID),true)
REF_DIRS_TO_BUILD += \
linux/bellagio_fbdev
endif
endif #ifeq ($(BUILD_SHARED),true)

ifeq ($(BUILD_MALI),true)
REF_DIRS_TO_BUILD += linux/mali400
  ifeq ($(BUILD_KHRONOS_CTS),true)
    REF_DIRS_TO_BUILD += linux/khronos_cts
  endif
endif

ifeq ($(BUILD_VALID),true)
ifeq ($(BUILD_SHARED),true)
REF_DIRS_TO_BUILD += \
shared/mmte
endif #ifeq ($(BUILD_SHARED),true)
ifeq ($(BUILD_AUDIO),true)
REF_DIRS_TO_BUILD += \
audio/audio_valid 
endif
endif

