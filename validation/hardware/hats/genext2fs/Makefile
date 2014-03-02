#
#***************************************************************
#
# Synopsis: Genext2fs Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=genext2fs
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_VERSION=1.4.1
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_ALIAS).tar.gz
PACKAGE_PATCHES=patches
PACKAGE_PATCHES_ARCHIVE=$(PACKAGE_PATCHES).tar

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/.prepare
CONFIG=$(OUTPUT)/Makefile
SYMBOLS=$(OUTPUT)/$(PACKAGE_EXEC_NAME)

HOST_INSTALL_DIR=$(HOME)

HOSTFS=$(HOST_INSTALL_DIR)/bin/$(PACKAGE_EXEC_NAME)

VARS+= --prefix=$(HOST_INSTALL_DIR)

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE)
	tar -xf $(PACKAGE_PATCHES_ARCHIVE)
	$(foreach mpatch, $(wildcard patches/*.patch), \
		patch -d $(PACKAGE_ALIAS) -p1 < $(mpatch);)
	touch $(PREPARE)

$(CONFIG): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS)

$(SYMBOLS): $(CONFIG)
	$(MAKE) -s -C $(OUTPUT)

$(HOSTFS): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) install

install: $(HOSTFS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT)
	$(RM) -rf $(PACKAGE_PATCHES)
	$(RM) -rf $(PACKAGE_ALIAS)
