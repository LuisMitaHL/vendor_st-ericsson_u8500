################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################

include $(MM_MAKEFILES_DIR)/SharedConfig.mk
# We are generating an OMX component .so - request to be installed in correct place
IS_OMX_COMPONENT_LIB:=true
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I../proxy

SOLIBNAMES+=ste_speech_proc
LIBOBJ_ste_speech_proc = ste_omx_ens_component

AUDIO_LIBS = st_speech_proc_host

ifeq (u5500,$(filter u5500%,$(STE_PLATFORM)))
AUDIO_NMF_HOST_COMPONENTS = speech_proc.nmf.host.composite.speech_proc
AUDIO_LIBS += resampling
else ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
AUDIO_NMF_HOST_COMPONENTS = speech_proc.nmf.host.composite.speech_proc
AUDIO_LIBS += resampling
else
AUDIO_NMF_HOST_COMPONENTS = speech_proc.nmf.host.composite.speech_proc_hybrid
endif

SOLIBS_ste_speech_proc += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_speech_proc = $(AUDIO_LIBS)

FLAGS_ste_speech_proc = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_audio_common -llos -lspeech_processing

INSTALL_SOLIB = ste_speech_proc

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

