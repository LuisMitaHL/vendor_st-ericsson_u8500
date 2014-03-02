#
#***************************************************************
#
# Synopsis: LpDDR2 Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=regddr2read
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_ALIAS=$(PACKAGE_NAME)

SOURCES= ddr2lib.c \
	main.c
HEADERS= $(wildcard *.h)
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
SYMBOLS= $(HATS_SYM)/$(PACKAGE_EXEC_NAME)
BINARIES= $(HATS_BIN)/$(PACKAGE_EXEC_NAME)


.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADER_FILES)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $(@)

$(BINARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

install: $(BINARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(BINARIES)
