#
#***************************************************************
#
# Synopsis: Sk-utils Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=sk-utils
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_ALIAS=$(PACKAGE_NAME)

SOURCES= sk-utils.c
HEADERS= $(wildcard *.h)
SCRIPTS= sk.sh
SCRIPTS:= $(addprefix $(HATS_BIN)/,$(SCRIPTS))
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
SYMBOLS= $(HATS_SYM)/$(PACKAGE_EXEC_NAME)
BINARIES= $(HATS_BIN)/$(PACKAGE_EXEC_NAME)

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -O2 -o $(@)

$(BINARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_BIN)/%.sh:%.sh
	$(INSTALL_CMD) -D $< $@

install: $(BINARIES) $(SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(BINARIES) $(SCRIPTS)
