# -*- Mode: Makefile -*-
# vim:syntax=make:

# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedCheck.mk

ifeq ($(CORE_NUMBER),2)
DIRECTORIES=mmdsp arm
else
DIRECTORIES=arm
endif

include $(MM_MAKEFILES_DIR)/SharedDispatch.mk
