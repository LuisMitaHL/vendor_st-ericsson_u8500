#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifndef CORE
  CORE=mmdsp
endif

include $(MM_MAKEFILES_DIR)/SharedCheck.mk 

SRCPATH  += $(PWD)

ifneq ($(MMSHARED),)
  ## MMSHARED defined only when MAKEFILES_BACKWARD_COMPATIBLE=yes to get osttrace NMF, ENS, ... sources
  SRCPATH  += $(MMSHARED)
endif

## We include last the installed NMF sources as may provides the .conf but not the 'src'
ifeq ($(CORE),mmdsp)
  ## Where NMF sources of other components are installed
  SRCPATH   += $(MM_NMF_COMP_SRC_MMDSP_DIR) $(MM_NMF_COMP_SRC_MMDSP_DIR)/common
  SHAREPATH += $(MM_NMF_COMP_SRC_COMMON_DIR) $(OST_INC_AUTOGEN)
else
  ## Where NMF sources of other components are installed
  SRCPATH   += $(MM_NMF_COMP_SRC_HOSTEE_DIR) $(MM_NMF_COMP_SRC_HOSTEE_DIR)/common
  SHAREPATH += $(MM_NMF_COMP_SRC_COMMON_DIR) $(OST_INC_AUTOGEN)
endif

SRCPATH += $(MM_NMF_COMP_SRC_COMMON_DIR)

## Headers installed this component
SRCPATH += $(INSTALL_HEADER_BASEDIR)/shared
ifneq ($(INSTALL_HEADER_BASEDIR),$(MM_HEADER_BASEDIR))
  ## Headers installed by other components
  SRCPATH += $(MM_HEADER_BASEDIR)/shared
endif

CFLAGS     += $(FIXED_CFLAGS)
ASMFLAGS   += $(FIXED_ASMFLAGS)
LIBLDFLAGS += $(FIXED_LDLIBFLAGS)
CXXFLAGS   += $(FIXED_CXXFLAGS)
CPPFLAGS   += $(FIXED_CPPFLAGS)

ifeq ($(CORE),arm)
  ifneq ($(findstring x86,$(MMPROCESSOR)),x86)
    ifneq ($(MMENV_SYSTEM),android)
      ## Include kernel headers
      CPPFLAGS += -I$(MMBUILDIN)/$(HEADER_DIR_NAME)/$(PLATFORM)/kernel
    endif
  endif
endif

ifeq ($(CORE),mmdsp)
  TARGET=dsp24
  SRCPATH += $(NMF_TOOLS_DIR)/api/mpcee
else
  ifeq ($(MMCOMPILER),rvct)
    TARGET=rvct
  else
    ifeq ($(MMCOMPILER),gnu)
      ifeq ($(MMPROCESSOR),x86)
        TARGET=unix
      else
        TARGET=armgcc
      endif # x86
    endif # gnu
  endif # rvct
  # mpcee is needed for ARM/DSP composite
  SRCPATH += $(NMF_TOOLS_DIR)/api/smpee $(NMF_TOOLS_DIR)/api/mpcee
  ## We are getting things from nmf.linux component directly only when MAKEFILES_BACKWARD_COMPATIBLE=yes
  ifneq ($(NMF_LINUX_API_DIR),)
    SRCPATH += $(NMF_LINUX_API_DIR)
  endif
endif

NMFENV = \
  CPPFLAGS="$(CPPFLAGS)" \
  CFLAGS="$(CFLAGS)" \
  CXXFLAGS="$(CXXFLAGS)" \
  LDFLAGS="$(LDFLAGS)" \
  LIBLDFLAGS="$(LIBLDFLAGS)" \
  CC="$(CC)" \
  CXX="$(CXX)" \
  ASM="$(ASM)" \
  AR="$(AR)" \
  EXELD="$(LD_EXE)" \
  LIBLD="$(LD_LIB)" \
  TARGET="$(TARGET)" \
  SRCPATH="$(SRCPATH)" \
  SHAREPATH="$(SHAREPATH)" \
  BUILD="$(BUILD)" \
  MCPROJECT="$(MCPROJECT)"

