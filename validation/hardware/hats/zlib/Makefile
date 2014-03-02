#
#***************************************************************
#
# Synopsis: Zlib Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=zlib
PACKAGE_LIB_NAME=libz.so
PACKAGE_VERSION=1.2.3
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz
PACKAGE_PATCHES=patches
PACKAGE_PATCHES_ARCHIVE=$(PACKAGE_PATCHES).tar.gz

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/zlib.h
PATCH=$(PACKAGE_ALIAS)/configure.in
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/.libs/$(PACKAGE_LIB_NAME)
LIBRARIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)

VARS+= --host=$(CROSS_COMPILE_TARGET) --disable-static --enable-shared \
	--prefix=$(OUTPUT) --libdir=$(HATS_LIB) \
	--includedir=$(HATS_INC)/$(PACKAGE_NAME)

.PHONY: all install clean

all: install

$(PREPARE):
	echo PREPARE = $(PREPARE)
	tar -xf $(PACKAGE_ARCHIVE)
	tar -xf $(PACKAGE_PATCHES_ARCHIVE)

$(PATCH): $(PREPARE)
	echo PATCH = $(PATCH)
	$(foreach mpatch, $(wildcard $(PACKAGE_PATCHES)/*.patch), \
		cd .. && git apply --whitespace=nowarn $(PACKAGE_NAME)/$(mpatch);)

$(CONFIG): $(PATCH)
	echo CONFIG = $(CONFIG)
	$(MKDIR) -p $(OUTPUT)
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS)

$(OBJECTS): $(CONFIG)
	$(MAKE) V=0 -s -C $(OUTPUT)

$(LIBRARIES): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) V=0 -s -C $(OUTPUT) install-strip

install: $(LIBRARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT) $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(LIBRARIES)
	$(RM) -rf $(PACKAGE_ALIAS)
	$(RM) -rf $(PACKAGE_PATCHES)
