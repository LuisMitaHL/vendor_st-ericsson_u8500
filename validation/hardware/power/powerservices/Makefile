#
#***************************************************************
#
# Synopsis: powerservices Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk
include $(HATS_CONFIG)

PACKAGE_NAME=powerservices
PACKAGE_EXEC_NAME=$(PACKAGE_NAME)
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES= hats_power.c
HEADERS= hats_power.h
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS= $(addprefix $(OUTPUT)/,$(SOURCES:.c=.o))
SYMBOLS= $(HATS_SYM)/$(PACKAGE_LIB_NAME)
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(HEADERS))

ifeq ($(HATSCONF_PWS_9540),y)
CFLAGS+= -I$(HATS_INC)/libdb -I$(HATS_INC)/libab \
         -DPWS_9540 \
	-Wall -Wextra -O2 \
	-I$(HATS_INC)/dthlmodemmngt
LDFLAGS+=-ldthlmodemmngt
else
ifeq ($(HATSCONF_PWS_8540),y)
# reuse 9540, plus 8540
CFLAGS+= -I$(HATS_INC)/libdb -I$(HATS_INC)/libab \
	 -DPWS_9540 \
         -DPWS_8540 \
	-Wall -Wextra -O2 \
	-I$(HATS_INC)/dthlmodemmngt
LDFLAGS+=-ldthlmodemmngt
else
CFLAGS+= -I$(HATS_INC)/modem_lib -I$(HATS_INC)/isi_driver -I$(HATS_INC)/libdb -I$(HATS_INC)/libab \
		 -Werror -Wall -Wextra -O2
		 
LDFLAGS+= -lmodem_lib -lisi_driver
endif
endif

.PHONY: all install uninstall clean

all: install

$(OUTPUT)/%.o: %.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(SYMBOLS) $(INCLUDES)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_INC)/$(PACKAGE_NAME)/%.h: $(CURDIR)/%.h
	$(MKDIR) -p $(dir $@)
	$(LN) -sf $< $@

install: $(LIBRARIES) $(INCLUDES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES) $(INCLUDES)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
