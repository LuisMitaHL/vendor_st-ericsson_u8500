# -*- Mode: Makefile -*-
# vim:syntax=make:

# Note: CORE macro must be placed before Make.nmf.config
CORE=arm

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

ifdef DEBUG
CPPFLAGS += -DDEBUG -D__DEBUG
endif

SHAREPATH += share
CXXFLAGS  += -g -O0
COMPONENTS = fileio_wrp

INSTALL_COMPONENTS = fileio_wrp \
		     fileio

## Request sources to be installed 
INSTALL_NMF_COMP_SRC=true
MMINSTALL_ARGS += -n=/ee/api/trace.hpp \
                  -n=/ee/api/panic.idt.h

## Install share directory content
INSTALL_NMF_COMP_SRC_HOSTEE = share/fileio_types.idt 

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

ifeq ($(findstring symbian,$(PLATFORM)),symbian)
NO_VARIANT = true
include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif
