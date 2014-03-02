# Flash Kit Flash Tool CLI Makefile

TOOL_VERSION ?= tool_version.txt
TARGET_OUT_INTERMEDIATES ?= .
FTB_GIT_DIR ?= ../flash_tool_backend

ifndef FLASH_KIT_TARGET_DIR
  FLASH_KIT_TARGET_DIR:=./flash-kit-tools
endif

SRC:=$(shell find . -type f | grep -v '/tool_version.txt\|/build/\|/bin/' | sed "s/[ ]/\\\\ /g")
BASEDIR:=$(shell pwd)
GET_TOOL_VERSION:=$(FTB_GIT_DIR)/get_version.sh $(BASEDIR) $(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION)

INSTALL:=$(shell which install)
FIND:=$(shell which find)
CP:=$(shell which cp)
CHMOD:=$(shell which chmod)
XARGS:=$(shell which xargs)
MKDIR:=$(shell which mkdir)
RM:=$(shell which rm)

ifdef ANT_HOME
  ANT:=$(ANT_HOME)/bin/ant
else
  ANT:=$(shell which ant)
endif
ifeq ($(ANT),)
$(error ant is not installed)
endif

default: build

.PHONY: install
install: build
	$(MKDIR) -p $(FLASH_KIT_TARGET_DIR)/flash-tool-cli
	$(INSTALL) -m 0644 $(TARGET_OUT_INTERMEDIATES)/build/release/*.jar $(FLASH_KIT_TARGET_DIR)/flash-tool-cli
	$(INSTALL) -m 0755 scripts/flash-tool.sh $(FLASH_KIT_TARGET_DIR)/flash-tool-cli
	$(INSTALL) -m 0755 scripts/flash-tool.bat $(FLASH_KIT_TARGET_DIR)/flash-tool-cli
	$(INSTALL) -m 0644 commons-cli-LICENSE.txt $(FLASH_KIT_TARGET_DIR)/flash-tool-cli
	$(CP) -r doc $(FLASH_KIT_TARGET_DIR)/flash-tool-cli
	$(FIND) $(FLASH_KIT_TARGET_DIR)/flash-tool-cli/doc -type f | $(XARGS) $(CHMOD) 0644
	$(INSTALL) -m 0644 HowToUseManPages.txt $(FLASH_KIT_TARGET_DIR)/flash-tool-cli

.PHONY: build
build: $(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION)

$(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION): $(SRC)
	$(ANT) -f build-release.xml -DTARGET_OUT_INTERMEDIATES=$(TARGET_OUT_INTERMEDIATES)
	$(GET_TOOL_VERSION)

.PHONY: clean
clean:
	$(ANT) -f build-release.xml flash-tool-cli.CLEAN -DTARGET_OUT_INTERMEDIATES=$(TARGET_OUT_INTERMEDIATES)
	$(RM) -f $(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION)

.PHONY: distclean
distclean: clean
	$(RM) -rf $(FLASH_KIT_TARGET_DIR)/flash-tool-cli

