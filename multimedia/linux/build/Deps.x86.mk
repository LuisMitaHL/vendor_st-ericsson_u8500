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

shared/nmf/tools-deps :=
shared/ste_shai-deps :=
shared/ostgrp-deps :=
shared/osttools/osttc-deps := shared/ostgrp

################################################################################

audio/makefiles-deps := shared/makefiles
audio/mmprobe-deps := audio/makefiles
audio/audio_chipset_apis-deps := shared/ens
audio/audiolibs-deps := audio/makefiles audio/tools shared/osttools/osttc shared/nmf/tools
audio/mp3-deps := audio/audiolibs audio/makefiles
audio/aac_arm-deps := audio/audiolibs audio/makefiles
audio/amr-deps := audio/audiolibs audio/makefiles
audio/amrwb-deps := audio/audiolibs audio/makefiles
audio/updownmix-deps := audio/audiolibs audio/makefiles
audio/samplerateconv-deps :=  audio/audiolibs audio/makefiles
audio/volctrl-deps :=  audio/audiolibs audio/makefiles audio/audio_chipset_apis audio/updownmix
audio/mixer-deps :=  audio/audiolibs audio/makefiles audio/audio_chipset_apis audio/samplerateconv audio/volctrl audio/updownmix
audio/aacenc-deps :=  audio/audiolibs audio/makefiles
audio/amrwb-deps :=  audio/audiolibs audio/makefiles
audio/g711-deps :=  audio/audiolibs audio/makefiles
audio/gsmfr-deps :=  audio/audiolibs audio/makefiles
audio/gsmhr-deps :=  audio/audiolibs audio/makefiles
audio/stereowidener-deps := audio/audio_chipset_apis  audio/audiolibs audio/makefiles
audio/pcm_splitter-deps :=  audio/audiolibs audio/makefiles audio/audio_chipset_apis audio/samplerateconv audio/volctrl
audio/mdrc-deps :=  audio/audiolibs audio/makefiles
audio/speech_proc_itf-deps := audio/makefiles
audio/drc-deps :=  audio/audiolibs audio/makefiles
audio/speech_enhancement-deps :=
audio/tty-deps :=
audio/spl-deps :=  audio/audiolibs audio/makefiles
audio/comfortnoise-deps :=  audio/audiolibs audio/makefiles
audio/speech_proc_vcs-deps :=  audio/speech_enhancement audio/tty audio/drc audio/comfortnoise
audio/speech_proc-deps := audio/makefiles audio/samplerateconv audio/speech_proc_itf  shared/ens audio/speech_proc_vcs audio/speech_proc_spea
audio/binary_splitter-deps :=  audio/audiolibs audio/makefiles
audio/transducer_equalizer-deps :=  audio/audiolibs audio/makefiles
audio/equalizer-deps :=  audio/audiolibs audio/makefiles
audio/flac-deps :=  audio/audiolibs audio/makefiles
audio/audio_3D_mixer-deps :=  audio/audiolibs audio/makefiles audio/audio_chipset_apis audio/samplerateconv audio/volctrl
audio/tonegen-deps :=  audio/audiolibs audio/makefiles
audio/reverb-deps :=  audio/audiolibs audio/makefiles
audio/spectrum_analyzer-deps :=  audio/audiolibs audio/makefiles
audio/timescale-deps :=  audio/audiolibs audio/makefiles
audio/virtual_surround-deps :=  audio/audiolibs audio/makefiles
audio/channel_synthesis-deps :=  audio/audiolibs audio/makefiles
audio/g729-deps :=  audio/audiolibs audio/makefiles
audio/aac_arm-deps :=  audio/audiolibs audio/makefiles
audio/chorus-deps :=  audio/audiolibs audio/makefiles
audio/audio_visualization-deps :=  audio/audiolibs audio/makefiles
audio/noise_reduction-deps :=  audio/audiolibs audio/makefiles audio/speech_enhancement
audio/alsasink-deps := audio/makefiles audio/audiolibs audio/alsactrl
audio/wmapro_v10-deps :=  audio/audiolibs audio/makefiles
audio/alsasource-deps := audio/makefiles audio/audiolibs 
audio/cscall-deps :=  audio/amr audio/gsmfr audio/gsmhr audio/amrwb audio/samplerateconv shared/ste_shai audio/audio_chipset_apis audio/mmprobe
audio/audio_valid-deps :=

video/g1dec-deps := shared/host_trace shared/utils
