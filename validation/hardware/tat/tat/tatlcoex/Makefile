#
#**********************************************************************
#
# Synopsis: TAT COEXISTENCE Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk

DTH_INPUT=tatlcoex.xml
DTH_BASE=$(HATS_OBJ)/$(PACKAGE_NAME)/tatl11coexgen
DTH_FILES=$(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad
FM_RESULTS_DIR= /var/local/connectivityservices/connservices/FM_results.txt

CONFIG_DIR=config
MENU_CONFIG_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

#tatlcoex.xml
MOD_VERS=`md5sum $(DTH_INPUT) | awk '{print $$1}'`

PACKAGE_NAME=tatlcoex
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES=tatl00dthcoex.c tatl01misc.c tatl02memory.c tatl03display.c \
	tatl04pwm.c tatl05sensor.c tatl06i2c.c tatl07victim.c \
	tatl08audio.c tatl09modi2c.c tatl10apei2c.c tatl12conn.c
SOURCES_DTH=$(DTH_BASE).c
HEADERS=tatl01misc.h tatl02memory.h tatl03display.h \
	tatl04pwm.h tatl05sensor.h tatl06i2c.h tatl07victim.h \
	tatl08audio.h tatl09modi2c.h tatl10apei2c.h tatl12conn.h \
	dthcoex.h tatlcoex.h
HEADERS_DTH=$(DTH_BASE).h
SCRIPTS=$(wildcard scripts/*.*)
HATS_BIN_SCRIPTS=$(HATS_BIN)/scripts
HATS_SCRIPTS_SERVICES=$(HATS_BIN_SCRIPTS)/hats_services
SCRIPTS:=$(addprefix $(HATS_SCRIPTS_SERVICES)/,$(notdir $(SCRIPTS)))
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS=$(addprefix $(OUTPUT)/, $(SOURCES:.c=.o)) $(SOURCES_DTH:.c=.o)
LIBRAIRIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES=$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS))) \
	$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS_DTH)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_CONFIG_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

CFLAGS+=-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-DFM_RESULTS_PATH='"$(FM_RESULTS_DIR)"' \
	-I$(HATS_KERNEL_MACH_INCLUDE) \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/connservices \
	-I$(HATS_INC)/audioservices \
	-I$(HATS_INC)/tatlrf \
	-I$(HATS_INC)/libi2c \
	-I$(HATS_INC)/tatlsim \
	-I$(HATS_INC)/tatlvideo \
	-I$(HATS_OBJ)/tatlvideo \
	-I$(HATS_INC)/libab \
	-I$(OUTPUT) -I. \
	-fPIC -g

LDFLAGS+= -lpthread -lpng -lz -lm -lab -ldthsrvhelper -ltatlvideo -li2c -lconnservices -ltatlvideo -ltatlsim -ltatlrf

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatlcoex -template $(DTH_RAD_DIR) -output "$(DTH_FILES)" -prefix tatlcoex -mode make -header dthcoex.h -version "$(MOD_VERS)" > /dev/null

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
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)

install: $(LIBRAIRIES) $(INCLUDES) $(SCRIPTS) $(PLUGINS) $(MENUS) $(PANELS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(DTH_FILES) $(SCRIPTS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
