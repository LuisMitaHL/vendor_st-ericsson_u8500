#
#***************************************************************
#
# Synopsis: ALSA-UTILS Makefile- Linux Application Build Rules.
#
#***************************************************************
#
include .config.mk

PACKAGE_NAME=alsa-utils
PACKAGE_EXEC_NAME=aplay
EXEC_NAME=amixer aplay
LINK_NAME=arecord
PACKAGE_VERSION=1.0.16
PATCH_DIRECTORY=patches
PACKAGE_PATCH=$(PATCH_DIRECTORY).tgz
PATCHLIST=`cat $(PATCH_DIRECTORY)/series`
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.bz2
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/bin/$(PACKAGE_EXEC_NAME)
SHARE=alsa sounds
SYMBOLS=$(addprefix $(HATS_SYM)/, $(EXEC_NAME))
EXEC=$(addprefix $(HATS_BIN)/, $(EXEC_NAME))
LINK=$(addprefix $(HATS_BIN)/, $(LINK_NAME))
SHARE:=$(addprefix $(HATS_SHARE)/, $(SHARE))

CFLAGS+=-mfloat-abi=softfp -I$(HATS_INC)/ncurses -I$(HATS_INC)

VARS+=--target=$(CROSS_COMPILE_TARGET) --host=$(CROSS_COMPILE_TARGET) --prefix=$(OUTPUT) \
	--with-alsa-prefix=$(HATS_LIB) --with-alsa-inc-prefix=$(HATS_INC) \
	--disable-alsatest --disable-nls --with-gnu-ld \
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
	tar -xf $(PACKAGE_ARCHIVE)
	tar -xf $(PACKAGE_PATCH)
	for mpatch in $(PATCHLIST); do \
		patch -d $(PACKAGE_ALIAS) -p1 < $(PATCH_DIRECTORY)/$${mpatch}; \
	done ;

$(CONFIG): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS) $(CONFIGURE_OPT_DEBUG)

$(OBJECTS): $(CONFIG)
	$(MAKE) -s -C $(OUTPUT) install

$(SYMBOLS): $(OBJECTS)
	$(CP) -df $(OUTPUT)/bin/$(notdir $@) $@

$(EXEC): $(OBJECTS) $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(CP) -df $(OUTPUT)/bin/$(notdir $@) $@
	$(STRIP) --strip-unneeded $@

$(LINK): $(EXEC)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(CP) -df $(OUTPUT)/bin/$(notdir $@) $@

$(SHARE): $(OBJECTS)
	$(CP) -Rdf $(OUTPUT)/share/$(notdir $@) $@


install: $(EXEC) $(SHARE) $(LINK)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT) $(PACKAGE_ALIAS) $(EXEC) $(LINK) $(SYMBOLS) $(SHARE) $(PATCH_DIRECTORY)
