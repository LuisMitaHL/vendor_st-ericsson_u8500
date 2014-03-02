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

SOLIBNAMES+=ste_enc_jpeg
LIBOBJ_ste_enc_jpeg = ste_omx_ens_component

ifeq ($(STE_PLATFORM),u5500)
  CPPFLAGS+=-DSW_ENCODER
endif #end if STE_PLATFORM

VIDEO_LIBS = st_vfm_jpegenc st_vfm_jpegenc_stsk

VIDEO_NMF_HOST_COMPONENTS = jpegenc.arm_nmf.jpegenc_sw

ifeq ($(STE_PLATFORM),u5500)
VIDEO_NMF_HOST_COMPONENTS += jpegenc.arm_nmf.hva_jpegenc
endif #end if STE_PLATFORM

SOLIBS_ste_enc_jpeg+= $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_enc_jpeg= $(VIDEO_LIBS)
FLAGS_ste_enc_jpeg = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common
ifeq ($(STE_PLATFORM),u5500)
FLAGS_ste_enc_jpeg += -lhva
endif
INSTALL_SOLIB = ste_enc_jpeg

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

