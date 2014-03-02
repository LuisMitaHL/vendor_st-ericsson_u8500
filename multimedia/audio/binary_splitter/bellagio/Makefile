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

SOLIBNAMES+=ste_binary_splitter
LIBOBJ_ste_binary_splitter = ste_omx_ens_component

AUDIO_LIBS = st_BinarySplitter

#ifeq ($(MMENV_SYSTEM), NOTTOBEUSED)
AUDIO_NMF_HOST_COMPONENTS = binary_splitter.nmfil.host.composite.pp_splitter
SOLIBS_ste_binary_splitter += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)
#endif

EXTRALIBS_ste_binary_splitter= $(AUDIO_LIBS)  
FLAGS_ste_binary_splitter = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_binary_splitter

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

