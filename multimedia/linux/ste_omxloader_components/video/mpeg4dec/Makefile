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

SOLIBNAMES+=ste_dec_mpeg4
LIBOBJ_ste_dec_mpeg4 = ste_omx_ens_component

ifeq ($(STE_PLATFORM),u5500)
CPPFLAGS+=-DSW_DECODER
endif #end if STE_PLATFORM

VIDEO_LIBS = st_vfm_mpeg4dec st_vfm_mpeg4dec_stsk st_vfm_mpeg4dec_stsk_sw mpeg4swcodec

VIDEO_NMF_HOST_COMPONENTS = mpeg4dec.arm_nmf.mpeg4deccomparm mpeg4dec.arm_nmf.mpeg4dec_sw
SOLIBS_ste_dec_mpeg4 += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_dec_mpeg4= $(VIDEO_LIBS)

FLAGS_ste_dec_mpeg4 = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common

INSTALL_SOLIB = ste_dec_mpeg4

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

