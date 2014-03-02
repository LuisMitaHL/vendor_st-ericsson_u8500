#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifeq ($(DEBUG),1)
CPPFLAGS     += -DDEBUG
endif

# shared library
SOLIBNAMES    += blt_hw
LIBOBJ_blt_hw  = blt_b2r2
INSTALL_SOLIB  = blt_hw

# static library
LIBNAME = blt_hw
LIBOBJ  = blt_b2r2
INSTALL_LIB = libblt_hw.a

INSTALL_HEADER = blt_api.h

ifneq ($(MMSYSTEM),android)
  FLAGS_blt_hw += -pthread
endif

todo: solib lib

include $(MM_MAKEFILES_DIR)/SharedRules.mk
