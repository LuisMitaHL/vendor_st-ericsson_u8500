#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

# This file is defining inter components dependencies
HAS_DEFINED_DEPS := true

################################################################################

shared/makefiles-$(SETUP_TARGET): shared/mmenv-$(SETUP_TARGET)
audio/makefiles-$(SETUP_TARGET): shared/makefiles-$(SETUP_TARGET)
video/makefiles-$(SETUP_TARGET): shared/makefiles-$(SETUP_TARGET)

################################################################################

shared/nmf/mpcee-deps := shared/nmf/tools
shared/ste_shai-deps :=
shared/ostgrp-deps :=
shared/osttools/osttc-deps := shared/ostgrp
linux/trace-deps :=
shared/nmf/linux-deps := shared/nmf/tools linux/trace
shared/nmf/tests-deps := shared/nmf/tools shared/nmf/linux
shared/mmhwbuffer_api-deps :=
shared/omxil-deps :=
shared/host_trace-deps := shared/utils
shared/utils-deps :=
shared/omxilosalservices_api-deps :=
shared/omxilosalservices_los-deps := shared/omxilosalservices_api shared/omxil shared/utils shared/nmf/linux
shared/osttrace-deps := shared/ostgrp shared/nmf/linux linux/trace shared/utils
shared/rme-deps := shared/omxil shared/omxilosalservices_api shared/host_trace shared/omxilosalservices_los
shared/ens_interface-deps := shared/nmf/tools shared/nmf/linux shared/nmf/mpcee shared/rme shared/host_trace shared/osttrace shared/osttools/osttc
shared/ens-deps := shared/mmhwbuffer_api shared/nmf/tools shared/nmf/linux shared/nmf/mpcee shared/rme shared/host_trace shared/osttrace shared/osttools/osttc shared/ste_shai shared/ens_interface
shared/mmhwbuffer_osi-deps := shared/mmhwbuffer_api shared/omxil shared/omxilosalservices_api shared/utils shared/host_trace
shared/enswrapper-deps := shared/ens shared/rme shared/osttrace shared/ostgrp shared/utils shared/mmhwbuffer_osi audio/audio_chipset_apis video/video_chipset_apis
linux/bellagio_omxcore-deps := linux/bellagio_ste_omxloader
linux/bellagio_ste_omxloader-deps := shared/omxil shared/nmf/tools shared/utils shared/nmf/linux linux/trace shared/mmhwbuffer_osi shared/host_trace shared/omxilosalservices_los shared/ens shared/enswrapper shared/osttrace shared/hash
shared/cli-deps := shared/host_trace shared/nmf/linux
shared/hash-deps :=
shared/mmte_api-deps :=
shared/mmte_nmf-deps := shared/cli shared/utils shared/nmf/linux
shared/mmte-deps := linux/bellagio_ste_omxloader shared/nmf/tools shared/host_trace shared/cli shared/hash shared/mmte_api shared/omxil shared/makefiles shared/mmte_nmf shared/nmf/linux shared/mmhwbuffer_osi linux/hw_buffer shared/enswrapper audio/audio_chipset_apis audio/audiolibs audio/afm audio/aac_arm audio/ate_ext audio/mp3 audio/amr audio/amrwb audio/gsmfr audio/gsmhr audio/mixer audio/samplerateconv audio/cscall audio/speech_proc audio/volctrl audio/transducer_equalizer audio/spl imaging/ifm imaging/hsmcamera imaging/ispproc shared/fakesink imaging/fakesource imaging/ite_omx_use_cases video/video_valid_common video/video_valid_mpeg4dec video/video_valid_mpeg4enc video/video_valid_jpegdec video/video_valid_jpegenc video/video_valid_multicomp video/video_valid_vp6dec video/components/jpegenc video/components/jpegdec video/components/mpeg4dec video/components/mpeg4enc video/components/h264dec video/components/h264enc video/components/vc1dec video/video_valid_exifmixer video/components/mpeg2dec video/components/val_source_sink imaging/wrapper_openmax imaging/norcos imaging/redeye_detector imaging/redeye_corrector imaging/aiq_common imaging/armivproc imaging/face_detector audio/pcm_splitter audio/mdrc audio/wmapro_v10 audio/libeffects

