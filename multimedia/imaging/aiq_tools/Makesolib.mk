#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
MMBOARD=x
MMSYSTEM=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

SOLIBNAMES = ste_aiq_tools
IMAGE_LIBS = ste_aiq_tools st_aiq_common

ifneq ($(MMCOMPILER),android)
FLAGS_ste_aiq_tools += -lrt
endif


EXTRALIBS_$(SOLIBNAMES) = $(IMAGE_LIBS)

INSTALL_SOLIB = $(SOLIBNAMES)

todo: solib

include $(MM_MAKEFILES_DIR)/SharedRules.mk

