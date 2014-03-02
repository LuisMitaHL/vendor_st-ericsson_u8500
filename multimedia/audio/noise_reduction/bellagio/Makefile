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

SOLIBNAMES+=ste_noise_reduction
LIBOBJ_ste_noise_reduction = ste_omx_ens_component

AUDIO_LIBS = st_NoiseReduction noise_reduction speech_enhancement spl

AUDIO_NMF_HOST_COMPONENTS = noise_reduction.nmfil.host.effectWrapped
SOLIBS_ste_noise_reduction += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_noise_reduction= $(AUDIO_LIBS)
FLAGS_ste_noise_reduction = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_noise_reduction

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk
