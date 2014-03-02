#
#***************************************************************
#
# Synopsis: Autotest Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk
include $(HATS_CONFIG)

ifneq ($(HATSCONF_AB_LIB_8500),y)
ifneq ($(HATSCONF_AB_LIB_8505),y)
$(error AB library not supported)
endif
endif

PACKAGE_NAME:=autotest
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_ALIAS=$(PACKAGE_NAME)

SCRIPTS=$(shell cd scripts && find -type f)
SCRIPTS:=$(addprefix $(HATS_BIN)/, $(SCRIPTS))

.PHONY: all install clean

all: install

$(HATS_BIN)/%: scripts/%
	$(INSTALL_CMD) -D $< $@

install: $(SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(SCRIPTS)
