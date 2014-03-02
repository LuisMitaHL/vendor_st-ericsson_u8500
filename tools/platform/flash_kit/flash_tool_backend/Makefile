# Flash Kit Flash Tool Backend Makefile

STE_PLATFORM ?= u8500
TOOL_VERSION ?= tool_version.txt
TARGET_OUT_INTERMEDIATES ?= .
FTB_GIT_DIR ?= .

ifndef FLASH_KIT_TARGET_DIR
  FLASH_KIT_TARGET_DIR:=./flash-kit-tools
endif

SRC:=$(shell find $(FTB_GIT_DIR) -type f -and -not -name "~*" -and -not -name ".*" -and -not -name "tool*.txt" -and -not -name "*.so" -and -not -regex ".*/release/.*" -and -not -regex ".*/build/.*" -and -not -regex ".*/bin/.*" -and -not -regex ".*/\..*/.*" | sed "s/[ ]/\\\\ /g")

GET_TOOL_VERSION:=$(FTB_GIT_DIR)/get_version.sh $(FTB_GIT_DIR) $(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION)

FLASH_TOOL_BACKEND_RELEASE_DIR:=$(TARGET_OUT_INTERMEDIATES)/com.stericsson.sdk.backend.build/release

SYS_NAME := $(shell uname -s)
SYS_ARCH := $(shell uname -m)

INSTALL:=$(shell which install)
FIND:=$(shell which find)
CP:=$(shell which cp)
MV:=$(shell which mv)
CHMOD:=$(shell which chmod)
XARGS:=$(shell which xargs)
MKDIR:=$(shell which mkdir)
RM:=$(shell which rm)

ifdef ANT_HOME
  ANT:=$(ANT_HOME)/bin/ant
else
  ANT:=$(shell which ant)
endif
ifeq ($(ANT),)
  $(error ant is not installed)
endif

ifeq ($(SYS_NAME), Darwin)
  PRIVATE_ANT_OPTS=-Dosgi.ws=cocoa -Dosgi.os=macosx
else ifeq ($(SYS_NAME), Linux)
  PRIVATE_ANT_OPTS=-Dosgi.ws=gtk -Dosgi.os=linux
endif

ifeq ($(SYS_ARCH), x86_64)
  PRIVATE_ANT_OPTS+= -Dosgi.arch=x86_64
  LBITS=64
else
  PRIVATE_ANT_OPTS+= -Dosgi.arch=x86
  LBITS=32
endif

default: build

.PHONY: install
install: build
#
# Create directory for Flash Tool Backend
#
	$(MKDIR) -p $(FLASH_KIT_TARGET_DIR)/flash-tool-backend

#
# Install licenses file
#
	$(INSTALL) -m 0644 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/LICENSES-flash-tool-backend.txt $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/

#
# Install launch scripts
#
	$(INSTALL) -m 0755 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/flash-tool-backend.sh $(FLASH_KIT_TARGET_DIR)/flash-tool-backend
	$(INSTALL) -m 0755 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/flash-tool-backend.bat $(FLASH_KIT_TARGET_DIR)/flash-tool-backend
	$(INSTALL) -m 0755 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/check-driver-version.bat $(FLASH_KIT_TARGET_DIR)/flash-tool-backend
	$(INSTALL) -m 0644 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/check-driver-version.config $(FLASH_KIT_TARGET_DIR)/flash-tool-backend

#
# Create and populate doc directory and copy README file
#
	$(CP) -r $(FLASH_TOOL_BACKEND_RELEASE_DIR)/doc $(FLASH_KIT_TARGET_DIR)/flash-tool-backend
	$(FIND) $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/doc -type f | $(XARGS) $(CHMOD) 0644
	$(MV) $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/doc/README.html $(FLASH_KIT_TARGET_DIR)
	$(MV) $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/doc/README.txt $(FLASH_KIT_TARGET_DIR)
	$(INSTALL) -m 0644 HowToUseManPages.txt $(FLASH_KIT_TARGET_DIR)/flash-tool-backend
