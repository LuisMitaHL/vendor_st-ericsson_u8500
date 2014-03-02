#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

## Use the STD OMX headers
CPPFLAGS+= -I$(MM_HEADER_DIR)/omxil

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

ifeq ($(MMCOMPILER),android)
  CPPFLAGS+= -I$(BASS_APP_INCLUDE_PATH)
  CPPFLAGS+= -DDBGT_PREFIX=\"OSAL\" -DDBGT_CONFIG_DEBUG -DDBGT_CONFIG_AUTOVAR -DDBGT_TAG=\"omxilosal\"
endif

ifeq ($(MMENV_SYSTEM),linux)
  CPPFLAGS+= -DDBGT_PREFIX=\"OSAL\" -DDBGT_CONFIG_DEBUG -DDBGT_CONFIG_AUTOVAR -DDBGT_TAG=\"omxilosal\"
  CPPFLAGS+= -I$(ANDROID_BSP_ROOT)/system/core/include
endif
LIBOBJ= omxilosalmutex \
	omxilosaltimerimpl \
	omxilosaldatetime \
	omxilosalprm \
	omxilosalproductid \
	omxilosalproperty

LIBNAME        = st_omxilosalservices
INSTALL_LIB    = libst_omxilosalservices.a 

todo: lib 

#-------------------------------
# Generic Rules
#-------------------------------
include $(MM_MAKEFILES_DIR)/SharedRules.mk

#-------------------------------
# local Rules
#-------------------------------

test cleantest:
	@echo No Test...
