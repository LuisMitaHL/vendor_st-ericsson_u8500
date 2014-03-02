#
#***************************************************************
#
# Synopsis: TATLPWS Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk
include $(HATS_CONFIG)

ifeq ($(HATSCONF_AB_LIB_8505),y)
CFLAGS+= -DHATSCONF_AB_LIB_8505
endif

DTH_INPUT=tatlpws.xml
DTH_BASE=$(OUTPUT)/tatpwsdth
DTH_FILES=$(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad

CONFIG_DIR=config
MENU_CONFIG_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)
PACKAGE_FILES_PATH=$(HATS_LOCALBIN)/tat/$(PACKAGE_NAME)
PACKAGE_INPUT_FILES1=DBgpios.csv
PACKAGE_INPUT_FILES2=ABgpios.csv
#tatlpws.xml
MOD_VERS=`md5sum $(DTH_INPUT) | awk '{print $$1}'`

PACKAGE_NAME=tatlpws
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES=tatpws0dth.c tatpws1gpio.c tatpws2mode.c tatpws3opp.c tatpws4abgpio.c tatpws5rst.c
SOURCES_DTH=$(DTH_BASE).c
HEADERS=tatpws.h
HEADERS_DTH=$(DTH_BASE).h
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
SCRIPTS=$(wildcard scripts/*.*)
HATS_BIN_SCRIPTS=$(HATS_BIN)/scripts
HATS_SCRIPTS_SERVICES=$(HATS_BIN_SCRIPTS)/hats_services
SCRIPTS:=$(addprefix $(HATS_SCRIPTS_SERVICES)/,$(notdir $(SCRIPTS)))
OBJECTS=$(addprefix $(OUTPUT)/, $(SOURCES:.c=.o)) $(SOURCES_DTH:.c=.o)
LIBRAIRIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES=$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS))) \
	$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS_DTH)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_CONFIG_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

CFLAGS+=-I$(HATS_KERNEL)/drivers/char/hwreg \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/AB_APE_lowlevel \
	-I$(HATS_INC)/libab \
	-I$(HATS_INC)/libdb \
	-I$(HATS_INC)/powerservices \
	-I$(OUTPUT) -I. \
	-fPIC -g

LDFLAGS+=-lm -ldthsrvhelper -lpowerservices -lab -ldb -lpthread\
	-Wl,-soname,$(PACKAGE_LIB_NAME)

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatlpws -template $(DTH_RAD_DIR) -output "$(DTH_FILES)" \
		-prefix tatlpws -mode make -header tatpws.h -version "$(MOD_VERS)" > /dev/null

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
	$(MKDIR) -p $(PACKAGE_DATA_PATH)/$(MENU_CONFIG_DIR)

$(MENUS):
	$(INSTALL_CMD) -D $(CURDIR)/$(MENU_CONFIG_DIR)/$(notdir $@) $@

$(PANELS):
	$(INSTALL_CMD) -D $(CURDIR)/$(PANEL_DIR)/$(notdir $@) $@

$(PLUGINS):
	$(MKDIR) -p $(HATS_LIB)/tat/plugins
	$(INSTALL_CMD) -D $(PACKAGE_INPUT_FILES1) $(PACKAGE_DATA_PATH)/$(PACKAGE_INPUT_FILES1)
	$(INSTALL_CMD) -D $(PACKAGE_INPUT_FILES2) $(PACKAGE_DATA_PATH)/$(PACKAGE_INPUT_FILES2)
	touch $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(OUTPUT)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(CURDIR)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(SCRIPTS):
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)

install: $(LIBRAIRIES) $(INCLUDES) $(PLUGINS) $(MENUS) $(PANELS) $(SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(DTH_FILES) $(SCRIPTS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
	$(RM) -rf  $(HATS_BIN)/tat/$(PACKAGE_NAME)
