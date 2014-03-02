#Android makefile to build preflash tool as a part of Android Build

#LOCAL_PATH := $(my-dir)
include $(CLEAR_VARS)

.phony: clean-preflash_tool config-preflash_tool build-preflash_tool install-preflash_tool distclean-preflash_tool clobber

#Source location
PRIVATE_PREFLASH_TOOLS_PATH := $(abspath $(TOOLS_PATH))
PRIVATE_PREFLASH_PROCESSING_PATH := $(abspath $(PROCESSING_PATH))
PRIVATE_PREFLASH_TOOL_DIR := $(PRIVATE_PREFLASH_TOOLS_PATH)/platform/flash_kit/preflash_tool
PRIVATE_PREFLASH_STORAGE_PATH := $(abspath $(STORAGE_PATH))

#Output
#FLASHKIT_INSTALL_BASE defined in vendor/st-ericsson/u8500/BoardConfig.mk
PRIVATE_PREFLASH_TOOL_INSTALLDIR := $(FLASHKIT_INSTALL_BASE)/preflash_tool

#Locations and flags for modules pre-flash tool depend on
PRIVATE_LOADERS_AUTO_DIR := $(abspath $(TOP))/$(TARGET_OUT_INTERMEDIATES)/FLASHKIT/LOADERS/loader_communication_module
PRIVATE_LOADERS_SOURCE := $(PRIVATE_PREFLASH_TOOLS_PATH)/platform/flash_kit/loaders/src
PRIVATE_LOADERS_DIR := $(PRIVATE_PREFLASH_TOOLS_PATH)/platform/flash_kit/loaders
PRIVATE_LOADERS_CB := $(PRIVATE_PREFLASH_TOOLS_PATH)/platform/flash_kit/loaders/src/loader_code_base
PRIVATE_CSPSA_DIR := $(PRIVATE_PREFLASH_STORAGE_PATH)/parameter_storage/cspsa
PRIVATE_LCM_DIR := $(PRIVATE_PREFLASH_TOOLS_PATH)/platform/flash_kit/loader_communication/lcmodule

PRIVATE_PREFLASH_TOOL_OUT_DIR = $(abspath $(TOP))/$(TARGET_OUT_INTERMEDIATES)/FLASHKIT/PREFLASH_TOOL
PRIVATE_LC_LCM_OUT_DIR = $(abspath $(TOP))/$(TARGET_OUT_INTERMEDIATES)/FLASHKIT/LCM

PRIVATE_PREFLASH_TOOL_FLAGS := PREFLASH_TOOL_INSTALLDIR=$(PRIVATE_PREFLASH_TOOL_INSTALLDIR)\
			 LDR_DIR=$(PRIVATE_LOADERS_DIR)\
			 CSPSA_DIR=$(PRIVATE_CSPSA_DIR)\
			 LCM_DIR=$(PRIVATE_LCM_DIR)\
			 PRIVATE_LC_LCM_OUT_DIR_AUTO=$(PRIVATE_LC_LCM_OUT_DIR)/out/autogen\
			 PRFL_DIR=$(PRIVATE_PREFLASH_TOOL_DIR)

config-preflash_tool:
	mkdir -p $(PRIVATE_PREFLASH_TOOL_OUT_DIR)
	PATH=$(PATH) $(MAKE) -C $(PRIVATE_PREFLASH_TOOL_DIR) -f Makefile $(PRIVATE_PREFLASH_TOOL_FLAGS) CONFIG_DIR=$(PRIVATE_PREFLASH_TOOL_OUT_DIR) BUILDFOLDER=$(PRIVATE_PREFLASH_TOOL_OUT_DIR) config

build-preflash_tool: config-preflash_tool build-lcm
	PATH=$(PATH) $(MAKE) -C $(PRIVATE_PREFLASH_TOOL_DIR) -f Makefile $(PRIVATE_PREFLASH_TOOL_FLAGS) CONFIG_DIR=$(PRIVATE_PREFLASH_TOOL_OUT_DIR) build

install-preflash_tool: build-preflash_tool
	PATH=$(PATH) $(MAKE) -C $(PRIVATE_PREFLASH_TOOL_DIR) -f Makefile $(PRIVATE_PREFLASH_TOOL_FLAGS) CONFIG_DIR=$(PRIVATE_PREFLASH_TOOL_OUT_DIR) install

clean-preflash_tool:
	PATH=$(PATH) $(MAKE) -C $(PRIVATE_PREFLASH_TOOL_DIR) -f Makefile $(PRIVATE_PREFLASH_TOOL_FLAGS) CONFIG_DIR=$(PRIVATE_PREFLASH_TOOL_OUT_DIR) clean

distclean-preflash_tool:
	PATH=$(PATH) $(MAKE) -C $(PRIVATE_PREFLASH_TOOL_DIR) -f Makefile $(PRIVATE_PREFLASH_TOOL_FLAGS) CONFIG_DIR=$(PRIVATE_PREFLASH_TOOL_OUT_DIR) distclean

clean clobber: distclean-preflash_tool

st-ericsson-flashkit: config-preflash_tool build-preflash_tool install-preflash_tool
