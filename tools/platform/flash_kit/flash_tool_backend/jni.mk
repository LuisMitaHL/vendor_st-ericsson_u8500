#
#  Created on: 06.1.2012
#      Author: mbodan01

SRC_DIR=src
CC=g++
LBITS?=64

SEPARATOR = --------
SYS_INFO := $(shell uname -a)

LIBRARY64_PREFIX=$(LIBRARY_PREFIX)_x64

MACOS_LIBRARY_SUFIX=jnilib
WIN_LIBRARY_SUFIX=dll
LINUX_LIBRARY_SUFIX=so

UPDATE_DIR?=../../flash-kit-tools/flash-tool-backend/osgi

ifneq ( , $(findstring Darwin, $(SYS_INFO)))
#Mac specific settings
  JNI_INCLUDE?=/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers
  DIST_DIR=$(MACOS_DIST_DIR)
  OUTPUT_FILE=lib$(LIBRARY64_PREFIX).$(MACOS_LIBRARY_SUFIX)
  INCLUDES=-I"$(JNI_INCLUDE)" -I../../com.stericsson.sdk.equipment.io.uart/jni/src/
  BUILD_DIR=build/macos
  CFLAGS=-Wall -D__MACOSX__ -fPIC
#  LFLAGS=-Wall -shared -fPIC
  LFLAGS=-Wall -fPIC
else ifneq ( , $(findstring Linux, $(SYS_INFO)))
#Linux specific settings
  JNI_INCLUDE=$(JAVA_HOME)/include
  INCLUDES=-I$(JNI_INCLUDE) -I$(JNI_INCLUDE)/linux -I../../com.stericsson.sdk.equipment.io.uart/jni/src/
  ifeq ($(LBITS),64)
    DIST_DIR=$(LINUX64_DIST_DIR)
    OUTPUT_FILE=lib$(LIBRARY64_PREFIX).$(LINUX_LIBRARY_SUFIX)
    BUILD_DIR=build/linux64
    CFLAGS=-Wall -D__LINUX__ -fPIC
#    LFLAGS=-Wall -shared -fPIC
    LFLAGS=-Wall -fPIC
  else
    DIST_DIR=$(LINUX_DIST_DIR)
    OUTPUT_FILE=lib$(LIBRARY_PREFIX).$(LINUX_LIBRARY_SUFIX)
    BUILD_DIR=build/linux32
    CFLAGS=-Wall -D__LINUX__ -fPIC -m32
#    LFLAGS=-Wall -shared -fPIC -m32
    LFLAGS=-Wall -fPIC -m32
  endif
else
#Windows specific settings
  JNI_INCLUDE=$(JAVA_HOME)/include
  DIST_DIR=$(WIN_DIST_DIR)
  OUTPUT_FILE=$(LIBRARY_PREFIX).$(WIN_LIBRARY_SUFIX)
  INCLUDES=-I"$(JNI_INCLUDE)" -I"$(JNI_INCLUDE)/win32" -I../../com.stericsson.sdk.equipment.io.uart/jni/src/
  BUILD_DIR=build/windows
  CFLAGS=-Wall -D__WIN__ -D_JNI_IMPLEMENTATION -m32
#  LFLAGS=-Wall -shared -Wl,--add-stdcall-alias -Wl,--kill-at -m32
  LFLAGS=-Wall -Wl,--add-stdcall-alias -Wl,--kill-at -m32
endif

OUTPUT_JAR=$(shell find $(UPDATE_DIR) -name "$(notdir $(DIST_DIR))*.jar")

#Path to find sources and header files
VPATH=$(SRC_DIR)

.PHONY: build
.PHONY: clean_build
.PHONY: install
.PHONY: uninstall
.PHONY: clean
.PHONY: info
.PHONY: help
.PHONY: preapre
.PHONE: update

#Do only build
build: info $(BUILD_DIR)/$(OUTPUT_FILE)

