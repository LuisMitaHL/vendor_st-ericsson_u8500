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

SOLIBNAMES+=ste_dec_mpeg2
LIBOBJ_ste_dec_mpeg2 = ste_omx_ens_component

VIDEO_LIBS = st_vfm_mpeg2dec st_vfm_mpeg2dec_stsk

VIDEO_NMF_HOST_COMPONENTS = mpeg2dec.arm_nmf.mpeg2deccomparm
SOLIBS_ste_dec_mpeg2 += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_dec_mpeg2= $(VIDEO_LIBS)
FLAGS_ste_dec_mpeg2 = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common

INSTALL_SOLIB = ste_dec_mpeg2

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

