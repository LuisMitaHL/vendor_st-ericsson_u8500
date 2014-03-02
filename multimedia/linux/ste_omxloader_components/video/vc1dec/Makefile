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

SOLIBNAMES+=ste_dec_vc1
LIBOBJ_ste_dec_vc1 = ste_omx_ens_component

VIDEO_LIBS = st_vc1dec_ddep

VIDEO_NMF_HOST_COMPONENTS = vc1dec.arm_nmf.proxynmf
SOLIBS_ste_dec_vc1 += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_dec_vc1= $(VIDEO_LIBS)

FLAGS_ste_dec_vc1 = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_video_common

INSTALL_SOLIB = ste_dec_vc1

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

