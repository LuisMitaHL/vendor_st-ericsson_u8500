PRIVATE_FLASHTOOLCLI_TOOLS_PATH := $(abspath $(TOOLS_PATH))
PRIVATE_FLASHTOOLCLI_GIT_DIR := $(PRIVATE_FLASHTOOLCLI_TOOLS_PATH)/platform/flash_kit/flash_tool_cli

ifdef TARGET_OUT_INTERMEDIATES
  PRIVATE_FLASHTOOLCLI_OUT_INTERIM:=$(abspath $(TARGET_OUT_INTERMEDIATES))/flash_kit/flash_tool_cli
  $(shell mkdir -p $(PRIVATE_FLASHTOOLCLI_OUT_INTERIM))
else
  PRIVATE_FLASHTOOLCLI_OUT_INTERIM:=$(PRIVATE_FLASHTOOLCLI_GIT_DIR)
endif

PRIVATE_FLASHTOOLCLI_FLAGS := \
 FLASH_KIT_TARGET_DIR=$(FLASHKIT_INSTALL_BASE)/flashkit \
 ANT_HOME=$(ANT_HOME) \
 TARGET_OUT_INTERMEDIATES=$(PRIVATE_FLASHTOOLCLI_OUT_INTERIM) \
 FTB_GIT_DIR=$(PRIVATE_FLASHTOOLCLI_TOOLS_PATH)/platform/flash_kit/flash_tool_backend


st-ericsson-flashkit: build_flash_tool_cli

.phony: build_flash_tool_cli clean_flash_tool_cli distclean_flash_tool_cli

build_flash_tool_cli:
	@$(PRIVATE_FLASHTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_FLASHTOOLCLI_GIT_DIR) -f Makefile
	@$(PRIVATE_FLASHTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_FLASHTOOLCLI_GIT_DIR) -f Makefile install

clean_flash_tool_cli:
	@$(PRIVATE_FLASHTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_FLASHTOOLCLI_GIT_DIR) -f Makefile clean

distclean_flash_tool_cli:
	@$(PRIVATE_FLASHTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_FLASHTOOLCLI_GIT_DIR) -f Makefile distclean

clean: clean_flash_tool_cli
