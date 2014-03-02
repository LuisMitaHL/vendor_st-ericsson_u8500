#
#**********************************************************************
#
# Synopsis: HATS RF Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk
include $(HATS_CONFIG)

DTH_INPUT=hatsrf.xml
DTH_BASE=$(OUTPUT)/hatsrf
DTH_FILES=$(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad
PACKAGE_PATH=tatlrf

ifeq ($(HATSCONF_MODEM_TYPE_U8500),y)
HATS_MODEM_INC=$(BUILD_TOP)/modem/u8500/include
endif

CONFIG_DIR=config
MENU_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)
CSV_FILE_NAME=rfparams.csv
TATLRF_DATA_FILE_PATH=$(HATS_VARLOCAL_PATH)/tat/tatlrf
TATLRF_RX_IQ_SAMPLES_SCRIPT=$(HATS_LOCALLIB_PATH)/$(PACKAGE_PATH)

#tatlrf.xml
MOD_VERS1=`md5sum hatsrf.xml | awk '{print $$1}'`
MOD_VERS2=`md5sum rfparams.csv | awk '{ print $$1 }'`
MOD_VERS="$(MOD_VERS1)_$(MOD_VERS2)"

PACKAGE_NAME=tatlrf
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES=$(wildcard *.c)
SOURCES_DTH=$(DTH_BASE).c
HEADERS= $(wildcard *.h)
HEADERS_DTH=$(DTH_BASE).h
SCRIPTS= $(wildcard scripts/*.*)
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

RF_DATA_FILE_PATH=$(HATS_VARLOCAL_PATH)/tat/$(PACKAGE_PATH)
RF_RX_IQ_SAMPLES_SCRIPT=$(HATS_VARLOCAL_PATH)/tat/$(PACKAGE_PATH)
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

CFLAGS+=-I$(HATS_MODEM_INC)/rf_headers \
	-I$(HATS_MODEM_INC)/rmc_headers \
	-I$(HATS_INC)/isi_common -I$(HATS_INC)/isi_driver \
	-I$(HATS_INC)/modem_lib -I$(HATS_INC)/tatmodemlib \
	-I$(HATS_INC)/DthFramework \
	-I$(OUTPUT) -I. \
	-DRF_DATA_FILE_DIR='"/$(RF_DATA_FILE_PATH)"' \
	-DTATLRF_DATA_FILE_DIR='"/$(TATLRF_DATA_FILE_PATH)"' \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-DIQSAMPLE_SCRIPT_DIR='"/$(RF_RX_IQ_SAMPLES_SCRIPT)"' \
	-DTATLRF_RX_IQ_SCRIPT='"/$(TATLRF_RX_IQ_SAMPLES_SCRIPT)"' \
	-g -fPIC

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatlrf --template $(DTH_RAD_DIR) --output "$(DTH_FILES)" \
		--prefix tatlrf --mode make --header "dthrf.h selftest.h" --version "$(MOD_VERS)" --incl "isimiss.h \
		$(HATS_MODEM_INC)/hal_headers/*.h $(HATS_MODEM_INC)/rmc_headers/*.h"

$(OUTPUT)/%.o: $(OUTPUT)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(OUTPUT)/%.o: $(CURDIR)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -o $(@) -ldthsrvhelper -ltatmodemlib -lpthread

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
	$(MKDIR) -p $(HATS_BIN)/tat/$(PACKAGE_NAME)
	cp $(CSV_FILE_NAME) $(HATS_BIN)/tat/$(PACKAGE_NAME)
	$(INSTALL_CMD) -D $(CSV_FILE_NAME) $(PACKAGE_DATA_PATH)/$(CSV_FILE_NAME)

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(OUTPUT)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(CURDIR)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(SCRIPTS):
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)

install:  $(INCLUDES) $(LIBRAIRIES) $(SCRIPTS) $(PLUGINS) $(MENUS) $(PANELS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(DTH_FILES) $(SCRIPTS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
	$(RM) -rf $(HATS_BIN)/tat/$(PACKAGE_NAME)
