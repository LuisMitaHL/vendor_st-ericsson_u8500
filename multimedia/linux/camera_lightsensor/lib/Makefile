# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS+= -I../../../imaging/include/

## static library
LIBNAME     = als
LIBOBJ      = Lightsensorlib_stub
INSTALL_LIB = libals.a

## shared library
SOLIBNAMES = als
EXTRALIBS_$(SOLIBNAMES) = $(LIBNAME)
INSTALL_SOLIB = $(SOLIBNAMES)

TOBECLEAN = dep obj lib bin

todo: lib solib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
