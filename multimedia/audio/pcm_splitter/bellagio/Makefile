################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################

include $(MM_MAKEFILES_DIR)/SharedConfig.mk
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I../common

SOLIBNAMES+=ste_pcm_splitter
LIBOBJ_ste_pcm_splitter = ste_omx_ens_component

AUDIO_LIBS = st_PcmSplitter st_VolCtrl st_VolCtrl_host st_SampleRateConv volctrl resampling vector updownmix

AUDIO_NMF_HOST_COMPONENTS = volctrl.nmfil.host.effectWrapped samplerateconv.nmfil.host.effectWrapped pcm_splitter.nmfil.host.wrapperWrapped
SOLIBS_ste_pcm_splitter += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_pcm_splitter= $(AUDIO_LIBS)  
FLAGS_ste_pcm_splitter = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_pcm_splitter

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

