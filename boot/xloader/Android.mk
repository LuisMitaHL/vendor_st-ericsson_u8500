PRIVATE_PATH:= $(call my-dir)

include $(CLEAR_VARS)



.phony: build-xloader install-xloader clean-xloader distclean-xloader

#Source location
PRIVATE_XLOADER_DIR := $(abspath $(BOOT_PATH)/xloader/build)
PRIVATE_OUT_DIR := $(FLASHKIT_INSTALL_BASE)

build-xloader:
	@echo "PRIVATE_PATH " $(PRIVATE_PATH)
	$(MAKE) -C $(PRIVATE_PATH)/build -f Makefile CROSS_COMPILE=$(CROSS_COMPILE) STE_PLATFORM=$(XLOADER_SET_PLATFORM) PRIVATE_OUT_DIR=$(PRIVATE_OUT_DIR)

install-xloader: build-xloader
	$(MAKE) -C $(PRIVATE_XLOADER_DIR) -f Makefile install PRIVATE_OUT_DIR=$(PRIVATE_OUT_DIR) STE_PLATFORM=$(XLOADER_SET_PLATFORM)

clean-xloader:
	$(MAKE) -C $(PRIVATE_XLOADER_DIR) -f Makefile clean STE_PLATFORM=$(XLOADER_SET_PLATFORM) PRIVATE_OUT_DIR=$(PRIVATE_OUT_DIR)

distclean-xloader: clean-xloader

clean clobber: clean-xloader

st-ericsson-flashkit: build-xloader install-xloader
