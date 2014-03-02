#
#***************************************************************
#
# Synopsis: Treedyn Makefile - Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME= libtreedyn

TOOLS= TATReg2xml.pl
TOOLS:= $(addprefix $(HATS_OBJ)/$(PACKAGE_NAME)/, $(TOOLS))

.PHONY: all install clean

all: install

$(TOOLS):
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

install: $(TOOLS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(HATS_OBJ)/$(PACKAGE_NAME) $(HATS_INC)/$(PACKAGE_NAME)
