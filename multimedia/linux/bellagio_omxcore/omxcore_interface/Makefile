#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

# Get Bellagio core OMX headers
CPPFLAGS+= -I../libomxil-bellagio/include

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

BUILD_WITHOUT_PV ?= false

ifeq ($(MMCOMPILER),android)
  ROOT_INSTALL_PATH:=/system/lib
else
  ROOT_INSTALL_PATH:=/usr/lib
endif

INSTALL_SOLIB_DIR := $(INSTALL_ROOTFS_DIR)$(ROOT_INSTALL_PATH)

SOLIBNAMES+=ste_omxil-interface
LIBOBJ_ste_omxil-interface = ste_omxil_interface
FLAGS_ste_omxil-interface = -ldl
INSTALL_SOLIB += ste_omxil-interface

todo: solib

EXEOBJ = ste_omxil_test 
EXENAME = ste_omxil_test 
INSTALL_BIN = ste_omxil_test 
EXE_LDFLAGS += -L$(LIB_DIR) -lste_omxil-interface -ldl  

INSTALL_HEADERDIR = ./

todo: exe

ifeq ($(MMCOMPILER),android)
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/frameworks/native/include
ifneq ($(BUILD_WITHOUT_PV),true)

PVTOP = $(ANDROID_BSP_ROOT)/external/opencore

CPPFLAGS+=$(PV_CFLAGS)
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/external/opencore/extern_libs_v2/khronos/openmax/include
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/external/opencore/oscl/oscl/config/shared
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/external/opencore/build_config/opencore_dynamic
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/system/core/include
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/external/opencore/oscl/oscl/osclbase/src
CPPFLAGS+= -I$(ANDROID_OUT_TARGET_PRODUCT_DIRECTORY)/obj/include/libpv
CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/external/opencore/oscl/oscl/config/linux

SOLIBNAMES+=ste_pv_omxil-bellagio
LIBOBJ_ste_pv_omxil-bellagio = ste_pv_omxil_bellagio
FLAGS_ste_pv_omxil-bellagio = -ldl -L$(LIB_DIR) -lste_omxil-interface
INSTALL_SOLIB += ste_pv_omxil-bellagio

todo: solib

OMXCORE_STE_CFG = omxcore_ste.cfg

installcfg:
	$(MMINSTALL) $(OMXCORE_STE_CFG) $(INSTALL_ROOTFS_DIR)/system/etc/$(OMXCORE_STE_CFG)

todo: installcfg

endif #ifneq ($(BUILD_WITHOUT_PV),false)
endif #ifeq ($(MMCOMPILER),android)

include $(MM_MAKEFILES_DIR)/SharedRules.mk

