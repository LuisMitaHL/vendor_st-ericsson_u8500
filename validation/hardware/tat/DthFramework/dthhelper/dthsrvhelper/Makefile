#
#**********************************************************************
#
# Synopsis: 9P SERVICES HELPER Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk

PACKAGE_NAME=dthsrvhelper
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES= dthsrvhelper.c dthsrvpub.c dthsrvhelper_9p_ifaces.c dthsrvhelper_error.c
HEADERS= $(wildcard *.h)
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)

CFLAGS+= -I$(HATS_INC)/DthFramework/dthsrvhelper \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/libxml2 \
	-DDEBUG_9PSERVER

LDFLAGS+= -L$(HATS_SYM) -lxml2 -ldthfs

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
	$(MKDIR) -p $(HATS_SHARE)/tat

install: $(LIBRARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES)

