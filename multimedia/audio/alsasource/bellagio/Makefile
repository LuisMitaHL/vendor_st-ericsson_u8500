################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################


include $(MM_MAKEFILES_DIR)/SharedConfig.mk
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES+=ste_alsasource
LIBOBJ_ste_alsasource = ste_omx_ens_component

AUDIO_LIBS = 
AUDIO_NMF_HOST_COMPONENTS =  alsasource.nmfil.host.composite.alsasource \
                             alsasource.nmfil.host.composite.wrapper

SOLIBS_ste_alsasource += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_alsasource = st_AlsaSource

FLAGS_ste_alsasource = -ltinyalsa $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
FLAGS_ste_alsasource += -L$(MMROOT)/audio/alsactrl/lib/$(MMPLATFORM)
FLAGS_ste_alsasource += -lalsactrl
ifeq ($(STE_SOFTWARE),android)
  FLAGS_ste_alsasource += -lstlport
endif

ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
  FLAGS_ste_alsasource += -lmmprobe
endif

INSTALL_SOLIB = ste_alsasource

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

