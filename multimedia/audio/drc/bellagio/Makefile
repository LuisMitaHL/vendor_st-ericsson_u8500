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

SOLIBNAMES+=ste_drc
LIBOBJ_ste_drc = ste_omx_ens_component

AUDIO_LIBS = st_Drc drc

#ifeq ($(MMENV_SYSTEM), NOTTOBEUSED)
AUDIO_NMF_HOST_COMPONENTS = drc.nmfil.host.composite.effect
SOLIBS_ste_drc += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)
#endif

EXTRALIBS_ste_drc= $(AUDIO_LIBS)  
FLAGS_ste_drc = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_drc

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

