#
#***************************************************************
#
# Synopsis: Modemfs Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=modemfs
PACKAGE_ALIAS=$(PACKAGE_NAME)
PACKAGE_SCRIPT=StartModemfs.sh

PREPARE=$(HATS_MODEMFS)/CONF

.PHONY: all install clean

all: install

$(PREPARE):
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	./$(PACKAGE_SCRIPT) $(HATS_MODEMFS)

install: $(PREPARE)

clean::
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(HATS_MODEMFS)/*
