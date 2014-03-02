PRIVATE_PATH:= $(call my-dir)
include $(CLEAR_VARS)

all_modules:build-xloader
.phony: build-xloader install-xloader clean-xloader

PRIVATE_XLOADER_DIR := $(abspath $(BOOT_PATH)/xloader)
PRIVATE_OUT_DIR := $(FLASHKIT_INSTALL_BASE)

build-xloader:
	$(MAKE) -C $(PRIVATE_XLOADER_DIR) -f Makefile PRIVATE_OUT_DIR=$(PRIVATE_OUT_DIR) STE_PLATFORM=$(XLOADER_SET_PLATFORM) build

install-xloader:build-xloader
	$(MAKE) -C $(PRIVATE_XLOADER_DIR) -f Makefile PRIVATE_OUT_DIR=$(PRIVATE_OUT_DIR) STE_PLATFORM=$(XLOADER_SET_PLATFORM) install

clean-xloader:
	$(MAKE) -C $(PRIVATE_XLOADER_DIR) -f Makefile PRIVATE_OUT_DIR=$(PRIVATE_OUT_DIR) STE_PLATFORM=$(XLOADER_SET_PLATFORM) clean

st-ericsson-flashkit: build-xloader install-xloader clean-xloader
