#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifndef CORE
  CORE=arm
endif

include $(MM_MAKEFILES_DIR)/SharedCheck.mk

#-----------------------------------
#  Variables
#-----------------------------------
AR_OPTION  =-rc
OBJ_EXT    =o
LIB_EXT    =a
NOLINK     =-c
MODE16     =0
DEPS_FLAGS =-MD -MF

#-----------------------------------
#  COMPILER specific setup
#-----------------------------------
ifeq ($(MMCOMPILER),flexcc)
  USE_OPT_COMPILER=false
  AR_NAME="mmdsp AR"
else
  AR_NAME="target GNU AR"
  ifeq ($(USE_OPT_COMPILER), true)
    ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
      ## Discarding USE_OPT_COMPILER for x86* cores
      USE_OPT_COMPILER=false
      AR_NAME="host GNU AR  "
    else
      OPT_CC  += $(OPT_FIXED_CFLAGS)
      OPT_CXX += $(OPT_FIXED_CXXFLAGS)
      OPT_ASM += $(OPT_FIXED_ASMFLAGS)
      CPPFLAGS+= $(OPT_FIXED_CPPFLAGS)
      AR_NAME="RVDS AR      "
    endif
  endif
endif

ifneq ($(USE_OPT_COMPILER),true)
  CC  += $(FIXED_CFLAGS)
  CXX += $(FIXED_CXXFLAGS)
  ASM += $(FIXED_ASMFLAGS)
  CPPFLAGS+= $(FIXED_CPPFLAGS)
else
  ## Make sure we don't try to link with Android libs
  TARGET_CRTBEGIN_O:=
  TARGET_CRTEND_O:=
endif

ifeq ($(findstring rvct,$(MMCOMPILER)),rvct)
  EXE_EXT    =.axf
  COMPILER_FLAVOR=rvct
  DEPS_FLAGS = --md --depend=
  COMPILER_NAME="ARM RVDS  "
else
  ifeq ($(MMCOMPILER),gnu)
    COMPILER_FLAVOR=gcc
    PRE_LIBS   = -Wl,--start-group
    POST_LIBS  = -Wl,--end-group
    PRE_SOLIBS   = -Wl,--whole-archive
    POST_SOLIBS  = -Wl,--no-whole-archive
    ifeq ($(findstring x86,$(MMPROCESSOR)),x86)
      COMPILER_NAME="host GCC  "
      EXE_EXT =.exe
    else
      COMPILER_NAME="target GCC"
      EXE_EXT =
    endif
  else
    ifeq ($(MMCOMPILER),flexcc)
      #dsp target
      COMPILER_NAME="mmdsp     "
      COMPILER_FLAVOR=flexcc
      PRE_LIBS=link="--start-group
      POST_LIBS=--end-group"
      LIB_EXT       =a
      EXE_EXT       =.elf
      NOLINK        =-link
      WFLAG1        =-Wall
      WFLAG2        =-Wrestrict
      DEPS_FLAGS    = -MM -MF=
      CPPFLAGS+= -I$(DSPTOOLS)/include -I"$(MCPROJECT)/include" -I$(DSPTOOLS)/bit_true/include
      ## Make sure we don't try to link with Android libs
      TARGET_CRTBEGIN_O:=
      TARGET_CRTEND_O:=
    else
      ifeq ($(MMCOMPILER),android)
        COMPILER_NAME="target GCC"
        COMPILER_FLAVOR=gcc
        EXE_EXT =
        PRE_LIBS   = -Wl,--start-group
        POST_LIBS  = -Wl,--end-group
        PRE_SOLIBS   = -Wl,--whole-archive
        POST_SOLIBS  = -Wl,--no-whole-archive
      else
        $(error "Invalid MMCOMPILER: $(MMCOMPILER)")
      endif
    endif
  endif
endif

#-----------------------------------
# Default value 
#-----------------------------------

## .o and .d are always out of source tree
OBJ_DIR = $(BUILD_TMP_DIR)/$(OBJ_DIR_NAME)/$(MMPLATFORM)
DEP_DIR = $(BUILD_TMP_DIR)/$(DEP_DIR_NAME)/$(UNAME)/$(MMPLATFORM)

## When building for packaging BUILD_INTERMEDIATES_DIR will point to source tree
## (defined in shared/mmenv/SharedDefines.mk)
BIN_DIR = $(BUILD_INTERMEDIATES_DIR)/$(BIN_DIR_NAME)/$(MMPLATFORM)
LIB_DIR = $(BUILD_INTERMEDIATES_DIR)/$(LIB_DIR_NAME)/$(MMPLATFORM)

################################################################################


ifeq ($(BUILD_ARM_NMF),bin)
  ## We perform then only installation and the prebuild library are local to the module so install those ones
  LIB_DIR = $(MODULE)/$(LIB_DIR_NAME)/$(MMPLATFORM)
endif

################################################################################
## Keep below lines in sync with default full source build options
## Force generation of RVCT libraries in source tree as under SCM
## and may be used by GCC build so must be under source control
ifeq ($(USE_OPT_COMPILER),true)
  ## Don't care about packaging for the time being as binaries are always in source tree
  LIB_DIR = $(MODULE)/$(LIB_DIR_NAME)/$(MMPLATFORM)
else
  ifeq ($(BUILD_ARM_CODE),bin)
    ## Only installation done, the prebuilt libraries are local to the module so install those ones
    LIB_DIR = $(MODULE)/$(LIB_DIR_NAME)/$(MMPLATFORM)
  endif
endif

