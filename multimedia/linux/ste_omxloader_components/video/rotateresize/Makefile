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

SOLIBNAMES += ste_rotateresize
LIBOBJ_ste_rotateresize = ste_omx_ens_component 

VIDEO_LIBS = vfm_rotateresize

VIDEO_NMF_HOST_COMPONENTS = rotateresize.arm_nmf.rotateresize
SOLIBS_ste_rotateresize += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_rotateresize= $(VIDEO_LIBS)

FLAGS_ste_rotateresize += $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common

## Android case: $(MM_INSTALL_DIR)/system/lib
## Linux case: $(MM_LIB_DIR)
FLAGS_ste_rotateresize += -L$(MM_INSTALL_DIR)/system/lib -L$(MM_LIB_DIR) -lblt_hw

INSTALL_SOLIB = ste_rotateresize

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

