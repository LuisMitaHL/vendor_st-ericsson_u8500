#
#***************************************************************
#
# Synopsis: Iptables Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=iptables
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)-multi
PACKAGE_VERSION=1.4.9.1
PACKAGE_ALIAS=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
PACKAGE_ARCHIVE=$(PACKAGE_ALIAS).tar.bz2

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
PREPARE=$(PACKAGE_ALIAS)/configure
CONFIG=$(OUTPUT)/Makefile
CONF_CC=$(OUTPUT)/conf-cc
CONF_LD=$(OUTPUT)/conf-ld
OBJECTS=$(OUTPUT)/$(PACKAGE_EXEC_NAME)
BINARIES=$(HATS_SYSTEMFS)/usr/sbin/$(PACKAGE_EXEC_NAME)

VARS+= --host=$(CROSS_COMPILE_TARGET) --prefix=$(OUTPUT) \
	--exec-prefix=/usr --disable-ipv6 --disable-largefile --enable-devel=no

CFLAGS+= -mcpu=cortex-a9

.PHONY: all install clean

all: install

$(PREPARE):
	tar -xjf $(PACKAGE_ARCHIVE)

$(CONFIG): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	cd $(OUTPUT) && $(CURDIR)/$(PACKAGE_ALIAS)/configure -q -C $(VARS)

$(CONF_CC): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	$(ECHO) "$(CC) $(CFLAGS) O=$(OUTPUT)" >$(@)

$(CONF_LD): $(PREPARE)
	$(MKDIR) -p $(OUTPUT)
	$(ECHO) "$(CC) $(LDFLAGS) O=$(OUTPUT)" >$(@)

$(OBJECTS): $(CONFIG) $(CONF_CC) $(CONF_LD)
	$(MAKE) -s -C $(OUTPUT)

$(BINARIES): $(OBJECTS)
	@$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	@$(MAKE) INSTALL_STRIP_FLAG=-s INSTALL_PROGRAM='/usr/bin/install -c -s --strip-program=$(STRIP)'\
	 -s -C $(OUTPUT) install-exec-am DESTDIR=$(HATS_SYSTEMFS)

install: $(BINARIES)

clean:
	@$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	@$(RM) -rf $(HATS_SYSTEMFS)/usr/sbin/$(PACKAGE_NAME)*
	@$(RM) -rf $(HATS_SYSTEMFS)/usr/bin/$(PACKAGE_NAME)*
	@$(RM) -rf $(OUTPUT)
	@$(RM) -rf $(PACKAGE_ALIAS)
