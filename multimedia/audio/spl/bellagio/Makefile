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

SOLIBNAMES+=ste_spl
LIBOBJ_ste_spl = ste_omx_ens_component

AUDIO_LIBS = st_SPL SPL

EXTRALIBS_ste_spl= $(AUDIO_LIBS)
FLAGS_ste_spl = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_spl

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk
