#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

MKFLAGS_shared_enswrapper := EXTRA_CPPFLAGS='-DEXECUTE_COMMANDS_SEQUENTIALLY=1'

ifneq ($(findstring android,$(PLATFORM)),)
  MKFLAGS_video_components_jpegenc := EXTRA_CPPFLAGS='-DPACKET_VIDEO_SUPPORT=1'
  MKFLAGS_video_components_val_source_sink := EXTRA_CPPFLAGS='-DPACKET_VIDEO_SUPPORT=1'
  MKFLAGS_video_vfm := EXTRA_CPPFLAGS='-DPACKET_VIDEO_SUPPORT=1'
endif

ifeq ($(CAMERA_SET_PRIMARY_SENSOR),IMX072)
  IMG_CONFIG=201
else
  ifeq ($(CAMERA_SET_SECONDARY_SENSOR),IMX072)
    IMG_CONFIG=201
  else
    IMG_CONFIG=100
  endif
endif

ifeq ($(IMG_CONFIG),)
  $(error IMG_CONFIG not defined)
else
  export IMG_CONFIG
endif

MKFLAGS_audio_cscall := EXTRA_CPPFLAGS='-DMAI_PROTOCOL=1 -DCSCALL_ENABLE_FEATURE_CAIF=1'

ifneq ($(CAMERA_SET_PRIMARY_SENSOR),)
  export CAMERA_SET_PRIMARY_SENSOR
endif

ifneq ($(CAMERA_SET_SECONDARY_SENSOR),)
  export CAMERA_SET_SECONDARY_SENSOR
endif
