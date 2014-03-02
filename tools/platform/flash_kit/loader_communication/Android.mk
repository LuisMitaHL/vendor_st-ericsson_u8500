# If FLASHKIT_INSTALL_PATH is set it is assumed to be part of a flashkit build
# in the forest, otherwise it is assumed to be a build of loader_communication
# to use together with MFA
ifeq ($(FLASHKIT_INSTALL_PATH),)
  PRIVATE_BUILDTYPE_FLASHKIT=false
else
  PRIVATE_BUILDTYPE_FLASHKIT=true
endif

ifeq ($(PRIVATE_BUILDTYPE_FLASHKIT), true)
#Build of loader_communication for flashkit
#Android makefile to build loader communication as a part of Android Build
include $(CLEAR_VARS)

.phony: config-lcm build-lcm install-lcm config-lcd build-lcd install-lcd clean-lcm distclean-lcm clean-lcd distclean-lcd clobber

#Source location
PRIVATE_LC_TOOLS_PATH := $(abspath $(TOOLS_PATH))
PRIVATE_LC_LOADERS_DIR := $(PRIVATE_LC_TOOLS_PATH)/platform/flash_kit/loaders

PRIVATE_LC_LCM_DIR := $(PRIVATE_LC_TOOLS_PATH)/platform/flash_kit/loader_communication/lcmodule
PRIVATE_LC_LCD_DIR := $(PRIVATE_LC_TOOLS_PATH)/platform/flash_kit/loader_communication

#Output
PRIVATE_LC_FLASH_KIT_TOOL_DIR := $(FLASHKIT_INSTALL_BASE)/flashkit

PRIVATE_LC_LCM_OUT_DIR = $(abspath $(TOP))/$(TARGET_OUT_INTERMEDIATES)/FLASHKIT/LCM
PRIVATE_LC_LCD_OUT_DIR = $(abspath $(TOP))/$(TARGET_OUT_INTERMEDIATES)/FLASHKIT/LCD

PRIVATE_LC_LCM_INSTALLDIR := $(abspath $(TOP))/$(TARGET_OUT_INTERMEDIATES)/FLASHKIT/LOADERS/loader_communication_module
PRIVATE_LC_LCMLIB_INSTALLDIR := $(PRIVATE_LC_FLASH_KIT_TOOL_DIR)/flash-tool-backend/native/
PRIVATE_LC_LCD_INSTALLDIR := $(PRIVATE_LC_FLASH_KIT_TOOL_DIR)/flash-tool-backend/native/

#Other flags
PRIVATE_LC_LCD_FLAGS := LCD_INSTALLDIR=$(PRIVATE_LC_LCD_INSTALLDIR) PATH=$(PATH)

PRIVATE_LC_LCM_FLAGS := LCM_PATH=$(PRIVATE_LC_LCM_DIR) LOADER_DIR=$(PRIVATE_LC_LOADERS_DIR) \
			 LCMLIB_INSTALLDIR=$(PRIVATE_LC_LCMLIB_INSTALLDIR) \
			 PATH=$(PATH)

#LCM TARGETS
.PHONY: config-lcm
config-lcm:
	mkdir -p $(PRIVATE_LC_LCMLIB_INSTALLDIR)
	mkdir -p $(PRIVATE_LC_LCM_OUT_DIR)
	$(MAKE) -C $(PRIVATE_LC_LCM_DIR) $(PRIVATE_LC_LCM_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCM_OUT_DIR) BUILDFOLDER=$(PRIVATE_LC_LCM_OUT_DIR) config

.PHONY: build-lcm
build-lcm: config-lcm
	$(MAKE) -C $(PRIVATE_LC_LCM_DIR) $(PRIVATE_LC_LCM_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCM_OUT_DIR) build

.PHONY: install-lcm
install-lcm: build-lcm
	$(MAKE) -C $(PRIVATE_LC_LCM_DIR) $(PRIVATE_LC_LCM_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCM_OUT_DIR) install

