# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.config
CPPFLAGS+= -I./include -I../common
CPPFLAGS+= -g -O0

## The library
LIBOBJ      = appReadHwReg
LIBNAME     = ste_appReadHwReg
INSTALL_LIB = libste_appReadHwReg.a

## The test executable
EXENAME     = appReadHwReg
LIBS        = libste_appReadHwReg.a
INSTALL_BIN = $(EXENAME)

TOBECLEAN = dep obj exe

todo: lib exe 


#-------------------------------
# Generic Rules
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.rules