################################################################################

audio/makefiles-deps := shared/makefiles
audio/audio_chipset_apis-deps := shared/ens
audio/audiolibs-deps := audio/makefiles audio/tools shared/nmf/tools shared/nmf/linux shared/nmf/mpcee shared/host_trace shared/osttrace shared/osttools/osttc
audio/afm-deps := audio/makefiles shared/ens shared/osttrace audio/audiolibs audio/libeffects linux/bellagio_ste_omxloader audio/audio_chipset_apis shared/ste_shai audio/tools
audio/mp3-deps := audio/afm audio/audiolibs audio/makefiles
audio/aac_arm-deps := audio/afm audio/audiolibs audio/makefiles
audio/amr-deps := audio/afm audio/audiolibs audio/makefiles
audio/updownmix-deps := audio/audiolibs audio/makefiles
audio/samplerateconv-deps := audio/afm audio/audiolibs audio/makefiles audio/libeffects
audio/volctrl-deps := audio/afm audio/audiolibs audio/makefiles audio/audio_chipset_apis audio/updownmix audio/libeffects
audio/libeffects-deps := audio/makefiles shared/ens shared/osttrace audio/audio_chipset_apis audio/audiolibs
audio/mixer-deps := audio/afm audio/makefiles audio/audio_chipset_apis audio/volctrl audio/updownmix audio/libeffects audio/samplerateconv audio/spl audio/transducer_equalizer audio/mdrc
audio/amrwb-deps := audio/afm audio/audiolibs audio/makefiles
audio/gsmfr-deps := audio/afm audio/audiolibs audio/makefiles
audio/gsmhr-deps := audio/afm audio/audiolibs audio/makefiles
audio/pcm_splitter-deps := audio/afm audio/audiolibs audio/makefiles audio/audio_chipset_apis audio/samplerateconv audio/volctrl audio/libeffects
audio/mdrc-deps := audio/afm audio/audiolibs audio/libeffects audio/makefiles
audio/speech_proc_itf-deps := audio/makefiles
audio/drc-deps := audio/afm audio/audiolibs audio/makefiles
audio/speech_enhancement-deps :=
audio/spl-deps := audio/afm audio/libeffects audio/makefiles
audio/comfortnoise-deps := audio/afm audio/audiolibs audio/makefiles
audio/speech_proc_vcs-deps := audio/afm audio/speech_enhancement audio/drc audio/comfortnoise  audio/spl
audio/speech_proc-deps := audio/makefiles audio/samplerateconv audio/libeffects audio/speech_proc_itf audio/afm shared/ens audio/speech_proc_vcs
audio/audio_hwctrl-deps := audio/audio_chipset_apis
audio/transducer_equalizer-deps := audio/afm audio/libeffects audio/audiolibs audio/makefiles
audio/tonegen-deps := audio/afm audio/audiolibs audio/makefiles
audio/virtual_surround-deps := audio/afm audio/audiolibs audio/makefiles
audio/aac_arm-deps := audio/afm audio/audiolibs audio/makefiles
audio/noise_reduction-deps := audio/afm audio/audiolibs audio/makefiles audio/speech_enhancement
audio/audiocodec-deps := audio/makefiles audio/volctrl audio/afm audio/audio_chipset_apis linux/bellagio_ste_omxloader audio/audio_hwctrl
audio/alsasink-deps := audio/makefiles audio/audiolibs audio/afm audio/audio_hwctrl
audio/wmapro_v10-deps := audio/afm audio/audiolibs audio/makefiles
audio/alsasource-deps := audio/makefiles audio/audiolibs audio/afm
audio/cscall-deps := audio/afm audio/amr audio/gsmfr audio/gsmhr audio/amrwb audio/samplerateconv audio/libeffects shared/ste_shai audio/audio_chipset_apis
audio/cscall_mpc-deps := audio/afm audio/audio_chipset_apis audio/audio_hwctrl audio/makefiles
audio/audio_valid-deps :=
audio/ate_ext-deps := shared/mmte_api shared/cli shared/ste_shai shared/omxil shared/nmf/linux audio/afm audio/audio_hwctrl

