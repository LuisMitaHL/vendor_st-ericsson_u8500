# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk
LDFLAGS+= -lomxil-bellagio

CPPFLAGS+= -g -O0

## The executable
EXENAME     = nvm_app
INSTALL_BIN = ${EXENAME}
EXEOBJ     += nvm_app
EXEOBJ     += stecc

TOBECLEAN = bin dep obj

todo: exe

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
#include $(MM_MAKEFILES_DIR)/SharedDispatch.mk