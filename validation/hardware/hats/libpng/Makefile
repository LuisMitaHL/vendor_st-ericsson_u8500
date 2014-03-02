#
#***************************************************************
#
# Synopsis: Libpng Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=libpng
PACKAGE_LIB_NAME=$(PACKAGE_NAME).so
PACKAGE_VERSION=1.2.34
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.bz2

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/.prepare
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/.libs/$(PACKAGE_LIB_NAME)
LIBRARIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)

CFLAGS+= -I$(HATS_INC)/zlib

VARS+= --host=$(CROSS_COMPILE_TARGET) --disable-static --enable-shared \
	--prefix=$(OUTPUT) --libdir=$(HATS_LIB) \
	--includedir=$(HATS_INC)/$(PACKAGE_NAME) \
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" CPPFLAGS="$(CFLAGS)"

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

$(LIBRARIES): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) install-strip

install: $(LIBRARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	if test -e $(LIBRARIES); then $(MAKE) -s -C $(OUTPUT) uninstall; fi;
	$(RM) -rf $(HATS_LIB)/$(PACKAGE_LIB_NAME)*
	$(RM) -rf $(OUTPUT)
	$(RM) -rf $(PACKAGE_ALIAS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
