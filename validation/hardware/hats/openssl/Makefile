#
#***************************************************************
#
# Synopsis: Openssl Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=openssl
PACKAGE_LIB_NAME=libssl.so
PACKAGE_VERSION=1.0.0d
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz
PACKAGE_PATCHES=patches
PACKAGE_PATCHES_ARCHIVE=$(PACKAGE_PATCHES).tar.gz

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/config
PATCH=$(PACKAGE_ALIAS)/configure.in
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/.libs/$(PACKAGE_LIB_NAME)
LIBRARIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)

VARS += --host=$(CROSS_COMPILE_TARGET) --disable-static --enable-shared \
	--prefix=$(OUTPUT) --libdir=$(HATS_LIB) \
	--includedir=$(HATS_INC)/$(PACKAGE_NAME) \
	CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
	CPPFLAGS="$(CFLAGS)"

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
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS)

$(OBJECTS): $(CONFIG)
	$(MAKE) -s -C $(OUTPUT)

$(LIBRARIES): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) install-strip

install: $(LIBRARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT) $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(LIBRARIES)
	$(RM) -rf $(PACKAGE_ALIAS)
	$(RM) -rf $(PACKAGE_PATCHES)
