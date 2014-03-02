# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS+= -I./include
CPPFLAGS+= -I../../../imaging/include/ -I../common
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/kernel/include

## static library
LIBNAME     = camera_flash
LIBOBJ      = camera_flash
INSTALL_LIB = libcamera_flash.a

## shared library
SOLIBNAMES = camera_flash
EXTRALIBS_$(SOLIBNAMES) = $(LIBNAME)
INSTALL_SOLIB = $(SOLIBNAMES)

## The test executable
EXENAME     = flash_test
INSTALL_BIN = ${EXENAME}
EXEOBJ     += flash_test
EXE_LDFLAGS += -lcamera_flash

TOBECLEAN = bin dep obj lib

todo: lib solib exe

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
