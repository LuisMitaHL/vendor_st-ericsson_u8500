ifeq ($(CONFIG_DIR),)
    config_file=./.config
else
    config_file=$(CONFIG_DIR)/.config
endif
-include $(config_file)

# Prevention rules if the include files are not generated yet
$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.d: ;
$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.d: ;

-include $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/*.d
-include $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/*.d

MINGW_X32_CC:=i586-mingw32msvc-
MINGW_X64_CC:=amd64-mingw32msvc-
XALAN_PATH:=./lcmodule/tools/xalan-j_2_7_1/
LCD_CONFIG:=./source/config/
LCD_DIR:=./
LCM_ERR_DESC_PATH:=../../lcmodule/source/cnh1606344_ldr_communication_module/config/error_codes.xml

ifneq ($(shell which $(MINGW_X32_CC)g++), )
USE_MINGW_X32 := 1
else
USE_MINGW_X32 := 0
endif
ifneq ($(shell which $(MINGW_X64_CC)g++), )
USE_MINGW_X64 := 1
else
USE_MINGW_X64 := 0
endif

OS_NAME := $(shell uname)
ifeq ($(OS_NAME), Linux)
LIB_EXTENSION := so
else
LIB_EXTENSION := dylib
endif

LIBSRC := \
	source/utilities/Serialization.cpp\
	source/utilities/Logger.cpp\
	source/utilities/MemMappedFile.cpp\
	source/utilities/CaptiveThreadObject.cpp\
	source/utilities/BulkHandler.cpp\
	source/utilities/String_s.cpp\
	source/CEH/a2_commands_impl.cpp\
	source/CEH/CmdResult.cpp\
	source/CEH/commands_impl.cpp\
	source/CEH/ProtromRpcInterface.cpp\
	source/CEH/ZRpcInterface.cpp\
	source/LcmInterface.cpp\
	source/LCDriverThread.cpp\
	source/LCDriverMethods.cpp\
	source/LCDriverEntry.cpp\
	source/LCDriver.cpp\
	source/LCM/Buffers.cpp\
	source/LCM/Hash.cpp\
	source/LCM/Queue.cpp\
	source/LCM/Timer.cpp\
	source/LCDriverInterface.cpp\
	source/security_algorithms/SecurityAlgorithms.cpp\
	source/security_algorithms/sha/sha2.cpp\
	$(AUTO_DIR_LIB)/commands_marshal.cpp\
	$(AUTO_DIR_LIB)/a2_commands_marshal.cpp\
	$(AUTO_DIR_LIB)/LcdVersion.cpp\
	$(AUTO_DIR_LIB)/error_codes_desc.cpp
ifeq ($(BUILD_WIN),)
LIBSRC += \
	source/api_wrappers/linux/CThreadWrapper.cpp\
	source/api_wrappers/linux/CWaitableObject.cpp\
	source/api_wrappers/linux/CSemaphore.cpp\
	source/api_wrappers/linux/CSemaphoreQueue.cpp\
	source/api_wrappers/linux/CEventObject.cpp\
	source/api_wrappers/linux/CWaitableObjectCollection.cpp\
	source/api_wrappers/linux/OS.cpp
endif

LIBOBJ_x32 := $(addprefix $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/, $(notdir $(LIBSRC:.cpp=.o)))
LIBOBJ_x64 := $(addprefix $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/, $(notdir $(LIBSRC:.cpp=.o)))

ifeq ($(BUILD_WIN),1)
LIBOBJ_x32 += $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/outLCDriver.o
endif
ifeq ($(BUILD_WIN),2)
LIBOBJ_x64 += $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/outLCDriver.o
endif

AUTOGEN_FILES := $(AUTO_DIR_LIB)/command_ids.h\
		$(AUTO_DIR_LIB)/commands.h\
		$(AUTO_DIR_LIB)/commands_impl.h\
		$(AUTO_DIR_LIB)/commands_marshal.cpp\
		$(AUTO_DIR_LIB)/lcdriver_error_codes.h\
		$(AUTO_DIR_LIB)/a2_command_ids.h\
		$(AUTO_DIR_LIB)/a2_commands.h\
		$(AUTO_DIR_LIB)/a2_commands_impl.h\
		$(AUTO_DIR_LIB)/a2_commands_marshal.cpp\
		$(AUTO_DIR_LIB)/LcdVersion.cpp\
		$(AUTO_DIR_LIB)/error_codes_desc.cpp


#include directories
INCLUDES := \
	-Isource\
	-Isource/utilities\
	-Isource/LCM\
	-Isource/LCM/include\
	-Isource/security_algorithms\
	-Isource/CEH\
	-Isource/security_algorithms/sha\
	-I$(AUTO_DIR_LIB)

ifeq ($(BUILD_WIN),)
INCLUDES += \
	-Isource/api_wrappers/linux
else
INCLUDES += \
	-Isource/api_wrappers/windows
endif

BYTE_ORDER := -DLITTLE_ENDIAN=1234 -DBIG_ENDIAN=4321 -DBYTE_ORDER=LITTLE_ENDIAN
# C++ compiler flags (-g -O2 -Wall)
ifeq ($(BUILD_WIN),)
CXXFLAGS := -c -O2 -Wall -fPIC -fvisibility=hidden -fno-strict-aliasing -DLCDRIVER_EXPORTS -D_FILE_OFFSET_BITS=64
else
# For Windows x32 and x64 version compile flags
CXXFLAGS := -D__WIN32__ -O2 -mwindows -mthreads -fno-strict-aliasing -Wall $(BYTE_ORDER) -DWIN32 -DWIN32_LEAN_AND_MEAN -DNDEBUG -D_WINDOWS -D_USRDLL -DLCDRIVER_EXPORTS -D_FILE_OFFSET_BITS=64
endif

ifeq ($(BUILD_WIN),)
LDFLAGS := -fPIC -fvisibility=hidden -shared -Wl -o liblcdriver.$(LIB_EXTENSION)
LIBS := -lpthread -ldl
else
# Win x32 linker flags
ifeq ($(BUILD_WIN),1)
LDFLAGS := -D__WIN32__ -s -mwindows -mthreads -mdll -Wl -o LCDriver_CNH1606432.dll
endif
# Win x64 linker flags
ifeq ($(BUILD_WIN),2)
LDFLAGS := -D__WIN32__ -s -mwindows -mthreads -mdll -Wl -o LCDriver_CNH1606432_x64.dll
endif
endif

build: $(SCRIPT)
	$(MAKE) -C . start-build
# Start Win x32 Build
ifeq ($(USE_MINGW_X32),1)
	$(MAKE) -C . start-build BUILD_WIN=1
else
	@echo "*** warning: No Cross Compiler $(MINGW_X32_CC)g++ found ***"
endif
# Start Win x64 Build
ifeq ($(USE_MINGW_X64),1)
	$(MAKE) -C . start-build BUILD_WIN=2
else
	@echo "*** warning: No Cross Compiler $(MINGW_X64_CC)g++ found ***"
endif

LBITS := $(shell getconf LONG_BIT)

#
# do Linux stuff here
#
ifeq ($(BUILD_WIN),)

ifeq ($(LBITS),64)
#
# do 64 bit stuff here, like set some CFLAGS
#
CXXFLAGS += -DLINUX_64
start-build: configfile setup_folders $(LIB_x32) $(LIB_x64)
else
#
# do 32 bit stuff here
#
CXXFLAGS += -DLINUX_32
start-build: configfile setup_folders $(LIB_x32)
endif

else
#
# do Windows stuff here
#
ifeq ($(BUILD_WIN),1)
start-build: configfile setup_folders $(LIB_x32)
endif
ifeq ($(BUILD_WIN),2)
start-build: configfile setup_folders $(LIB_x64)
endif

endif

$(LIB_x32): $(LIBOBJ_x32)
	@echo "Linking in /flash_kit/loader_communication/out/out_linux/x32, for windows_x32"
	@$(CXX) $(LDFLAGS) -m32 -o $(LIB_x32) $(addprefix $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/, $(^F)) $(LIBS)

$(LIB_x64): $(LIBOBJ_x64)
	@echo "Linking in /flash_kit/loader_communication/out/out_linux/x64, for windows_x64"
	@$(CXX) $(LDFLAGS) -o $(LIB_x64) $(addprefix $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/, $(^F)) $(LIBS)

#
# Source files build x32
#
$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: source/utilities/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: source/CEH/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: source/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: source/LCM/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: $(AUTO_DIR_LIB)/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: source/security_algorithms/sha/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: source/security_algorithms/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp

ifeq ($(BUILD_WIN),)
$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: source/api_wrappers/linux/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -m32 -c $< -o $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$*.d.tmp
endif

ifeq ($(BUILD_WIN),1)
$(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/%.o: $(AUTO_DIR_LIB)/outLCDriver.rc $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "Linking outLCDriver.rc for windows_x32"
	@$(MINGW_X32_CC)windres $(AUTO_DIR_LIB)/outLCDriver.rc $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR)/$(@F)
endif

#
# Source files build x64
#
$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: source/utilities/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: source/CEH/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: source/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: source/LCM/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: source/security_algorithms/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: source/security_algorithms/sha/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp

$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: $(AUTO_DIR_LIB)/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "$(CXX)     $<"
	@	$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp

ifeq ($(BUILD_WIN),)
$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: source/api_wrappers/linux/%.cpp $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -MMD -c $< -o $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
	@cp $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp >> $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d
	@rm -f $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$*.d.tmp
endif

ifeq ($(BUILD_WIN),2)
$(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/%.o: $(AUTO_DIR_LIB)/outLCDriver.rc $(AUTOGEN_FILES)
	@mkdir -p $(dir $@)
	@echo "Linking outLCDriver.rc for windows_x64"
	@$(MINGW_X64_CC)windres $(AUTO_DIR_LIB)/outLCDriver.rc $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR)/$(@F)
endif

#Autogen files
$(AUTO_DIR_LIB)/command_ids.h: $(LCD_CONFIG)commands.xml $(LCD_CONFIG)command_ids_h.xsl | setup_folders
	@echo "xalan: $(XALAN)"
	@echo "Generating autogen     $(AUTO_DIR_LIB)command_ids.h..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)commands.xml -xsl $(LCD_CONFIG)command_ids_h.xsl -out $(AUTO_DIR_LIB)/command_ids.h

$(AUTO_DIR_LIB)/commands.h: $(LCD_CONFIG)commands.xml $(LCD_CONFIG)commands_h.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)commands.h..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)commands.xml -xsl $(LCD_CONFIG)commands_h.xsl -out $(AUTO_DIR_LIB)/commands.h

$(AUTO_DIR_LIB)/commands_impl.h: $(LCD_CONFIG)commands.xml $(LCD_CONFIG)commands_impl_h.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)commands_impl.h..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)commands.xml -xsl $(LCD_CONFIG)commands_impl_h.xsl -out $(AUTO_DIR_LIB)/commands_impl.h

$(AUTO_DIR_LIB)/commands_marshal.cpp: $(LCD_CONFIG)commands.xml $(LCD_CONFIG)commands_marshal_cpp.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)commands_marshal.cpp..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)commands.xml -xsl $(LCD_CONFIG)commands_marshal_cpp.xsl -out $(AUTO_DIR_LIB)/commands_marshal.cpp

$(AUTO_DIR_LIB)/lcdriver_error_codes.h: $(LCD_CONFIG)lcdriver_error_codes.xml $(LCD_CONFIG)lcdriver_error_codes_h.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)lcdriver_error_codes.h..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)lcdriver_error_codes.xml -xsl $(LCD_CONFIG)lcdriver_error_codes_h.xsl -out $(AUTO_DIR_LIB)/lcdriver_error_codes.h

$(AUTO_DIR_LIB)/a2_command_ids.h: $(LCD_CONFIG)a2_commands.xml $(LCD_CONFIG)a2_command_ids_h.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)a2_command_ids.h..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)a2_commands.xml -xsl $(LCD_CONFIG)a2_command_ids_h.xsl -out $(AUTO_DIR_LIB)/a2_command_ids.h

$(AUTO_DIR_LIB)/a2_commands.h: $(LCD_CONFIG)a2_commands.xml $(LCD_CONFIG)a2_commands_h.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)a2_commands.h..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)a2_commands.xml -xsl $(LCD_CONFIG)a2_commands_h.xsl -out $(AUTO_DIR_LIB)/a2_commands.h

$(AUTO_DIR_LIB)/a2_commands_impl.h: $(LCD_CONFIG)a2_commands.xml $(LCD_CONFIG)a2_commands_impl_h.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)a2_commands_impl.h..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)a2_commands.xml -xsl $(LCD_CONFIG)a2_commands_impl_h.xsl -out $(AUTO_DIR_LIB)/a2_commands_impl.h

$(AUTO_DIR_LIB)/a2_commands_marshal.cpp: $(LCD_CONFIG)a2_commands.xml $(LCD_CONFIG)a2_commands_marshal_cpp.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)a2_commands_marshal.cpp..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)a2_commands.xml -xsl $(LCD_CONFIG)a2_commands_marshal_cpp.xsl -out $(AUTO_DIR_LIB)/a2_commands_marshal.cpp

$(AUTO_DIR_LIB)/error_codes_desc.cpp: $(LCD_CONFIG)lcdriver_error_codes.xml $(LCD_CONFIG)error_codes_desc_cpp.xsl | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)error_codes_desc.cpp..."
	@java -classpath $(XALAN) org.apache.xalan.xslt.Process -in $(LCD_CONFIG)lcdriver_error_codes.xml -xsl $(LCD_CONFIG)error_codes_desc_cpp.xsl -out $@ -PARAM errorCodesLcmXml $(LCM_ERR_DESC_PATH)

$(AUTO_DIR_LIB)/LcdVersion.cpp: $(LCD_DIR)source/gen_version_files.sh | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)LcdVersion.cpp..."
	bash $(LCD_DIR)source/gen_version_files.sh --lcd $(abspath $(AUTO_DIR_LIB)) $(abspath $(LCD_DIR))

$(AUTO_DIR_LIB)/outLCDriver.rc: $(LCD_DIR)source/gen_rc.sh | setup_folders
	@echo "Generating autogen     $(AUTO_DIR_LIB)outLCDriver.rc..."
	bash $(LCD_DIR)source/gen_rc.sh --lcd $(AUTO_DIR_LIB)

#setting up needed folders
$(BUILDFOLDER): | configfile
	@mkdir -p $@ 2> /dev/null

$(BUILDFOLDER)/.builddummy: | $(BUILDFOLDER)
	@touch $@

$(AUTO_DIR_LIB): | configfile
	@mkdir -p $@ 2> /dev/null

$(AUTO_DIR_LIB)/.autodirdummy: | $(AUTO_DIR_LIB)
	@touch $@

$(LCD_INSTALLDIR): | configfile
	@mkdir -p $@ 2> /dev/null

$(LCD_INSTALLDIR)/.installdummy: | $(LCD_INSTALLDIR)
	@touch $@

.PHONY: setup_folders
setup_folders: validatevariables
setup_folders: $(BUILDFOLDER)/.builddummy $(AUTO_DIR_LIB)/.autodirdummy $(LCD_INSTALLDIR)/.installdummy
	@echo $< > /dev/null

.PHONY: validatevariables
validatevariables: configfile
	$(if $(CXX),, $(error "CXX not set"))
	$(if $(BUILDFOLDER),, $(error "BUILDFOLDER not set"))
	$(if $(AUTO_DIR_LIB),, $(error "AUTO_DIR_LIB not set"))
	$(if $(LIB_x32),, $(error "LIB_x32 not set"))
	$(if $(LIB_x64),, $(error "LIB_x64 not set"))
	$(if $(LIB_x32_OBJ_DIR),, $(error "LIB_x32_OBJ_DIR not set"))
	$(if $(LIB_x64_OBJ_DIR),, $(error "LIB_x64_OBJ_DIR not set"))
	$(if $(LCD_INSTALLDIR),, $(error "LCD_INSTALLDIR not set"))

#if the wildcard function finds any file matching the name
#of the configfile then this will result in
#"configfile: " so the config target will not be unnecessarily
#called. If however there is no file matching the configfile
#this will become "configfile: config" and the config target
#will be called and the configfile created.
#(calling config directly will always recreate the configfile
#so any values that has been changed will be overwritten with
#the new value)
.PHONY: configfile
configfile: $(if $(wildcard $(config_file)),,config)
	@echo $< > /dev/null

.PHONY: config
config: LIB_x32_OBJ_DIR := x32
config: LIB_x64_OBJ_DIR := x64
config: LCD_INSTALLDIR := /tmp/
config: XALAN := $(XALAN_PATH)xalan.jar
config:
ifeq ($(CONFIG_DIR),)
    BUILDOUT := $(LCD_DIR)out
else
    BUILDOUT := $(CONFIG_DIR)/out
endif
config:
	@echo Generating config file...
	@rm -f $(config_file)
	@touch $(config_file)
	@echo "BUILDOUT := $(BUILDOUT)" >> $(config_file)

	@echo "ifeq (\$$(BUILD_WIN),)" >> $(config_file)
	@echo "BUILDFOLDER := \$$(BUILDOUT)/out_linux" >> $(config_file)
	@echo "CXX := $(CROSS_PREFIX)g++"  >> $(config_file)
	@echo "LIB_x32 := \$$(BUILDFOLDER)/liblcdriver.$(LIB_EXTENSION)"  >> $(config_file)
	@echo "LIB_x64 := \$$(BUILDFOLDER)/liblcdriver_x64.$(LIB_EXTENSION)"  >> $(config_file)
	@echo "else"  >> $(config_file)

	@echo "ifeq (\$$(BUILD_WIN),1)" >> $(config_file)
	@echo "BUILDFOLDER := \$$(BUILDOUT)/out_win" >> $(config_file)
	@echo "CXX := $(CROSS_PREFIX)$(MINGW_X32_CC)g++" >> $(config_file)
	@echo "LIB_x32 := \$$(BUILDFOLDER)/LCDriver_CNH1606432.dll" >> $(config_file)
	@echo "LIB_x64 := \$$(BUILDFOLDER)/LCDriver_CNH1606432_x64.dll" >> $(config_file)
	@echo "endif"  >> $(config_file)

	@echo "ifeq (\$$(BUILD_WIN),2)" >> $(config_file)
	@echo "BUILDFOLDER := \$$(BUILDOUT)/out_win" >> $(config_file)
	@echo "CXX := $(CROSS_PREFIX)$(MINGW_X64_CC)g++" >> $(config_file)
	@echo "LIB_x32 := \$$(BUILDFOLDER)/LCDriver_CNH1606432.dll" >> $(config_file)
	@echo "LIB_x64 := \$$(BUILDFOLDER)/LCDriver_CNH1606432_x64.dll" >> $(config_file)
	@echo "endif"  >> $(config_file)

	@echo "endif"  >> $(config_file)
	@echo "AUTO_DIR_LIB := \$$(BUILDOUT)/autogen/" >> $(config_file)
	@echo "LIB_x32_OBJ_DIR := $(LIB_x32_OBJ_DIR)" >> $(config_file)
	@echo "LIB_x64_OBJ_DIR := $(LIB_x64_OBJ_DIR)" >> $(config_file)
	@echo "LCD_INSTALLDIR := $(LCD_INSTALLDIR)" >> $(config_file)
	@echo "XALAN := $(XALAN)" >> $(config_file)

install:
	$(MAKE) -C . start-install
ifeq ($(USE_MINGW_X32),1)
	$(MAKE) -C . start-install BUILD_WIN=1
endif
ifeq ($(USE_MINGW_X64),1)
	$(MAKE) -C . start-install BUILD_WIN=2
endif

start-install: start-build
ifeq ($(BUILD_WIN),)
	install -m 0755 $(BUILDFOLDER)/liblcdriver.$(LIB_EXTENSION) $(LCD_INSTALLDIR)
ifeq ($(LBITS),64)
	install -m 0755 $(BUILDFOLDER)/liblcdriver_x64.$(LIB_EXTENSION) $(LCD_INSTALLDIR)
endif
else

	install -m 0755 $(LCD_DIR)mingw_prebuilt/mingwm10.dll $(LCD_INSTALLDIR)
ifeq ($(BUILD_WIN),1)
	install -m 0755 $(BUILDFOLDER)/LCDriver_CNH1606432.dll $(LCD_INSTALLDIR)
endif
ifeq ($(BUILD_WIN),2)
	install -m 0755 $(BUILDFOLDER)/LCDriver_CNH1606432_x64.dll $(LCD_INSTALLDIR)
endif

endif


clean:
	$(MAKE) -C . start-clean
	$(MAKE) -C . start-clean BUILD_WIN=1
	$(MAKE) -C . start-clean BUILD_WIN=2

start-clean:
ifeq ($(LBITS),64)
	$(if $(BUILDFOLDER), \
		$(if $(LIB_x64_OBJ_DIR), \
			@rm -rf $(BUILDFOLDER)/$(LIB_x64_OBJ_DIR) \
			@rm -rf $(BUILDFOLDER),),)
else
	$(if $(BUILDFOLDER), \
		$(if $(LIB_x32_OBJ_DIR), \
			@rm -rf $(BUILDFOLDER)/$(LIB_x32_OBJ_DIR) \
			@rm -rf $(BUILDFOLDER),),)
endif
	$(if $(BUILDFOLDER), \
		@rm -f $(BUILDFOLDER)/*.so* \
		@rm -f $(BUILDFOLDER)/*.dll \
		@rm -rf $(BUILDFOLDER),)
	$(if ${BUILDOUT}, \
		@rm -rf ${BUILDOUT},)

distclean: clean
	$(if $(AUTO_DIR_LIB), \
		@rm -f $(AUTO_DIR_LIB)/*.cpp \
		@rm -f $(AUTO_DIR_LIB)/*.h,)
ifeq ($(LBITS),64)
	$(if $(LCD_INSTALLDIR), \
		@rm -f $(LCD_INSTALLDIR)/LCDriver_CNH1606432_x64.dll \
		@rm -f $(LCD_INSTALLDIR)/liblcdriver_x64.$(LIB_EXTENSION),)
else
	$(if $(LCD_INSTALLDIR), \
		@rm -f $(LCD_INSTALLDIR)/LCDriver_CNH1606432.dll \
		@rm -f $(LCD_INSTALLDIR)/liblcdriver.$(LIB_EXTENSION),)
endif
	$(if $(config_file), \
		@rm -f $(config_file),)

COV_DATA_DIR=cov_data
COV_INTER_DATA_DIR=cov_inter

coverity:
	@if [ -d "$(COV_DATA_DIR)" ]; then \
	@cov-stop-gui --datadir $(COV_DATA_DIR); \
	fi
	@cov-configure --compiler $(CC) --comptype gcc
	@if [ -d "$(COV_DATA_DIR)" ]; then \
		echo coverity gui already installed; \
	else \
		cov-install-gui --password admin --datadir $(COV_DATA_DIR) --product lcd --domain "C/C++"; \
	fi

	@cov-build --dir $(COV_INTER_DATA_DIR) $(MAKE) build
	@cov-analyze --dir $(COV_INTER_DATA_DIR) --aggressiveness-level medium --all
	@cov-commit-defects --datadir $(COV_DATA_DIR) --product lcd --user admin --dir $(COV_INTER_DATA_DIR)
	@cov-start-gui --datadir $(COV_DATA_DIR) --port 1122
	echo Go to localhost port 1122 in webbrowser and login with username admin and password admin to review result

astyle:
	astyle --style=k/r --indent=spaces  --break-blocks --convert-tabs --add-brackets \
	--unpad-paren --pad-header --pad-oper --indent-col1-comments --align-pointer=name \
	-R "*.h" -R "*.c" -R "*.cpp"
