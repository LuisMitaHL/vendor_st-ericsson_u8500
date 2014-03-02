# -*- Mode: Makefile -*-
# vim:syntax=make:

# Note: CORE macro must be placed before Make.nmf.config
CORE=arm

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

ifdef DEBUG
CPPFLAGS += -DDEBUG -D__DEBUG
endif

CPPFLAGS += -I $(MM_HEADER_DIR)

SHAREPATH += share
CXXFLAGS  += -g -O0
COMPONENTS = asyncflash_wrp

INSTALL_COMPONENTS = asyncflash

INSTALL_NMF_COMP_SRC=true
MMINSTALL_ARGS += -n=/ee/api/trace.hpp
MMINSTALL_ARGS += -n=/ee/api/panic.idt.h

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

ifeq ($(findstring symbian,$(PLATFORM)),symbian)
NO_VARIANT = true
include ${MEDIUM_DIR}/symbian_makerules/mmh.mk
endif
