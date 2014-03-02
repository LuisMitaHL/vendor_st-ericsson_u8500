# -*- Mode: Makefile -*-
# vim:syntax=make:
MMSYSTEM=x
MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

MMVALIDROOT = usr/share/mm-valid


ifneq ($(MYMMSYSTEM),android)
# Prepare all tscripts to be copied in the Linux ramdisk
install:
	$(MMINSTALL) -d . $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/splitter/mmte_script

clean: realclean
realclean:
	$(MMINSTALL) -r -d . $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/splitter/mmte_script	
else
install:
clean:
realclean:	
endif	
	
todo: install
all: install