.PHONY: clean-lcm
clean-lcm: config-lcm
	$(MAKE) -C $(PRIVATE_LC_LCM_DIR) $(PRIVATE_LC_LCM_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCM_OUT_DIR) clean

.PHONY: distclean-lcm
distclean-lcm: config-lcm
	$(MAKE) -C $(PRIVATE_LC_LCM_DIR) $(PRIVATE_LC_LCM_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCM_OUT_DIR) distclean

#LCD TARGETS
.PHONY: config-lcd
config-lcd:
	mkdir -p $(PRIVATE_LC_LCD_INSTALLDIR)
	mkdir -p $(PRIVATE_LC_LCD_OUT_DIR)
	$(MAKE) -C $(PRIVATE_LC_LCD_DIR) $(PRIVATE_LC_LCD_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCD_OUT_DIR) BUILDFOLDER=$(PRIVATE_LC_LCD_OUT_DIR) config

.PHONY: build-lcd
build-lcd: config-lcd
	$(MAKE) -C $(PRIVATE_LC_LCD_DIR) $(PRIVATE_LC_LCD_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCD_OUT_DIR) build

.PHONY: install-lcd
install-lcd: build-lcd
	$(MAKE) -C $(PRIVATE_LC_LCD_DIR) $(PRIVATE_LC_LCD_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCD_OUT_DIR) install

.PHONY: clean-lcd
clean-lcd: config-lcd
	$(MAKE) -C $(PRIVATE_LC_LCD_DIR) $(PRIVATE_LC_LCD_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCD_OUT_DIR) clean

.PHONY: distclean-lcd
distclean-lcd: config-lcd
	$(MAKE) -C $(PRIVATE_LC_LCD_DIR) $(PRIVATE_LC_LCD_FLAGS) CONFIG_DIR=$(PRIVATE_LC_LCD_OUT_DIR) distclean

clean clobber: clean-lcm clean-lcd

st-ericsson-flashkit: config-lcm build-lcm install-lcm config-lcd build-lcd install-lcd

else
#MFA build of loader_communication
#Android makefile to build loader communication as an Android shared library object

# build liblcdriver.so
LOCAL_PATH := $(my-dir)
XSL_LOCAL_PATH := $(LOCAL_PATH)

# Path for the tool used for automatic code generation
PRIVATE_CLASSPATH := $(LOCAL_PATH)/lcmodule/tools/xalan-j_2_7_1/xalan.jar org.apache.xalan.xslt.Process

include $(CLEAR_VARS)

LOCAL_MODULE := liblcdriver
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES

# Automatic Code Generation
LOCAL_AUTO_DIR := $(local-intermediates-dir)/source/autogen
LOCAL_CONFIG_PATH := $(LOCAL_PATH)/source/config
SCRIPT_PATH := $(LOCAL_PATH)/source

# Generate lcdriver_error_codes.h, error_codes_desc.cpp
GEN := $(LOCAL_AUTO_DIR)/lcdriver_error_codes.h $(LOCAL_AUTO_DIR)/error_codes_desc.cpp
$(GEN) : $(shell mkdir -p $(LOCAL_AUTO_DIR))
$(GEN) : PRIVATE_INPUT_XML := $(LOCAL_CONFIG_PATH)/lcdriver_error_codes.xml
$(GEN) : PRIVATE_INPUT_XSL = $(XSL_LOCAL_PATH)/source/config/$(patsubst %.h,%_h.xsl,$(patsubst %.cpp,%_cpp.xsl,$(@F)))
$(GEN) : $(PRIVATE_INPUT_XML) $(PRIVATE_INPUT_XSL)
$(GEN) : PRIVATE_CUSTOM_TOOL = java -classpath $(PRIVATE_CLASSPATH) -in $(PRIVATE_INPUT_XML) -xsl $(PRIVATE_INPUT_XSL) -out $@ -PARAM target lcm -PARAM errorCodesLcmXml $(PRIVATE_LC_LCD_DIR)../../lcmodule/source/cnh1606344_ldr_communication_module/config/error_codes.xml
$(GEN) :
	$(transform-generated-source)
	@echo $@

