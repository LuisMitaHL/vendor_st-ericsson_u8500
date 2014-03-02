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

SOLIBNAMES+=ste_val_src_sink
LIBOBJ_ste_val_src_sink = ste_omx_ens_component

VIDEO_LIBS = st_vfm_val_source_sink_stsk st_vfm_val_ss sw_hash cli blt_hw

VIDEO_NMF_HOST_COMPONENTS = val_source_sink_wrp
SOLIBS_ste_val_src_sink += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_val_src_sink= $(VIDEO_LIBS) 
FLAGS_ste_val_src_sink = $(LDFLAGS_STE_ENS_OMX_COMPONENTS)  -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common -llos

INSTALL_SOLIB = ste_val_src_sink

ifneq ($(MMSYSTEM),android)
  CPPFLAGS += -pthread
  LDSOFLAGS += -lpthread
endif

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

