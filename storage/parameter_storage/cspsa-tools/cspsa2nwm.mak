#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2010 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released either under the terms of the BSD-style   *
# *  license accompanying CSPSA or a proprietary license obtained from   *
# *  ST-Ericsson SA.                                                     *
# *                                                                      *
# *  Author: Martin LUNDHOLM <martin.xa.lundholm AT stericsson.com>      *
# *                                                                      *
# ************************************************************************/

ifeq ($(O),)
	BUILDDIR := $(CURDIR)/
else
	BUILDDIR := $(O)/
endif

#Common flags
#-fno-short-enums is needed by Loaders
export CFLAGS = -std=gnu99 -fno-short-enums -DCFG_CSPSA_LBP=1 -O3 -ffunction-sections -fdata-sections
export V
export O

#This is the first target, it will be execute if no target is given to 'make'.
.PHONY: all
all:
	$(MAKE) b-cspsa2nwm

.PHONY: install
install: all
	@$(PACKAGE_FILE) /usr/bin/cspsa2nwm $(BUILDDIR)cspsa2nwm 755 0 0
	# Package CSPSA tools startup script
	@$(PACKAGE_FILE) /etc/init.d/cspsa-tools $(CURDIR)/rc.cspsa-tools 755 0 0
	@$(PACKAGE_LINK) /etc/rc.d/S51_cspsa-tools ../init.d/cspsa-tools 755 0 0

.PHONY: clean
clean:
	$(MAKE) clean

