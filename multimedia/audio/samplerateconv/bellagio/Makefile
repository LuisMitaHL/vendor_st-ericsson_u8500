################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################

include $(MM_MAKEFILES_DIR)/SharedConfig.mk
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I../common

SOLIBNAMES+=ste_ens_audio_samplerateconv

AUDIO_LIBS = resampling
AUDIO_NMF_HOST_COMPONENTS = samplerateconv.nmfil.host.effectWrapped

SOLIBS_ste_ens_audio_samplerateconv += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)
EXTRALIBS_ste_ens_audio_samplerateconv = $(AUDIO_LIBS)

FLAGS__ste_ens_audio_samplerateconv = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_ens_audio_samplerateconv

todo: lib solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

