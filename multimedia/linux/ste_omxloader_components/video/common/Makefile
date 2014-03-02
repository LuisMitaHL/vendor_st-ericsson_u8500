################################################################################
#                        (C) ST-Ericsson
#    Reproduction and Communication of this document is strictly prohibited 
#          unless specifically authorized in writing by ST-Ericsson.
################################################################################


include $(MM_MAKEFILES_DIR)/SharedConfig.mk
include $(MM_MAKEFILES_DIR)/STEOMXLoaderConfig.mk

SOLIBNAMES+=ste_ens_video_common
LIBOBJ_ste_ens_video_common = ste_ens_video_weakcomponents

ifndef DEBUG
VIDEO_LIBS = st_vfm 
else

CHECK_DEBUG_LIBS= st_vfm_debug
ifeq ($(CHECK_DEBUG_LIBS_AVAILABLE) , )
VIDEO_LIBS = st_vfm
else
VIDEO_LIBS = st_vfm_debug
endif

endif

EXTRALIBS_ste_ens_video_common = $(VIDEO_LIBS) 
FLAGS_ste_ens_video_common = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -llos

INSTALL_SOLIB = ste_ens_video_common

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

