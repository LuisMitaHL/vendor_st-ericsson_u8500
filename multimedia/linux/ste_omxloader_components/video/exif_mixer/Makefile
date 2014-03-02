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

SOLIBNAMES+=ste_exif_mixer
LIBOBJ_ste_exif_mixer = ste_omx_ens_component

ifeq ($(STE_PLATFORM),u5500)
CPPFLAGS+=-DSW_DECODER
endif #end if STE_PLATFORM

VIDEO_LIBS = st_vfm_exifmixer

VIDEO_NMF_HOST_COMPONENTS = exif_mixer.arm_nmf.exifmixer
SOLIBS_ste_exif_mixer += $(VIDEO_NMF_HOST_COMPONENTS_LIBS)

EXTRALIBS_ste_exif_mixer= $(VIDEO_LIBS)

FLAGS_ste_exif_mixer = $(LDFLAGS_STE_ENS_OMX_COMPONENTS) -lste_ens_video_common

INSTALL_SOLIB = ste_exif_mixer

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

