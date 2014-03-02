#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

## We may not use makefiles as it depends on us

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

TO_INSTALL := $(addprefix $(INSTALL_HEADER_DIR)/, ste_ost_group_map.h)

VPATH = include

all install: $(TO_INSTALL)

$(INSTALL_HEADER_DIR)/%: %
	$(MMINSTALL) $< $@

clean realclean :

uninstall:
	$(RM) $(TO_INSTALL)

