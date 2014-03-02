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

SOLIBNAMES+=ste_virtual_surround
LIBOBJ_ste_virtual_surround = ste_omx_ens_component

ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
AUDIO_LIBS += stm-arm-neon
endif

AUDIO_LIBS += st_VirtualSurround vs

#ifeq ($(MMENV_SYSTEM), NOTTOBEUSED)
AUDIO_NMF_HOST_COMPONENTS = virtual_surround.nmfil.host.composite.effect
SOLIBS_ste_virtual_surround += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)
#endif

EXTRALIBS_ste_virtual_surround= $(AUDIO_LIBS)  
FLAGS_ste_virtual_surround = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_virtual_surround

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

