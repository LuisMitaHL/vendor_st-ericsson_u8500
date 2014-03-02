#
#**********************************************************************
#
# Synopsis: genAPIdoc Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk

PACKAGE_NAME=genapidoc
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_ALIAS=$(PACKAGE_NAME)

SOURCES=genAPIdoc.c
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
SYMBOLS= $(HATS_SYM)/$(PACKAGE_EXEC_NAME)
BINARIES= $(HATS_BIN)/$(PACKAGE_EXEC_NAME)

LDFLAGS+=-ldthsrvhelper $(HATS_SYM)/libdthfs.a -ldl -Winline  -lxml2
CFLAGS+= -I$(HATS_INC)/DthFramework

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) $(EXEFLAGS)-O2 -o $(@)

$(BINARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_BIN)
	$(STRIP) --strip-unneeded $(@)

install: $(BINARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME): $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(BINARIES)