NMFCONFC = $(NMFENV) $(NMF_TOOLS_DIR)/nmfconfc
NMFITFC  = $(NMFENV) $(NMF_TOOLS_DIR)/nmfitfc
ITF_PREFIX = $(MEDIUM)

NMFCONFC_FLAGS :=
NMFITFC_FLAGS  :=

ifeq ($(CORE),mmdsp)
  ## The MMDSP binaries are always generated in the component's dir
  ## => use $(MODULE) and not $(BUILD_INTERMEDIATES_DIR)
  ## Because filename must be < 256 tmp dir also there
  BUILD     = $(MODULE)/$(NMF_REPO_DIR)/$(MMPROCESSOR)$(BUILDING_MM_VARIANT)
  TMPDIR    = $(MODULE)/$(NMF_TMP_DIR)/$(MMPROCESSOR)$(BUILDING_MM_VARIANT)
  REALCLEAN_DIRS = $(MODULE)/$(NMF_REPO_DIR)/$(MMPROCESSOR)$(BUILDING_MM_VARIANT) $(MODULE)/$(NMF_TMP_DIR)/$(MMPROCESSOR)$(BUILDING_MM_VARIANT)
else
  ifeq ($(BUILD_ARM_NMF),bin)
    ## binaries are to be installed from source tree
    BUILD   = $(MODULE)/$(NMF_REPO_DIR)/$(MMPLATFORM)
    REALCLEAN_DIRS += $(MODULE)/$(NMF_REPO_DIR)/$(MMPLATFORM)
  else
    BUILD   = $(BUILD_INTERMEDIATES_DIR)/$(NMF_REPO_DIR)/$(MMPLATFORM)
    REALCLEAN_DIRS += $(BUILD_INTERMEDIATES_DIR)/$(NMF_REPO_DIR)/$(MMPLATFORM)
  endif
  TMPDIR    = $(BUILD_TMP_DIR)/$(NMF_TMP_DIR)/$(MMPLATFORM)
  REALCLEAN_DIRS += $(BUILD_TMP_DIR)/$(NMF_TMP_DIR)/$(MMPLATFORM)
endif

ifdef ENS_VERSION
CPPFLAGS += -DENS_VERSION=$(ENS_VERSION)
CPPFLAGS += -DENS_VERSION_SHARED=$(ENS_VERSION_SHARED)
CPPFLAGS += -DENS_VERSION_VIDEO=$(ENS_VERSION_VIDEO)
CPPFLAGS += -DENS_VERSION_AUDIO=$(ENS_VERSION_AUDIO)
CPPFLAGS += -DENS_VERSION_IMAGING=$(ENS_VERSION_IMAGING)

ifeq ($(CORE),mmdsp)
else
ifeq ($(ENS_VERSION),$(ENS_VERSION_VIDEO))
SRCPATH := $(MM_NMF_COMP_SRC_HOSTEE_DIR)/ens_video $(SRCPATH)
SRCPATH := $(MM_NMF_COMP_SRC_HOSTEE_DIR)/ens_video/common $(SRCPATH)
endif

ifeq ($(ENS_VERSION),$(ENS_VERSION_IMAGING))
SRCPATH := $(MM_NMF_COMP_SRC_HOSTEE_DIR)/ens_imaging $(SRCPATH)
SRCPATH := $(MM_NMF_COMP_SRC_HOSTEE_DIR)/ens_imaging/common $(SRCPATH)
endif

ifeq ($(ENS_VERSION),$(ENS_VERSION_AUDIO))
SRCPATH := $(MM_NMF_COMP_SRC_HOSTEE_DIR)/ens_audio $(SRCPATH)
SRCPATH := $(MM_NMF_COMP_SRC_HOSTEE_DIR)/ens_audio/common $(SRCPATH)
endif
endif

endif

