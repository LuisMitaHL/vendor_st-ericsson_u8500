# vim:syntax=make:
# -*- Mode: Makefile -*-
include $(MM_MAKEFILES_DIR)/AudioAll.mk

doxygen: afm.dox
	doxygen $<

documentation:
	$(MAKE) doxygen

DIRS_TO_INSTALL := $(wildcard tools/bin/Linux)

AUDIOTOOLS_DIR_NAME:=audio/afm

TO_INSTALL:=$(INSTALL_TOOLS_DIR)/$(AUDIOTOOLS_DIR_NAME)/$(DIRS_TO_INSTALL)

all: tools_build tools_install

clean realclean: tools_clean

tools_clean:
	$(MAKE) -C tools clean

tools_install:
	$(MAKE) install_tools

install_tools: $(TO_INSTALL)

tools_build:
	$(MAKE) -C tools all

$(INSTALL_TOOLS_DIR)/$(AUDIOTOOLS_DIR_NAME)/%: %
	if [ -d $< ] ; then $(MMINSTALL) -d $< $@ ; else $(MMINSTALL) $< $@ ; fi

$(INSTALL_TOOLS_DIR)/$(AUDIOTOOLS_DIR_NAME):
	mkdir -p $(INSTALL_TOOLS_DIR)/$(AUDIOTOOLS_DIR_NAME)


p-%:
	@echo "$*=$($*)"
