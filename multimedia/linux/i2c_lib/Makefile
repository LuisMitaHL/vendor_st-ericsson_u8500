# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedConfig.mk

CPPFLAGS+=  -I./include -I$(KERNEL_BUILD_DIR)/include 

ifeq ($(MMSYSTEM),android)
  ## To find needed headers in case kernel objects are not generated in kernel source tree
  CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/kernel/include
endif

## The library
LIBOBJ      = i2c_lib
LIBNAME     = st_i2c 
INSTALL_LIB = libst_i2c.a
INSTALL_HEADER = i2c_lib.h

## The test executable
#EXENAME     = i2c_test
#EXEOBJ     += i2c_test
LIBS        = libst_i2c.a libst_mmio.a

TOBECLEAN = bin dep obj

#todo: lib exe 
todo: lib  


#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

