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

SOLIBNAMES+=ste_mdrc
LIBOBJ_ste_mdrc = ste_omx_ens_component

AUDIO_LIBS = st_Mdrc mdrc5b mdrc5b_CA9 mdrc5b_filters_design

#ifeq ($(MMENV_SYSTEM), NOTTOBEUSED)
AUDIO_NMF_HOST_COMPONENTS = mdrc.nmfil.host.effectWrapped
SOLIBS_ste_mdrc += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)
#endif

EXTRALIBS_ste_mdrc= $(AUDIO_LIBS)  
FLAGS_ste_mdrc = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common 
INSTALL_SOLIB = ste_mdrc

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

