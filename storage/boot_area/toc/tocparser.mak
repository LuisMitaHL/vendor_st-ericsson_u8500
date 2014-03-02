#/************************************************************************
# *                                                                      *
# *  Copyright (C) 2011 ST-Ericsson SA                                   *
# *                                                                      *
# *  This software is released under the terms of the BSD-style          *
# *  license accompanying TOC.                                           *
# *                                                                      *
# *  Author: Mikael Gullberg <mikael.xx.gullberg AT stericssom.com >     *
# *                                                                      *
# ************************************************************************/

.PHONY: install clean

install:
	$(MAKE) all
	@$(PACKAGE_FILE) /usr/bin/tocparser $(CURDIR)/tocparser 755 0 0
clean:
	$(MAKE) clean

