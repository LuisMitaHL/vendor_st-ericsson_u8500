#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

MMSYSTEM=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

INSTALL_HEADERDIR= inc
CPPFLAGS+= -Iinc
CPPFLAGS+= -I$(MMROOT)/shared/utils/include
# We are still using the RME Get_DefaultDDRDomain deprecated api
CPPFLAGS+= -DDEPRECATED_API
VPATH+= src

LIBOBJ = \
	ENS_Wrapper \
	ENS_Wrapper_Init \
	ENS_Wrapper_NMF \
	ENS_Wrapper_OMX_UserCB \
	ENS_Queue \
	ENS_Wrapper_Services \
	ENS_HwRm \
	ENS_RME_Wrapper \
	ENS_Wrapper_OSTtrace \
	ENS_Wrapper_Construct \
	ENS_Wrapper_CtxTrace \
	ENS_Wrapper_DataDump \
	ENS_Wrapper_Dump

ifeq ($(MYMMSYSTEM),android)
CPPFLAGS += -I$(ANDROID_BSP_ROOT)/frameworks/base/include \
            -I$(ANDROID_BSP_ROOT)/hardware/libhardware/include \
            -I$(ANDROID_BSP_ROOT)/frameworks/native/include \
            -I$(ANDROID_BUILD_TOP)/frameworks/native/include/media/hardware
endif

ifeq ($(MMCOMPILER),android)
  LIBOBJ += ENS_WrapperDumpMemInfo mapinfo
endif

LIBNAME  = st_enswrapper
INSTALL_LIB = libst_enswrapper.a

todo: lib

test:
	@echo "No Test Available for enswrapper component"

include $(MM_MAKEFILES_DIR)/SharedRules.mk

