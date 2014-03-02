# vim:syntax=make:
# -*- Mode: Makefile -*-

#-------------------------------
# Default Config
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.config

CPPFLAGS+= -I$(MMROOT)/imaging/include -I../common -I$(MMROOT)/shared/omxil
CPPFLAGS+= -I./include
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/kernel/include
CPPFLAGS += -DCONFIG_DEBUG -DCONFIG_DEBUG_AUTOVAR -DDBGT_VAR=mDebugTraceVarMMIOCamera

ifeq ($(MMSYSTEM),linux)
## Below to get KERNEL_BUILD_DIR defined in MMD environment
-include $(MMROOT)/linux/kernel/Make.config

CPPFLAGS+= -I$(KERNEL_BUILD_DIR)/include/../../kernel/include
endif

#Uncomment below line to enable PMIC sequence operation
#CPPFLAGS+= -IENABLE_PMIC

ifeq ($(MMIO_ENABLE_NEW_IF),true)
  CPPFLAGS += -DMMIO_ENABLE_NEW_IF
endif

## The library
LIBOBJ      = MMIO_Camera
LIBNAME     = st_mmio
INSTALL_LIB = libst_mmio.a
INSTALL_HEADER = mmio_linux_api.h

## The test executable
EXENAME     = MMIO_Test
EXEOBJ     += MMIO_Test
LIBS        = libst_i2c.a libst_mmio.a

LDFLAGS+= -lstelpcutils

ifeq ($(MMSYSTEM),android)
      EXE_LDFLAGS += -L$(ANDROID_OUT_TARGET_PRODUCT_DIRECTORY)/obj/lib
      LDFLAGS+= -lcutils 
endif

TOBECLEAN = bin dep obj

todo: lib exe

#-------------------------------
# Generic Rules
#-------------------------------
include $(MMROOT)/shared/makefiles/Make.rules
