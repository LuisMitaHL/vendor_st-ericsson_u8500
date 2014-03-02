#
#***************************************************************
#
# Synopsis: DTHTREEDYN Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME:=libdthtreedyn
PACKAGE_PATH=$(HATS_LOCALLIB_PATH)/generalservices/$(PACKAGE_NAME)
PACKAGE_ARCHIVE=
PACKAGE_ALIAS=$(PACKAGE_NAME)
PACKAGE_EXTRAVERSION=

HEADERS= $(wildcard *.h)
SOURCES= libdthtreedyn.c

OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
PACKAGE_LIB_NAME= $(PACKAGE_NAME).so
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)

CFLAGS+=-I$(HATS_KERNEL_MACH_INCLUDE) \
		-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
		-I$(HATS_INC)/DthFramework

VARS+=ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADER_FILES)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)

$(INCLUDES):
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

install: $(LIBRARIES)  $(INCLUDES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES) $(HATS_INC)/$(PACKAGE_NAME)
