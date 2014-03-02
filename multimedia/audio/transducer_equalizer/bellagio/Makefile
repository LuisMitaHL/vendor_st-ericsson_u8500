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

SOLIBNAMES+=ste_teq
LIBOBJ_ste_teq = ste_omx_ens_component

AUDIO_LIBS = st_TransducerEqualizer transducer_equalizer_CA9 transducer_equalizer_control

#ifeq ($(MMENV_SYSTEM), NOTTOBEUSED)
AUDIO_NMF_HOST_COMPONENTS = transducer_equalizer.nmfil.host.composite.effect
SOLIBS_ste_teq += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)
#endif

EXTRALIBS_ste_teq= $(AUDIO_LIBS)  
FLAGS_ste_teq = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common 
INSTALL_SOLIB = ste_teq

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