LOCAL_GENERATED_SOURCES += $(GEN)

# Generate a2_command_ids.h, a2_commands.h, a2_commands_impl.h, and a2_commands_marshal.cpp
GEN := $(LOCAL_AUTO_DIR)/a2_command_ids.h $(LOCAL_AUTO_DIR)/a2_commands.h $(LOCAL_AUTO_DIR)/a2_commands_impl.h $(LOCAL_AUTO_DIR)/a2_commands_marshal.cpp
$(GEN) : $(shell mkdir -p $(LOCAL_AUTO_DIR))
$(GEN) : PRIVATE_INPUT_XML := $(LOCAL_CONFIG_PATH)/a2_commands.xml
$(GEN) : PRIVATE_INPUT_XSL = $(XSL_LOCAL_PATH)/source/config/$(patsubst %.h,%_h.xsl,$(patsubst %.cpp,%_cpp.xsl,$(@F)))
$(GEN) : $(PRIVATE_INPUT_XML) $(PRIVATE_INPUT_XSL)
$(GEN) : PRIVATE_CUSTOM_TOOL = java -classpath $(PRIVATE_CLASSPATH) -in $(PRIVATE_INPUT_XML) -xsl $(PRIVATE_INPUT_XSL) -out $@ -PARAM target lcm
$(GEN) :
	$(transform-generated-source)
	@echo $@

LOCAL_GENERATED_SOURCES += $(GEN)

# Generate command_ids.h, commands.h, commands_impl.h, and commands_marshal.cpp
GEN := $(LOCAL_AUTO_DIR)/command_ids.h $(LOCAL_AUTO_DIR)/commands.h $(LOCAL_AUTO_DIR)/commands_impl.h $(LOCAL_AUTO_DIR)/commands_marshal.cpp
$(GEN) : $(shell mkdir -p $(LOCAL_AUTO_DIR))
$(GEN) : PRIVATE_INPUT_XML := $(LOCAL_CONFIG_PATH)/commands.xml
$(GEN) : PRIVATE_INPUT_XSL = $(XSL_LOCAL_PATH)/source/config/$(patsubst %.h,%_h.xsl,$(patsubst %.cpp,%_cpp.xsl,$(@F)))
$(GEN) : $(PRIVATE_INPUT_XML) $(PRIVATE_INPUT_XSL)
$(GEN) : PRIVATE_CUSTOM_TOOL = java -classpath $(PRIVATE_CLASSPATH) -in $(PRIVATE_INPUT_XML) -xsl $(PRIVATE_INPUT_XSL) -out $@ -PARAM target lcm
$(GEN) :
	$(transform-generated-source)
	@echo $@

LOCAL_GENERATED_SOURCES += $(GEN)

# Generate LcdVersion.cpp
GEN := $(LOCAL_AUTO_DIR)/LcdVersion.cpp
$(GEN) : $(shell mkdir -p $(LOCAL_AUTO_DIR))
$(GEN) : $(shell bash $(SCRIPT_PATH)/gen_version_files.sh --lcd $(abspath $(LOCAL_AUTO_DIR)) $(abspath $(LOCAL_PATH)))
$(GEN) :
	$(transform-generated-source)
	@echo $@

LOCAL_GENERATED_SOURCES += $(GEN)

