# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

LIBNAME     = st_tuning
INSTALL_LIB = libst_tuning.a

# CR337836 Activate dynamic trace
CPPFLAGS += -DCONFIG_DEBUG -DDBGT_VAR=mDebugTraceVarTuning -DCONFIG_DEBUG_AUTOVAR

ifdef TRACE_FUNC
CPPFLAGS += -DTUNING_TRACE_FUNCTION
endif

ifdef TRACE_INFO
CPPFLAGS += -DTUNING_TRACE_INFO
endif

ifdef TRACE_ERROR
CPPFLAGS += -DTUNING_TRACE_ERROR
endif

ifdef TRACE_PARAM
CPPFLAGS += -DTUNING_TRACE_PARAM
endif

ifdef PARAM_NAMES
CPPFLAGS += -DTUNING_PARAM_NAMES_ENABLED
endif

ifdef TRACE_ALL
CPPFLAGS += -DTUNING_TRACE_ALL
endif

ifdef DEBUG
CPPFLAGS += -DOSI_DEBUG -D__DEBUG
CFLAGS    = -g -O0
OBJ_EXT   = odebug 
else
CPPFLAGS += -DRELEASE -D__RELEASE
endif

LIBOBJ  = tuning
LIBOBJ += tuning_bin
LIBOBJ += tuning_iqset
LIBOBJ += tuning_osal
LIBOBJ += tuning_parser
LIBOBJ += tuning_user
LIBOBJ += tuning_writer

INSTALL_HEADER  = tuning.h
INSTALL_HEADER += tuning_types.h
INSTALL_HEADER += tuning_osal.h
INSTALL_HEADER += tuning_parser.h
INSTALL_HEADER += tuning_bin.h
INSTALL_HEADER += tuning_iqset.h
INSTALL_HEADER += tuning_user.h
INSTALL_HEADER += tuning_writer.h
INSTALL_HEADER += tuning_macros.h
INSTALL_HEADER += tuning_params.h


#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib 


#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
ifeq ($(findstring symbian,$(PLATFORM)),symbian)
 NO_VARIANT=true
 include ${MMROOT}/imaging/symbian_makerules/mmh.mk
endif

