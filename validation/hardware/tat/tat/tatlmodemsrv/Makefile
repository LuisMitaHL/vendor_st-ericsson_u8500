#
#**********************************************************************
#
# Synopsis: TATMODEMSERVICES Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk
include $(HATS_CONFIG)

PACKAGE_NAME=tatmodemlib
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES=tatmodemlib.c
HEADERS=tatmodemlib.h
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS=$(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
LIBRAIRIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)

ifeq ($(HATSCONF_MODEM_TYPE_U8500),y)
HATS_MODEM_INC=$(BUILD_TOP)/modem/u8500/include
endif

CFLAGS+=-I$(HATS_KERNEL_MACH_INCLUDE) \
	-I$(HATS_MODEM_INC)/rmc_headers \
	-I$(HATS_INC)/isi_common \
	-I$(HATS_INC)/isi_driver \
	-I$(HATS_INC)/modem_lib \
	-fPIC -g

ifeq ($(HATSCONF_PHONET_HATS),y)
CFLAGS+=-DCONFIG_PHONET
endif

LDFLAGS+= -lisi_common -lisi_driver -lmodem_lib

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -Wl,-soname,$(PACKAGE_LIB_NAME) -o $(@)

$(LIBRAIRIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)


$(INCLUDES):
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

install:  $(LIBRAIRIES) $(INCLUDES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
