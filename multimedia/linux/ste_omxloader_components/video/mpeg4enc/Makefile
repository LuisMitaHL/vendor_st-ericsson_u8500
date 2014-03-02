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

ifeq ($(STE_PLATFORM),u5500)
CPPFLAGS+=-DSW_ENCODER
endif #end if STE_PLATFORM
SOLIBNAMES+=ste_enc_mpeg4
LIBOBJ_ste_enc_mpeg4 = ste_omx_ens_component

VIDEO_LIBS = st_vfm_mpeg4enc st_vfm_mpeg4enc_stsk mpeg4enc_sw_algo

VIDEO_NMF_HOST_COMPONENTS = mpeg4enc.arm_nmf.mpeg4enc_sw
SOLIBS_ste_enc_mpeg4 += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_enc_mpeg4= $(VIDEO_LIBS) 

FLAGS_ste_enc_mpeg4 = $(LDFLAGS_STE_ENS_OMX_COMPONENTS)  -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common

INSTALL_SOLIB = ste_enc_mpeg4

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

