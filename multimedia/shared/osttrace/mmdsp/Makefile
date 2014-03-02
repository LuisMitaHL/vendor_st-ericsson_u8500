# vim:syntax=make:
# -*- Mode: Makefile -*-
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifndef MMSYSTEM
MMSYSTEM=x
endif
ifndef MMBOARD
MMBOARD=x
endif

CORE=mmdsp

include $(MM_MAKEFILES_DIR)/SharedNmfConfig.mk

CFLAGS += -DEXECUTION_CORE=1 -I$(MM_HEADER_DIR) -I$(COMPONENT_TOP_DIR)/include

COMPONENTS = osttrace.mmdsp

## Request installation of components definition as needed by other components
INSTALL_COMPONENTS = osttrace.mmdsp
## Request src installation for the components
INSTALL_NMF_COMP_SRC=true

## Below line requires that osttrace component is always extracted/installed
## in a directory named osttrace
SRCPATH += $(COMPONENT_TOP_DIR)/..
SRCPATH += $(MM_HEADER_DIR)
SRCPATH += $(COMPONENT_TOP_DIR)/include
CFLAGS += -g

include $(MM_MAKEFILES_DIR)/SharedNmfRules.mk

