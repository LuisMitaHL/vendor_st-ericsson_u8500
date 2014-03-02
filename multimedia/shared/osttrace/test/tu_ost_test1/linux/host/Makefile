# vim:syntax=make:
# -*- Mode: Makefile -*-
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

#Not possible since lib dep are SOC dependent
#MMBOARD=x

include ../make.ost_conf
OST_COMPONENT_ID = $(OST_COMPONENT_ID_HOST)
OST_DICO_NAME = $(OST_COMPONENT_NAME)_host_arm
OST_SRC_DIR = src
INSTALL_DICO = $(OST_DICO_NAME).xml

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifeq ($(MMSYSTEM),linux)
  MMSYSTEM_FLAVOR=linux
endif
ifeq ($(MMSYSTEM),android)
  MMSYSTEM_FLAVOR=linux
  EXE_LDFLAGS += $(ANDROID_OUT_TARGET_PRODUCT_DIRECTORY)/obj/STATIC_LIBRARIES/liblog_intermediates/liblog.a
  EXE_LDFLAGS += -L$(ANDROID_OUT_TARGET_PRODUCT_DIRECTORY)/obj/lib
  LDFLAGS+= -lcutils
endif

CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces -I../../common/host/src

VPATH += src
VPATH += $(SHARED_REPOSITORY)/host

EXEOBJ += example
EXENAME = st_osttest1 

ifeq ($(MMSYSTEM_FLAVOR),linux)
  LDFLAGS += -llos -lnmfee -ltrace -lstelpcutils
  ifneq ($(CORE_NUMBER),1)
    # we have some DSP for the PLATFORM we are compiling for so link with NMF-CM lib + XTI-STM user lib
    LDFLAGS += -lnmf
  endif
  INSTALL_BIN = st_osttest1
endif

ifneq ($(findstring x86,$(MYMMBOARD)),x86)
  NMF_HOST_COMPONENTS = network network2
else
  NMF_HOST_COMPONENTS = network2
endif

NMFLIBS := $(wildcard $(addsuffix .a, \
           $(foreach tmp, $(MMSEARCHPLAT), \
           $(addprefix $(INSTALL_NMF_REPO_DIR)/$(tmp)/tu_ost_test/,$(subst .,/,$(NMF_HOST_COMPONENTS))))))

LIBS += libosttrace.a  $(NMFLIBS) libst_osttest_common.a $(COMMON_LIBS) 

todo: exe

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

