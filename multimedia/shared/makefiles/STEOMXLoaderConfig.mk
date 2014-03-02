#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

## Sub directory in which we install the STE OMX components .so
OMX_COMPONENTS_DIR:=ste_omxcomponents

ifeq ($(MMCOMPILER),android)
  COMPONENT_PATH:=/system/lib
else
  COMPONENT_PATH:=/usr/lib
endif

ifeq ($(IS_OMX_COMPONENT_LIB),true)
  INSTALL_SOLIB_DIR := $(INSTALL_ROOTFS_DIR)$(COMPONENT_PATH)/$(OMX_COMPONENTS_DIR)
else
  INSTALL_SOLIB_DIR := $(INSTALL_ROOTFS_DIR)$(COMPONENT_PATH)
endif

#### Management of the HOST only vs DSP platforms
ifneq ($(CORE_NUMBER),1)
# we have some DSP for the PLATFORM we are compiling for so link with NMF-CM lib + XTI-STM user lib
  NMF_CM_LIB = -lnmf
else
  NMF_CM_LIB =
endif

#### The NMF SMP execution engine library available for all platforms
NMF_HOST_EE_LIB = -lnmfee

#### The XTI user library available for 8500, 5500
ifneq ($(MMENV_BOARD),x86)
  STM_TRACE_LIB   = -ltrace
endif

#### Helper variables providing needed ld flags for shared libraries
ALL_MM_ROOTFS_DIR = $(foreach mmplat,$(MMSEARCHPLAT), \
                      $(foreach subdir,$(COMPONENT_PATH) $(COMPONENT_PATH)/$(OMX_COMPONENTS_DIR), \
                        $(MMBUILDIN)/$(ROOTFS_DIR_NAME)/$(mmplat)/$(subdir)))

LDFLAGS_STE_ENS_OMX_COMPONENTS := -ldl -L$(MM_LIB_DIR) \
	$(addprefix -L,$(realpath $(wildcard $(ALL_MM_ROOTFS_DIR)))) \
	$(STM_TRACE_LIB) -lste_ensloader -lstelpcutils $(NMF_HOST_EE_LIB) $(NMF_CM_LIB)

ifneq ($(STE_SOFTWARE),android)
  LDFLAGS_STE_ENS_OMX_COMPONENTS += -lpthread -lrt
endif

#### Helpers for HOST NMF components
SHARED_NMF_HOST_COMPONENTS_LIBS  = $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(MM_SHARED_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(SHARED_NMF_HOST_COMPONENTS))))))
AUDIO_NMF_HOST_COMPONENTS_LIBS   = $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(MM_AUDIO_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(AUDIO_NMF_HOST_COMPONENTS))))))
VIDEO_NMF_HOST_COMPONENTS_LIBS   = $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(MM_VIDEO_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(VIDEO_NMF_HOST_COMPONENTS))))))
IMAGING_NMF_HOST_COMPONENTS_LIBS = $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(MM_IMAGING_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(IMAGING_NMF_HOST_COMPONENTS))))))

ifneq ($(MM_NMF_REPO_DIR),$(INSTALL_NMF_REPO_DIR))
  SHARED_NMF_HOST_COMPONENTS_LIBS  += $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(INSTALL_SHARED_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(SHARED_NMF_HOST_COMPONENTS))))))
  AUDIO_NMF_HOST_COMPONENTS_LIBS   += $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(INSTALL_AUDIO_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(AUDIO_NMF_HOST_COMPONENTS))))))
  VIDEO_NMF_HOST_COMPONENTS_LIBS   += $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(INSTALL_VIDEO_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(VIDEO_NMF_HOST_COMPONENTS))))))
  IMAGING_NMF_HOST_COMPONENTS_LIBS += $(wildcard $(addsuffix .$(LIB_EXT),$(foreach path,$(MMSEARCHPLAT),$(addprefix $(INSTALL_IMAGING_NMF_REPO_DIR)/$(path)/,$(subst .,/,$(IMAGING_NMF_HOST_COMPONENTS))))))
endif

#### Helpers to check debug libs availablity
CHECK_DEBUG_LIBS_AVAILABLE = $(wildcard $(foreach lib,$(CHECK_DEBUG_LIBS),$(foreach plat,$(MMSEARCHPLAT),$(addprefix $(MM_LIB_DIRNAME)/$(plat),$(addsuffix .$(LIB_EXT),$(lib))))))

