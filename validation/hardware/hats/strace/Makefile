#
#***************************************************************
#
# Synopsis: Strace Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=strace
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_VERSION=4.6
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.bz2

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/.prepare
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/$(PACKAGE_EXEC_NAME)
BINARIES=$(HATS_BIN)/$(PACKAGE_EXEC_NAME)

VARS+= --host=$(CROSS_COMPILE_TARGET) --prefix=/usr

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE)
	touch $(PREPARE)

$(CONFIG): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS)

$(OBJECTS): $(CONFIG)
	$(MAKE) -s -C $(OUTPUT)

$(BINARIES): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) install-strip DESTDIR=$(HATS_SYSTEMFS)

install: $(BINARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	if test -e $(BINARIES); then $(MAKE) -s -C $(OUTPUT) uninstall DESTDIR=$(HATS_SYSTEMFS);fi;
	$(RM) -rf $(OUTPUT) $(PACKAGE_ALIAS)
