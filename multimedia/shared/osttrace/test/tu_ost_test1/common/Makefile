# vim:syntax=make:
# -*- Mode: Makefile -*-
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include make.ost_conf

include $(MM_MAKEFILES_DIR)/SharedCheck.mk

all clean realclean install uninstall debug:
	$(MAKE) -C nmf $@
	$(MAKE) -C host $@ 
#	$(MAKE) -C inc $@












