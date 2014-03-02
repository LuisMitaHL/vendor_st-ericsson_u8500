# vim:syntax=make:
# -*- Mode: Makefile -*-

#WARNING COMPONENT_PATH is MANDATORY for Make.config and Make.rules
#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

#----------------------------

VPATH = .

EXENAME = fileio_nmf_test

ifdef DEBUG
CPPFLAGS += -DDEBUG -D__DEBUG
CFLAGS = -g -O0
LIB_SUFFIX = _debug
else
CPPFLAGS += -DRELEASE -D__RELEASE
LIB_SUFFIX = 
endif

CPPFLAGS += -I $(MM_HEADER_DIR)
CPPFLAGS += -I include

LIBOBJ  = fileio_nmf_test
EXEOBJ  = ${LIBOBJ}
LIBS   += ${MM_IMAGING_NMF_REPO_DIR}/${MYMMBOARD}/fileio_wrp.a
LIBS   += libnmfee.a
LIBS   += libhost_trace.a

todo: exe

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
