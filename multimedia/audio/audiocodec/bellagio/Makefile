################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################


include $(MM_MAKEFILES_DIR)/AudioConfig.mk
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I../proxy -I../proxy/common -I../inc
CPPFLAGS+= -I$(MM_HEADER_DIR)/volctrl

SOLIBNAMES+=ste_audio_source_sink
LIBOBJ_ste_audio_source_sink = ste_omx_ens_component

AUDIO_LIBS = st_audiocodec volctrl st_VolCtrl updownmix
AUDIO_NMF_HOST_COMPONENTS =  volctrl.nmfil.host.composite.effect
SOLIBS_ste_audio_source_sink += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_audio_source_sink= $(AUDIO_LIBS) 

FLAGS_ste_audio_source_sink = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common

CPPFLAGS += -I$(MMROOT)/audio/alsactrl/include
FLAGS_ste_audio_source_sink += -lalsactrl

ifeq ($(MMSYSTEM),android)
FLAGS_ste_audio_source_sink += -L$(ANDROID_OUT_TARGET_PRODUCT_DIRECTORY)/obj/lib
endif
INSTALL_SOLIB = ste_audio_source_sink

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

