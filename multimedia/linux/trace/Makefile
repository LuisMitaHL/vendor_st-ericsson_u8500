#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifeq ($(strip $(PLATFORM)),)
  $(error Specify which platform to use)
endif

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

all: x_all x_install

install: x_install

uninstall: x_uninstall

clean: x_clean

realclean: x_clean
	rm -rf lib src/lib/obj
	find . -name "*~" | xargs rm -rf

x_%:
	$(MAKE) -C src $*