LOCAL_SRC_FILES := \
	source/utilities/Serialization.cpp\
	source/utilities/Logger.cpp\
	source/utilities/MemMappedFile.cpp\
	source/utilities/CaptiveThreadObject.cpp\
	source/utilities/BulkHandler.cpp\
	source/utilities/String_s.cpp\
	source/CEH/ProtromRpcInterface.cpp\
	source/CEH/commands_impl.cpp\
	source/CEH/a2_commands_impl.cpp\
	source/CEH/ZRpcInterface.cpp\
	source/CEH/CmdResult.cpp\
	source/LcmInterface.cpp\
	source/LCDriverThread.cpp\
	source/LCDriverMethods.cpp\
	source/LCDriverEntry.cpp\
	source/LCDriver.cpp\
	source/LCM/Hash.cpp\
	source/LCM/Buffers.cpp\
	source/LCM/Queue.cpp\
	source/LCM/Timer.cpp\
	source/api_wrappers/linux/CThreadWrapper.cpp\
	source/api_wrappers/linux/CWaitableObject.cpp\
	source/api_wrappers/linux/CSemaphore.cpp\
	source/api_wrappers/linux/CSemaphoreQueue.cpp\
	source/api_wrappers/linux/CEventObject.cpp\
	source/api_wrappers/linux/CWaitableObjectCollection.cpp\
	source/api_wrappers/linux/OS.cpp\
	source/LCDriverInterface.cpp\
	source/security_algorithms/SecurityAlgorithms.cpp\
	source/security_algorithms/sha/sha2.cpp

LOCAL_C_INCLUDES := \
	$(TOP)/external/stlport/stlport\
	$(TOP)/bionic\
	$(TOP)/bionic/libc/include\
	$(LOCAL_PATH)/source\
	$(LOCAL_PATH)/source/api_wrappers/linux\
	$(LOCAL_PATH)/source/utilities\
	$(LOCAL_PATH)/source/LCM\
	$(LOCAL_PATH)/source/LCM/include\
	$(LOCAL_PATH)/source/security_algorithms\
	$(LOCAL_PATH)/source/CEH\
	$(LOCAL_PATH)/source/security_algorithms/sha\
	$(LOCAL_AUTO_DIR)

LOCAL_CFLAGS := -Wall -fPIC -fvisibility=hidden -fno-strict-aliasing -DLCDRIVER_EXPORTS -DLITTLE_ENDIAN=1234 -DBIG_ENDIAN=4321 -DBYTE_ORDER=LITTLE_ENDIAN

LOCAL_SHARED_LIBRARIES := libc libdl libstlport

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

# build liblcm.so
include $(CLEAR_VARS)

LOCAL_MODULE := liblcm
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_AUTO_DIR := $(local-intermediates-dir)/lcmodule/source/autogen
LOCAL_CONFIG_PATH := $(LOCAL_PATH)/lcmodule/source/cnh1606344_ldr_communication_module/config

# Generate error_codes.h
GEN := $(LOCAL_AUTO_DIR)/error_codes.h
$(GEN) : $(shell mkdir -p $(LOCAL_AUTO_DIR))
$(GEN) : PRIVATE_INPUT_XML := $(LOCAL_CONFIG_PATH)/error_codes.xml
$(GEN) : PRIVATE_INPUT_XSL := $(LOCAL_CONFIG_PATH)/errorcode_h.xsl
$(GEN) : $(PRIVATE_INPUT_XML) $(PRIVATE_INPUT_XSL)
$(GEN) : PRIVATE_CUSTOM_TOOL = java -classpath $(PRIVATE_CLASSPATH) -in $(PRIVATE_INPUT_XML) -xsl $(PRIVATE_INPUT_XSL) -out $@ -PARAM target lcm
$(GEN) :
	$(transform-generated-source)
	@echo $@

LOCAL_GENERATED_SOURCES += $(GEN)

