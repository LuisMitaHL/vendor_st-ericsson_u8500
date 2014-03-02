#
#***************************************************************
#
# Synopsis: TATLHDMI Makefile- Linux Application Build Rules.
#
#***************************************************************
#

include .config.mk

DTH_INPUT=tatlhdmi.xml
DTH_BASE=$(OUTPUT)/tatl02hdmigen
DTH_FILES=$(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad

CONFIG_DIR=config
MENU_CONFIG_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)

#tatlhdmi.xml
MOD_VERS=`md5sum $(DTH_INPUT) | awk '{print $$1}'`

PACKAGE_NAME=tatlhdmi
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES= tatl00hdmi.c tatl01hdmifunc.c
SOURCES_DTH=$(DTH_BASE).c
HEADERS= tatlhdmi.h tatl01hdmifunc.h dthhdmi.h hats_hdmi.h
HEADERS_DTH=$(DTH_BASE).h
OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS=$(addprefix $(OUTPUT)/, $(SOURCES:.c=.o)) $(SOURCES_DTH:.c=.o)
LIBRAIRIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES=$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS))) \
	$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS_DTH)))
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_CONFIG_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))
PLUGINS=$(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)

PACKAGE_EXEC_NAME=tathdmi

SOURCES_EXE= tathdmi.c
HEADERS_EXE= tatlhdmi.h tatl01hdmifunc.h hats_hdmi.h
OUTPUT_EXE=$(HATS_OBJ)/$(PACKAGE_EXEC_NAME)
OBJECTS_EXE=$(addprefix $(OUTPUT_EXE)/,$(SOURCES_EXE:.c=.o))
BINARIES=$(HATS_BIN)/$(PACKAGE_EXEC_NAME)
SYMBOLS_EXE=$(HATS_SYM)/$(PACKAGE_EXEC_NAME)

CFLAGS+=-I$(HATS_KERNEL_MACH_INCLUDE) \
	-I$(HATS_KERNEL_INCLUDE) \
	-I$(HATS_KERNEL)/drivers/ \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-I$(HATS_INC)/DthFramework \
	-I$(HATS_INC)/libdthfs \
	-I$(HATS_INC)/$(PACKAGE_NAME) \
	-I$(OUTPUT) -I.


LDFLAGS+=$(HATS_SYM)/libdthfs.a

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatlhdmi -template $(DTH_RAD_DIR) -output "$(DTH_FILES)" \
		-prefix tatlhdmi -mode make -header dthhdmi.h -version "$(MOD_VERS)" > /dev/null

$(OUTPUT_EXE)/%.o: %.c $(HEADERS_EXE) $(HEADERS_DTH)
	$(MKDIR) -p $(OUTPUT_EXE)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(SYMBOLS_EXE): $(OBJECTS_EXE) $(LIBRAIRIES)
	$(CC) $(OBJECTS_EXE) $(LDFLAGS) $(EXEFLAGS) -o $(@) -ltatlhdmi -ldthsrvhelper $(HATS_SYM)/libdthfs.a  -lxml2

$(BINARIES): $(LIBRAIRIES) $(SYMBOLS_EXE)
	$(ECHO) -e "\t-----> $(PACKAGE_EXEC_NAME) : $@"
	$(INSTALL_CMD) -D $(SYMBOLS_EXE) $(HATS_BIN)
	$(STRIP) --strip-unneeded $(@)

$(OUTPUT)/%.o: $(OUTPUT)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -fPIC -g -o $(@)

$(OUTPUT)/%.o: $(CURDIR)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -fPIC -g -o $(@)


$(SYMBOLS): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBFLAGS) -o $(@) -ldthsrvhelper

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

install: $(INCLUDES) $(LIBRAIRIES) $(BINARIES) $(PLUGINS) $(MENUS) $(PANELS)
	$(LN) -sf $(PACKAGE_EXEC_NAME) $(HATS_BIN)/dthhdmi

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRAIRIES) $(BINARIES) $(DTH_FILES)
	$(RM) -rf $(OUTPUT_EXE) $(SYMBOLS_EXE) $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)/$(MENU_CONFIG_DIR)
	$(RM) -rf $(PACKAGE_DATA_PATH)/$(PANEL_DIR)
	$(RM) -rf $(HATS_LIB)/tat/plugins/$(PACKAGE_LIB_NAME)
