#
#***************************************************************
#
# Synopsis: Libdb Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME:=libdb
PACKAGE_LIB_NAME=$(PACKAGE_NAME).so
SOURCES= $(wildcard *.c)
HEADERS= $(wildcard *.h)
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(AR) rc  $(SYMBOLS) $(OBJECTS)
	ranlib $(SYMBOLS)

$(LIBRARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)

$(INCLUDES):
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

install: $(LIBRARIES) $(INCLUDES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES) $(HATS_INC)/$(PACKAGE_NAME)
