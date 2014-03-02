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

SOLIBNAMES+=ste_enc_h264
LIBOBJ_ste_enc_h264 = ste_omx_ens_component

VIDEO_LIBS = st_h264enc_ddep st_h264enc_ddep_stub

VIDEO_NMF_HOST_COMPONENTS = h264enc.arm_nmf.h264enc \
                            h264enc.arm_nmf.h264enc_hdtv \
                            h264enc.arm_nmf.h264enc_stub \
                            h264enc.arm_nmf.proxynmf_stub \
                            h264enc.arm_nmf.proxynmf

SOLIBS_ste_enc_h264 += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_enc_h264= $(VIDEO_LIBS)

FLAGS_ste_enc_h264 = $(LDFLAGS_STE_ENS_OMX_COMPONENTS)  -L$(COMPONENT_TOP_DIR)/video/common/lib/$(MMPLATFORM) -lste_ens_video_common
ifeq ($(STE_PLATFORM),u5500)
FLAGS_ste_enc_h264 += -lhva
endif
INSTALL_SOLIB = ste_enc_h264

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

