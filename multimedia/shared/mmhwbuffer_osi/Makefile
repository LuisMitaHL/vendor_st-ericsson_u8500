#
# Copyright (C) ST-Ericsson SA 2010 All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

MMSYSTEM=x
MMBOARD=x

## Get the OMX headers directly from omxil and not from the ENS ones
CPPFLAGS += -I$(MM_HEADER_DIR)/omxil

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

LIBOBJ       = MMHwBufferLinux \
               MMHwBufferPool \
               MMHwBufferList \
               MMNativeBuffer \
               mmhwbuffer_c_wrapper

INSTALL_LIB  = libst_mmhwbuffer.a
LIBNAME      = st_mmhwbuffer

ifeq ($(MYMMSYSTEM),android)
CPPFLAGS += -I$(ANDROID_BUILD_TOP)/frameworks/base/include \
            -I$(ANDROID_BUILD_TOP)/frameworks/native/include \
            -I$(ANDROID_BUILD_TOP)/frameworks/native/include/media/hardware \
            -I$(ANDROID_BUILD_TOP)/hardware/libhardware/include \
            -I$(GRALLOC_PATH)
endif # MYMMSYSTEM = android

INSTALL_HEADER = mmhwbuffer_ext.h MMNativeBuffer.h

todo: lib

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk
