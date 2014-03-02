#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

# Makefile to set following variables, based on PLATFORM variable
# all variables are C or C++ compiler settings for MMDSP-based code
#	CC
#	CXX
#	ASM
#	AR
#	LD_EXE
#	LD_LIB
#	CFLAGS
#	ASMFLAGS
#	CPPFLAGS
#	LDFLAGS
#	CXXFLAGS
#	MMDSPCORE
#

MMENV_BOARD=$(shell echo $(PLATFORM) | cut -d - -f 1)

################################################################################

define flag_mmdsp
CC:=ac
CXX:=
ASM:=mmdsp-as
AR:=mmdsp-ar
LD_EXE:=ac
LD_LIB:=ac
TARGET:=dsp24
OPTIMIZ_CFLAGS:=-O3
FIXED_CFLAGS:=
FIXED_ASMFLAGS:=
FIXED_CPPFLAGS:=
FIXED_LDFLAGS:=
FIXED_CXXFLAGS:=
FIXED_LDEXEFLAGS:=--noremove --entry=Reset_Handler
endef

define flag_mmdsp_8500_v2
$(eval $(flag_mmdsp))
FIXED_CPPFLAGS :=$$(FIXED_CPPFLAGS) -D__STN_8500=30
endef

define flag_mmdsp_9540_v1
$(eval $(flag_mmdsp))
FIXED_CPPFLAGS :=$$(FIXED_CPPFLAGS) -D__STN_9540=10
endef

define flag_mmdsp_8540_v1
$(eval $(flag_mmdsp))
FIXED_CPPFLAGS :=$$(FIXED_CPPFLAGS) -D__STN_8540=10
endef

################################################################################

ifeq ($(PLATFORM), x86-linux)
  MMDSPCORE=x86
endif

ifeq ($(PLATFORM), x86_mmdsp-linux)
  MMDSPCORE=x86_mmdsp
endif

ifeq ($(PLATFORM), x86_cortexA9-linux)
  MMDSPCORE=x86_cortexA9
endif


ifeq ($(findstring 5500,$(MMENV_BOARD)),5500)
  MMDSPCORE=5500
endif

ifeq ($(findstring u8500_v2,$(PLATFORM)),u8500_v2)
  MMDSPCORE=mmdsp_8500_v2
  NMF_CHIP=STn8500
  ifeq ($(CORE),mmdsp)
    $(eval $(flag_mmdsp_8500_v2))
  endif
endif

ifeq ($(MMENV_BOARD),u9540_v1)
  MMDSPCORE=mmdsp_9540_v1
  NMF_CHIP=STn9540
  ifeq ($(CORE),mmdsp)
    $(eval $(flag_mmdsp_9540_v1))
  endif
endif

ifeq ($(MMENV_BOARD),l8540_v1)
  MMDSPCORE=mmdsp_8540_v1
  NMF_CHIP=STn8540
  ifeq ($(CORE),mmdsp)
    $(eval $(flag_mmdsp_8540_v1))
  endif
endif

ifeq ($(MMDSPCORE),)
  $(error MMDSPCORE undefined for target platform: PLATFORM='$(PLATFORM)')
endif

