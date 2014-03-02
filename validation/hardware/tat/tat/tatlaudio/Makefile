#
#**********************************************************************
#
# Synopsis: TATLAUDIO Makefile- Linux Application Build Rules.
#
#**********************************************************************
#

include .config.mk
include $(HATS_CONFIG)

PACKAGE_NAME=tatlaudio
DTH_BASE=$(OUTPUT)/tatl00audiodth

ifeq ($(HATSCONF_AB_LIB_8500),y)
$(info AB 8500)
SOURCES_PARAM= tatl03param8500.c
HEADER_PARAM= tatl03param8500.h
AUDIO_HEADER=AB_APE_lowlevel/ab8500_Audio_types.h
DTH_INPUT=tatlaudio8500.xml
CFLAGS+=-DHATS_AB_8500_HW
else ifeq ($(HATSCONF_AB_LIB_8505),y)
$(info AB 8520)
SOURCES_PARAM= tatl03param8520.c
HEADER_PARAM= tatl03param8520.h
AUDIO_HEADER=AB_APE_lowlevel/ab9540_Audio_types.h
DTH_INPUT=tatlaudio8520.xml
CFLAGS+=-DHATS_AB_8520_HW
endif

XML_PARAM_DEPS=$(CURDIR)/$(HEADER_PARAM) $(CURDIR)/$(SOURCES_PARAM) $(HATS_INC)/$(AUDIO_HEADER)

PACKAGE_NAME=tatlaudio
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

OUTPUT=$(HATS_OBJ)/$(PACKAGE_NAME)

SOURCES_COMMON= tatl01main.c tatl02func.c tatl00audiodth.c
OBJECTS_COMMON= $(addprefix $(OUTPUT)/,$(SOURCES_COMMON:.c=.o))
OBJECTS_PARAM= $(addprefix $(OUTPUT)/,$(SOURCES_PARAM:.c=.o))

DTH_BASE=$(OUTPUT)/tatl00audiodth
DTH_FILES=$(DTH_BASE).h $(DTH_BASE).c
DTH_RAD_DIR=$(HATS_OBJ)/tatrad

XML_PERL_FILE=$(OUTPUT)/tatparam2xml.pl
XML_AUDIO_FILE=$(OUTPUT)/$(DTH_INPUT)
XML_PARAM_FILE=$(OUTPUT)/audioparam.xml

#tatlaudio.xml
MOD_VERS1=`md5sum $(XML_AUDIO_FILE) | awk '{print $$1}'`
MOD_VERS2=`md5sum $(XML_PARAM_FILE) | awk '{ print $$1 }'`
MOD_VERS="$(MOD_VERS1)_$(MOD_VERS2)"


LIBRARIES=$(HATS_LIB)/$(PACKAGE_LIB_NAME)
PLUGINS_DIR=$(HATS_LIB)/tat/plugins
PLUGINS=$(PLUGINS_DIR)/$(PACKAGE_LIB_NAME)
HEADERS= dthaudio.h tatl02func.h $(HEADER_PARAM) tatlaudio.h
HEADERS_DTH=$(DTH_BASE).h

CONFIG_DIR=config
MENU_DIR=$(CONFIG_DIR)/menus
PANEL_DIR=$(CONFIG_DIR)/panels
PACKAGE_DATA_PATH=$(HATS_VARLOCAL)/tat/$(PACKAGE_NAME)
SYMBOLS=$(HATS_SYM)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS))) \
	$(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS_DTH)))
MENUS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(MENU_DIR)/*))
PANELS=$(addprefix $(PACKAGE_DATA_PATH)/, $(wildcard $(PANEL_DIR)/*))

CFLAGS+=-I$(HATS_KERNEL_MACH_INCLUDE) \
	-I$(HATS_INC)/AB_APE_lowlevel \
	-I$(HATS_INC)/libab \
	-I$(HATS_INC)/audioservices \
	-DCR_CAP_73_001_STW4500_AUDIOLOOP \
	-DTAT_PATH_OPT='"/$(HATS_SHARE_PATH)"' \
	-DTAT_PATH_VAR='"/$(HATS_VARLOCAL_PATH)"' \
	-I$(HATS_INC)/$(PACKAGE_NAME) \
	-I$(HATS_INC)/DthFramework \
	-g -D_DEBUG -fPIC


LDFLAGS+= -ldthsrvhelper -lAB_APE_lowlevel -laudioservices -lab -ltatasync

.PHONY: all install clean

all: install

$(DTH_FILES): $(DTH_INPUT) $(XML_PARAM_FILE)
	$(MKDIR) -p $(OUTPUT)
	$(TAT_PERL) $(DTH_RAD_DIR)/tatrad.pl $(DTH_INPUT) tatlaudio -template $(DTH_RAD_DIR) -output "$(DTH_FILES)" \
		-prefix tatlaudio -header dthaudio.h -version "$(MOD_VERS)" > /dev/null

$(XML_PARAM_FILE): $(XML_PARAM_DEPS)
	$(MKDIR) -p $(OUTPUT)
	$(LN) -sf $(CURDIR)/$(notdir $(XML_PERL_FILE)) $(XML_PERL_FILE)
	$(LN) -sf $(CURDIR)/$(notdir $(XML_AUDIO_FILE)) $(XML_AUDIO_FILE)
	$(LN) -sf $(CURDIR)/$(notdir $(XML_PARAM_FILE)) $(XML_PARAM_FILE)
	cd $(OUTPUT) && $(TAT_PERL)  $(XML_PERL_FILE) $(XML_PARAM_DEPS) $(XML_PARAM_FILE)

$(OUTPUT)/%.o: $(OUTPUT)/%.c $(HEADERS) $(PACKAGE_PARAM_FILE) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(OUTPUT)/%.o: $(CURDIR)/%.c $(HEADERS) $(HEADERS_DTH)
	$(MKDIR) -p $(HATS_OBJ)/$(PACKAGE_NAME)
	$(CC) -c $< $(CFLAGS) -o $(@)

$(SYMBOLS): $(OBJECTS_PARAM) $(OBJECTS_COMMON)
	$(CC) $(OBJECTS_PARAM) $(OBJECTS_COMMON) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(HATS_LIB)
	$(STRIP) --strip-unneeded $(@)

$(MENUS):
	$(INSTALL_CMD) -D $(CURDIR)/$(MENU_DIR)/$(notdir $@) $@

$(PANELS):
	$(INSTALL_CMD) -D $(CURDIR)/$(PANEL_DIR)/$(notdir $@) $@

$(PLUGINS):
	$(MKDIR) -p $(PLUGINS_DIR)
	touch $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(OUTPUT)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(HATS_INC)/$(PACKAGE_NAME)/%.h : $(CURDIR)/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $< $@

$(SCRIPTS):
	$(INSTALL_CMD) -D scripts/$(notdir $(@)) $(@)

install: $(INCLUDES) $(PACKAGE_PARAM_FILE) $(LIBRARIES) $(SCRIPTS) $(PLUGINS) $(MENUS) $(PANELS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(SYMBOLS) $(LIBRARIES) $(DTH_FILES)
	$(RM) -rf $(SCRIPTS) $(PACKAGE_PARAM_FILE) $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(PACKAGE_DATA_PATH)/$(MENU_DIR)
	$(RM) -rf $(PACKAGE_DATA_PATH)/$(PANEL_DIR)
	$(RM) -rf $(PLUGINS_DIR)/$(PACKAGE_LIB_NAME)

