################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################


include $(MM_MAKEFILES_DIR)/SharedConfig.mk
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES+=ste_alsasink
LIBOBJ_ste_alsasink = ste_omx_ens_component

AUDIO_LIBS = 
AUDIO_NMF_HOST_COMPONENTS =  alsasink.nmfil.host.composite.alsasink \
                             alsasink.nmfil.host.composite.wrapper

SOLIBS_ste_alsasink += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_alsasink = st_AlsaSink

FLAGS_ste_alsasink = -ltinyalsa $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common
FLAGS_ste_alsasink += -L$(MMROOT)/audio/audio_hwctrl/lib/$(MMPLATFORM)
FLAGS_ste_alsasink += -lalsactrl
ifeq ($(STE_SOFTWARE),android)
  FLAGS_ste_alsasink += -lstlport
endif

ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
  FLAGS_ste_alsasink += -lmmprobe
endif

INSTALL_SOLIB = ste_alsasink

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

