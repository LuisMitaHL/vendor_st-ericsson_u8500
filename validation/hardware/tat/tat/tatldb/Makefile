#
#***************************************************************
#
# Synopsis: TATLDB Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk
include $(HATS_CONFIG)

ifeq ($(HATSCONF_DB_HW_REVISION_8500),y)
XML_FILE_NAME=DB8500_V2.xml
else ifeq ($(HATSCONF_DB_HW_REVISION_X600),y)
XML_FILE_NAME=x600.xml
else
$(error DB Hardware revision not selected)
endif

PACKAGE_NAME:=tatldb
CONFIG_DIR=config
MENU_CONFIG_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

DTH_GENXML_DIR=../libtreedyn

XML_FILE=$(PACKAGE_DATA_PATH)/$(XML_FILE_NAME)

HEADERS= $(wildcard *.h)
SOURCES= tatl00db.c  tatl01dbfunc.c
OBJECTS= $(addprefix $(OUTPUT)/,$(notdir $(SOURCES:.c=.o)))
SCRIPTS=$(wildcard scripts/*.*)
HATS_BIN_SCRIPTS=$(HATS_BIN)/scripts
HATS_SCRIPTS_SERVICES=$(HATS_BIN_SCRIPTS)/hats_services
SCRIPTS:=$(addprefix $(HATS_SCRIPTS_SERVICES)/,$(notdir $(SCRIPTS)))
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_CONFIG_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

CFLAGS+=-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/libdb \
	-I$(HATS_INC)/libdthtreedyn \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"'

LDFLAGS+= -ldthtreedyn -ldthsrvhelper -ldb

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
	$(MKDIR) -p $(PACKAGE_DATA_PATH)/$(MENU_CONFIG_DIR)

$(INCLUDES):
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

$(MENUS):
	$(INSTALL_CMD) -D $(CURDIR)/$(MENU_CONFIG_DIR)/$(notdir $@) $@

$(PANELS):
	$(INSTALL_CMD) -D $(CURDIR)/$(PANEL_DIR)/$(notdir $@) $@

$(PLUGINS):
	$(MKDIR) -p $(HATS_LIB)/tat/plugins
	touch $@

$(XML_FILE): $(CURDIR)/digitalbaseband.xml
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(MKDIR) -p $(OUTPUT)
	$(LN) -sf $(CURDIR)/digitalbaseband.xml $(OUTPUT)/digitalbaseband.xml
	$(ECHO) -e "Will generate XML"
ifeq ($(HATSCONF_DB_HW_REVISION_8500),y)
	$(LN) -sf $(CURDIR)/DB8500_V2.per $(OUTPUT)/DB8500_V2.per
	cd $(OUTPUT) && $(MKDIR) -p $(HATS_INC)/DB8500_lowlevel && \
		$(TAT_PERL) $(DTH_GENXML_DIR)/TATReg2xml.pl -db 8500 -version V2 \
		-discard CRYP-0,HASH-0,MSS,PKA,RNG,SCR -header $(HATS_INC)/DB8500_lowlevel/db8500_registers.h
else ifeq ($(HATSCONF_DB_HW_REVISION_X600),y)
	$(LN) -sf $(CURDIR)/DB8500_V2.per $(OUTPUT)/x600.per
	cd $(OUTPUT) && $(TAT_PERL) $(DTH_GENXML_DIR)/TATReg2xml.pl -db -version x600

else
$(error DB Hardware revision not selected)
endif

	$(ECHO)  -e "Generated XML"
	$(INSTALL_CMD) -D $(OUTPUT)/$(notdir $(XML_FILE)) $@

$(SCRIPTS):
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)

install: $(INCLUDES) $(XML_FILE) $(LIBRARIES) $(PLUGINS) $(MENUS) $(PANELS) $(SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES) $(DTH_FILES) $(SCRIPTS) $(HATS_INC)/DB8500_lowlevel
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)/$(MENU_CONFIG_DIR)
	$(RM) -rf $(PACKAGE_DATA_PATH)/$(PANEL_DIR)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
	$(RM) -rf $(XML_FILE)
