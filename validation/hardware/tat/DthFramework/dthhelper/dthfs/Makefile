#
#***************************************************************
#
# Synopsis: DTHFS Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk


PACKAGE_NAME=dthfs
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).a

SOURCES= dth_filesystem.c dth_porting.c
HEADERS= $(wildcard *.h)
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
LIBRARIES= $(HATS_SYM)/$(PACKAGE_LIB_NAME)

CFLAGS+= -I$(HATS_INC)/DthFramework/dthfs \
	-I$(HATS_INC)/DthFramework

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(LIBRARIES): $(OBJECTS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(AR) rc  $(LIBRARIES) $(OBJECTS)
	ranlib $(LIBRARIES)

install: $(LIBRARIES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(LIBRARIES)

