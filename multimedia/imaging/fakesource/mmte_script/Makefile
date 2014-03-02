# -*- Mode: Makefile -*-
# vim:syntax=make:

MMSYSTEM=x
MMBOARD=x

include $(MM_MAKEFILES_DIR)/SharedConfig.mk

RAMDISK_DIR = $(INSTALL_ROOTFS_DIR)
MMVALIDROOT = usr/share/mm-valid

ifneq ($(MYMMSYSTEM),android)
# Prepare all scripts to be copied in the Linux ramdisk
install:
	$(MMINSTALL) -d . $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/fakesource/mmte_script 

uninstall:
	$(MMINSTALL) -r -d . $(INSTALL_ROOTFS_DIR)/$(MMVALIDROOT)/imaging/fakesource/mmte_script 
else
install:
uninstall:
endif

todo: install
all: install
clean realclean : uninstall 