################################################################################

video/video_chipset_apis-deps := video/makefiles shared/ens shared/ste_shai
video/videotools-deps := video/makefiles
video/components/resource_manager-deps := video/makefiles video/components/common video/components/hardware video/components/registers_def
video/components/vpp-deps := video/makefiles video/components/resource_manager video/components/common video/components/trace video/components/hardware
video/components/common-deps := video/makefiles video/components/registers_def
video/components/hardware-deps := video/makefiles video/videotools video/components/registers_def video/components/common
video/vfm-deps := shared/ens shared/mmhwbuffer_api video/components/common shared/omxilosalservices_api video/video_chipset_apis shared/nmf/linux shared/nmf/tools video/makefiles
video/components/jpegdec-deps := video/vfm video/components/resource_manager video/components/common shared/host_trace shared/ens video/components/hardware video/components/scheduler video/makefiles
video/components/jpegenc-deps := video/vfm video/components/resource_manager video/components/common video/components/scheduler shared/host_trace shared/ens video/components/vpp video/components/hardware video/components/scheduler shared/enswrapper video/makefiles
video/components/mpeg4dec-deps := video/vfm video/components/resource_manager video/components/common video/components/scheduler shared/host_trace shared/ens video/components/vpp video/components/hardware video/makefiles
video/components/mpeg2dec-deps := video/vfm video/components/resource_manager video/components/common shared/host_trace shared/ens video/components/vpp video/components/debug video/components/hardware shared/enswrapper video/makefiles
video/components/vc1dec-deps := shared/ens video/vfm video/makefiles video/videotools video/components/common video/components/debug video/components/hardware video/components/registers_def video/components/resource_manager video/components/scheduler shared/mmhwbuffer_osi shared/omxilosalservices_los shared/osttools/osttc
video/components/mpeg4enc-deps := video/vfm video/components/resource_manager video/components/common shared/host_trace shared/ens video/components/hardware video/makefiles
video/components/debug-deps := video/makefiles video/videotools video/components/common
video/components/h264dec-deps := shared/ens video/vfm video/makefiles video/videotools video/components/common video/components/debug video/components/hardware video/components/registers_def video/components/resource_manager video/components/scheduler shared/mmhwbuffer_osi shared/omxilosalservices_los shared/osttools/osttc
video/components/h264enc-deps := shared/ens video/vfm video/makefiles video/videotools video/components/common video/components/debug video/components/hardware video/components/registers_def video/components/resource_manager video/components/scheduler shared/mmhwbuffer_api shared/mmhwbuffer_osi shared/omxilosalservices_los
video/components/hva-deps := shared/nmf/linux shared/nmf/tools video/makefiles video/components/common shared/mmhwbuffer_api shared/mmhwbuffer_osi shared/host_trace linux/bellagio_omxcore
video/components/vp6dec-deps := shared/ens video/vfm video/makefiles video/videotools video/components/common video/components/debug video/components/hardware video/components/registers_def video/components/resource_manager video/components/scheduler shared/mmhwbuffer_api shared/mmhwbuffer_osi shared/omxilosalservices_los
video/components/exif_mixer-deps := video/vfm video/components/common shared/host_trace shared/ens shared/enswrapper video/video_chipset_apis
linux/ste_omxloader_components-deps := linux/bellagio_ste_omxloader video/vfm video/components/exif_mixer video/components/h264dec video/components/h264enc video/components/jpegdec video/components/jpegenc video/components/mpeg2dec video/components/mpeg4dec video/components/mpeg4enc video/components/rotateresize video/components/val_source_sink video/components/vc1dec video/components/vp6dec
video/components/val_source_sink-deps := video/vfm shared/mmhwbuffer_api shared/mmhwbuffer_osi shared/omxilosalservices_api shared/cli
video/video_valid_common-deps := shared/ens shared/cli shared/hash shared/mmte_nmf shared/mmte_api shared/nmf/linux shared/omxil shared/hash shared/cli video/makefiles
video/video_valid_jpegdec-deps := video/video_valid_common video/components/jpegdec
video/video_valid_jpegenc-deps := video/video_valid_common video/components/jpegenc
video/video_valid_mpeg4dec-deps := video/video_valid_common video/components/mpeg4dec
video/video_valid_mpeg4enc-deps := video/video_valid_common video/components/mpeg4enc
video/video_valid_exifmixer-deps := video/video_valid_common video/components/exif_mixer
video/video_valid_multicomp-deps := video/video_valid_jpegdec video/video_valid_jpegenc video/video_valid_exifmixer
video/video_valid_vp6dec-deps := video/video_valid_common video/components/vp6dec video/video_valid_mpeg4dec
video/rotateresize-deps := video/video_valid_common
video/g1dec-deps := shared/host_trace shared/utils linux/bellagio_omxcore

