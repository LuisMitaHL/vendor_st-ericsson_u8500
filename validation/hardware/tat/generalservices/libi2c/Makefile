#
#***************************************************************
#
# Synopsis: LIBI2C Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

DEPENDENCES=

PACKAGE_NAME:=libi2c
PACKAGE_LIB_NAME=$(PACKAGE_NAME).so

HEADERS= $(wildcard *.h)
SOURCES_LIB= libi2c.c
OBJECTS_LIB= $(addprefix $(OUTPUT)/,$(SOURCES_LIB:.c=.o))
SYMBOLS_LIB=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
BINARIES= $(HATS_BIN)/$(PACKAGE_EXEC_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS_LIB): $(OBJECTS_LIB)
	$(CC) $^ $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(SYMBOLS_LIB)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)

$(INCLUDES):
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

install: $(LIBRARIES) $(INCLUDES) $(BINARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(LIBRARIES) $(SYMBOLS_LIB)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
