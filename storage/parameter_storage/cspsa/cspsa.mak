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

# Install target for LBP
# ================
ifeq ($(O),)
	BUILDDIR := $(CURDIR)/
else
	BUILDDIR := $(O)/
endif
.PHONY: install
install:

	$(MAKE) O=$(O) libs exe test
	@$(PACKAGE_FILE) /usr/bin/cspsa-server $(BUILDDIR)cspsa-server 755 0 0
	@$(PACKAGE_FILE) /usr/bin/cspsa-cmd $(BUILDDIR)cspsa-cmd 755 0 0
	@$(PACKAGE_FILE) /usr/bin/cspsa-test-reader $(BUILDDIR)cspsa-test-reader 755 0 0
	@$(PACKAGE_FILE) /usr/bin/cspsa-test-writer $(BUILDDIR)cspsa-test-writer 755 0 0
	@$(PACKAGE_FILE) /usr/lib/libcspsa.so.1.0 $(BUILDDIR)libcspsa.so.1.0 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libcspsa.so.1 libcspsa.so.1.0 777 0 0
	@$(PACKAGE_FILE) /etc/cspsa.conf $(CURDIR)/config/cspsa.lbp.conf 755 0 0
	# Package CSPSA server startup script
	@$(PACKAGE_FILE) /etc/init.d/cspsa $(CURDIR)/rc.cspsa 755 0 0
	@$(PACKAGE_LINK) /etc/rc.d/S50_cspsa ../init.d/cspsa 755 0 0

clean:
	$(MAKE) clean

