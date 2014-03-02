# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

LIBNAME = los

CPPFLAGS += $(FIXED_CPPFLAGS)
CPPFLAGS += -I$(COMPONENT_TOP_DIR)/api -I$(COMPONENT_TOP_DIR)/src/common -I.
CPPFLAGS += -Wall -g
ifneq ($(BUILDING_FOR_MEEGO),yes)
  ## In Meego libc vscanf has attribute warn_unused_result so do not convert warnings into errors
  CPPFLAGS += -Werror
endif


ifneq ($(PLATFORM),x86-linux)
  CPPFLAGS += -fvisibility=hidden
endif
ifneq ($(MMENV_SYSTEM), android)
  LDSOFLAGS += -pthread -lrt
endif

SRCS := $(shell find . -name "*.c")
LIBOBJ = $(SRCS:.c=)

INSTALL_LIB   = liblos.a

todo: lib

SOLIBNAMES+=los
EXTRALIBS_los = los
INSTALL_SOLIB = los

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

