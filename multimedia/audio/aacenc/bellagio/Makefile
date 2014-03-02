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

SOLIBNAMES+=ste_enc_aac
LIBOBJ_ste_enc_aac = ste_omx_ens_component

AUDIO_LIBS = st_AACEncoder eaacplus_stand encalgo vector

#ifeq ($(MMENV_SYSTEM), NOTTOBEUSED)
AUDIO_NMF_HOST_COMPONENTS = aacenc.nmfil.host.composite.encalgo
SOLIBS_ste_enc_aac += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)
#endif
EXTRALIBS_ste_enc_aac= $(AUDIO_LIBS) 
#LIBS_ste_enc_aac= $(AUDIO_LIBS)  
FLAGS_ste_enc_aac = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common

INSTALL_SOLIB = ste_enc_aac

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

