# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

LIBNAME = nmf

## CM internal header files are in the driver dir
CPPFLAGS += -I$(COMPONENT_TOP_DIR)/src/common \
           -I$(COMPONENT_TOP_DIR)/src/driver -I. \
	   -Wall -Werror -g -fvisibility=hidden -fno-strict-aliasing #-DCM_API_TIMING

CXXFLAGS += -Wno-write-strings

ifneq ($(MMENV_SYSTEM), android)
  LDSOFLAGS += -pthread
endif

#Files that compose the libnmf

CMPROXYSRC:=$(shell find cm -name "*.c") $(wildcard *.c)
CMPROXYCPPSRC:=$(shell find cm -name "*.cpp")

LIBOBJ        = $(CMPROXYSRC:.c=) $(CMPROXYCPPSRC:.cpp=)
INSTALL_LIB   = libnmf.a

todo: lib

SOLIBNAMES+=nmf
EXTRALIBS_nmf = nmf
INSTALL_SOLIB = nmf

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk
