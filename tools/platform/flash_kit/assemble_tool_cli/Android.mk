PRIVATE_ASSEMBLETOOLCLI_TOOLS_PATH := $(abspath $(TOOLS_PATH))
PRIVATE_ASSEMBLETOOLCLI_GIT_DIR := $(PRIVATE_ASSEMBLETOOLCLI_TOOLS_PATH)/platform/flash_kit/assemble_tool_cli

ifdef TARGET_OUT_INTERMEDIATES
  PRIVATE_ASSEMBLETOOLCLI_OUT_INTERIM:=$(abspath $(TARGET_OUT_INTERMEDIATES))/flash_kit/assemble_tool_cli
  $(shell mkdir -p $(PRIVATE_ASSEMBLETOOLCLI_OUT_INTERIM))
else
  PRIVATE_ASSEMBLETOOLCLI_OUT_INTERIM:=$(PRIVATE_ASSEMBLETOOLCLI_GIT_DIR)
endif

PRIVATE_ASSEMBLETOOLCLI_FLAGS := \
 FLASH_KIT_TARGET_DIR=$(FLASHKIT_INSTALL_BASE)/flashkit \
 ANT_HOME=$(ANT_HOME) \
 TARGET_OUT_INTERMEDIATES=$(PRIVATE_ASSEMBLETOOLCLI_OUT_INTERIM) \
 FTB_GIT_DIR=$(PRIVATE_ASSEMBLETOOLCLI_TOOLS_PATH)/platform/flash_kit/flash_tool_backend

st-ericsson-flashkit: build_assemble_tool_cli

.phony: build_assemble_tool_cli clean_assemble_tool_cli distclean_assemble_tool_cli

build_assemble_tool_cli:
	@$(PRIVATE_ASSEMBLETOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_ASSEMBLETOOLCLI_GIT_DIR) -f Makefile
	@$(PRIVATE_ASSEMBLETOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_ASSEMBLETOOLCLI_GIT_DIR) -f Makefile install

clean_assemble_tool_cli:
	@$(PRIVATE_ASSEMBLETOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_ASSEMBLETOOLCLI_GIT_DIR) -f Makefile clean

distclean_assemble_tool_cli:
	@$(PRIVATE_ASSEMBLETOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_ASSEMBLETOOLCLI_GIT_DIR) -f Makefile distclean

clean: clean_assemble_tool_cli
