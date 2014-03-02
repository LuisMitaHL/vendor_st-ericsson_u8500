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

SOLIBNAMES+=ste_tonegen
LIBOBJ_ste_tonegen = ste_omx_ens_component

AUDIO_LIBS = st_ToneGenerator audiotonegen

AUDIO_NMF_HOST_COMPONENTS = tonegen.nmfil.host.composite.wrapper tonegen.nmfil.host.composite.tonegen
SOLIBS_ste_tonegen += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_tonegen= $(AUDIO_LIBS)
FLAGS_ste_tonegen = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_tonegen

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk
