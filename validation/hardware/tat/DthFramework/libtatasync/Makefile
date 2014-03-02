#
#***************************************************************
#
# Synopsis: Asynchronous event Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

PACKAGE_NAME=tatasync
PACKAGE_EXEC_NAME=asyncEventd
PACKAGE_EXEC_NAME_TEST=asyncEvent_test
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES=tatasync.c
SOURCES_EXEC=asyncEventd.c
SOURCES_EXEC_TEST=test.c
HEADERS= $(wildcard *.h)
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
OBJECTS_EXEC= $(addprefix $(OUTPUT)/,$(SOURCES_EXEC:.c=.o))
OBJECTS_EXEC_TEST= $(addprefix $(OUTPUT)/,$(SOURCES_EXEC_TEST:.c=.o))
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
SYMBOLS= $(HATS_SYM)/$(PACKAGE_LIB_NAME)
SYMBOLS_EXEC= $(HATS_SYM)/$(PACKAGE_EXEC_NAME)
SYMBOLS_EXEC_TEST= $(HATS_SYM)/$(PACKAGE_EXEC_NAME_TEST)
INCLUDES= $(HATS_INC)/$(PACKAGE_NAME)
BINARIES= $(HATS_BIN)/$(PACKAGE_EXEC_NAME)
TEST_BINARIES= $(HATS_BIN)/$(PACKAGE_EXEC_NAME_TEST)

CFLAGS+=-I$(HATS_INC) \
	-I$(HATS_INC)/DthFramework
LDFLAGS+= -lssl -lcrypto
EXEFLAGS+=-ltatasync

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)

$(SYMBOLS_EXEC_TEST): $(OBJECTS_EXEC_TEST) $(LIBRARIES)
	$(CC) $< $(LDFLAGS) $(EXEFLAGS) -o $(@)

$(SYMBOLS_EXEC):  $(OBJECTS_EXEC) $(LIBRARIES)
	$(CC) $<  $(CFLAGS) $(LDFLAGS) $(EXEFLAGS) -o $(@)

$(BINARIES): $(SYMBOLS_EXEC)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_BIN)
	$(STRIP) --strip-unneeded $(@)

$(TEST_BINARIES): $(SYMBOLS_EXEC_TEST)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_BIN)
	$(STRIP) --strip-unneeded $(@)

install: $(LIBRARIES) $(BINARIES) $(TEST_BINARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(SYMBOLS_EXEC) $(SYMBOLS_EXEC_TEST)
	$(RM) -rf $(LIBRARIES) $(BINARIES) $(TEST_BINARIES)
