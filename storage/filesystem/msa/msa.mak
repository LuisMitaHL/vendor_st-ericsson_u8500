#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2012 ST-Ericsson                                      *
# *                                                                      *
# *  Author: Axel Lilliecrantz <axel.lilliecrantz AT stericsson.com>     *
# *                                                                      *
# ************************************************************************/

PACKAGE_NAME=msa

build:
	make -f Makefile msa

install: build
	@$(PACKAGE_FILE) /usr/bin/msa $(CURDIR)/msa 755 0 0
	# Package MSA tools startup script
	@$(PACKAGE_FILE) /etc/init.d/msa $(CURDIR)/rc.msa 755 0 0
	@$(PACKAGE_LINK) /etc/rc.d/S52_msa ../init.d/msa 755 0 0

clean:
	make -f Makefile clean