## MMDSP libraries are used in the NMF MPC generation which are under source tree
## no need to keep them in source tree as MMDSP build is globally enable/disabled

################################################################################

LIBS_PATH = $(foreach platform, $(MMPLATFORM) $(COMPATPLAT), \
                      $(addsuffix $(platform), $(INSTALL_LIB_DIRNAME)/))

ifneq ($(INSTALL_LIB_DIRNAME),$(MM_LIB_DIRNAME))
  LIBS_PATH += $(foreach platform, $(MMPLATFORM) $(COMPATPLAT), \
                         $(addsuffix $(platform), $(MM_LIB_DIRNAME)/))
endif

CPPFLAGS += $(EXTRA_CPPFLAGS)

## For now default to implicit CPPFLAGS but may change in the future
USE_IMPLICIT_CPPFLAGS ?= yes

ifeq ($(USE_IMPLICIT_CPPFLAGS),yes)

  INCLUDE_DIRS += inc include

  ifneq ($(MYMMSYSTEM),symbian)

    ## Include the MEDIUM specific headers
    INCLUDE_DIRS += $(INSTALL_HEADER_DIR)
    ifneq ($(INSTALL_HEADER_BASEDIR),$(MM_HEADER_BASEDIR))
      INCLUDE_DIRS += $(MM_HEADER_DIR)
    endif

    ifneq ($(CORE),mmdsp)

      ## Include headers from shared components
      INCLUDE_DIRS += $(INSTALL_HEADER_BASEDIR)/shared
      ifneq ($(INSTALL_HEADER_BASEDIR),$(MM_HEADER_BASEDIR))
        INCLUDE_DIRS += $(MM_HEADER_BASEDIR)/shared
      endif

      ## Include headers from linux components
      ## video not included as jpegdec:utils.h header conflict with bellagio:utils.h
      ifeq ($(MEDIUM),shared)
        INCLUDE_LINUX_HEADERS=yes
      endif
      ifeq ($(MEDIUM),audio)
        INCLUDE_LINUX_HEADERS=yes
      endif
      ifeq ($(MEDIUM),imaging)
        INCLUDE_LINUX_HEADERS=yes
      endif

      ifeq ($(INCLUDE_LINUX_HEADERS),yes)
        INCLUDE_DIRS += $(INSTALL_HEADER_BASEDIR)/linux
        ifneq ($(INSTALL_HEADER_BASEDIR),$(MM_HEADER_BASEDIR))
          INCLUDE_DIRS += $(MM_HEADER_BASEDIR)/linux
        endif
      endif

      ## Include kernel headers
      ifneq ($(findstring x86,$(MMPROCESSOR)),x86)
        ifneq ($(MMENV_SYSTEM),android)
          INCLUDE_DIRS += $(MMBUILDIN)/$(HEADER_DIR_NAME)/$(PLATFORM)/kernel
        endif
      endif

      ifneq ($(NMF_LINUX_API_DIR),)
        INCLUDE_DIRS += $(NMF_LINUX_API_DIR)
      endif

      INCLUDE_DIRS += $(SHARED_REPOSITORY_MPC) \
                      $(AUDIO_REPOSITORY_MPC) \
                      $(VIDEO_REPOSITORY_MPC) \
                      $(IMG_REPOSITORY_MPC)

      INCLUDE_DIRS += $(NMF_TOOLS_DIR)/api/smpee

    endif

  endif #ifneq ($(MYMMSYSTEM),symbian)

  ifneq ($(MYMMSYSTEM),symbian)
    INCLUDE_DIRS += $(HOST_NMF_SEARCH_DIRS)
  endif

  ## Make sure directories exist
  INCLUDE_DIRS := $(realpath $(INCLUDE_DIRS))

  CPPFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

  VPATH = inc include src

endif #ifeq ($(USE_IMPLICIT_CPPFLAGS),yes)

NMF_CHIP_REP=$(shell echo $(NMF_CHIP) | awk '{print(tolower($$0));}')

ifeq ($(CORE),mmdsp)
  ifeq ($(strip $(ARCHI)),)
    ARCHI_FROM_TOOLS = $(shell cat $(subst \,/,$(MCPROJECT))/*.prj | cut -d = -f2)
  else
    ARCHI_FROM_TOOLS = $(ARCHI)
  endif
endif

ifeq (x86_mmdsp,$(MYMMBOARD))
  BITTRUE_LIB16=$(DSPTOOLS)/bit_true/lib/$(UNAME)/r_lldsplib16.a
  BITTRUE_LIB24=$(DSPTOOLS)/bit_true/lib/$(UNAME)/r_lldsplib24.a
  CPPFLAGS+= -I$(DSPTOOLS)/bit_true/include
endif
ifeq (x86_cortexA9,$(MYMMBOARD))
   BITTRUE_LIB+=$(MMAUDIO)/tools/ARM/cmodels/cortexA9/stm-arm-neon-c-models/lib/libstm-arm-neon.a
   CPPFLAGS+= -I$(MMAUDIO)/tools/ARM/cmodels/cortexA9/stm-arm-neon-c-models/include/
endif

ifdef ENS_VERSION
CPPFLAGS += -DENS_VERSION=$(ENS_VERSION)
CPPFLAGS += -DENS_VERSION_SHARED=$(ENS_VERSION_SHARED)
CPPFLAGS += -DENS_VERSION_VIDEO=$(ENS_VERSION_VIDEO)
CPPFLAGS += -DENS_VERSION_AUDIO=$(ENS_VERSION_AUDIO)
CPPFLAGS += -DENS_VERSION_IMAGING=$(ENS_VERSION_IMAGING)
endif
