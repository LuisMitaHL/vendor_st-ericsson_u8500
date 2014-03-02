#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

FILES_TO_INSTALL := tracecompiler.pl
DIRS_TO_INSTALL  := XML

TO_INSTALL := $(addprefix $(INSTALL_TOOLS_DIR)/$(TRACECOMPILER_DIR_NAME)/, $(FILES_TO_INSTALL) $(DIRS_TO_INSTALL))

all install: $(TO_INSTALL)

$(INSTALL_TOOLS_DIR)/$(TRACECOMPILER_DIR_NAME)/%: %
	if [ -d $< ] ; then $(MMINSTALL) -d $< $@ ; else $(MMINSTALL) $< $@ ; fi

clean realclean :
	$(MAKE) -C test realclean 

uninstall:
	$(RM) $(TO_INSTALL)
	$(MAKE) -C test uninstall 

test:
	$(MAKE) -C test $@ 
	$(MAKE) -C test install

.PHONY: test