# Generate command_ids.h and commands.h
GEN := $(LOCAL_AUTO_DIR)/command_ids.h $(LOCAL_AUTO_DIR)/commands.h
$(GEN) : $(shell mkdir -p $(LOCAL_AUTO_DIR))
$(GEN) : PRIVATE_INPUT_XML := $(LOCAL_CONFIG_PATH)/commands.xml
$(GEN) : PRIVATE_INPUT_XSL = $(LOCAL_CONFIG_PATH)/$(patsubst %.h,%_h.xsl,$(patsubst %.cpp,%_cpp.xsl,$(@F)))
$(GEN) : $(PRIVATE_INPUT_XML) $(PRIVATE_INPUT_XSL)
$(GEN) : PRIVATE_CUSTOM_TOOL = java -classpath $(PRIVATE_CLASSPATH) -in $(PRIVATE_INPUT_XML) -xsl $(PRIVATE_INPUT_XSL) -out $@ -PARAM target lcm
$(GEN) :
	$(transform-generated-source)
	@echo $@

LOCAL_GENERATED_SOURCES += $(GEN)

# Generate lcm_version.c
GEN := $(LOCAL_AUTO_DIR)/lcm_version.c
$(GEN) : $(shell mkdir -p $(LOCAL_AUTO_DIR))
$(GEN) : $(shell bash $(SCRIPT_PATH)/gen_version_files.sh --lcm $(abspath $(LOCAL_AUTO_DIR)) $(abspath $(LOCAL_PATH)))
$(GEN) :
	$(transform-generated-source)
	@echo $@

LOCAL_GENERATED_SOURCES += $(GEN)

LOCAL_SRC_FILES := \
	lcmodule/source/cnh1605204_ldr_transport_layer/source/bulk_protocol.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/command_protocol.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/protrom_protocol.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/protrom_transport.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/r15_transport_layer.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/a2_transport.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/a2_protocol.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/a2_speedflash.c\
	lcmodule/source/cnh1605204_ldr_transport_layer/source/z_transport.c\
	lcmodule/source/cnh1605205_ldr_network_layer/source/a2_header.c\
	lcmodule/source/cnh1605205_ldr_network_layer/source/a2_network.c\
	lcmodule/source/cnh1605205_ldr_network_layer/source/r15_header.c\
	lcmodule/source/cnh1605205_ldr_network_layer/source/r15_network_layer.c\
	lcmodule/source/cnh1605205_ldr_network_layer/source/z_network.c\
	lcmodule/source/cnh1605205_ldr_network_layer/source/protrom_header.c\
	lcmodule/source/cnh1605205_ldr_network_layer/source/protrom_network.c\
	lcmodule/source/cnh1606344_ldr_communication_module/source/communication_service.c\
	lcmodule/source/cnh1606344_ldr_communication_module/source/protrom_family.c\
	lcmodule/source/cnh1606344_ldr_communication_module/source/z_family.c\
	lcmodule/source/cnh1606344_ldr_communication_module/source/r15_family.c\
	lcmodule/source/cnh1606344_ldr_communication_module/source/a2_family.c\
	lcmodule/source/cnh1605551_ldr_utilities/critical_section/critical_section_linux.c\
	lcmodule/source/serialization.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/lcmodule/source/legacy_compatibility/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605204_ldr_transport_layer/include/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605205_ldr_network_layer/include/\
	$(LOCAL_PATH)/lcmodule/source/cnh1606344_ldr_communication_module/include/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605720_ldr_time_utilities/include/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605551_ldr_utilities/include/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605551_ldr_utilities/critical_section/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605551_ldr_utilities/communication_abstraction/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605203_ldr_communication_buffer/include/\
	$(LOCAL_PATH)/lcmodule/source/cnh1605721_ldr_security_algorithms/include/\
	$(LOCAL_AUTO_DIR)

LOCAL_CFLAGS := -Wall -fPIC -fvisibility=hidden -fno-strict-aliasing -DUINT64_SUPPORTED -DSINT64_SUPPORTED -DINT64_BASE_TYPE="long long" -DCFG_ENABLE_A2_FAMILY -DLCM_EXPORTS

LOCAL_SHARED_LIBRARIES := libc

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

endif
