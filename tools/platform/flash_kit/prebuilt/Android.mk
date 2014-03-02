#Android makefile to build loaders as a part of Android Build

#LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

.phony: config-prebuilt build-prebuilt install-prebuilt clean-prebuilt distclean-prebuilt

#Source location
PRIVATE_PREBUILT_PREBUILT_DIR := $(abspath $(TOOLS_PATH)/platform/flash_kit/prebuilt)

PRIVATE_PREBUILT_COPS_DIR := $(abspath $(PROCESSING_PATH)/security_framework/cops)
PRIVATE_PREBUILT_COPS_MAKEFILE := ldr.mk

#Output
PRIVATE_PREBUILT_OUT := $(FLASHKIT_INSTALL_BASE)/
PRIVATE_PREBUILT_CONFIG := $(PRIVATE_PREBUILT_OUT)/flashkit

PRIVATE_PREBUILT_FLAGS := PREFIX=$(PRIVATE_PREBUILT_OUT) COPS_DIR=$(PRIVATE_PREBUILT_COPS_DIR) COPS_MAKEFILE=$(PRIVATE_PREBUILT_COPS_MAKEFILE)

config-prebuilt:
	mkdir -p $(PRIVATE_PREBUILT_CONFIG)
	$(MAKE) -C $(PRIVATE_PREBUILT_PREBUILT_DIR) $(PRIVATE_PREBUILT_FLAGS) CONFIG_DIR=$(PRIVATE_PREBUILT_CONFIG) COPS_PLATFORM_TYPE=$(COPS_PLATFORM_TYPE) config

build-prebuilt: config-prebuilt
	$(MAKE) -C $(PRIVATE_PREBUILT_PREBUILT_DIR) CONFIG_DIR=$(PRIVATE_PREBUILT_CONFIG) COPS_PLATFORM_TYPE=$(COPS_PLATFORM_TYPE) build

install-prebuilt: build-prebuilt install-issw
	$(MAKE) -C $(PRIVATE_PREBUILT_PREBUILT_DIR) CONFIG_DIR=$(PRIVATE_PREBUILT_CONFIG)  COPS_PLATFORM_TYPE=$(COPS_PLATFORM_TYPE) install

clean-prebuilt:
	$(MAKE) -C $(PRIVATE_PREBUILT_PREBUILT_DIR) CONFIG_DIR=$(PRIVATE_PREBUILT_CONFIG)  COPS_PLATFORM_TYPE=$(COPS_PLATFORM_TYPE) clean

distclean-prebuilt:
	$(MAKE) -C $(PRIVATE_PREBUILT_PREBUILT_DIR) CONFIG_DIR=$(PRIVATE_PREBUILT_CONFIG)  COPS_PLATFORM_TYPE=$(COPS_PLATFORM_TYPE) distclean

clean clobber: clean-prebuilt


st-ericsson-flashkit: config-prebuilt build-prebuilt install-prebuilt
