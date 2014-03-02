#
#**********************************************************************
#
# Synopsis: TATLMODEM Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk
include $(HATS_CONFIG)

DTH_INPUT=tatlmodem.xml
DTH_BASE=$(OUTPUT)/tatl00modemdth
DTH_FILES= $(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad

ifeq ($(HATSCONF_MODEM_TYPE_U8500),y)
HATS_MODEM_INC=$(BUILD_TOP)/modem/u8500/include
endif

CONFIG_DIR=config
MENU_CONFIG_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

#tatlmodem.xml
MOD_VERS=`md5sum $(DTH_INPUT) | awk '{print $$1}'`

PACKAGE_NAME=tatlmodem
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so
SOURCE_DIR= $(CURDIR)/srce
SOURCES=srce/tatl01main.c
SOURCES_DTH=$(DTH_BASE).c
HEADERS=srce/tatlmodem.h
HEADERS_DTH=$(DTH_BASE).h
SCRIPTS=$(wildcard scripts/*.*)
HATS_BIN_SCRIPTS=$(HATS_BIN)/scripts
HATS_SCRIPTS_SERVICES=$(HATS_BIN_SCRIPTS)/hats_services
SCRIPTS:=$(addprefix $(HATS_SCRIPTS_SERVICES)/,$(notdir $(SCRIPTS)))
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS=$(addprefix $(OUTPUT)/,  $(notdir $(SOURCES:.c=.o))) $(SOURCES_DTH:.c=.o)
LIBRAIRIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES=$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS))) \
	$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS_DTH)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_CONFIG_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

CFLAGS+= -Isrce \
	-I$(HATS_INC)/powerservices \
	-I$(HATS_MODEM_INC)/rmc_headers \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/tatlmodem \
	-I$(HATS_INC)/tatmodemlib \
	-I$(HATS_INC)/isi_common \
	-I$(OUTPUT) -I. \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-fPIC -g

ifeq ($(HATSCONF_PHONET_HATS),y)
CFLAGS+=-DCONFIG_PHONET
endif

LDFLAGS+= -ldthsrvhelper -ltatmodemlib -lpowerservices

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatlmodem -template $(DTH_RAD_DIR) -output "$(DTH_FILES)" \
		-prefix tatlmodem -mode make -header tatlmodem.h -version "$(MOD_VERS)" > /dev/null

$(OUTPUT)/%.o: $(OUTPUT)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(OUTPUT)/%.o: $(SOURCE_DIR)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -Wl,-soname,$(PACKAGE_LIB_NAME) -o $(@)

$(LIBRAIRIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)

$(MENUS):
	$(INSTALL_CMD) -D $(CURDIR)/$(MENU_CONFIG_DIR)/$(notdir $@) $@

$(PANELS):
	$(INSTALL_CMD) -D $(CURDIR)/$(PANEL_DIR)/$(notdir $@) $@

$(PLUGINS):
	$(MKDIR) -p $(HATS_LIB)/tat/plugins
	touch $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(OUTPUT)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(SOURCE_DIR)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(SCRIPTS):
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)

install: $(INCLUDES) $(SCRIPTS) $(LIBRAIRIES) $(PLUGINS) $(MENUS) $(PANELS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(DTH_FILES) $(SCRIPTS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