#Do clean before build
clean_build: info clean $(BUILD_DIR)/$(OUTPUT_FILE)

$(DIST_DIR)/$(OUTPUT_FILE): $(BUILD_DIR)/$(OUTPUT_FILE)
	@echo
	@echo "$(SEPARATOR)$(SEPARATOR)"
	@echo "$(SEPARATOR) Installing '$(OUTPUT_FILE)' to $(DIST_DIR).."
	@echo "$(SEPARATOR)$(SEPARATOR)"
	mkdir -p $(DIST_DIR)
	cp $(BUILD_DIR)/$(OUTPUT_FILE) $(DIST_DIR)

#Install will copy resulting library into DIST_DIR
install: $(DIST_DIR)/$(OUTPUT_FILE)

#Uninstall remove JNI library from DIST_DIR
uninstall:
	@echo
	@echo "$(SEPARATOR)$(SEPARATOR)"
	@echo "$(SEPARATOR) Attempt to uninstall/remove '$(OUTPUT_FILE)' from $(DIST_DIR).."
	@echo "$(SEPARATOR)$(SEPARATOR)"
	rm -f $(DIST_DIR)/$(OUTPUT_FILE)

#Make unistall and clean all object files
clean:
	@echo
	@echo "$(SEPARATOR)$(SEPARATOR)"
	@echo "$(SEPARATOR) Taking clean actions.."
	@echo "$(SEPARATOR)$(SEPARATOR)"
	rm -f $(BUILD_DIR)/$(OUTPUT_FILE)
	rm -rf $(BUILD_DIR)

help: info
	@echo
	@echo "$(SEPARATOR)$(SEPARATOR)"
	@echo "$(SEPARATOR) Available targets: [target_name] : [usage] : [short description]"
	@echo "      build : make             : Default target to invoke incremental build."
	@echo "clean_build : make clean_build : Will invoke clean build."
	@echo "    install : make install     : Build shared library and store in DIST_DIR."
	@echo "  uninstall : make uninstall   : Remove shared library from DIST_DIR."
	@echo "      clean : make clean       : Clean directory for built artifacts."
	@echo "     update : make update      : Build and update shared library directly into package file."
	@echo "       info : make info        : Will show system info."
	@echo "       help : make help        : Show this help"
	@echo "$(SEPARATOR)$(SEPARATOR)"

info:
	@echo
	@echo "$(SEPARATOR)$(SEPARATOR)"
	@echo "$(SEPARATOR) System info: $(SYS_INFO)"
	@echo "$(SEPARATOR)$(SEPARATOR)"

#Rule for compiling sources from SRC_DIR and putting related object files into appropriate BUILD_DIR
$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo
	@echo $(SEPARATOR)$(SEPARATOR)
	@echo "$(SEPARATOR) Compiling '$<'"
	@echo $(SEPARATOR)$(SEPARATOR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o "$@"

$(BUILD_DIR)/$(OUTPUT_FILE): $(OBJ)
	@echo
	@echo $(SEPARATOR)$(SEPARATOR)
	@echo "$(SEPARATOR) Linking '$(OUTPUT_FILE)'"
	@echo $(SEPARATOR)$(SEPARATOR)
	$(CC) $(LFLAGS) -shared $(OBJ) -o $(BUILD_DIR)/$(OUTPUT_FILE) $(LLIBS)

$(OBJ): $(filter-out $(wildcard $(BUILD_DIR)), $(BUILD_DIR)) $(HEAD)

update: info $(BUILD_DIR)/$(OUTPUT_FILE)
	@echo
	@echo "$(SEPARATOR)$(SEPARATOR)"
	@echo "$(SEPARATOR) Updating '$(OUTPUT_JAR)'"
	@echo "$(SEPARATOR)$(SEPARATOR)"
	@(cd $(BUILD_DIR) && zip -u $(OUTPUT_JAR) $(OUTPUT_FILE)) || echo "Nothing to update."

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(DIST_DIR)

