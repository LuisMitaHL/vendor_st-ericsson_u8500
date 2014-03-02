#
#***************************************************************
#
# Synopsis: Ncurses Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=ncurses
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so
PACKAGE_SUBVERSION=5
PACKAGE_VERSION=5.6
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/.prepare
CONFIG=$(OUTPUT)/Makefile
OBJECTS=$(OUTPUT)/lib/$(PACKAGE_LIB_NAME)
INCLUDES=$(HATS_INC)/$(PACKAGE_NAME)/$(PACKAGE_NAME).h
DATAS=$(HATS_SHARE)/terminfo/x/xterm
LIBRARIES_SO= libpanel.so libform.so libmenu.so libncurses.so
LIBRARIES_SO:= $(addprefix $(HATS_LIB)/, $(LIBRARIES_SO))
LIBRARIES_SO_5:= $(addsuffix .5, $(LIBRARIES_SO))
LIBRARIES:= $(addsuffix .$(PACKAGE_VERSION), $(LIBRARIES_SO))

VARS+= --host=$(CROSS_COMPILE_TARGET) --enable-silent-rules \
	--without-profile --without-ada --without-manpages \
	--without-debug --without-cxx-binding --with-shared \
	--prefix=/usr --includedir=/usr/include \
	--exec-prefix=/usr -datadir=/usr/share --with-install-prefix=$(OUTPUT)

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xf $(PACKAGE_ARCHIVE)
	touch $(PREPARE)

$(CONFIG): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	$(MKDIR) -p $(OUTPUT)/usr/share
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS)

$(OBJECTS): $(CONFIG)
	$(MAKE) -s -C $(OUTPUT)

$(INCLUDES): $(OBJECTS)
	$(MAKE) -s -C $(OUTPUT) install.includes
	$(CP) -r $(OUTPUT)/usr/include $(HATS_INC)/$(PACKAGE_NAME)

$(DATAS): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(MAKE) -C $(OUTPUT) install.data
	$(INSTALL_CMD) -D $(OUTPUT)/usr/share/terminfo/v/vt102 $(HATS_SYSTEMFS)/vt102
	$(INSTALL_CMD) -D $(OUTPUT)/usr/share/terminfo/v/vt102 $(HATS_SHARE)/terminfo/v/vt102
	$(INSTALL_CMD) -D $(OUTPUT)/usr/share/terminfo/x/xterm $(HATS_SHARE)/terminfo/x/xterm

$(LIBRARIES): $(OBJECTS) | $(LIBRARIES_SO_5) $(LIBRARIES_SO)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(INSTALL_CMD) -D $(OUTPUT)/lib/$(notdir $@) $@
	$(STRIP) --strip-unneeded $(@)

%.so:
	$(LN) -sf $(notdir $@).$(PACKAGE_VERSION) $@

%.so.5:
	$(LN) -sf $(notdir $@).6 $@

install: $(DATAS) $(INCLUDES) $(LIBRARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(HATS_SYSTEMFS)/vt102
	$(RM) -rf $(HATS_SHARE)/terminfo
	$(RM) -rf $(OUTPUT) $(LIBRARIES) $(LIBRARIES_SO_5) $(LIBRARIES_SO)
	$(RM) -rf $(PACKAGE_ALIAS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