#
# Create and populate configuration directory
#
	$(MKDIR) -p $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/configuration

	$(INSTALL) -m 0644 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/configuration/debug_log_cfg.xml $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/configuration
	$(INSTALL) -m 0644 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/configuration/log_cfg.xml $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/configuration
	#copy backend.mesp and profiles for right platform
	$(INSTALL) -m 0644 $(TARGET_OUT_INTERMEDIATES)/com.stericsson.sdk.backend.build/configuration/$(STE_PLATFORM)/* $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/configuration

#
# Create and populate OSGI bundle directory
#
	$(RM) -rf $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/osgi
	$(MKDIR) -p $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/osgi
	$(INSTALL) -m 0644 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/osgi/*.jar $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/osgi

#
# Create and populate OSGI configuration directory
#
	$(MKDIR) -p $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/osgi/configuration

	$(INSTALL) -m 0644 $(FLASH_TOOL_BACKEND_RELEASE_DIR)/osgi/configuration/config.ini $(FLASH_KIT_TARGET_DIR)/flash-tool-backend/osgi/configuration

#
# Copy files needed if running on Windows PC
#
	$(CP) -r windows-usb-driver $(FLASH_KIT_TARGET_DIR)
	$(FIND) $(FLASH_KIT_TARGET_DIR)/windows-usb-driver -type f | $(XARGS) $(CHMOD) 0644

#
# Copy files needed if running on MACOS PC
#
	$(CP) -r macos-usb-driver $(FLASH_KIT_TARGET_DIR)

.PHONY: build
build: $(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION) 

$(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION): $(SRC)
ifneq ($(SYS_NAME), Darwin)
	make -C com.stericsson.sdk.equipment.io.uart/jni LBITS=32 install
	make -C com.stericsson.sdk.equipment.io.usb/jni LBITS=32 install
	make -C com.stericsson.sdk.loader.communication/jni LBITS=32 install
endif
ifeq ($(LBITS),64)
	make -C com.stericsson.sdk.equipment.io.uart/jni LBITS=64 install
	make -C com.stericsson.sdk.equipment.io.usb/jni LBITS=64 install	
	make -C com.stericsson.sdk.loader.communication/jni LBITS=64 install
endif
	ANT_OPTS="$(PRIVATE_ANT_OPTS)" $(ANT) -f com.stericsson.sdk.backend.build/build.xml -Dorg.eclipse.jdt.launching.JRE_CONTAINER/org.eclipse.jdt.internal.debug.ui.launcher.StandardVMType/J2SE-1.5=$(JAVA_HOME)
	$(GET_TOOL_VERSION)

.PHONY: shared_libraries
shared_libraries:
ifneq ($(SYS_NAME), Darwin)
	make -C com.stericsson.sdk.equipment.io.uart/jni LBITS=32 install
	make -C com.stericsson.sdk.equipment.io.usb/jni LBITS=32 install	
	make -C com.stericsson.sdk.loader.communication/jni LBITS=32 install
endif
ifeq ($(LBITS),64)
	make -C com.stericsson.sdk.equipment.io.uart/jni LBITS=64 install
	make -C com.stericsson.sdk.equipment.io.usb/jni LBITS=64 install	
	make -C com.stericsson.sdk.loader.communication/jni LBITS=64 install
endif

.PHONY: clean
clean: clean_libraries
	$(RM) -f $(TARGET_OUT_INTERMEDIATES)/$(TOOL_VERSION)

.PHONY: clean_libraries
clean_libraries:
ifneq ($(SYS_NAME), Darwin)
	make -C com.stericsson.sdk.equipment.io.usb/jni LBITS=32 clean
	make -C com.stericsson.sdk.equipment.io.uart/jni LBITS=32 clean
	make -C com.stericsson.sdk.loader.communication/jni LBITS=32 clean
endif
ifeq ($(LBITS),64)
	make -C com.stericsson.sdk.equipment.io.usb/jni LBITS=64 clean
	make -C com.stericsson.sdk.equipment.io.uart/jni LBITS=64 clean
	make -C com.stericsson.sdk.loader.communication/jni LBITS=64 clean
endif

.PHONY: distclean
distclean: clean
	$(RM) -rf $(FLASH_TOOL_BACKEND_RELEASE_DIR)
	$(RM) -rf $(FLASH_KIT_TARGET_DIR)/flash-tool-backend
	$(RM) -rf $(FLASH_KIT_TARGET_DIR)/windows-usb-driver
