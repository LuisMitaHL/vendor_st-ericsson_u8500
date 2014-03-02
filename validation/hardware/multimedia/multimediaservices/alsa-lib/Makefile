#
#***************************************************************
#
# Synopsis: ALSA-LIB Makefile- Linux Application Build Rules.
#
#***************************************************************
#
include .config.mk


PACKAGE_NAME:=alsa-lib
LIB_NAME:=libasound
PACKAGE_LIB_NAME=$(LIB_NAME).so
PACKAGE_VERSION=1.0.16
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.bz2

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/.unpacked
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/src/.libs/$(PACKAGE_LIB_NAME)
LIBRARIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)

CFLAGS+=-mfloat-abi=softfp

VARS+= --host=$(CROSS_COMPILE_TARGET) --with-gnu-ld --disable-python --enable-shared \
	--enable-static=no --prefix=$(OUTPUT) --includedir=$(HATS_INC) \
	LDFLAGS="$(LDFLAGS)" CFLAGS="$(CFLAGS)"

#overwrite MODULE_CONFIGURE_OPT_DEBUG
ifeq ($(DEBUG),yes)
CONFIGURE_OPT_DEBUG=--with-debug=full
else
CONFIGURE_OPT_DEBUG=--with-debug=none
endif

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE);
	touch $(PACKAGE_ALIAS)/.unpacked;

$(CONFIG): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS) $(CONFIGURE_OPT_DEBUG)
	sed -ie 's/ALSA_CONFIG_DIR.*/ALSA_CONFIG_DIR \"\/usr\/share\/alsa\"/g' $(OUTPUT)/include/config.h
	sed -ie 's/ALSA_PLUGIN_DIR.*/ALSA_PLUGIN_DIR \"\/usr\/share\/alsa\"/g' $(OUTPUT)/include/config.h

$(OBJECTS): $(CONFIG)
	$(MAKE) -s -C $(OUTPUT)

$(LIBRARIES): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -s -C $(OUTPUT) install
	$(CP) -df $(OBJECTS)* $(HATS_SYM)
	$(CP) -df $(OBJECTS)* $(HATS_LIB)
	$(CP) -rdf $(OUTPUT)/share/alsa $(HATS_SHARE)
	$(STRIP) --strip-unneeded $(HATS_LIB)/$(PACKAGE_LIB_NAME)

install: $(LIBRARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT) $(PACKAGE_ALIAS) $(HATS_SHARE)/alsa
	$(RM) -rf $(HATS_SYM)/$(PACKAGE_LIB_NAME)*
	$(RM) -rf $(HATS_LIB)/$(PACKAGE_LIB_NAME)*
	$(RM) -rf $(HATS_INC)/alsa $(HATS_INC)/sys
