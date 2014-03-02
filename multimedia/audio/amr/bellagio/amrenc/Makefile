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

SOLIBNAMES+=ste_enc_amr
LIBOBJ_ste_enc_amr = ste_omx_ens_component

AUDIO_LIBS = st_AMREncoder amrcommon amrenc amrenc24

AUDIO_NMF_HOST_COMPONENTS = amr.nmfil.host.composite.encalgo

SOLIBS_ste_enc_amr += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_enc_amr= $(AUDIO_LIBS)
FLAGS_ste_enc_amr = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
INSTALL_SOLIB = ste_enc_amr 

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

