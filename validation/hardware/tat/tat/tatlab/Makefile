#
#***************************************************************
#
# Synopsis: TATLAB Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk
include $(HATS_CONFIG)

ifeq ($(HATSCONF_AB_LIB_8500),y)
DEVICENAME=8500
XML_FILE_NAME=AB8500.xml
else ifeq ($(HATSCONF_AB_LIB_8505),y)
DEVICENAME=9540
XML_FILE_NAME=AB9540.xml
endif

INPUT_DIR=AB_APE_lowlevel

CONFIG_DIR=config
MENU_CONFIG_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
SCRIPTS=$(wildcard scripts/*.txt)
HATS_BIN_SCRIPTS=$(HATS_BIN)/scripts
HATS_SCRIPTS_SERVICES=$(HATS_BIN_SCRIPTS)/hats_services
SCRIPTS:=$(addprefix $(HATS_SCRIPTS_SERVICES)/,$(notdir $(SCRIPTS)))

DTH_GENXML_DIR=$(HATS_OBJ)/libtreedyn
XML_FILE=$(PACKAGE_DATA_PATH)/$(XML_FILE_NAME)

PACKAGE_NAME:=tatlab
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

HEADERS= $(wildcard *.h)
SOURCES= tatl00ab.c  tatl01abfunc.c
OBJECTS= $(addprefix $(OUTPUT)/,$(notdir $(SOURCES:.c=.o)))
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_CONFIG_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

CFLAGS+=-I$(HATS_KERNEL_MACH_INCLUDE) \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/libab \
	-I$(HATS_INC)/libdthtreedyn \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"'

LDFLAGS+= -ldthtreedyn -ldthsrvhelper -lab

.PHONY: genxml all install clean

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

$(MENUS):
	$(INSTALL_CMD) -D $(CURDIR)/$(MENU_CONFIG_DIR)/$(notdir $@) $@

$(PANELS):
	$(INSTALL_CMD) -D $(CURDIR)/$(PANEL_DIR)/$(notdir $@) $@

$(PLUGINS): $(XML_FILE)
	$(MKDIR) -p $(HATS_LIB)/tat/plugins
	touch $@
	$(INSTALL_CMD) -D $(OUTPUT)/$(notdir $(XML_FILE)) $(PACKAGE_DATA_PATH)/$(XML_FILE_NAME)

$(INCLUDES):
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$(notdir $@) $@

$(XML_FILE):
	$(ECHO) -e "\txml----> $(PACKAGE_NAME) : $@"
	$(MKDIR) -p $(OUTPUT)
	$(LN) -sf $(CURDIR)/analogbaseband.xml $(OUTPUT)/analogbaseband.xml
	$(ECHO) -e "Will generate XML"
	cd $(OUTPUT) && $(TAT_PERL) $(DTH_GENXML_DIR)/TATReg2xml.pl -ab $(DEVICENAME) -path $(HATS_INC)/$(INPUT_DIR)
	$(ECHO) -e "Generated XML"
	$(INSTALL_CMD) -D $(OUTPUT)/$(notdir $(XML_FILE)) $@

$(SCRIPTS):
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)


install: $(LIBRARIES) $(INCLUDES) $(PLUGINS) $(MENUS) $(PANELS) $(XML_FILE) $(SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES) $(DTH_FILES) $(SCRIPTS) $(HATS_INC)/$(INPUT_DIR)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(ADDONS_IMAGES_DIR)/$(PACKAGE_DATA_PATH)/$(MENU_CONFIG_DIR)
	$(RM) -rf $(ADDONS_IMAGES_DIR)/$(PACKAGE_DATA_PATH)/$(PANEL_DIR)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
	$(RM) -rf $(XML_FILE)
