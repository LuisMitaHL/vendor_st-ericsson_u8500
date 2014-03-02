ifeq ($(CONFIG_DIR),)
    config_file=./.config
    LCM_DIR_AUTO:=$(LCM_DIR)/out/autogen
else
    config_file=$(CONFIG_DIR)/.config
    LCM_DIR_AUTO:=$(PRIVATE_LC_LCM_OUT_DIR_AUTO)
endif

-include $(config_file)

INCLUDEDIRS = \
	-Iinclude \
	-I$(AUTO_DIR)/ \
	-I$(LCM_DIR)/source/cnh1605205_ldr_network_layer/include \
	-I$(LCM_DIR)/source/cnh1605204_ldr_transport_layer/include \
	-I$(LCM_DIR)/source/cnh1606344_ldr_communication_module/include \
	-I$(LDR_DIR)/src/loader_communication_module \
	-I$(LDR_DIR)/src/platform_support/stn8500/legacy_compatibility \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/include \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/zip_parser \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605208_ldr_io_subsystem/include \
	-I$(LDR_DIR)/src/loader_code_base/cnh1607300_ldr_lcm_dll/include \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605194_ldr_cops_public/include/u8500 \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605194_ldr_cops_public/source/u8500 \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605721_ldr_security_algorithms/include \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/manifest_parser \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/flashlayout_parser \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/critical_section \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605208_ldr_io_subsystem/source \
	-I$(LDR_DIR)/src/loader_code_base/cnh1606644_ldr_service_management/services/flash_manager \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/string \
	-I$(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/toc_handler \
	-I$(PRFL_DIR)/ \
	-I$(BUILDFOLDER) \
	-I$(LCM_DIR_AUTO) \
	-I$(CSPSA_DIR)

vpath %.c src
vpath %.c $(CSPSA_DIR)
vpath %.c $(LDR_DIR)/src/loader_code_base/cnh1605208_ldr_io_subsystem/source
vpath %.c $(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/string
vpath %.c $(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/zip_parser
vpath %.c $(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/flashlayout_parser
vpath %.c $(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/manifest_parser
vpath %.c $(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/toc_handler
vpath %.c $(LDR_DIR)/src/loader_code_base/cnh1605551_ldr_utilities/source
vpath %.c $(LCM_DIR)/source/cnh1605551_ldr_utilities/critical_section
vpath %.c $(LCM_DIR)/source/cnh1605204_ldr_transport_layer/source
vpath %.c $(LCM_DIR)/source/cnh1605205_ldr_network_layer/source
vpath %.c $(LCM_DIR)/source/cnh1606344_ldr_communication_module/source
vpath %.c $(LCM_DIR_AUTO)

OBJ_COMMON = \
	$(BUILDFOLDER)/cspsa_handler.o \
	$(BUILDFOLDER)/gdf_converter.o \
	$(BUILDFOLDER)/cli_parser.o \
	$(BUILDFOLDER)/preflash_utils.o \
	$(BUILDFOLDER)/file_handler.o \
	$(BUILDFOLDER)/preflash_tool.o \
	$(BUILDFOLDER)/bulk_protocol.o \
	$(BUILDFOLDER)/r15_transport_layer.o \
	$(BUILDFOLDER)/command_protocol.o \
	$(BUILDFOLDER)/communication_service.o \
	$(BUILDFOLDER)/lcm_version.o \
	$(BUILDFOLDER)/r15_family.o\
	$(BUILDFOLDER)/cspsa_api_osfree.o \
	$(BUILDFOLDER)/cspsa_ll.o \
	$(BUILDFOLDER)/crc32.o \
	$(BUILDFOLDER)/cspsa_core.o \
	$(BUILDFOLDER)/io_subsystem.o \
	$(BUILDFOLDER)/zip_parser.o \
	$(BUILDFOLDER)/flashlayout_parser.o \
	$(BUILDFOLDER)/manifest_parser.o \
	$(BUILDFOLDER)/toc_handler_utils.o \
	$(BUILDFOLDER)/serialization.o \
	$(BUILDFOLDER)/io_wrap.o \
	$(BUILDFOLDER)/path_utilities.o \
	$(BUILDFOLDER)/r15_network_layer.o \
	$(BUILDFOLDER)/r15_header.o


ifeq ($(BUILD_WIN32),)
OBJ = $(OBJ_COMMON) $(BUILDFOLDER)/critical_section_linux.o
else
OBJ = $(OBJ_COMMON) $(BUILDFOLDER)/critical_section_win32.o
endif

build: configfile preflash-tool

$(PREFLASH_TOOL_INSTALLDIR): | configfile
	@mkdir -p $@ 2> /dev/null

.PHONY: configfile
configfile: $(if $(wildcard $(config_file)),,config)
	@echo $< > /dev/null

.PHONY: setup_folders
setup_folders: validatevariables
setup_folders: $(BUILDFOLDER)/.builddummy
	@echo $< > /dev/null

.PHONY: validatevariables
validatevariables: configfile
	$(if $(CC),, $(error "CC not set"))
	$(if $(BUILDFOLDER),, $(error "BUILDFOLDER not set"))
	$(if $(AR),, $(error "AR not set"))
	$(if $(OUT_FILE),, $(error "OUT_FILE not set"))
	$(if $(AUTO_DIR),, $(error "AUTO_DIR not set"))

.PHONY: config
config: AUTO_DIR:= $(LCM_DIR_AUTO)/loader
config: PREFLASH_TOOL_INSTALLDIR := /tmp/
config:
ifeq ($(CONFIG_DIR),)
    LCM_DIR_AUTO:=$(LCM_DIR)/out/autogen
else
    LCM_DIR_AUTO:=$(PRIVATE_LC_LCM_OUT_DIR_AUTO)
endif
config:
ifeq ($(BUILD_WIN32),)
config: CC := gcc
config: AR := ar
config: CFLAGS = -std=c99 -Wall -DUINT64_SUPPORTED -DSINT64_SUPPORTED -DINT64_BASE_TYPE='"long long"' -DCFG_ENABLE_LOADER_TYPE -DMACH_TYPE_STN8500 -D__RELEASE -DDISABLE_SECURITY
config: LINKOPTS := -L$(BUILDFOLDER)
config: OUT_FILE := preflash_tool
else
config: CC := i586-mingw32msvc-gcc
config: AR := i586-mingw32msvc-ar
config: CFLAGS := -Wall -D__WIN32__ -mwindows -DCFG_ENABLE_LOADER_TYPE -DMACH_TYPE_STN8500 -D__RELEASE -DLCM_EXPORTS -DDISABLE_SECURITY
config: LINKOPTS := -L$(BUILDFOLDER)
config: OUT_FILE := preflash_tool.exe
endif
config:
	@echo Generating config file...
	@rm -f $(config_file)
	@touch $(config_file)
	@mkdir -p $(BUILDFOLDER)
	@echo BUILDFOLDER: $(BUILDFOLDER)
	@echo "BUILDFOLDER := $(BUILDFOLDER)" >> $(config_file)
	@echo PREFLASH_TOOL_INSTALLDIR: $(PREFLASH_TOOL_INSTALLDIR)
	@echo "PREFLASH_TOOL_INSTALLDIR := $(PREFLASH_TOOL_INSTALLDIR)" >> $(config_file)
	@echo CC: $(CC)
	@echo "CC := $(CC)" >> $(config_file)
	@echo AR: $(AR)
	@echo "AR := $(AR)" >> $(config_file)
	@echo CFLAGS: $(CFLAGS)
	@echo "CFLAGS := $(CFLAGS)" >> $(config_file)
	@echo LINKOPTS: $(LINKOPTS)
	@echo "LINKOPTS := $(LINKOPTS)" >> $(config_file)
	@echo BUILD_WIN32: $(BUILD_WIN32)
	@echo "BUILD_WIN32 := $(BUILD_WIN32)" >> $(config_file)
	@echo OUT_FILE: $(OUT_FILE)
	@echo "OUT_FILE := $(OUT_FILE)" >> $(config_file)
	@echo AUTO_DIR: $(AUTO_DIR)
	@echo "AUTO_DIR := $(AUTO_DIR)" >> $(config_file)

preflash-tool: $(OBJ)
	@echo Linking...
	$(CC) $(OBJ) $(LINKOPTS) -o $(BUILDFOLDER)/$(OUT_FILE)

$(BUILDFOLDER)/%.o: %.c
	$(CC) -g $(CFLAGS) $(INCLUDEDIRS) -c -o $@ $<

install: build
	@$(if $(PREFLASH_TOOL_INSTALLDIR), echo "Copying binaries to $(PREFLASH_TOOL_INSTALLDIR)" && mkdir -p $(PREFLASH_TOOL_INSTALLDIR)/ && \
	cp $(BUILDFOLDER)/$(OUT_FILE) $(PREFLASH_TOOL_INSTALLDIR)/ && \
	cp ./readme $(PREFLASH_TOOL_INSTALLDIR)/,)

clean:
	$(if ${BUILDFOLDER}, \
		@rm -f ${BUILDFOLDER}/*.o \
		@rm -rf ${BUILDFOLDER},)


distclean: clean
	$(if $(config_file), \
		rm -f $(config_file),)

astyle:
	astyle --style=k/r --indent=spaces  --break-blocks --convert-tabs --add-brackets \
        --unpad-paren --pad-header --pad-oper --indent-col1-comments --align-pointer=name \
        -R "*.h" -R "*.c"

