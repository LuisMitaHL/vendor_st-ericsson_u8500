#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

top: all

help:
	@echo "Known targets are: all, clean, realclean, install, uninstall"

ifneq ($(RAMDISK_COPY_FILES_TO_ANDROID),true)
-include Utils.mk
endif

MMENV_SYSTEM=$(shell echo $(PLATFORM) | cut -d - -f 2)

all: $(ALL_UTILS)
	$(MAKE) -f Ramdisk.mk $@

clean: $(CLEAN_UTILS)
	$(MAKE) -f Ramdisk.mk $@

realclean: clean
	$(MAKE) -f Ramdisk.mk $@

#Nothing to do
install:

#Nothing to do
uninstall:

%_rootfs_dir: $(ALL_UTILS)
	$(MAKE) -f Ramdisk.mk $@

rootfsdir: $(ALL_UTILS)
	$(MAKE) -f Ramdisk.mk $@

mmvalid: $(ALL_UTILS)
	$(MAKE) -f Ramdisk.mk mmvalid

p-%: $(ALL_UTILS)
	$(MAKE) -f Ramdisk.mk $@