################################################################################
IMAGING_ISP_FIRMWARES := $(patsubst $(MMROOT)/%,%, $(foreach dir, $(MMIMAGING)/isp_firmware,$(wildcard $(dir)/isp8500*)))
IMAGING_ISP_TUNINGS := $(patsubst $(MMROOT)/%,%, $(foreach dir, $(MMIMAGING)/isp_tuning,$(wildcard $(dir)/tuning_*)))

imaging/sia_hw_components_8500/nmf/mpc-deps :=
imaging/mmio_api-deps :=
linux/i2c_lib-deps :=
linux/mmio-deps := imaging/mmio_api shared/omxil linux/i2c_lib shared/utils shared/host_trace imaging/imaging_platform_configuration
imaging/stab-deps := shared/nmf/linux
imaging/sia_hw_components_8500-deps := shared/ens shared/nmf/linux shared/nmf/tools imaging/stab shared/ostgrp shared/osttrace shared/ste_shai
imaging/imaging_platform_configuration-deps :=
imaging/ifm-deps := shared/ens shared/mmhwbuffer_api shared/mmhwbuffer_osi imaging/sia_hw_components_8500 shared/ste_shai imaging/imaging_platform_configuration

imaging/tuning-deps := shared/host_trace shared/nmf/linux
imaging/nmx-deps :=
imaging/tuningdatabase-deps := shared/omxilosalservices_api imaging/tuning shared/host_trace shared/omxilosalservices_los linux/bellagio_ste_omxloader
imaging/damper-deps := shared/host_trace linux/bellagio_ste_omxloader
imaging/flash_api-deps :=
imaging/ste3a-deps := imaging/nmx
imaging/sw3A-deps := shared/host_trace imaging/tuning imaging/tuningdatabase shared/mmhwbuffer_api imaging/mmio_api shared/omxil shared/nmf/linux imaging/ifm $(IMAGING_ISP_FIRMWARES) imaging/lightsensor_api imaging/ste3a imaging/damper imaging/flash_api
imaging/tuningloader_api-deps :=
imaging/tuningloader-deps := shared/host_trace imaging/tuningloader_api imaging/tuningdatabase imaging/nmx imaging/tuning imaging/damper shared/nmf/linux imaging/sw3A
imaging/imgcommon-deps := imaging/ifm shared/enswrapper imaging/sia_hw_components_8500 $(IMAGING_ISP_FIRMWARES) imaging/imaging_platform_configuration imaging/tuningdatabase imaging/tuningloader imaging/tuning imaging/damper linux/mmio linux/i2c_lib linux/bellagio_ste_omxloader
imaging/asyncflash-deps := shared/host_trace shared/nmf/linux shared/nmf/tools imaging/flash_api
linux/camera_flash-deps := imaging/flash_api
imaging/hsmcamera-deps := shared/host_trace imaging/ifm imaging/mmio_api imaging/sia_hw_components_8500 shared/ens linux/mmio imaging/imgcommon imaging/tuning imaging/tuningdatabase imaging/tuningloader shared/ste_shai imaging/sw3A imaging/flash_api linux/camera_flash imaging/asyncflash imaging/damper imaging/tuning_nokia imaging/imaging_platform_configuration linux/bellagio_ste_omxloader linux/camera_lightsensor linux/camera_flashlibcc
imaging/ispproc-deps := imaging/imgcommon imaging/tuning imaging/tuningdatabase linux/bellagio_ste_omxloader
imaging/fileio-deps := shared/host_trace shared/nmf/linux shared/nmf/tools
imaging/aiq_common-deps :=
imaging/wrapper_openmax-deps := shared/ens shared/ste_shai imaging/ifm
imaging/armivproc-deps := imaging/ifm shared/ens linux/bellagio_ste_omxloader imaging/wrapper_openmax
imaging/norcos-deps := imaging/ifm shared/ens imaging/aiq_common imaging/wrapper_openmax linux/bellagio_ste_omxloader imaging/damper
imaging/redeye_detector-deps := imaging/ifm shared/ens linux/bellagio_ste_omxloader imaging/wrapper_openmax imaging/aiq_common
imaging/redeye_corrector-deps := imaging/ifm shared/ens linux/bellagio_ste_omxloader imaging/wrapper_openmax imaging/redeye_detector
imaging/splitter-deps := shared/ens imaging/wrapper_openmax imaging/ifm linux/bellagio_ste_omxloader
imaging/face_detector-deps := imaging/ifm shared/ens imaging/wrapper_openmax imaging/aiq_common linux/bellagio_ste_omxloader
imaging/ext_sia_hw_components_8500-deps := shared/ens shared/nmf/linux shared/nmf/tools imaging/stab shared/ostgrp shared/osttrace shared/ste_shai
imaging/ext_imgcommon-deps := imaging/ifm shared/enswrapper imaging/ext_sia_hw_components_8500 linux/mmio linux/i2c_lib linux/bellagio_ste_omxloader
imaging/ext_hsmcamera-deps := shared/host_trace imaging/ifm imaging/mmio_api imaging/ext_sia_hw_components_8500 shared/ens linux/mmio imaging/ext_imgcommon shared/ste_shai imaging/flash_api linux/camera_flash imaging/asyncflash linux/bellagio_ste_omxloader imaging/fileio
imaging/ite_omx_use_cases-deps := shared/cli shared/mmte_api imaging/ifm imaging/ite shared/ste_shai imaging/imgcommon linux/bellagio_ste_omxloader
imaging/bridge-deps := shared/ens imaging/ifm linux/bellagio_ste_omxloader imaging/wrapper_openmax
imaging/ite-deps := shared/omxil imaging/sia_hw_components_8500 $(IMAGING_ISP_FIRMWARES)
shared/fakesink-deps := shared/ens imaging/ifm imaging/imgcommon linux/bellagio_ste_omxloader imaging/ite
imaging/fakesource-deps := shared/ens imaging/ifm imaging/imgcommon linux/bellagio_ste_omxloader
imaging/ite_nmf-deps := shared/nmf/linux shared/nmf/tools linux/mmio linux/i2c_lib shared/mmhwbuffer_api shared/ens shared/omxilosalservices_api $(IMAGING_ISP_FIRMWARES) imaging/ifm linux/bellagio_ste_omxloader shared/cli imaging/ite imaging/sia_hw_components_8500 shared/utils

define imaging_firmwares
   $(1)-deps := imaging/sia_hw_components_8500 imaging/ifm
endef
$(foreach fw, $(patsubst $(MMROOT)/%,%,$(foreach dir,$(MMIMAGING)/isp_firmware,$(wildcard $(dir)/isp8500*))), \
     $(eval $(call imaging_firmwares,$(fw))))

define imaging_tunings
   $(1)-deps := imaging/ifm
endef
$(foreach tune, $(patsubst $(MMROOT)/%,%,$(foreach dir,$(MMIMAGING)/isp_tuning,$(wildcard $(dir)/tuning_*))), \
	$(eval $(call imaging_tunings,$(tune))))

################################################################################

linux/b2r2lib-deps :=
linux/bellagio_fbdev-deps := shared/ste_shai linux/bellagio_omxcore linux/bellagio_ste_omxloader linux/b2r2lib
