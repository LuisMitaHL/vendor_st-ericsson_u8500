#/*****************************************************************************/
#/**
#*  Copyright (C) ST-Ericsson 2009
#*  Reproduction and Communication of this document is strictly prohibited
#*  unless specifically authorized in writing by ST-Ericsson
#*
#* \brief   Makefile for Camera tests
#* \author  ST-Ericsson
#*/
#/*****************************************************************************/

include .config.mk

PACKAGE_NAME=cameraservices
PACKAGE_EXEC_NAME= MmteOutputManagement CameraTests
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so

SOURCES_LIB= CameraServices.c
SOURCES_MMTE= MmteOutputManagement.c
SOURCES_CAMERATESTS= CameraTests.c
HEADERS= incl/CameraServices.h

INIT_SCRIPTS_PATH=$(HATS_SHARE)/mm-valid/imaging
INIT_SCRIPTS=$(addprefix $(INIT_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/init/*.ite)))
USE_CASE_SCRIPTS_PATH=$(HATS_SHARE)/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases
USE_CASE_SCRIPTS=$(addprefix $(USE_CASE_SCRIPTS_PATH)/,$(notdir $(wildcard mmte_scripts/use_cases/*.ite)))

OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS_LIB= $(addprefix $(OUTPUT)/,$(SOURCES_LIB:.c=.o))
OBJECTS_MMTE= $(addprefix $(OUTPUT)/,$(SOURCES_MMTE:.c=.o))
OBJECTS_CAMERATESTS= $(addprefix $(OUTPUT)/,$(SOURCES_CAMERATESTS:.c=.o))
SYMBOLS_LIB= $(addprefix $(HATS_SYM)/,$(PACKAGE_LIB_NAME))
SYMBOLS_BIN= $(addprefix $(HATS_SYM)/,$(PACKAGE_EXEC_NAME))
LIBRARIES= $(addprefix $(HATS_LIB)/,$(PACKAGE_LIB_NAME))
BINARIES=$(addprefix $(HATS_BIN)/,$(PACKAGE_EXEC_NAME))
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/,$(notdir $(HEADERS)))

CFLAGS+= -Iincl

LDFLAGS+=-lm

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: srce/%.c $(HEADERS)
	$(MKDIR) -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(INIT_SCRIPTS_PATH)/%.ite: mmte_scripts/init/%.ite
	$(INSTALL_CMD) -D $< $@

$(USE_CASE_SCRIPTS_PATH)/%.ite: mmte_scripts/use_cases/%.ite
	$(INSTALL_CMD) -D $< $@

$(SYMBOLS_LIB): $(OBJECTS_LIB)
	$(CC) $(OBJECTS_LIB) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(SYMBOLS_LIB)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_SYM)/CameraTests: $(OBJECTS_CAMERATESTS) $(LIBRARIES)
	$(CC) $(OBJECTS_CAMERATESTS) $(LDFLAGS) $(EXEFLAGS) -l$(PACKAGE_NAME) -o $(@)

$(HATS_SYM)/MmteOutputManagement: $(OBJECTS_MMTE) $(LIBRARIES)
	$(CC) $(OBJECTS_MMTE) $(LDFLAGS) $(EXEFLAGS) -o $(@)

$(BINARIES): $(SYMBOLS_BIN)
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(INSTALL_CMD) -D $<  $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_INC)/$(PACKAGE_NAME)/%.h: incl/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$< $(@)

install: $(INCLUDES) $(LIBRARIES) $(BINARIES) $(INIT_SCRIPTS) $(USE_CASE_SCRIPTS)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(LIBRARIES) $(SYMBOLS_LIB) $(SYMBOLS_BIN)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
	$(RM) -rf $(BINARIES) $(INIT_SCRIPTS_PATH) $(USE_CASE_SCRIPTS)
