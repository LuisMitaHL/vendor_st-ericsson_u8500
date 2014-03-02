# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS+= -I../../../imaging/include/

## static library
LIBNAME     = flash_current_calculation
LIBOBJ      = Flashcclib_stub
INSTALL_LIB = libflash_current_calculation.a

## shared library
SOLIBNAMES = flash_current_calculation
EXTRALIBS_$(SOLIBNAMES) = $(LIBNAME)
INSTALL_SOLIB = $(SOLIBNAMES)

TOBECLEAN = dep obj lib bin

todo: lib solib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
