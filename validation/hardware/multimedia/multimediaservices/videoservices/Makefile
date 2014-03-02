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

PACKAGE_NAME=videoservices
PACKAGE_EXEC_NAME= ExecuteDisplayLoop VideoTests
PACKAGE_LIB_NAME=lib$(PACKAGE_NAME).so
LIB_SOURCES= VideoServices.c pngrw.c
SOURCES_VIDEOTESTS= VideoTests.c
SOURCES_EXECUTEDISPLAYLOOP= ExecuteDisplayLoop.c
HEADERS= $(wildcard incl/*.h) $(wildcard srce/*.h)

IMAGES_PATH=$(HATS_SHARE)/multimediaservices/$(PACKAGE_NAME)

OUTPUT= $(HATS_OBJ)/$(PACKAGE_NAME)
OBJECTS_LIB= $(addprefix $(OUTPUT)/,$(LIB_SOURCES:.c=.o))
OBJECTS_VIDEOTESTS= $(addprefix $(OUTPUT)/,$(SOURCES_VIDEOTESTS:.c=.o))
OBJECTS_EXECUTEDISPLAYLOOP= $(addprefix $(OUTPUT)/,$(SOURCES_EXECUTEDISPLAYLOOP:.c=.o))
LIB_SYMBOLS= $(HATS_SYM)/$(PACKAGE_LIB_NAME)
LIBRARIES= $(HATS_LIB)/$(PACKAGE_LIB_NAME)
BIN_SYMBOLS= $(addprefix $(HATS_SYM)/,$(PACKAGE_EXEC_NAME))
BINARIES=$(addprefix $(HATS_BIN)/,$(PACKAGE_EXEC_NAME))
INCLUDES= $(addprefix $(HATS_INC)/$(PACKAGE_NAME)/,$(notdir $(HEADERS)))
IMAGES=$(addprefix $(IMAGES_PATH)/,$(notdir $(wildcard images/*)))

CFLAGS+=-Isrce -Iincl -I$(HATS_INC)/kernel/include -I$(HATS_KERNEL_INCLUDE) \
	    -I$(HATS_KERNEL)/drivers/video/av8100 -Wall -I$(HATS_INC)/libpng \
	    -I$(HATS_INC)/zlib \
		-Werror -Wall -Wextra -O2

#-I$(HATS_KERNEL_MACH_INCLUDE) -I$(HATS_KERNEL_INCLUDE) \

LDFLAGS+= -lpng -lz -lm -lab

.PHONY: all install clean

all: install

$(OUTPUT)/%.o: srce/%.c $(HEADERS)
	mkdir -p $(OUTPUT)
	$(CC) $< -c $(CFLAGS) -o $(@)

$(IMAGES_PATH)/%: images/%
	install -D $< $@

$(LIB_SYMBOLS): $(OBJECTS_LIB)
	$(CC) $(OBJECTS_LIB) $(LDFLAGS) $(LIBFLAGS) -o $(@)

$(LIBRARIES): $(LIB_SYMBOLS)
	echo -e "\t-----> $(PACKAGE_NAME) : $@"
	install -D $< $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_SYM)/VideoTests: $(OBJECTS_VIDEOTESTS) $(LIBRARIES)
	$(CC) $(OBJECTS_VIDEOTESTS) $(LDFLAGS) $(EXEFLAGS) -l$(PACKAGE_NAME) -o $(@)

$(HATS_SYM)/ExecuteDisplayLoop: $(OBJECTS_EXECUTEDISPLAYLOOP) $(LIBRARIES)
	$(CC) $(OBJECTS_EXECUTEDISPLAYLOOP) $(LDFLAGS) $(EXEFLAGS) -l$(PACKAGE_NAME) -o $(@)

$(HATS_BIN)/%: $(HATS_SYM)/%
	$(INSTALL_CMD) -D $< $@
	$(STRIP) --strip-unneeded $(@)

$(HATS_INC)/$(PACKAGE_NAME)/%.h: incl/%.h
	$(MKDIR) -p $(HATS_INC)/$(PACKAGE_NAME)
	$(LN) -sf $(CURDIR)/$< $(@)

install: $(INCLUDES) $(LIBRARIES) $(BINARIES) $(IMAGES)

clean:
	$(ECHO) -e "\t-----> $(PACKAGE_NAME) : $@"
	$(RM) -rf $(OUTPUT) $(LIB_SYMBOLS) $(BIN_SYMBOLS) $(LIBRARIES)
	$(RM) -rf $(BINARIES) $(IMAGES)
	$(RM) -rf $(HATS_INC)/$(PACKAGE_NAME)
