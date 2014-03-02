#
#***************************************************************
#
# Synopsis: ppp Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=ppp
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)d
PACKAGE_VERSION=2.4.5
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz
PACKAGE_PATCHES=patches
PACKAGE_PATCHES_ARCHIVE=$(PACKAGE_PATCHES).tar.gz

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/README
PATCH=$(PACKAGE_ALIAS)/configure.in
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/pppd/$(PACKAGE_EXEC_NAME)
BINARIES=$(HATS_BIN)/$(PACKAGE_EXEC_NAME)

VARS+= --host=arm-none-linux-gnueabi  \
	--prefix=$(OUTPUT)

CFLAGS+= -mcpu=cortex-a9

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE)
	tar -xf $(PACKAGE_PATCHES_ARCHIVE)

$(PATCH): $(PREPARE)
	$(foreach mpatch, $(wildcard $(PACKAGE_PATCHES)/*.patch), \
		cd .. && git apply --whitespace=nowarn $(PACKAGE_NAME)/$(mpatch);)

$(CONFIG): $(PATCH)
	$(MKDIR) -p $(OUTPUT)
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure --enable-silent-rules -q $(VARS)

$(OBJECTS): $(CONFIG)
	$(MAKE) V=0 -s -C $(OUTPUT)

$(BINARIES): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) install
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

install: $(BINARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT)
	$(RM) -rf $(BINARIES)
	$(RM) -rf $(PACKAGE_ALIAS)
	$(RM) -rf $(PACKAGE_PATCHES)
