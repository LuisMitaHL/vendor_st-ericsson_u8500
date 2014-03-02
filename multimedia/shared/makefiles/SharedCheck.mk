#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

ifeq ($(MEDIUM),video)
  MCPROJECT=$(DSPTOOLS)/target/project/nomadik/$(NMF_CHIP)/video/
else
  MCPROJECT=$(DSPTOOLS)/target/project/nomadik/$(NMF_CHIP)/audio/
endif

########## NMF MPC repositories

## NMF repositories local to each component are built under MMDSPCORE (maybe generic name)
BASE_REPOSITORY_MPC  :=$(NMF_REPO_DIR)/$(MMDSPPROCESSOR)

## NMF repositories are installed under MYMMDSPCORE (board specific name)
REPOSITORY_MPC       :=$(INSTALL_NMF_REPO_BOARD_DIR)

MM_REPOSITORY_MPC    :=$(REPOSITORY_MPC)
SHARED_REPOSITORY_MPC:=$(REPOSITORY_MPC)
IMG_REPOSITORY_MPC   :=$(REPOSITORY_MPC)
VIDEO_REPOSITORY_MPC :=$(REPOSITORY_MPC)
AUDIO_REPOSITORY_MPC :=$(REPOSITORY_MPC)

########## NMF HOST repositories
BASE_REPOSITORY_HOST  :=$(NMF_REPO_DIR)/$(MMPLATFORM)

REPOSITORY_HOST       :=$(INSTALL_NMF_REPO_DIR)/$(MMPLATFORM)

MM_REPOSITORY_HOST    :=$(MM_NMF_REPO_DIR)/$(MMPLATFORM)
SHARED_REPOSITORY_HOST:=$(MM_REPOSITORY_HOST)
IMG_REPOSITORY_HOST   :=$(MM_REPOSITORY_HOST)
VIDEO_REPOSITORY_HOST :=$(MM_REPOSITORY_HOST)
AUDIO_REPOSITORY_HOST :=$(MM_REPOSITORY_HOST)

## Here only for compatibility reason with previous DSP NMF naming
## To be removed once all packages build with new convention
## This should only be used in the the CPPFLAGS in generic makefiles
ifeq ($(CORE_NUMBER),2)
BASE_REPOSITORY  :=$(BASE_REPOSITORY_MPC)
REPOSITORY       :=$(REPOSITORY_MPC)
SHARED_REPOSITORY:=$(SHARED_REPOSITORY_MPC)
IMG_REPOSITORY   :=$(IMG_REPOSITORY_MPC)
VIDEO_REPOSITORY :=$(VIDEO_REPOSITORY_MPC)
AUDIO_REPOSITORY :=$(AUDIO_REPOSITORY_MPC)
else
BASE_REPOSITORY  :=$(NMF_REPO_DIR)
REPOSITORY       :=$(INSTALL_NMF_REPO_DIR)
SHARED_REPOSITORY:=$(MM_SHARED_NMF_REPO_DIR)
IMG_REPOSITORY   :=$(MM_IMAGING_NMF_REPO_DIR)
VIDEO_REPOSITORY :=$(MM_VIDEO_NMF_REPO_DIR)
AUDIO_REPOSITORY :=$(MM_AUDIO_NMF_REPO_DIR)
endif
################################################################################

## Compute some path in which NMF on ARM headers are installed

HOST_NMF_SUB_DIRS = . host

HOST_NMF_PREFIXES = $(foreach dir, $(INSTALL_NMF_REPO_DIR), \
                      $(foreach path, $(MMSEARCHPLAT), \
                        $(foreach subdir, $(HOST_NMF_SUB_DIRS), \
                           $(dir)/$(path)/$(subdir))))

ifneq ($(INSTALL_NMF_REPO_DIR),$(MM_NMF_REPO_DIR))
  HOST_NMF_PREFIXES += $(foreach dir, $(MM_NMF_REPO_DIR), \
                         $(foreach path, $(MMSEARCHPLAT), \
                           $(foreach subdir, $(HOST_NMF_SUB_DIRS), \
                             $(dir)/$(path)/$(subdir))))
endif

HOST_NMF_SEARCH_DIRS := $(realpath $(wildcard $(HOST_NMF_PREFIXES)))

ENS_VERSION_SHARED  := 1
ENS_VERSION_VIDEO   := 2
ENS_VERSION_AUDIO   := 3
ENS_VERSION_IMAGING := 4

ifeq ($(MEDIUM),shared)
    ENS_VERSION := $(ENS_VERSION_SHARED)
else ifeq ($(MEDIUM),video)
    ENS_VERSION := $(ENS_VERSION_SHARED)
else ifeq ($(MEDIUM),imaging)
    ENS_VERSION := $(ENS_VERSION_SHARED)
else ifeq ($(MEDIUM),audio)
    ENS_VERSION := $(ENS_VERSION_SHARED)
else
    # Used for bellagio_ste_omxloader and others...
    ENS_VERSION := $(ENS_VERSION_SHARED)
endif
