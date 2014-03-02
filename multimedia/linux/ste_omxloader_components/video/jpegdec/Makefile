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

SOLIBNAMES+=ste_dec_jpeg
LIBOBJ_ste_dec_jpeg = ste_omx_ens_component

VIDEO_LIBS=st_vfm_jpegdec st_vfm_jpegdec_stsk jpegdec_sw_algo

VIDEO_NMF_HOST_COMPONENTS = jpegdec.arm_nmf.jpegdeccomparm jpegdec.arm_nmf.jpegdec_sw jpegdec.arm_nmf.jpegdec_dual

SOLIBS_ste_dec_jpeg += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_dec_jpeg= $(VIDEO_LIBS) 

FLAGS_ste_dec_jpeg = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common

INSTALL_SOLIB = ste_dec_jpeg

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

