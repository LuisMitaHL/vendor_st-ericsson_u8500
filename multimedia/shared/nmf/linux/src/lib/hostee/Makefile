# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

CORE=arm

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

include ../../../Make.config

#
# NMF Compilation flags
#
SRCPATH    += $(NMF_TOOLS)/api/c $(NMF_TOOLS)/api/smpee .
SHAREPATH  += $(COMPONENT_TOP_DIR)/api $(COMPONENT_TOP_DIR)/src/common
CPPFLAGS   += $(FIXED_CPPFLAGS)
CFLAGS     += $(FIXED_CFLAGS) -Wall -g -Werror
CXXFLAGS   += $(FIXED_CXXFLAGS) -Wall -g -Werror
LIBLDFLAGS += $(FIXED_LDLIBFLAGS) 

EECONF=nmfHostOnlyEE
ifeq ($(findstring x86,$(PLATFORM)),)
  ifeq ($(findstring 5500,$(PLATFORM)),)
    EECONF=nmfDistributedEE
  endif
endif
ifneq ($(PLATFORM),x86-linux)
  CFLAGS   += -fvisibility=hidden
  CXXFLAGS += -fvisibility=hidden
endif

ifeq ($(MMENV_SYSTEM), android)
  CXXFLAGS += -D__USE_POSIX_SIGEV_THREAD
else
  CXXFLAGS += -D__USE_THREAD
endif

## As we have both a C and C++ project we need to manage them in a seperate manner
NMFCONFC_NO_CPP := true

COMPONENTS := $(EECONF) timer

## time is a C++ based NMF component
NMFCONFC_FLAGS_timer = --cpp --partial

NMFCONFC_FLAGS_$(EECONF) = --partial

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk
