#
#**********************************************************************
#
# Synopsis: sk-tools Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk

PACKAGE_NAME:=sk-tools
PACKAGE_PATH_PLUGINS=$(HATS_LOCALLIB_PATH)
HEADERS= $(wildcard *.h)
PACKAGE_EXEC_NAME=sk-db sk-i2c sk-ab
SOURCES= sk-db.c  sk-i2c.c sk-ab.c
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_NAME)
BINARIES=  $(addprefix $(HATS_BIN)/,$(PACKAGE_EXEC_NAME))

CFLAGS+=-DPACKAGE_PATH_PLUGINS='"/$(PACKAGE_PATH_PLUGINS)"' \
	-I$(HATS_INC)/libdb \
	-I$(HATS_INC)/libi2c \
	-I$(HATS_INC)/libab

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(HATS_BIN)/sk-ab: $(OUTPUT)/sk-ab.o
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(CC) $< $(LDFLAGS) -lab -O2 -o $(@)
	$(STRIP) --strip-debug --strip-unneeded $(HATS_BIN)/sk-ab

$(HATS_BIN)/sk-db: $(OUTPUT)/sk-db.o
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(CC)  $(OUTPUT)/sk-db.o $(LDFLAGS) -ldb -O2 -o  $(HATS_BIN)/sk-db
	$(STRIP) --strip-debug --strip-unneeded $(HATS_BIN)/sk-db

$(HATS_BIN)/sk-i2c:  $(OUTPUT)/sk-i2c.o
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(CC) $(OUTPUT)/sk-i2c.o $(LDFLAGS) -li2c -O2 -o $(HATS_BIN)/sk-i2c
	$(STRIP) --strip-debug --strip-unneeded $(HATS_BIN)/sk-i2c

$(INCLUDES):
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

install: $(BINARIES) $(INCLUDES)

clean:
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(BINARIES) $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(HATS_BIN)/tatd-i2c  $(HATS_BIN)/tatd-i2c  $(HATS_BIN)/tatd-stw4
