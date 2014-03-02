#
#***************************************************************
#
# Synopsis: DthFramework Includes Makefile
#                              - Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=DthFramework

INCLUDES= dthfs dthsrvhelper dth_tools.h tatasync.h
INCLUDES:= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(INCLUDES))

.PHONY: all install clean

all: install

$(INCLUDES):
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

install: $(INCLUDES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
