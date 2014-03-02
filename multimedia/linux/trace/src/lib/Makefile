# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

include $(MM_MAKEFILES_DIR)/SharedConfig.mk
ifneq ($(PLATFORM),x86-linux)
  -include $(MM_MAKEFILES_DIR)/KernelConfig.mk
endif

################################################################################
# Header
VPATH += ../../api
INSTALL_HEADER = stm-trace.h

################################################################################
# Static lib

LIBNAME = trace

CPPFLAGS += -I../../api

LIBOBJ = trace

INSTALL_LIB = libtrace.a

################################################################################
# Dynamic lib

SOLIBNAMES+=trace

EXTRALIBS_trace = trace

ifneq ($(MMSYSTEM),android)
  FIXED_LDSOFLAGS += -pthread 
endif

INSTALL_SOLIB = trace

################################################################################

todo: lib solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk
