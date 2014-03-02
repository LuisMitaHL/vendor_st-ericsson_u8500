################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifeq ($(CORE_NUMBER),2)
#For MMDSP

# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I../common

SOLIBNAMES+=ste_comfort_noise
LIBOBJ_ste_comfort_noise = ste_omx_ens_component

AUDIO_LIBS = st_ComfortNoise

AUDIO_NMF_HOST_COMPONENTS = comfortnoise.effect.libcng comfortnoise.nmfil.effect
SOLIBS_ste_comfort_noise += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_comfort_noise = $(AUDIO_LIBS)  
FLAGS_ste_comfort_noise = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common 
INSTALL_SOLIB = ste_comfort_noise

endif

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

