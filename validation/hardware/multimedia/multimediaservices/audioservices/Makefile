#
#***************************************************************
#
# Synopsis: audioservices Makefile - Linux Application Build Rules.
#
#***************************************************************
#
include .config.mk
include $(HATS_CONFIG)

ifeq ($(HATSCONF_AB_LIB_8500),y)
lAB=lAB_APE_lowlevel
AB_TYPE=8500
else ifeq ($(HATSCONF_AB_LIB_8505),y)
lAB=lAB_APE_lowlevel
AB_TYPE=8520
else
$(error AB library not selected)
endif

ifeq ($(HATSCONF_AB_HW_REVISION_1.0),y)
AB_VERSION=V1
else ifeq ($(HATSCONF_AB_HW_REVISION_2.0),y)
AB_VERSION=V2
else ifeq ($(HATSCONF_AB_HW_REVISION_3.0),y)
AB_VERSION=V3
else
$(error AB version not selected)
endif

PACKAGE_NAME:=audioservices
PACKAGE_EXEC_NAME=audioautotest modem_audio_stub  wavconv
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

ifeq ($(HATSCONF_AB_LIB_9540),y)
SOURCES_LIB= hats_audio_9540.c wavconv.c
SOURCES_AUDIOAUTOTEST= debug_main_9540.c
HEADERS_LOCAL= srce/hats_audio_9540.hi
else
SOURCES_LIB= hats_audio.c hats_4500loop.c wavconv.c
SOURCES_AUDIOAUTOTEST= debug_main.c
HEADERS_LOCAL= srce/hats_audio.hi
endif

SOURCES_WAVECONV=wavconv.c wavconvexe.c
SOURCES_MODEMAUDIOSTUB=modem_audio_stub.c
SCRIPT_PATH=$(HATS_SYSTEMFS)/usr/local/MMTE_scripts
HATS_BIN_AUDIO=$(HATS_SYSTEMFS)/usr/local/bin/audio

HEADERS= $(wildcard incl/*.h)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/, $(notdir $(HEADERS)))
OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS_LIB= $(addprefix $(OUTPUT)/,$(SOURCES_LIB:.c=.o))
OBJECTS_AUDIOAUTOTEST= $(addprefix $(OUTPUT)/,$(SOURCES_AUDIOAUTOTEST:.c=.o))
OBJECTS_WAVECONV= $(addprefix $(OUTPUT)/,$(SOURCES_WAVECONV:.c=.o))
OBJECTS_MODEMAUDIOSTUB= $(addprefix $(OUTPUT)/,$(SOURCES_MODEMAUDIOSTUB:.c=.o))
SYMBOLS_LIB= $(HATS_SYM)/$(PACKAGE_LIB_NAME)
SYMBOLS_EXEC= $(addprefix $(HATS_SYM)/,$(PACKAGE_EXEC_NAME))
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
BINARIES=$(addprefix $(HATS_BIN)/,$(PACKAGE_EXEC_NAME))
SCRIPTS=$(addprefix $(HATS_BIN)/,$(notdir $(wildcard scripts/*.sh)))
ATESCRIPTS=$(addprefix $(SCRIPT_PATH)/,$(notdir $(wildcard scripts/*.ate)))

AUDIO_BIN_SCRIPTS=$(addprefix $(HATS_BIN_AUDIO)/,$(notdir $(wildcard scripts/$(AB_TYPE)/$(AB_VERSION)/*.sh)))
AUDIO_SCRIPTS=scripts/$(AB_TYPE)/$(AB_VERSION)/%.sh

CFLAGS+= -fPIC -I$(HATS_KERNEL_MACH_INCLUDE) -I$(HATS_KERNEL_INCLUDE) \
	     -DCR_CAP_73_001_STW4500_AUDIOLOOP -I$(HATS_INC)/$(PACKAGE_NAME) \
		 -Werror -Wall -Wextra -O2

EXEFLAGS+=-$(lAB) -lpthread -lab

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: srce/%.c $(HEADERS) $(HEADERS_LOCAL)
	$(MKDIR) -p $(OUTPUT)
	$(CC) -c $(CFLAGS) $< -o $(@)

$(HATS_BIN)/%.sh: scripts/%.sh
	$(INSTALL_CMD) -D $< $(@)

$(HATS_BIN_AUDIO)/%.sh: $(AUDIO_SCRIPTS)
	$(INSTALL_CMD) -D $< $(@)

$(SCRIPT_PATH)/%.ate: scripts/%.ate
	$(INSTALL_CMD) -D $< $(@)

$(SYMBOLS_LIB): $(OBJECTS_LIB) $(INCLUDES)
	$(CC) $(OBJECTS_LIB) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(SYMBOLS_LIB)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(@)
	$(STRIP) --strip-unneeded $(@)

$(HATS_INC)/$(PACKAGE_NAME)/%.h: incl/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$< $(@)

$(HATS_SYM)/audioautotest: $(OBJECTS_AUDIOAUTOTEST) $(LIBRARIES)
	$(CC) $(OBJECTS_AUDIOAUTOTEST) $(LDFLAGS) $(EXEFLAGS) -l$(PACKAGE_NAME) -o $(@)

$(HATS_SYM)/wavconv: $(OBJECTS_WAVECONV)
	$(CC) $(OBJECTS_WAVECONV) $(LDFLAGS) $(EXEFLAGS) -o $(@)

$(HATS_SYM)/modem_audio_stub: $(OBJECTS_MODEMAUDIOSTUB)
	$(CC) $(OBJECTS_MODEMAUDIOSTUB) $(LDFLAGS) $(EXEFLAGS) -o $(@)

$(HATS_BIN)/%: $(HATS_SYM)/%
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $(@)
	$(STRIP) --strip-unneeded $(@)

install: $(INCLUDES) $(LIBRARIES) $(BINARIES) $(SCRIPTS) $(ATESCRIPTS) $(AUDIO_BIN_SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME) $(OUTPUT) $(SYMBOLS)
	$(RM) -rf $(LIBRARIES) $(BINARIES) $(SYMBOLS_EXEC) $(SCRIPTS) $(ATESCRIPTS) $(AUDIO_BIN_SCRIPTS)
