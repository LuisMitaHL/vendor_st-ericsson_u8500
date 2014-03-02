################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################

include $(MM_MAKEFILES_DIR)/SharedConfig.mk
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

CPPFLAGS+=-I$(MM_HEADER_DIR)/

SOLIBNAMES+=ste_ens_audio_common
LIBOBJ_ste_ens_audio_common = ste_ens_audio_weakcomponents


AUDIO_LIBS = st_afm   
AUDIO_LIBS += st_afm_nmfhost
AUDIO_LIBS += st_afm_bindings_optim
AUDIO_LIBS += bitstream

ifneq ($(findstring 5500,$(MYMMBOARD)),5500)
AUDIO_LIBS += audiotables
endif #ifneq ($(findstring 5500,$(MYMMBOARD)),5500)

LIBS_ste_ens_audio_common = btrue

AUDIO_NMF_HOST_COMPONENTS =  \
        hst.composite.dec_fa_wrp  \
        hst.composite.dec_st_wrp  \
        hst.composite.enc_fa_wrp  \
        hst.composite.dec_st_wrp  \
        hst.composite.pp_wrp  \
        afm.nmfil.host.composite.decalgo  \
        afm.nmfil.host.composite.encalgo  \
        hst.composite.shmpcmin  \
        hst.composite.shmpcmout  \
        hst.composite.shmin  \
        hst.composite.shmout  \
        hst.composite.controller \
	hst.bindings.pcmadapterWrapped

SOLIBS_ste_ens_audio_common += $(AUDIO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_ens_audio_common = $(AUDIO_LIBS)

FLAGS_ste_ens_audio_common = $(LDFLAGS_STE_ENS_OMX_COMPONENTS)
# Link with mmprobe if feature enabled in BoardConfig.mk
ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
FLAGS_ste_ens_audio_common += -lmmprobe
endif

INSTALL_SOLIB = ste_ens_audio_common
INSTALL_HEADER= ste_ens_audio_components.h

todo: lib solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

