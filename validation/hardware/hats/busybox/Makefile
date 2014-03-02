#
#***************************************************************
#
# Synopsis: Busybox Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=busybox
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_VERSION=1.18.1
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_ALIAS).tar.bz2
PACKAGE_PATCHES=patches
PACKAGE_PATCHES_ARCHIVE=$(PACKAGE_PATCHES).tar

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/config
PATCH=$(PACKAGE_ALIAS)/configure.in
CONFIG=$(OUTPUT)/.config
CONF_CC=$(OUTPUT)/conf-cc
CONF_LD=$(OUTPUT)/conf-ld
OBJECTS=$(OUTPUT)/$(PACKAGE_EXEC_NAME)
SYSTEMFS=$(HATS_SYSTEMFS)/bin/$(PACKAGE_EXEC_NAME)
ROOTFS=$(HATS_ROOTFS)/bin/$(PACKAGE_EXEC_NAME)

CFLAGS+= -mcpu=cortex-a9

.PHONY: all install clean

all: install_system

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE)
	tar -xf $(PACKAGE_PATCHES_ARCHIVE)

$(PATCH): $(PREPARE)
	$(foreach mpatch, $(wildcard $(PACKAGE_PATCHES)/*.patch), \
		echo -e "\t-----> Applying $(mpatch)"; \
		pwd && patch -d $(PACKAGE_ALIAS) -p1 < $(mpatch);)

$(CONFIG): $(PATCH)
	$(MKDIR) -p $(OUTPUT)
	$(INSTALL_CMD) -D mop500_defconfig $(@)

$(CONF_CC): $(PATCH)
	$(ECHO) "$(CC) $(CFLAGS) O=$(OUTPUT)" >$(@)

$(CONF_LD): $(PATCH)
	$(ECHO) "$(CC) $(LDFLAGS) O=$(OUTPUT)" >$(@)

$(OBJECTS): $(CONFIG) $(CONF_CC) $(CONF_LD)
	$(MAKE) -s -C $(PACKAGE_ALIAS) O="$(OUTPUT)"

$(SYSTEMFS): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) CONFIG_PREFIX=$(HATS_SYSTEMFS) install

$(ROOTFS): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) CONFIG_PREFIX=$(HATS_ROOTFS) install

menuconfig: $(OBJECTS)
	$(MAKE) -s -C $(OUTPUT) $@ $(VARS)
	$(INSTALL_CMD) -D $(CONFIG) mop500_defconfig

install_system: $(SYSTEMFS)

install_root: $(ROOTFS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	if test -e $(SYSTEMFS); then $(MAKE) -C $(PACKAGE_ALIAS) O="$(OUTPUT)" CONFIG_PREFIX=$(HATS_SYSTEMFS) uninstall; fi;
	if test -e $(ROOTFS); then $(MAKE) -C $(PACKAGE_ALIAS) O="$(OUTPUT)" CONFIG_PREFIX=$(HATS_ROOTFS) uninstall; fi;
	$(RM) -rf $(OUTPUT)
	$(RM) -rf $(PACKAGE_PATCHES)
	$(RM) -rf $(PACKAGE_ALIAS)
