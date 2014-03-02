#
#***************************************************************
#
# Synopsis: TATLADC Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk
include $(HATS_CONFIG)

ifeq ($(HATSCONF_AB_LIB_8500),y)
CFLAGS+= -DHATSCONF_AB_LIB_8500
DTH_INPUT=tatladc_8500.xml
else ifeq ($(HATSCONF_AB_LIB_8505),y)
CFLAGS+= -DHATSCONF_AB_LIB_8505
DTH_INPUT=tatladc_8520.xml
endif

INPUT_DIR=AB_APE_lowlevel

DTH_BASE=$(OUTPUT)/tatl02adcgen
DTH_FILES=$(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad

CONFIG_DIR=config
MENU_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

MOD_VERS=`md5sum $(DTH_INPUT) | awk '{print $$1}'`

PACKAGE_NAME=tatladc
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES=tatl00adc.c tatl01adcfunc.c
SOURCES_DTH=$(DTH_BASE).c
HEADERS=dthadc.h tatl01adcfunc.h tatladc.h
HEADERS_DTH=$(DTH_BASE).h
SCRIPTS=$(wildcard scripts/*)
HATS_BIN_SCRIPTS=$(HATS_BIN)/scripts
HATS_SCRIPTS_SERVICES=$(HATS_BIN_SCRIPTS)/hats_services
SCRIPTS:=$(addprefix $(HATS_SCRIPTS_SERVICES)/,$(notdir $(SCRIPTS)))
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS=$(addprefix $(OUTPUT)/, $(SOURCES:.c=.o)) $(SOURCES_DTH:.c=.o)
LIBRAIRIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES=$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS))) \
	$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS_DTH)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

CFLAGS+= -DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/libab \
	-I$(HATS_INC)/$(INPUT_DIR) \
	-I$(OUTPUT) -I.

LDFLAGS+= -ldthsrvhelper -lm -lpthread -lab

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatladc -template $(DTH_RAD_DIR) -output "$(DTH_FILES)" \
	-prefix tatladc -mode make -header dthadc.h -version "$(MOD_VERS)" > /dev/null

$(OUTPUT)/%.o: $(OUTPUT)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(OUTPUT)/%.o: $(CURDIR)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRAIRIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)

$(MENUS):
	$(INSTALL_CMD) -D $(CURDIR)/$(MENU_DIR)/$(notdir $@) $@

$(PANELS):
	$(INSTALL_CMD) -D $(CURDIR)/$(PANEL_DIR)/$(notdir $@) $@

$(PLUGINS):
	$(MKDIR) -p $(HATS_LIB)/tat/plugins
	touch $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(OUTPUT)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(CURDIR)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(SCRIPTS):
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)

install: $(INCLUDES) $(LIBRAIRIES) $(SCRIPTS) $(PLUGINS) $(MENUS) $(PANELS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(SCRIPTS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
