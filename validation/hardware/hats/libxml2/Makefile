#
#***************************************************************
#
# Synopsis: Libxml2 Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=libxml2
PACKAGE_LIB_NAME=$(PACKAGE_NAME).so
PACKAGE_VERSION=2.7.3
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/configure.in
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/.libs/$(PACKAGE_LIB_NAME)
LIBRARIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)

VARS+= --host=$(CROSS_COMPILE_TARGET) --without-python --enable-shared \
	--enable-static=no --prefix=$(OUTPUT) --exec-prefix=$(HATS_SYSTEMFS)/usr \
	--includedir=$(HATS_INC)

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE)

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
	$(RM) -rf $(OUTPUT)
	$(RM) -rf $(PACKAGE_ALIAS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
