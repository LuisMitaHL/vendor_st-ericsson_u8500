#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
MMBOARD=x
MMSYSTEM=x

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

ifneq ($(PLATFORM),x86-linux-old)

all install clean realclean:
	make -f Makelib.mk $@
	make -f Makesolib.mk $@

else
all install clean realclean:
	@echo nothing to do for x86-linux

endif