#
#***************************************************************
#
# Synopsis: Gdb & Gdbserver Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=gdb
PACKAGE_EXEC_HOST_NAME=$(PACKAGE_NAME)
PACKAGE_EXEC_TARGET_NAME=$(PACKAGE_NAME)server
PACKAGE_VERSION=7.0
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_ALIAS).tar.gz
PACKAGE_PATCHES=patches
PACKAGE_PATCHES_ARCHIVE=$(PACKAGE_PATCHES).tar

OUTPUT_HOST=$(HATS_OBJ)/$(PACKAGE_EXEC_HOST_NAME)
OUTPUT_TARGET=$(HATS_OBJ)/$(PACKAGE_EXEC_TARGET_NAME)
PREPARE=$(PACKAGE_ALIAS)/.prepare
CONFIG_HOST=$(OUTPUT_HOST)/Makefile
CONFIG_TARGET=$(OUTPUT_TARGET)/Makefile
OBJECTS_HOST=$(OUTPUT_HOST)/$(PACKAGE_EXEC_HOST_NAME)
OBJECTS_TARGET=$(OUTPUT_TARGET)/$(PACKAGE_EXEC_TARGET_NAME)

HOST_INSTALL_DIR=$(OUTPUT_HOST)/binaries

HOSTFS=$(HOST_INSTALL_DIR)/bin/$(CROSS_COMPILE)$(PACKAGE_EXEC_HOST_NAME)
SYSTEMFS=$(HATS_BIN)/$(CROSS_COMPILE)$(PACKAGE_EXEC_TARGET_NAME)

TARGET_VARS:=--target=$(CROSS_COMPILE_TARGET) --host=$(CROSS_COMPILE_TARGET) --prefix="$(HATS_SYSTEMFS)/usr"
HOST_VARS:=--target=$(CROSS_COMPILE_TARGET) --prefix=$(HOST_INSTALL_DIR)

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE)
	tar -xf $(PACKAGE_PATCHES_ARCHIVE)
	$(foreach mpatch, $(wildcard patches/*.patch), \
		patch -d $(PACKAGE_ALIAS) -p1 < $(mpatch);)
	touch $(PREPARE)

$(CONFIG_HOST): $(PREPARE)
	$(MKDIR) -p $(OUTPUT_HOST)
	cd $(OUTPUT_HOST) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(HOST_VARS)

$(CONFIG_TARGET): $(PREPARE)
	$(MKDIR) -p $(OUTPUT_TARGET)
	cd $(OUTPUT_TARGET) && $(CURDIR)/$(PACKAGE_ALIAS)/gdb/gdbserver/configure -q -C $(TARGET_VARS)

$(OBJECTS_HOST): $(CONFIG_HOST)
	$(MAKE) -s -C $(OUTPUT_HOST)

$(OBJECTS_TARGET): $(CONFIG_TARGET)
	$(MAKE) -s -C $(OUTPUT_TARGET)

$(HOSTFS): $(OBJECTS_HOST)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT_HOST) install

$(SYSTEMFS): $(OBJECTS_TARGET)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_CMD) -c -s --strip-program=$(STRIP)' -s -C $(OUTPUT_TARGET) install

install: $(HOSTFS) $(SYSTEMFS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	if test -e $(OUTPUT_HOST); then $(MAKE) -s -C $(OUTPUT_HOST) uninstall; fi;
	if test -e $(OUTPUT_TARGET); then $(MAKE) -s -C $(OUTPUT_TARGET) uninstall; fi;
	$(RM) -rf $(OUTPUT_HOST) $(OUTPUT_TARGET)
	$(RM) -rf $(PACKAGE_PATCHES)
	$(RM) -rf $(PACKAGE_ALIAS)
