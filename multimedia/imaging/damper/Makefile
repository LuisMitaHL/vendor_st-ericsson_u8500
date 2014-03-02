# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include make.ost_conf
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

LIBNAME     = st_damper
INSTALL_LIB = libst_damper.a

ifdef DEBUG
CPPFLAGS += -DOSI_DEBUG -D__DEBUG
CFLAGS    = -g -O0
OBJ_EXT   = odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
endif

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarDamper -DCONFIG_DEBUG_AUTOVAR

CPPFLAGS+= -I$(OST_COMPONENT_INC)/autogentraces

LIBOBJ          = damper_func
LIBOBJ         += damper_base
LIBOBJ         += damper_shared

INSTALL_HEADER  = damper_func.h
INSTALL_HEADER += damper_base.h
INSTALL_HEADER += damper_shared.h


#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
ifeq ($(MMSYSTEM),symbian) 
NO_VARIANT=true
include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

