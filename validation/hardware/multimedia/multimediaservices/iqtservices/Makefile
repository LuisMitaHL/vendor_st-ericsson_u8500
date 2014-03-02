#/*****************************************************************************/
#/**
#*  Copyright (C) ST-Ericsson 2009
#*  Reproduction and Communication of this document is strictly prohibited
#*  unless specifically authorized in writing by ST-Ericsson
#*
#* \brief   Makefile for Video tests (Display (Main, Sub, TV-OUT), Camera)
#* \author  ST-Ericsson
#*/
#/*****************************************************************************/

include .config.mk

PACKAGE_NAME=iqtservices
PACKAGE_EXEC_NAME= MmteOutputConsoleManagement IqtServices
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so
LIB_SOURCES= IqtServices.c
MMTE_OUT_MGT_SOURCES= MmteOutputConsoleManagement.c
IQT_SERVICES_SOURCES= IqtTests.c
HEADERS= incl/IqtServices.h

INIT_SCRIPTS_PATH=$(HATS_SHARE)/mm-valid/imaging
INIT_SCRIPTS=$(addprefix $(INIT_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/init/*.ite)))
USE_CASE_SCRIPTS_PATH=$(HATS_SHARE)/mm-valid/imaging/ite_omx_use_cases_for_IQT/imaging_scripts/intern_usecases
USE_CASE_SCRIPTS=$(addprefix $(USE_CASE_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/pe/*.ite))) \
                 $(addprefix $(USE_CASE_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/state_machine/*.ite))) \
                 $(addprefix $(USE_CASE_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/still/*.ite))) \
                 $(addprefix $(USE_CASE_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/test/*.ite))) \
                 $(addprefix $(USE_CASE_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/video/*.ite))) \
                 $(addprefix $(USE_CASE_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/viewfinder/*.ite)))

OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS_LIB= $(addprefix $(OUTPUT)/,$(LIB_SOURCES:.c=.o))
OBJECTS_MMTE_OUT_MGT= $(addprefix $(OUTPUT)/,$(MMTE_OUT_MGT_SOURCES:.c=.o))
OBJECTS_IQT_SERVICES= $(addprefix $(OUTPUT)/,$(IQT_SERVICES_SOURCES:.c=.o))
LIB_SYMBOLS= $(HATS_SYM)/$(PACKAGE_LIB_NAME)
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/,$(notdir $(HEADERS)))
BINARIES=$(addprefix $(HATS_BIN)/,$(PACKAGE_EXEC_NAME))

CFLAGS+= -I./incl -I$(HATS_KERNEL_MACH_INCLUDE) -Wall \

LDFLAGS+=-lm

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: srce/%.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(INIT_SCRIPTS_PATH)/%.ite: mmte_scripts/init/%.ite
	$(INSTALL_CMD) -D $< $@

$(USE_CASE_SCRIPTS_PATH)/%.ite: mmte_scripts/pe/%.ite
	$(INSTALL_CMD) -D $< $@

$(USE_CASE_SCRIPTS_PATH)/%.ite: mmte_scripts/state_machine/%.ite
	$(INSTALL_CMD) -D $< $@

$(USE_CASE_SCRIPTS_PATH)/%.ite: mmte_scripts/still/%.ite
	$(INSTALL_CMD) -D $< $@

$(USE_CASE_SCRIPTS_PATH)/%.ite: mmte_scripts/test/%.ite
	$(INSTALL_CMD) -D $< $@

$(USE_CASE_SCRIPTS_PATH)/%.ite: mmte_scripts/video/%.ite
	$(INSTALL_CMD) -D $< $@

$(USE_CASE_SCRIPTS_PATH)/%.ite: mmte_scripts/viewfinder/%.ite
	$(INSTALL_CMD) -D $< $@

$(LIB_SYMBOLS): $(OBJECTS_LIB)
	$(CC) $(OBJECTS_LIB) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(LIB_SYMBOLS)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_SYM)/MmteOutputConsoleManagement: $(OBJECTS_MMTE_OUT_MGT) $(LIBRARIES)
	$(CC) $(OBJECTS_MMTE_OUT_MGT) $(LDFLAGS) $(EXEFLAGS) -o $(@)

$(HATS_SYM)/IqtServices: $(OBJECTS_IQT_SERVICES) $(LIBRARIES)
	$(CC) $(OBJECTS_IQT_SERVICES) $(LDFLAGS) $(EXEFLAGS) -l$(PACKAGE_NAME) -o $(@)

$(HATS_BIN)/%: $(HATS_SYM)/%
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_INC)/$(PACKAGE_NAME)/%.h: incl/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$< $(@)

install: $(INCLUDES) $(LIBRARIES) $(BINARIES) $(INIT_SCRIPTS) $(USE_CASE_SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(LIBRARIES) $(LIB_SYMBOLS) $(BIN_SYMBOLS)
	$(RM) -rf $(BINARIES) $(INIT_SCRIPTS) $(USE_CASE_SCRIPTS)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
