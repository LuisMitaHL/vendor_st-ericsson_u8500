#
#***************************************************************
#
# Synopsis: tatrad Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=tatrad

TOOLS= $(HATS_OBJ)/$(PACKAGE_NAME)

.PHONY: all install clean

all: install

$(TOOLS):
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(LN) -sf $(CURDIR)/$(PACKAGE_NAME) $@

install: $(TOOLS)

clean:
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	rm -rf $(TOOLS)

