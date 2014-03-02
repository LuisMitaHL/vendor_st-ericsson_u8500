#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

include $(MM_MAKEFILES_DIR)/SharedDefines.mk

FILES_TO_INSTALL := nmftools.jar \
                    nmfconfc  nmfconfc.cmd  nmfitfc  nmfitfc.cmd \
                    windows.dll

DIRS_TO_INSTALL := api

TO_INSTALL := $(addprefix $(INSTALL_TOOLS_DIR)/$(NMFTOOLS_DIR_NAME)/, $(FILES_TO_INSTALL) $(DIRS_TO_INSTALL))

all: nmftools.jar $(TO_INSTALL)


install: $(TO_INSTALL)

$(INSTALL_TOOLS_DIR)/$(NMFTOOLS_DIR_NAME)/%: %
	if [ -d $< ] ; then $(MMINSTALL) -d $< $@ ; else $(MMINSTALL) $< $@ ; fi

clean realclean :
	$(RM) -rf classes
	$(RM) nmftools.jar

uninstall:
	$(RM) $(TO_INSTALL)

SRC=src
CLASSES=classes

#
# Rules for compiling Java
#
JAVA = $(SRC)/tools/doelf.java $(SRC)/tools/dobinding.java $(shell find $(SRC) -name *.java)

ifneq ($(findstring indows,$(OS)),)
PATHSEPARATOR=";"
else
PATHSEPARATOR=:
endif
	
$(CLASSES):
		mkdir -p $@
$(CLASSES)/%.class: $(SRC)/%.java 
	javac -target "1.5" -d $(CLASSES) -classpath $(SRC) -deprecation $<

#
# Rule for Jar
#
nmftools.jar: $(CLASSES) $(patsubst $(SRC)/%,$(CLASSES)/%,$(patsubst %.java,%.class,$(JAVA)))
	jar cf $@  -C $(CLASSES) tools
