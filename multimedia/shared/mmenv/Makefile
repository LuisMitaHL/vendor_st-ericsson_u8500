#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include SharedMMDSPFlags.mk
include SharedDefines.mk

## The files we'll install
MK_FILES := $(addprefix $(INSTALL_MAKEFILES_DIR)/, $(wildcard *.mk) mminstall ste-mm-mkdefines.sh ste-mm-spec-files.pl)

## As mminstall is not yet installed we need to fix MMINSTALL
MMINSTALL:=./mminstall -c $(MMINSTALL_ARGS)

FILES_TO_INSTALL := tools.sh

TO_INSTALL := $(addprefix $(INSTALL_TOOLS_DIR)/, $(FILES_TO_INSTALL))

.PHONY: all install uninstall clean realclean test

all install clean realclean test: $(MK_FILES) $(TO_INSTALL)

uninstall:
	$(RM) $(MK_FILES)
	$(RM) $(TO_INSTALL)


$(INSTALL_MAKEFILES_DIR)/%: %
	@echo "  MMINSTALL $(REL_BUILD_INTERMEDIATES_DIR)/$<"
	$(VERCHAR)$(MMINSTALL) $< $@

$(INSTALL_TOOLS_DIR)/%: %
	$(MMINSTALL) $< $@



