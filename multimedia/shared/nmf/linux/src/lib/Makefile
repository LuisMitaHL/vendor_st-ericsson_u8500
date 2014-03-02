# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.

SUBDIRS = CMUserProxy los hostee

.PHONY: $(SUBDIRS)

%:
ifeq ($(findstring x86,$(PLATFORM)),)
  ifeq ($(findstring 5500,$(PLATFORM)),)
	$(MAKE) -C CMUserProxy $*
  endif
endif
	$(MAKE) -C los $*
	$(MAKE) -C hostee $*
	$(MAKE) -C hosteeso $*
