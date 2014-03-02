#
#**********************************************************************
#
# Synopsis: TATLGPS Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk
include $(HATS_CONFIG)

DTH_INPUT=tatlgps.xml
DTH_BASE=$(OUTPUT)/tatlgpsdth
DTH_FILES=$(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad

ifeq ($(HATSCONF_MODEM_TYPE_U8500),y)
HATS_MODEM_INC=$(BUILD_TOP)/modem/u8500/include
endif

CONFIG_DIR=config
MENU_CONFIG_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

#tatlsim.xml
MOD_VERS=`md5sum $(DTH_INPUT) | awk '{print $$1}'`

PACKAGE_NAME=tatlgps
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES=tatl00dth.c tatl01selftest.c tatl02misc.c tatl03modem.c
SOURCES_DTH=$(DTH_BASE).c
HEADERS=dthgps.h tatl01selftest.h tatl02misc.h tatl03modem.h tatlgps.h
HEADERS_DTH=$(DTH_BASE).h
SCRIPTS=initmodem.sh selftest_timemark.sh selftest_tx.sh
SCRIPTS:=$(addprefix $(HATS_BIN)/,$(SCRIPTS))
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS=$(addprefix $(OUTPUT)/, $(SOURCES:.c=.o)) $(SOURCES_DTH:.c=.o)
LIBRAIRIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES=$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS))) \
	$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS_DTH)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_CONFIG_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

CFLAGS+=-I$(HATS_INC)/modem_lib/NWM_include \
	-I$(HATS_KERNEL_MACH_INCLUDE) \
	-I$(HATS_MODEM_INC)/rmc_headers \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/tatmodemlib \
	-I$(HATS_INC)/isi_common \
	-I$(OUTPUT) -I. \
	-fPIC -g

LDFLAGS+=-ldthsrvhelper -lisi_common -ltatmodemlib

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatlgps -template $(DTH_RAD_DIR) -output "$(DTH_FILES)" -prefix tatlgps -mode make -header dthgps.h -version "$(MOD_VERS)"> /dev/null

$(OUTPUT)/%.o: $(OUTPUT)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(OUTPUT)/%.o: $(CURDIR)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -Wl,-soname,$(PACKAGE_LIB_NAME) -o $(@)

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
	touch $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(OUTPUT)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(CURDIR)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(SCRIPTS):
	$(INSTALL_CMD) -D tstu/$(notdir $(@)) $(@)

install: $(INCLUDES) $(LIBRAIRIES) $(SCRIPTS) $(PLUGINS) $(MENUS) $(PANELS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(DTH_FILES) $(SCRIPTS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
