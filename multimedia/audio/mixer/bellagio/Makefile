################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################

include $(MM_MAKEFILES_DIR)/AudioConfig.mk
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I$(COMPONENT_TOP_DIR)/common -I$(COMPONENT_TOP_DIR)/proxy

SOLIBNAMES+=ste_audio_mixer
LIBOBJ_ste_audio_mixer = ste_omx_ens_component

AUDIO_LIBS = st_VolCtrl st_VolCtrl_host st_SampleRateConv st_Mixer volctrl resampling vector mixer updownmix
AUDIO_LIBS += st_SPL_AudioEffect st_TEQ_AudioEffect transducer_equalizer_control st_MDRC_AudioEffect mdrc5b_filters_design

##dependent libs for mixer are st_SampleRateConv st_VolCtrl
AUDIO_NMF_HOST_COMPONENTS =  volctrl.nmfil.host.effectWrapped samplerateconv.nmfil.host.effectWrapped mixer.nmfil.host.wrapperWrapped 
SOLIBS_ste_audio_mixer += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_audio_mixer= $(AUDIO_LIBS) 
FLAGS_ste_audio_mixer = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_audio_mixer

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

