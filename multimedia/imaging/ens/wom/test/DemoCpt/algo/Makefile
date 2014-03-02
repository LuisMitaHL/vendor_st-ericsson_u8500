# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x
# This will make use of RVCT under Linux and allow to deliver pre-build library
USE_OPT_COMPILER=true

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS += -Iinc
CPPFLAGS += -Iapi
ALGO_FLAGS +=

CPPFLAGS += $(ALGO_FLAGS)

ifdef DEBUG
	DBG_SUF   =_dbg
	CPPFLAGS += -D_DEBUG
	LIBNAME   = ste_womDemoCpt_algo_$(DBG_SUF)
	CFLAGS    =-g -O0
	OBJ_EXT   =odebug
else
	DBG_SUF   =
	CPPFLAGS += -DRELEASE -D__RELEASE
	LIBNAME   = ste_womDemoCpt_algo
endif

LIBOBJ   = womDemoCpt_algo
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 
targets: clean realclean install uninstall lib


#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

print:
	@echo Display variables content for VARS=$(VARS)
	@echo -e '$(foreach VAR, $(VARS),$(VAR)=$($(VAR))\n)'
