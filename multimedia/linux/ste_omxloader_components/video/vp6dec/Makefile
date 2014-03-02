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

SOLIBNAMES+=ste_dec_vp6
LIBOBJ_ste_dec_vp6 = ste_omx_ens_component

VIDEO_LIBS = st_vfm_vp6dec vp6_decoder_nmf vp6_parser_nmf

VIDEO_NMF_HOST_COMPONENTS = vp6dec.arm_nmf.vp6dec_sw
SOLIBS_ste_dec_vp6 += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_dec_vp6= $(VIDEO_LIBS)

FLAGS_ste_dec_vp6 = $(LDFLAGS_STE_ENS_OMX_COMPONENTS)  -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common

INSTALL_SOLIB = ste_dec_vp6

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

