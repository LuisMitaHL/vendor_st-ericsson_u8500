# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
MMSYSTEM=x
MMBOARD=x
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

BUILD_INCLUDES = -I ./
BUILD_INCLUDES+= -I ./inc

CPPFLAGS += $(BUILD_INCLUDES)

# in order to enable memory tracer & performance tracer; By default, disabled
# CPPFLAGS += -DAIQ_MEMTRACE_ENABLE -DAIQ_PERFTRACE_ENABLE

ifdef DEBUG
	CPPFLAGS += -DDEBUG -D__DEBUG
	LIBNAME   = st_aiq_common
	SOLIBNAME   = st_aiq_common
	CFLAGS=-g -O0
	OBJ_EXT =odebug
else
	CPPFLAGS += -DRELEASE -D__RELEASE
	LIBNAME   = st_aiq_common
	SOLIBNAME   = st_aiq_common
endif

LIBOBJ   = AIQ_Memory AIQ_PerfTracer
SOLIBOBj = $(LIBOBJ)

INSTALL_HEADER = AIQ_Memory.h AIQ_PerfTracer.h AIQ_CommonTypes.h 
INSTALL_LIB    = $(addprefix lib, $(addsuffix .a, $(LIBNAME)))

#WARNING todo target is MANDATORY (exe and/or lib)
todo: lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#ifeq ($(MMSYSTEM),symbian) 
#NO_VARIANT=true
#include $(MEDIUM_DIR)/symbian_makerules/mmh.mk
#endif
