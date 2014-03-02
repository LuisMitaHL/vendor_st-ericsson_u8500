#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2011 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released either under the terms of the BSD-style   *
# *  license accompanying CSPSA or a proprietary license obtained from   *
# *  ST-Ericsson SA.                                                     *
# *                                                                      *
# *  Author: Hans Holmberg <hans.xh.holmberg AT stericsson.com>      *
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

#This is the first target, it will be executed if no target is given to 'make'.
.PHONY: all
all:
	$(MAKE) b-cspsa2map b-map2cspsa b-map2q b-q2map

# Install target for LBP
# ================
.PHONY: install
install: all
	@$(PACKAGE_FILE) /usr/bin/cspsa2map $(BUILDDIR)cspsa2map 755 0 0
	@$(PACKAGE_FILE) /usr/bin/map2cspsa $(BUILDDIR)map2cspsa 755 0 0
	@$(PACKAGE_FILE) /usr/bin/map2q $(BUILDDIR)map2q 755 0 0
	@$(PACKAGE_FILE) /usr/bin/q2map $(BUILDDIR)q2map 755 0 0
	# Package CSPSA tools startup script
	@$(PACKAGE_FILE) /etc/init.d/cspsa-tools-cspsa2qstore $(CURDIR)/rc.cspsa-tools-cspsa2qstore 755 0 0
	@$(PACKAGE_LINK) /etc/rc.d/S51_cspsa-tools-cspsa2qstore ../init.d/rc.cspsa-tools-cspsa2qstore 755 0 0

clean:
	$(MAKE) clean
