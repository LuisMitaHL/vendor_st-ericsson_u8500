ifeq ($(CONFIG_DIR),)
    configfile=.config
    specificflags_file=specific_flags.txt
    LCM_DIR_AUTO:=$(LCM_DIR)/out/autogen
else
    configfile=$(CONFIG_DIR)/.config
    specificflags_file=$(CONFIG_DIR)/specific_flags.txt
    LCM_DIR_AUTO:=$(PRIVATE_LC_LCM_OUT_DIR)/out/autogen
endif
-include $(configfile)

# Prevention rules if the include files are not generated yet
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.d: ;
$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/%.d: ;

-include $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/*.d
-include $(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/*.d

LOADERNAME := FlashLoader

LOADERVERSION=\"$(shell git describe --tags --always)\"
ifneq ($(LCD_DIR),)
	LCVERSION=\"$(shell git --git-dir $(LCD_DIR)/.git describe --tags --always)\"
endif
ifneq ($(BASSAPP_DIR),)
	BASSAPVERSION=\"$(shell git --git-dir $(BASSAPP_DIR)/.git describe --tags --always)\"
endif
ifneq ($(COPS_DIR),)
	COPSVERSION=\"$(shell git --git-dir $(COPS_DIR)/.git describe --tags --always\)\"
endif
ifneq ($(CSPSA),)
	CSPSAVERSION=\"$(shell git --git-dir $(CSPSA)/.git/ describe --tags --always)\"
endif

ifeq ($(STE_HW),all)
else ifeq ($(STE_HW),V2)
  HWTYPE_FLAGS := -DST_HREFV2
else ifeq ($(STE_HW),V1)
  HWTYPE_FLAGS := -DST_HREFV1
else ifeq ($(STE_HW),ED)
  HWTYPE_FLAGS := -D__DB_8500_ED -DST_8500ED -DST_HREFED -DST_8500_ED
else ifeq ($(STE_HW),U5500_V2)
  HWTYPE_FLAGS := -DST_5500V2
else ifeq ($(STE_HW),U5500_V2_eMMC2)
  HWTYPE_FLAGS := -DST_5500V2 -DSWITCH_EMMC_ORDER
#else
#  $(error "HW not supported!!!")
endif

# Never use CFLAGS(except in this case),
# by doing that the user might change
# default flags while we are able to
# keep other flags defined here.
# Couldnt find exact match for RVCT assembler flags:
#  --no_unaligned_access --apcs /interwork
# Couldnt find exact match for RVCT compiler flags:
#(-mfpu=vfp seem to indicate vfpv2 accroding to http://www.codesourcery.com/archives/arm-gnu/msg02408.html)
#Should -mabi=apcs-gnu be specified for --apcs?
#How to specify --library_interface=aeabi_clib90 and do we need to?
ifeq ($(STE_PLATFORM),u8500)
HCL_PATH := platform_support/stn8500/hardware_drivers/stn8500
else ifeq ($(STE_PLATFORM),u5500)
HCL_PATH := platform_support/u5500/hardware_drivers/u5500
HWTYPE_FLAGS += -D_DB5500
HWTYPE_FLAGS += -DSECURE_CHIP
else ifeq (u9500,$(findstring u9500,$(STE_PLATFORM)))
HCL_PATH := platform_support/stn8500/hardware_drivers/stn8500
else ifeq (u9540,$(findstring u9540,$(STE_PLATFORM)))
HCL_PATH := platform_support/stn8500/hardware_drivers/stn8500
HWTYPE_FLAGS += -DSTE_AP9540
else ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
HCL_PATH := platform_support/stn8500/hardware_drivers/stn8500
HWTYPE_FLAGS += -DSTE_AP8540
endif

ifeq ($(ENABLE_NONSIGNED_FLASHARCHIVE),1)
LOADER_FEATURES_FLAGS += -DNONSIGNED_ARCHIVE_ENABLED
endif

#Set DISABLE_LOADER_WD_RESTART to disable
#Loader feature for WD restarting if loader
#execution stall beacause of error
ifneq ($(DISABLE_LOADER_WD_RESTART),1)
ENABLE_LOADER_HWI2C_WD := 1
DISABLE_HWI2C_WD_DELAY_KICK := 1
endif

LEGACY_COMPAT_INCLUDE_PATH := -I./src/platform_support/stn8500/legacy_compatibility
REAL_INCLUDE_PATH := $(LEGACY_COMPAT_INCLUDE_PATH) \
		-I$(LCM_DIR)/source/cnh1605204_ldr_transport_layer/include/\
		-I$(LCM_DIR)/source/cnh1605205_ldr_network_layer/include/\
		-I$(LCM_DIR)/source/cnh1606344_ldr_communication_module/include/\
		-I$(LCM_DIR_AUTO)/loader/\
		-I$(CLIBINCL)/\
		-I$(GCCLIBINCL)/\
		-I$(CSPSA)/\
		-I./src/platform_support/\
		-I./src/platform_support/stn8500/chipid/\
		-I./src/platform_support/stn8500/hardware_drivers/stn8500/memmap/\
		-I./src/loader_code_base/cnh1605196_ldr_flash_app/include/\
		-I./src/loader_code_base/cnh1605196_ldr_flash_app/source/\
		-I./src/loader_code_base/cnh1605197_ldr_parameter_app/include/\
		-I./src/loader_code_base/cnh1605197_ldr_parameter_app/source/\
		-I./src/loader_code_base/cnh1605200_ldr_command_exec_service/include/\
		-I./src/loader_code_base/cnh1605195_ldr_system_app/include/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/include/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/critical_section/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/bitset\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/debug_subsystem\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/boot_records_handler/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/toc_handler/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/flashlayout_parser/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/versions_parser/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/stack/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/zip_parser/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/manifest_parser/\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/measurment_tool/\
		-I./src/loader_code_base/cnh1605203_ldr_communication_buffer/include/\
		-I./src/loader_code_base/cnh1605208_ldr_io_subsystem/include/\
		-I./src/loader_code_base/cnh1605208_ldr_io_subsystem/source/\
		-I./src/loader_code_base/cnh1605200_ldr_command_exec_service/source/\
		-I./src/loader_code_base/cnh1605720_ldr_time_utilities/include/\
		-I./src/loader_code_base/cnh1605721_ldr_security_algorithms/include/\
		-I./src/loader_code_base/cnh1605226_ldr_main/include/\
		-I./src/loader_code_base/cnh1605226_ldr_main/source/\
		-I./src/loader_code_base/cnh1606559_ldr_security_app/include/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/include/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/block_device_manager/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/flash_manager/hw_u8500/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/boot_area_manager/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/block_device_manager/emmc\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/boot_area_manager/emmc\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/file_system_manager/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/cops_data_manager/u8500/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/include/stn8500/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/flash_manager/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/global_data_manager/\
		-I./src/loader_code_base/cnh1606644_ldr_service_management/services/global_data_manager/cspsa/\
		-I./src/loader_code_base/cnh1605195_ldr_system_app/source/\
		-I./src/loader_code_base/cnh1608018_ldr_hw_abstraction/comm_devices/\
		-I./src/loader_code_base/cnh1608018_ldr_hw_abstraction/communication_abstraction/\
		-I./src/loader_code_base/cnh1608018_ldr_hw_abstraction/communication_abstraction/UART_abstraction/\
		-I./src/loader_code_base/cnh1608018_ldr_hw_abstraction/communication_abstraction/USB_abstraction/\
		-I./src/loader_code_base/cnh1608018_ldr_hw_abstraction/cpu_support/\
		-I./src/loader_code_base/cnh1608018_ldr_hw_abstraction/atomic_functions/\
		-I./src/loader_code_base/cnh1606558_ldr_otp_app/include/\
		-I./src/loader_code_base/cnh1606558_ldr_otp_app/source/\
		-I$(COPS_INSTALL)\
		-I$(BASSAPP_INSTALL)\
		-I./src/loader_code_base/cnh1605551_ldr_utilities/debug_subsystem/\
		-I./src/loader_code_base/cnh1605194_ldr_cops_public/include/u8500/\
		-I./src/loader_code_base/cnh1605194_ldr_cops_public/source/u8500/\
		-I./src/loader_code_base/cnh1605207_ldr_emulator/modules/cnh1601476_nand_pd/public/\
		-I./src/platform_support/stn8500/stn8500_time_utilities/\
		-I./src/platform_support/stn8500/stn8500_usb_dma_support/\
		-I./src/platform_support/stn8500/stn8500_system_applications_functions/\
		-I./src/platform_support/mmu/\
		-I./src/$(HCL_PATH)/services/src/init/\
		-I./src/$(HCL_PATH)/services/src/uart/\
		-I./src/$(HCL_PATH)/services/src/usb/\
		-I./src/$(HCL_PATH)/services/src/dma/\
		-I./src/$(HCL_PATH)/services/src/gpio/\
		-I./src/$(HCL_PATH)/hcl/src/uart/\
		-I./src/$(HCL_PATH)/hcl/src/ssp/\
		-I./src/$(HCL_PATH)/hcl/src/usb/\
		-I./src/$(HCL_PATH)/hcl/src/mmc_sd/\
		-I./src/$(HCL_PATH)/hcl/src/dma/\
		-I./src/$(HCL_PATH)/hcl/src/gic/\
		-I./src/$(HCL_PATH)/hcl/src/debug/\
		-I./src/$(HCL_PATH)/hcl/src/init/\
		-I./src/$(HCL_PATH)/hcl/src/tmr/\
		-I./src/$(HCL_PATH)/hcl/src/gpio/\
		-I./src/$(HCL_PATH)/services/src/rtc/\
		-I./src/$(HCL_PATH)/hcl/src/rtc/\
		-I../../../../storage/boot_area/toc/\

ifeq ($(STE_PLATFORM),u8500)
REAL_INCLUDE_PATH += -I./src/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/\
		-I./src/platform_support/watchdog/\
		-I./src/$(HCL_PATH)/services/src/i2c/\
		-I./src/$(HCL_PATH)/hcl/src/i2c/\
		-I./src/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/
else ifeq ($(STE_PLATFORM),u5500)
REAL_INCLUDE_PATH += -I./src/$(HCL_PATH)/platform/st_u5500/hcl/src/ab8500/ab8500_core/\
		-I./src/$(HCL_PATH)//platform/st_u5500/services/src/ab8500/ab8500_core/
else ifeq (u9500,$(findstring u9500,$(STE_PLATFORM)))
REAL_INCLUDE_PATH += -I./src/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/\
		-I./src/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/\
		-I./src/platform_support/watchdog/\
		-I./src/$(HCL_PATH)/services/src/i2c/\
		-I./src/$(HCL_PATH)/hcl/src/i2c/
else ifeq (u9540,$(findstring u9540,$(STE_PLATFORM)))
REAL_INCLUDE_PATH += -I./src/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/\
		-I./src/platform_support/watchdog/\
		-I./src/$(HCL_PATH)/services/src/i2c/\
		-I./src/$(HCL_PATH)/hcl/src/i2c/\
		-I./src/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/
else ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
REAL_INCLUDE_PATH += -I./src/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/\
		-I./src/platform_support/watchdog/\
		-I./src/$(HCL_PATH)/services/src/i2c/\
		-I./src/$(HCL_PATH)/hcl/src/i2c/\
		-I./src/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/
endif

OBJS :=	\
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/audit.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/command_marshal.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/ces_commands.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/custom_command_marshal.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/a2_protocol.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/a2_speedflash.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/a2_transport.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/bulk_protocol.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/command_protocol.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/protrom_protocol.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/protrom_transport.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/r15_transport_layer.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/z_transport.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/a2_header.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/a2_network.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/protrom_header.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/protrom_network.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/r15_header.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/r15_network_layer.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/z_network.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/a2_family.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/communication_service.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/lcm_version.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/protrom_family.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/r15_family.o \
	$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/z_family.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/chipid/ChipID.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/startup/init.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/startup/vectors.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/startup/handlers.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/clib_glue/clib_glue.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/currentdate.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/mmu/mmu.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605194_ldr_cops_public/source/u8500/seclib_functions.o	\
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605194_ldr_cops_public/source/u8500/loader_sec_lib_verify.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605194_ldr_cops_public/source/u8500/loader_sec_lib_port_funcs.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605196_ldr_flash_app/source/flash_applications_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605196_ldr_flash_app/source/flash_process_file_info.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605196_ldr_flash_app/source/flash_process_file_repeat.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605196_ldr_flash_app/source/flash_dump_area_repeat.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605197_ldr_parameter_app/source/parameter_applications_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605200_ldr_command_exec_service/source/command_exec_service.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605200_ldr_command_exec_service/source/running_commands.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605200_ldr_command_exec_service/source/command_audit.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605203_ldr_communication_buffer/source/communication_buffer.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605208_ldr_io_subsystem/source/io_bam.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605208_ldr_io_subsystem/source/io_bdm.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605208_ldr_io_subsystem/source/io_bulk.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605208_ldr_io_subsystem/source/io_fpd.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605208_ldr_io_subsystem/source/io_secure_wrap.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605208_ldr_io_subsystem/source/io_subsystem.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605208_ldr_io_subsystem/source/io_wrap.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605226_ldr_main/source/main.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605226_ldr_main/source/ldr_init.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605720_ldr_time_utilities/source/time_utilities.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/bitset/bitset.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/source/queue.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/source/serialization.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/critical_section/critical_section.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/toc_handler/toc_handler.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/toc_handler/toc_handler_utils.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/toc_handler/toc_handler_cops_wrap.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/source/path_utilities.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/flashlayout_parser/flashlayout_parser.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/stack/r_stack_utils.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/debug_subsystem/debug_subsystem.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/zip_parser/zip_parser.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/manifest_parser/manifest_parser.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/measurement_tool/measurement_tool.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605551_ldr_utilities/versions_parser/versions_parser.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605721_ldr_security_algorithms/source/security_algorithms.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1605195_ldr_system_app/source/system_applications_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606559_ldr_security_app/source/security_applications_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606559_ldr_security_app/source/security_applications_repeat_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/source/service_management.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/block_device_manager/block_device_management.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/block_device_manager/emmc/bdm_emmc.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/boot_area_manager/boot_area_management.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/boot_area_manager/emmc/bam_emmc.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/flash_manager/flash_manager.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/cops_data_manager/u8500/cops_data_manager.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/global_data_manager/global_data_manager.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/global_data_manager/cspsa/gdm_cspsa.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/global_data_manager/cspsa/cspsa_plugin.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1608018_ldr_hw_abstraction/atomic_functions/atomic_functions_arm.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1608018_ldr_hw_abstraction/comm_devices/comm_devices.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1608018_ldr_hw_abstraction/communication_abstraction/UART_abstraction/stn8500_uart_abstraction.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1608018_ldr_hw_abstraction/communication_abstraction/USB_abstraction/stn8500_usb_abstraction.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1608018_ldr_hw_abstraction/communication_abstraction/stn8500_communication_abstraction.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606558_ldr_otp_app/source/otp_applications_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/cpu_support.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/stn8500_usb_dma_support/stn8500_usb_dma_support.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/stn8500_loader_init.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/flash_manager/hw_u8500/fm_emmc.o\
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/loader_code_base/cnh1606644_ldr_service_management/services/flash_manager/hw_u8500/emmc_rpmb_cops_wrap.o\
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/stn8500_time_utilities/stn8500_time_utilities.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/stn8500_system_applications_functions/stn8500_system_power_applications_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/stn8500_system_applications_functions/stn8500_system_uart_applications_functions.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/mmc_sd/mmc_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/mmc_sd/mmc.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/mmc_sd/mmc_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/usb/usb_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/usb/usb.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/services/src/dma/dma_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/usb/usb_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/uart/uart_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/uart/uart.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/uart/uart_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/gpio/gpio_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/gpio/gpio.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/gpio/gpio_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/gic/gic_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/gic/gic.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/tmr/tmr.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/tmr/tmr_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/rtc/rtc.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/rtc/rtc_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/rtc/rtc_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/dma/dma.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/dma/dma_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/dma/dma_base.o \

ifeq ($(STE_PLATFORM),u8500)
OBJS +=	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/ssp/ssp_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/ab8500_core.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/ab8500_core_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/i2c/i2c_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/watchdog/watchdog.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp_irq.o
else ifeq ($(STE_PLATFORM),u5500)
OBJS += 	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_u5500/hcl/src/ab8500/ab8500_core/ab8500_core.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_u5500/services/src/ab8500/ab8500_core/ab8500_core_services.o
else ifeq (u9500,$(findstring u9500,$(STE_PLATFORM)))
OBJS +=	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/ssp/ssp_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/ab8500_core.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/ab8500_core_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/i2c/i2c_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/watchdog/watchdog.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp_irq.o
else ifeq (u9540,$(findstring u9540,$(STE_PLATFORM)))
OBJS +=	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/ssp/ssp_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/ab8500_core.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/ab8500_core_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/i2c/i2c_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/watchdog/watchdog.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp_irq.o
else ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
OBJS +=	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/ssp/ssp_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/hcl/ab8500/ab8500_core/ab8500_core.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/platform/st_pool/services/ab8500/ab8500_core/ab8500_core_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/services/src/i2c/i2c_services.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/stn8500/hardware_drivers/stn8500/hcl/src/i2c/i2c_irq.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/platform_support/watchdog/watchdog.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp.o \
	$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$(HCL_PATH)/hcl/src/ssp/ssp_irq.o
endif


ifeq ($(STE_PLATFORM),u8500)
STE_PLATFORM_NUM := 8500
LINKERFILE := loader_u8500.ld
else ifeq ($(STE_PLATFORM),u5500)
STE_PLATFORM_NUM := 5500
LINKERFILE := loader_u5500.ld
else ifeq (u9500,$(findstring u9500,$(STE_PLATFORM)))
STE_PLATFORM_NUM := 8500
LINKERFILE := loader_u8500.ld
else ifeq (u9540,$(findstring u9540,$(STE_PLATFORM)))
STE_PLATFORM_NUM := 8500
LINKERFILE := loader_u8500.ld
else ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
STE_PLATFORM_NUM := 8500
LINKERFILE := loader_u8500.ld
endif

#Use thumb mode when compiling and thumb version of libraries
USE_THUMB=1
APPCONFIG_FLAGS := -DCFG_ENABLE_APPLICATION_SYSTEM
HWCONFIG_FLAGS := -DCNH1606644_BDM_USES_EMMC -DCNH1606644_BAM_USES_EMMC -DCNH1606644_FPD_USES_EMMC \
                  -DENABLE_DEFAULT_BAM_CONFIGURATION -DENABLE_DEFAULT_BDM_CONFIGURATION \
                  -DCNH1606644_FM_USES_EMMC -DCNH1606644_BDM_USES_EMMC -DCNH1606644_BAM_USES_EMMC \
                  -D__MMCI_HCL_ENHANCED -D__MMCI_HCL_ELEMENTARY -D__HCL_USB_FB -DOS_FREE -D__I2C_ENHANCED \
                  -D__PRCM_HWI2C $(if $(ENABLE_BASS_APP_TEST), -DENABLE_BASS_APP_TEST, ) \
                   $(if $(ENABLE_LOADER_HWI2C_WD), -DENABLE_LOADER_HWI2C_WD, ) $(if $(DISABLE_HWI2C_WD_DELAY_KICK), -DDISABLE_HWI2C_WD_DELAY_KICK, )


ifeq ($(STE_PLATFORM), u9540)
HWCONFIG_FLAGS += -DDISABLE_HWI2C_WD_KICK
endif
ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
HWCONFIG_FLAGS += -DDISABLE_HWI2C_WD_KICK
endif

PROJECT_FLAGS := -fno-short-enums -fdollars-in-identifiers -DUINT64_SUPPORTED -DSINT64_SUPPORTED -DINT64_BASE_TYPE="long long" -DCFG_ENABLE_LOADER_SERIALIZATION \
                 -DCFG_ENABLE_COMMUNICATION_SINGLETON -DFAILSAFE_FLASHING -DCFG_ENABLE_LOADER_TYPE -D__UART_ENHANCED -DMACH_TYPE_STN8500 -D__RELEASE \
                 -DLOADERNAME=\"$(LOADERNAME)\" -DSTE_HW=\"$(STE_HW)\" -DSTE_PLATFORM_NUM=$(STE_PLATFORM_NUM) \
                 -DLOADERVERSION=$(LOADERVERSION) $(HWCONFIG_FLAGS) $(APPCONFIG_FLAGS) $(HWTYPE_FLAGS) $(LOADER_FEATURES_FLAGS) \
                 -DLCVERSION=$(LCVERSION) -DBASSAPVERSION=$(BASSAPVERSION) -DCOPSVERSION=$(COPSVERSION) -DCSPSAVERSION=$(CSPSAVERSION)

#Add libraries used by loader
ALL_LDFLAGS := -T $(LINKERFILE) -Map $(BUILDFOLDER)/loader-$(STE_HW)-$(SEC_NAME).map -static -nostdlib --gc-sections --cref -L$(IMP_CLIB) -L$(IMP_GCCLIB) -L$(BUILDFOLDER)/cspsa -L$(BASSAPP_INSTALL) -lcspsa-osfree --wrap printf --wrap puts

ifeq ($(STE_PLATFORM), u8500)
COPS_PLATFORM_TYPE := 8500
else ifeq ($(STE_PLATFORM), u5500)
COPS_PLATFORM_TYPE := 5500
else ifeq ($(STE_PLATFORM), u9540)
COPS_PLATFORM_TYPE := 9540
else ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
COPS_PLATFORM_TYPE := 9540
endif

ifdef ENABLE_BASS_APP_TEST
ifeq ($(SEC_NAME),sec)
	ALL_LDFLAGS += -lbassapp_test
endif
endif

ifeq ($(SEC_NAME),unsec)
	ifeq ($(STE_PLATFORM),u5500)
		PROJECT_FLAGS += -DDISABLE_SECURITY -DDISABLE_COPS -DSKIP_PAYLOAD_VERIFICATION
	else ifeq ($(STE_PLATFORM),u9540)
		PROJECT_FLAGS += -DDISABLE_SECURITY -DDISABLE_COPS -DSKIP_PAYLOAD_VERIFICATION -D_DISABLE_DMA
	else ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
		PROJECT_FLAGS += -DDISABLE_SECURITY -DDISABLE_COPS -DSKIP_PAYLOAD_VERIFICATION
	else #u8500 and u9500 target
		PROJECT_FLAGS += -DDISABLE_SECURITY -DDISABLE_COPS
		ifndef DISABLE_HASH
			ALL_LDFLAGS += -L$(BASSAPP_INSTALL) -lbassapp
		else
			PROJECT_FLAGS += -DSKIP_PAYLOAD_VERIFICATION
		endif
	endif
else ifeq ($(SEC_NAME),sec)
	ALL_LDFLAGS += -L$(COPS_INSTALL) -lcops
	ALL_LDFLAGS += -L$(BASSAPP_INSTALL) -lbassapp
	ifeq ($(STE_PLATFORM),u5500)
		PROJECT_FLAGS += -DOTP_SUPPORT
	else ifeq ($(STE_PLATFORM),u9540)
		PROJECT_FLAGS += -D_DISABLE_DMA
	endif
else
	#$(error "You need to either enable or disable security!")
endif

#add standard libraries
#if the import variable exists find all files ending with .a and filter
#matches in android subdirectory. After that remove .a and replace ./lib
#with -l
ifdef IMP_GCCLIB
	GCC_LIBS= $(patsubst lib%.a, -l%, $(notdir $(shell cd $(IMP_GCCLIB); find . -name '*.a' -type f|grep -v android)))
	ALL_LDFLAGS += -lc $(GCC_LIBS) -lg $(LDFLAGS)
endif

BASE_CFLAGS := -I. $(REAL_INCLUDE_PATH) -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=vfp -mfloat-abi=softfp -mthumb-interwork \
							 -ffunction-sections -fdata-sections -std=c99 -Wcast-align -static $(PROJECT_FLAGS) $(CFLAGS)
ifeq ($(USE_THUMB),1)
	BASE_CFLAGS += -mthumb
endif

BASE_AFLAGS := -mthumb-interwork -mcpu=cortex-a9 $(AFLAGS)

default: build

cspsa: configfile import
	@echo "Invoking CSPSA Makefile, cspsa-dirr    $(CSPSA)"
	@$(MAKE) -C $(CSPSA) CC=$(CC) AR=$(AR) INC=-I../../../tools/platform/flash_kit/loaders/src/platform_support/newlib/include O=$(BUILDFOLDER)/cspsa libcspsa-osfree
	@echo CSPSA creating done

cops : configfile import
	@echo Create cops $(COPS_INSTALL)
	@$(MAKE) -C $(COPS_DIR) -f $(COPS_DIR)/$(COPS_MAKEFILE) INSTALL_PATH=$(COPS_INSTALL) CC=$(CC) AR=$(AR) COPS_PLATFORM_TYPE=$(COPS_PLATFORM_TYPE) LDRINC=-I../../../tools/platform/flash_kit/loaders/src/platform_support/newlib/include $(STUBBED) STE_PLATFORM=$(STE_PLATFORM) CONFIG_DIR=$(CONFIG_DIR)
	@echo Create cops done

bassapp : configfile import
	@echo Create bassapp $(BASSAPP_INSTALL)
	@$(MAKE) -C $(BASSAPP_DIR) build-OSFREE build-TEST CC=$(CC) AR=$(AR) CFLAGS=-I../../../tools/platform/flash_kit/loaders/src/platform_support/newlib/include STE_PLATFORM=$(STE_PLATFORM) CONFIG_DIR=$(CONFIG_DIR) USE_RPMB=1 RPMB_PATH=../../../tools/platform/flash_kit/loaders/src/loader_code_base/cnh1606644_ldr_service_management/services/flash_manager/hw_u8500
	@$(MAKE) -C $(BASSAPP_DIR) install-OSFREE install-TEST includedir=$(BASSAPP_INSTALL) libdir=$(BASSAPP_INSTALL) STE_PLATFORM=$(STE_PLATFORM) CONFIG_DIR=$(CONFIG_DIR) USE_RPMB=1
	@echo Create bassapp done

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
configfile: $(if $(wildcard $(configfile)),,config)

# This target should generate the .config file in the root of the project.
# Default values for LCM folders
# CLIBDIR gets a default value if not set and if USE_THUMB is set
# then /thumb is added to the path as the libs used all
# keep thumb version of the libraries in a subfolder from the arm
# version called "thumb"
# GCCLIBINCL assumes that include files are located in install-tools/include/
# counting from the no-thumb version of the libgcc
.PHONY: config
config: BUILDFOLDER := $(OUT_DIR)
config: TRANSPORT_PATH:=$(LCM_DIR)/source/cnh1605204_ldr_transport_layer
config: NETWORK_PATH:=$(LCM_DIR)/source/cnh1605205_ldr_network_layer
config: LCM_CNH_PATH:=$(LCM_DIR)/source/cnh1606344_ldr_communication_module
config: IMPORT_DIR:=$(BUILDFOLDER)/imported_dependencies
config: LDR_DIR:=$(LDR_DIR)
config: CROSS_PREFIX := arm-eabi
config: CROSS_COMPILE := $(CROSS_PREFIX)-
config: CC := $(CROSS_COMPILE)gcc
config: AS := $(CROSS_COMPILE)as
#use ld.bfd if it is found else just use ld (for toolchain that does not have ld.gold enabled)
config: LD := $(if $(shell bash -c "type $(CROSS_COMPILE)ld"),$(CROSS_COMPILE)ld, $(CROSS_COMPILE)ld)
config: AR := $(CROSS_COMPILE)ar
config: OBJCOPY := $(CROSS_COMPILE)objcopy
config: PERL := "$(shell bash -c "which perl")"
config: CLIBDIR := ./src/platform_support/newlib/lib/
config: CLIBINCL:= $(CLIBDIR)/../include/
config: CLIBDIR := $(patsubst %/thumb,%,$(CLIBDIR))
config: CLIBDIR := $(patsubst %/thumb/,%,$(CLIBDIR))
config: CLIBDIR := $(if $(USE_THUMB), $(patsubst %,%/thumb,$(CLIBDIR)), $(CLIBDIR) )
config: GCCLIBDIR := $(patsubst %/libgcc.a,%,$(shell $(CC) $(BASE_CFLAGS) -print-libgcc-file-name))
config: GCCLIBINCL:= $(shell $(CC) arm-eabi-gcc -print-search-dirs|grep 'install:'|cut -d' ' -f2)/install-tools/include/
config: SRCDEST := ""
config: BINDEST := ""
config: COPS_MAKEFILE := $(COPS_MAKEFILE)
config: COPS_DIR := $(COPS_DIR)
config: COPS_INSTALL := $(COPS_INSTALL)
config: BASSAPP_MAKEFILE := $(BASSAPP_MAKEFILE)
config: BASSAPP_DIR := $(BASSAPP_DIR)
config: LCD_DIR := $(LCD_DIR)
config: BASSAPP_INSTALL := $(BASSAPP_INSTALL)
config: CSPSA := $(CSPSA)
config: LINEPREFIX := "$(shell bash -c "if [ ${VERBOSE} ]; then echo ""; else echo "@"; fi")"
config: IMP_CLIB := $(IMPORT_DIR)/newlib/
config: IMP_GCCLIB := $(IMPORT_DIR)/gcc/
config: COV_DATA_DIR := cov_data
config: COV_INTER_DATA_DIR := cov_inter
config: ENABLE_NONSIGNED_FLASHARCHIVE := $(FLASHING_NONSIGNED)
config:
	@echo Running loaders config
	@rm -f $(configfile)
	@touch $(configfile)
	@echo CROSS_PREFIX: $(CROSS_PREFIX)
	@echo "CROSS_PREFIX := $(CROSS_PREFIX)" >> $(configfile)
	@echo CROSS_COMPILE: $(CROSS_COMPILE)
	@echo "CROSS_COMPILE := $(CROSS_COMPILE)" >> $(configfile)
	@echo CC: $(CC)
	@echo "CC := $(CC)" >> $(configfile)
	@echo AS: $(AS)
	@echo "AS := $(AS)" >> $(configfile)
	@echo LD: $(LD)
	@echo "LD := $(LD)" >> $(configfile)
	@echo AR: $(AR)
	@echo "AR := $(AR)" >> $(configfile)
	@echo OBJCOPY: $(OBJCOPY)
	@echo "OBJCOPY := $(OBJCOPY)" >> $(configfile)
	@echo PERL: $(PERL)
	@echo "PERL := $(PERL)" >> $(configfile)
	@echo CLIBDIR: $(CLIBDIR)
	@echo "CLIBDIR := $(CLIBDIR)" >> $(configfile)
	@echo GCCLIBDIR: $(GCCLIBDIR)
	@echo "GCCLIBDIR := $(GCCLIBDIR)" >> $(configfile)
	@echo CLIBINCL: $(CLIBINCL)
	@echo "CLIBINCL := $(CLIBINCL)" >> $(configfile)
	@echo GCCLIBINCL: $(GCCLIBINCL)
	@echo "GCCLIBINCL := $(GCCLIBINCL)" >> $(configfile)
	@echo LDR_DIR: $(LDR_DIR)
	@echo "LDR_DIR := $(LDR_DIR)" >> $(configfile)
	@echo OUT_DIR: $(OUT_DIR)
	@echo "OUT_DIR := $(OUT_DIR)" >> $(configfile)
	@echo BUILDFOLDER: $(BUILDFOLDER)
	@echo "BUILDFOLDER := $(BUILDFOLDER)" >> $(configfile)
	@echo LCM_DIR: $(LCM_DIR)
	@echo "LCM_DIR := $(LCM_DIR)" >> $(configfile)
	@echo LCM_DIR_AUTO: $(LCM_DIR_AUTO)
	@echo "LCM_DIR_AUTO := $(LCM_DIR_AUTO)" >> $(configfile)
	@echo TRANSPORT_PATH: $(TRANSPORT_PATH)
	@echo "TRANSPORT_PATH:=$(TRANSPORT_PATH)" >> $(configfile)
	@echo NETWORK_PATH: $(NETWORK_PATH)
	@echo "NETWORK_PATH:=$(NETWORK_PATH)" >> $(configfile)
	@echo LCM_CNH_PATH: $(LCM_CNH_PATH)
	@echo "LCM_CNH_PATH:=$(LCM_CNH_PATH)" >> $(configfile)
	@echo IMPORT_DIR: $(IMPORT_DIR)
	@echo "IMPORT_DIR := $(IMPORT_DIR)" >> $(configfile)
	@echo SRCDEST: $(SRCDEST)
	@echo "SRCDEST := $(SRCDEST)" >> $(configfile)
	@echo BINDEST: $(BINDEST)
	@echo "BINDEST := $(BINDEST)" >> $(configfile)
	@echo COPS_MAKEFILE: $(COPS_MAKEFILE)
	@echo "COPS_MAKEFILE := $(COPS_MAKEFILE)" >> $(configfile)
	@echo COPS_DIR : $(COPS_DIR)
	@echo "COPS_DIR := $(COPS_DIR)" >> $(configfile)
	@echo COPS_INSTALL : $(COPS_INSTALL)
	@echo "COPS_INSTALL := $(COPS_INSTALL)" >> $(configfile)
	@echo COPS_PLATFORM_TYPE : $(COPS_PLATFORM_TYPE)
	@echo "COPS_PLATFORM_TYPE := $(COPS_PLATFORM_TYPE)" >> $(configfile)
	@echo BASSAPP_MAKEFILE: $(BASSAPP_MAKEFILE)
	@echo "BASSAPP_MAKEFILE := $(BASSAPP_MAKEFILE)" >> $(configfile)
	@echo BASSAPP_DIR : $(BASSAPP_DIR)
	@echo "BASSAPP_DIR := $(BASSAPP_DIR)" >> $(configfile)
	@echo LCD_DIR : $(LCD_DIR)
	@echo "LCD_DIR := $(LCD_DIR)" >> $(configfile)
	@echo BASSAPP_INSTALL : $(BASSAPP_INSTALL)
	@echo "BASSAPP_INSTALL := $(BASSAPP_INSTALL)" >> $(configfile)
	@echo VERBOSE: $(VERBOSE) # If verbose is not equal to 1 then LINEPREFIX will be @
	@echo "LINEPREFIX := $(LINEPREFIX)" >> $(configfile) # LINEPREFIX has been appended to the begining of each cmd, if it's @ it will remove echo of the cmd.
	@echo IMP_CLIB: $(IMP_CLIB)
	@echo "IMP_CLIB := $(IMP_CLIB)" >> $(configfile)
	@echo IMP_GCCLIB: $(IMP_GCCLIB)
	@echo "IMP_GCCLIB := $(IMP_GCCLIB)" >> $(configfile)
	@echo "COV_DATA_DIR := $(COV_DATA_DIR)" >> $(configfile)
	@echo COV_DATA_DIR: $(COV_DATA_DIR)
	@echo "COV_INTER_DATA_DIR := $(COV_INTER_DATA_DIR)" >> $(configfile)
	@echo COV_INTER_DATA_DIR: $(COV_INTER_DATA_DIR)
ifeq ($(FLASHING_NONSIGNED),1)
	@echo "ENABLE_NONSIGNED_FLASHARCHIVE ?= $(ENABLE_NONSIGNED_FLASHARCHIVE)" >> $(configfile)
	@echo ENABLE_NONSIGNED_FLASHARCHIVE: $(ENABLE_NONSIGNED_FLASHARCHIVE)
endif
	@echo "CSPSA := $(CSPSA)" >> $(configfile)
	@echo CSPSA: $(CSPSA)
	@echo "STE_PLATFORM := $(STE_PLATFORM)" >> $(configfile)
	@echo STE_PLATFORM: $(STE_PLATFORM)
	@echo CONFIG_DIR : $(CONFIG_DIR)
	@echo configfile : $(configfile)
	@if test ! -e $(specificflags_file); then \
		echo "#Addtional flags for specific files can be added in this file" > $(specificflags_file); \
		echo "#and will be used when compiling/assemble the source file" >>$(specificflags_file); \
		echo "#" >> $(specificflags_file); \
		echo "# Examples:" >> $(specificflags_file); \
		echo "#   *emmc*.c: -DSOMEFLAG " >> $(specificflags_file); \
		echo "#   /some/path/*init*.c: -O0 " >>$(specificflags_file); \
		echo "#   main.c: -DPRINT_A_ -DPRINT_B_ -DPRINT_C_" >> $(specificflags_file); \
		echo "#" >>$(specificflags_file); \
		echo "" >>$(specificflags_file); \
	fi

validatevariables: configfile
	$(if $(STE_PLATFORM),, $(error "STE_PLATFORM not set"))
	$(if $(CROSS_PREFIX),, $(error "CROSS_PREFIX not set"))
	$(if $(CROSS_COMPILE),, $(error "CROSS_COMPILE not set"))
	$(if $(BUILDFOLDER),, $(error "BUILDFOLDER not set"))
	$(if $(IMPORT_DIR),, $(error "IMPORT_DIR not set"))
	$(if $(IMP_CLIB),, $(error "IMP_CLIB not set"))
	$(if $(IMP_GCCLIB),, $(error "IMP_GCCLIB not set"))
	$(if $(CC),, $(error "CC not set"))
	$(if $(AS),, $(error "AS not set"))
	$(if $(LD),, $(error "LD not set"))
	$(if $(AR),, $(error "AR not set"))
	$(if $(OBJCOPY),, $(error "OBJCOPY not set"))
	$(if $(PERL),, $(error "PERL not set"))
	$(if $(CLIBINCL),, $(error "CLIBINCL not set"))
	$(if $(GCCLIBINCL),, $(error "GCCLIBINCL not set"))
	$(if $(CLIBDIR),, $(error "CLIBDIR not set"))
	$(if $(GCCLIBDIR),, $(error "GCCLIBDIR not set"))
	$(if $(LCM_DIR),, $(error "LCM_DIR not set"))
	$(if $(LCM_DIR_AUTO),, $(error "LCM_DIR_AUTO not set"))
	$(if $(COPS_MAKEFILE),, $(error "COPS_MAKEFILE not set"))
	$(if $(COPS_DIR),, $(error "COPS_DIR not set"))
	$(if $(COPS_INSTALL),, $(error "COPS_INSTALL not set"))
	$(if $(BASSAPP_MAKEFILE),, $(error "BASSAPP_MAKEFILE not set"))
	$(if $(BASSAPP_DIR),, $(error "BASSAPP_DIR not set"))
	$(if $(LCD_DIR),, $(error "LCD_DIR not set"))
	$(if $(BASSAPP_INSTALL),, $(error "BASSAPP_INSTALL not set"))
	$(if $(CSPSA),, $(error "CSPSA not set"))
	$(if $(BINDEST),, $(error "BINDEST not set"))

$(BUILDFOLDER): | configfile
	@mkdir -p $@ 2> /dev/null

$(BUILDFOLDER)/.builddummy: | $(BUILDFOLDER)
	@touch $@

$(BUILDFOLDER)/inc: | configfile
	@mkdir -p $@ 2> /dev/null

$(BUILDFOLDER)/inc/.incdummy: | $(BUILDFOLDER)/inc
	@touch $@

$(BUILDFOLDER)/autogenoutput: | configfile
	@mkdir -p $@ 2> /dev/null

$(BUILDFOLDER)/autogenoutput/.autogendummy: | $(BUILDFOLDER)/autogenoutput
	@touch $@

$(BUILDFOLDER)/output: | configfile
	@mkdir -p $@ 2> /dev/null

$(BUILDFOLDER)/output/.outputdummy: | $(BUILDFOLDER)/output
	@touch $@



$(IMPORT_DIR): | configfile
	@mkdir -p $@ 2> /dev/null

$(IMPORT_DIR)/.importdummy: | $(IMPORT_DIR)
	@touch $@

$(IMP_CLIB): | configfile
	@mkdir -p $@ 2> /dev/null

$(IMP_CLIB)/.clibdummy: | $(IMP_CLIB)
	@touch $@

$(IMP_GCCLIB): | configfile
	@mkdir -p $@ 2> /dev/null

$(IMP_GCCLIB)/.gcclibdummy: | $(IMP_GCCLIB)
	@touch $@

.PHONY: setupfolders
setupfolders: validatevariables
setupfolders: $(BUILDFOLDER)/.builddummy $(BUILDFOLDER)/inc/.incdummy $(BUILDFOLDER)/autogenoutput/.autogendummy
setupfolders: $(BUILDFOLDER)/output/.outputdummy $(IMPORT_DIR)/.importdummy
setupfolders: $(IMP_CLIB)/.clibdummy $(IMP_GCCLIB)/.gcclibdummy

# A module.s build system that executes
# import shall take the headers and
# libraries from other components and
# put it in a suitable place in its own
# build directory, so that they are visible
# during the build process. The build is
# then possible to execute without access
# to other components. This leads to having
# control over dependencies in the system
# and to be able to build modular.
#
# Note that it is only allowed to import
# headers and libraries.
import: setupfolders
	@cp -r $(CLIBDIR)/* $(IMP_CLIB)
	@cp -r $(GCCLIBDIR)/* $(IMP_GCCLIB)

# These targets should build the loader.
#build,debug: OPTIMIZE_AFLAGS :=
ifeq ($(STE_PLATFORM),u8500)
build: cspsa cops bassapp
	$(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=unsec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=sec
else ifeq ($(STE_PLATFORM),u5500)
build: cspsa cops bassapp
	$(MAKE) -C $(LDR_DIR) start-build STE_HW=U5500_V2 SEC_NAME=unsec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=U5500_V2 SEC_NAME=sec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=U5500_V2_eMMC2 SEC_NAME=unsec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=U5500_V2_eMMC2 SEC_NAME=sec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=U5500 SEC_NAME=unsec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=U5500 SEC_NAME=sec
else ifeq (u9500,$(findstring u9500,$(STE_PLATFORM)))
build: cspsa cops bassapp
	$(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=unsec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=sec
else ifeq (u9540,$(findstring u9540,$(STE_PLATFORM)))
build: cspsa cops bassapp
	$(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=unsec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=sec
else ifeq (l8540,$(findstring l8540,$(STE_PLATFORM)))
build: cspsa cops bassapp
	$(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=unsec && $(MAKE) -C $(LDR_DIR) start-build STE_HW=V2 SEC_NAME=sec
else
# fake when no platform is set so the build target exists
#but only checks variables to give output
build: validatevariables
	@echo "Can't build"
endif

.PHONY: start-build
#Note: debug info should be included both in "debug loader" and
#"no debug loader" therefore som parameters are duplicated both
#if the condition is true or false below
#(notice the , seperating the two cases)
start-build: DEBUG_AFLAGS := $(if $(LOADERS_DEBUG), --defsym ENABLE_DEBUG=1 -gdwarf-2 -gstrict-dwarf -g3, -gdwarf-2 -gstrict-dwarf -g3)
start-build: DEBUG_CFLAGS := $(if $(LOADERS_DEBUG), -DENABLE_DEBUG -DPRINT_A_ -DCFG_ENABLE_PRINT_SERVER -gdwarf-2 -gstrict-dwarf -g3, -gdwarf-2 -gstrict-dwarf -g3)
start-build: OPTIMIZE_CFLAGS := -O2
start-build: ALL_AFLAGS := $(OPTIMIZE_AFLAGS) $(DEBUG_AFLAGS) $(BASE_AFLAGS)
start-build: ALL_CFLAGS := $(OPTIMIZE_CFLAGS) $(DEBUG_CFLAGS) $(BASE_CFLAGS)
start-build: createloader

debug: LOADERS_DEBUG := 1
debug:
	$(MAKE) build LOADERS_DEBUG=1

#Main loader target
createloader: $(OBJS)
	@cd $(BUILDFOLDER)
	$(if $(NOLINK),$(error "Not linking"),)
	@echo Linking: $(LOADERNAME)_$(STE_HW)_$(SEC_NAME).elf
	$(LINEPREFIX) $(LD) $(OBJS) $(ALL_LDFLAGS) -o $(BUILDFOLDER)/$(LOADERNAME)_$(STE_HW)_$(SEC_NAME).elf ; \
		ret=$$? ; \
		if [ $$ret -eq 0 ] ; then \
			echo "$(LD) finished succesfully"; \
		else \
			echo "$(LD) failed ($$ret)" && return $$ret; \
		fi
	$(LINEPREFIX) $(OBJCOPY) -O binary $(BUILDFOLDER)/$(LOADERNAME)_$(STE_HW)_$(SEC_NAME).elf $(BUILDFOLDER)/$(LOADERNAME)_$(STE_HW)_$(SEC_NAME).bin ; \
		ret=$$? ; \
		if [ $$ret -eq 0 ] ; then \
			echo "$(OBJCOPY) finished succesfully"; \
		else \
			echo "$(OBJCOPY) failed ($$ret)" && return $$ret; \
		fi

#This target doesn't do anything, it just shows dependecy to lcm_generate
$(LCM_DIR)/%.h:
	@echo $< > /dev/null

#This target doesn't do anything, it just shows dependecy to lcm_generate
$(LCM_DIR)/%.c:
	@echo $< > /dev/null

#This target doesn't do anything, it just shows dependecy to lcm_generate
$(LCM_DIR_AUTO)/%.h:
	@echo $< > /dev/null

#This target doesn't do anything, it just shows dependecy to lcm_generate
$(LCM_DIR_AUTO)/%.c:
	@echo $< > /dev/null

#This target doesn't do anything, it just shows dependecy to the autogenerated header files
src/%.c: $(LCM_DIR_AUTO)/%.h $(LCM_DIR)/%.h
	@echo $< > /dev/null

#This is just a dummy so make won't try to match the files to any other target
src/%.xml:
	@echo $< > /dev/null

#This is just a dummy so make won't try to match the files to any other target
src/%.xsl:
	@echo $< > /dev/null

#Target for autogenerated .c files
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: SPECIFIC_CFLAGS =  $(shell bash -c "echo \"`awk -v input=$< -f ./tools/awk_scripting/get_specific_flags.awk $(specificflags_file)`\"")
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: $(LCM_DIR_AUTO)/loader/%.c
# @echo $< CFLAGS: $(ALL_CFLAGS) SPECIFIC CFLAGS: $(SPECIFIC_CFLAGS)
	@echo $< >>$(BUILDFOLDER)/sourcefiles.txt
	@mkdir -p `dirname $@`
	$(LINEPREFIX) $(CC) -MMD $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c $<
	@cp $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp >> $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d
	@rm -f $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp

#Target for autogenerated .c files
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: SPECIFIC_CFLAGS =  $(shell bash -c "echo \"`awk -v input=$< -f ./tools/awk_scripting/get_specific_flags.awk $(specificflags_file)`\"")
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: $(LCM_DIR_AUTO)/%.c
# @echo $< CFLAGS: $(ALL_CFLAGS) SPECIFIC CFLAGS: $(SPECIFIC_CFLAGS)
	@echo $< >>$(BUILDFOLDER)/sourcefiles.txt
	@mkdir -p `dirname $@`
	$(LINEPREFIX) $(CC) -MMD $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c $<
	@cp $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp >> $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d
	@rm -f $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp

#Target for .c files in src folder
#assignment of SPECIFIC_CFLAGS is not according to SWAG guidelines,
#value will be set when used because $< needs to have a value when assigned
$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/%.o: SPECIFIC_CFLAGS =  $(shell bash -c "echo \"`awk -v input=$< -f ./tools/awk_scripting/get_specific_flags.awk $(specificflags_file)`\"")
$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/%.o: src/%.c
#	@echo $< CFLAGS: $(ALL_CFLAGS) SPECIFIC CFLAGS: $(SPECIFIC_CFLAGS)
	@echo $< >>$(BUILDFOLDER)/sourcefiles.txt
	@mkdir -p `dirname $@`
	$(LINEPREFIX) $(CC) -MMD $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c $<
	@cp $(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$*.d $(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$*.d.tmp >> $(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$*.d
	@rm -f $(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/$*.d.tmp

#TRANSPORT_LAYER files
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: SPECIFIC_CFLAGS =  $(shell bash -c "echo \"`awk -v input=$< -f ./tools/awk_scripting/get_specific_flags.awk $(specificflags_file)`\"")
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: $(TRANSPORT_PATH)/source/%.c
#	@echo $< CFLAGS: $(ALL_CFLAGS) SPECIFIC CFLAGS: $(SPECIFIC_CFLAGS)
	@echo $< >>$(BUILDFOLDER)/sourcefiles.txt
	@mkdir -p `dirname $@`
#	@echo Building .c files in src folder
#	@echo $(LINEPREFIX) $(CC) $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c
	$(LINEPREFIX) $(CC) -MMD $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c $<
	@cp $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp >> $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d
	@rm -f $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
#	@echo Done Building

#NETWORK_PATH files
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: SPECIFIC_CFLAGS =  $(shell bash -c "echo \"`awk -v input=$< -f ./tools/awk_scripting/get_specific_flags.awk $(specificflags_file)`\"")
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: $(NETWORK_PATH)/source/%.c
#	@echo $< CFLAGS: $(ALL_CFLAGS) SPECIFIC CFLAGS: $(SPECIFIC_CFLAGS)
	@echo $< >>$(BUILDFOLDER)/sourcefiles.txt
	@mkdir -p `dirname $@`
#	@echo Building .c files in src folder
#	@echo $(LINEPREFIX) $(CC) $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c
	$(LINEPREFIX) $(CC) -MMD $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c $<
	@cp $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp >> $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d
	@rm -f $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
#	@echo Done Building

#LCM_CNH_PATH files
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: SPECIFIC_CFLAGS =  $(shell bash -c "echo \"`awk -v input=$< -f ./tools/awk_scripting/get_specific_flags.awk $(specificflags_file)`\"")
$(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/%.o: $(LCM_CNH_PATH)/source/%.c
#	@echo $< CFLAGS: $(ALL_CFLAGS) SPECIFIC CFLAGS: $(SPECIFIC_CFLAGS)
	@echo $< >>$(BUILDFOLDER)/sourcefiles.txt
	@mkdir -p `dirname $@`
#	@echo Building .c files in src folder
#	@echo $(LINEPREFIX) $(CC) $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c
	$(LINEPREFIX) $(CC) -MMD $(ALL_CFLAGS) $(SPECIFIC_CFLAGS) -o $@ -c $<
	@cp $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp >> $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d
	@rm -f $(BUILDFOLDER)/autogenoutput/$(STE_HW)/$(SEC_NAME)/$*.d.tmp
#	@echo Done Building


#Target for .S assembler files
#assignment of SPECIFIC_CFLAGS is not according to SWAG guidelines,
#value will be set when used because $< needs to have a value when assigned
$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/%.o: SPECIFIC_AFLAGS =  $(shell bash -c "echo \"`awk -v input=$< -f ./tools/awk_scripting/get_specific_flags.awk $(specificflags_file)`\"")
$(BUILDFOLDER)/output/$(STE_HW)/$(SEC_NAME)/%.o: src/%.S
#	@echo $< AFLAGS: $(ALL_AFLAGS) SPECIFIC AFLAGS: $(SPECIFIC_AFLAGS)
	@echo $< >>$(BUILDFOLDER)/sourcefiles.txt
	@mkdir -p `dirname $@`
	$(LINEPREFIX) $(AS) $(ALL_AFLAGS) $(SPECIFIC_AFLAGS) -o $@ $<

# This target produce a file called "filelist.txt"
# in the component's root directory. This file must
# follow the format expected by the gen_init_cpio tool.
# After that the makefile installs the built system.
#
# But what is install when it comes to loaders?
# Maybe it should execute ste-flash?
#
# Maybe we should deprecate this target,
# and echo a warning to user.
install: build
	@$(if $(SRCDEST), echo "Copying sourcefiles to $(SRCDEST)" && \
	find src/ -name *.h -print >>$(BUILDFOLDER)/sourcefiles.txt && \
	cp $(BUILDFOLDER)/$(LOADERNAME)*.elf $(SRCDEST)/ && \
	find $(BUILDFOLDER) -name *.h -print >>$(BUILDFOLDER)/sourcefiles.txt && \
	./tools/scripts/CopyFilesFromList.sh $(BUILDFOLDER)/sourcefiles.txt $(SRCDEST) $(BUILDFOLDER),)
	@$(if $(BINDEST), echo "Copying binaries to $(BINDEST)" && mkdir -p $(BINDEST)/ && \
	cp $(BUILDFOLDER)/$(LOADERNAME)*.bin $(BINDEST)/,)

# This target will sign the software with
# sign package selected during make config.
sign:

# Will remove every built file from
# the local copy.
.PHONY: clean
clean: clean-cops clean-bass
	$(if $(IMP_GCCLIB), \
		@rm -rf $(IMP_GCCLIB) 2> /dev/null,)
	$(if $(IMP_CLIB), \
		@rm -rf $(IMP_CLIB) 2> /dev/null,)
	$(if $(IMPORT_DIR), \
		@rm -rf $(IMPORT_DIR) 2> /dev/null,)
	$(if $(COPS_INSTALL), \
		@rm -rf $(COPS_INSTALL) 2> /dev/null,)
	$(if $(BASSAPP_INSTALL), \
		@rm -rf $(BASSAPP_INSTALL) 2> /dev/null,)
	$(if $(EXE_DIR), \
		@rm -f  $(EXE_DIR)/lcm.so 2> /dev/null,)
	$(if $(BUILDFOLDER), \
		@rm -f  $(BUILDFOLDER)/inc/memmap_logical.h 2> /dev/null \
		@rm -f  $(BUILDFOLDER)/inc/memmap_physical.h 2> /dev/null \
		@rm -rf $(BUILDFOLDER) 2> /dev/null,)

.PHONY: clean-cops
clean-cops:
	@$(if $(COPS_DIR), $(MAKE) -C $(COPS_DIR) -f $(COPS_DIR)/$(COPS_MAKEFILE) STE_PLATFORM=$(STE_PLATFORM) CONFIG_DIR=$(CONFIG_DIR) clean,)

.PHONY: clean-bass
clean-bass:
	@$(if $(BASSAPP_DIR), $(MAKE) -C $(BASSAPP_DIR) -f $(BASSAPP_DIR)/$(BASSAPP_MAKEFILE) STE_PLATFORM=$(STE_PLATFORM) CONFIG_DIR=$(CONFIG_DIR) clean,)

.PHONY: distclean-cops
distclean-cops:
	@$(if $(COPS_DIR), $(MAKE) -C $(COPS_DIR) -f $(COPS_DIR)/$(COPS_MAKEFILE) INSTALL_PATH=$(COPS_INSTALL) STE_PLATFORM=$(STE_PLATFORM) CONFIG_DIR=$(CONFIG_DIR) distclean,)

.PHONY: distclean-bass
distclean-bass:
	@$(if $(BASSAPP_DIR), $(MAKE) -C $(BASSAPP_DIR) -f $(BASSAPP_DIR)/$(BASSAPP_MAKEFILE) INSTALL_PATH=$(BASSAPP_INSTALL) STE_PLATFORM=$(STE_PLATFORM) CONFIG_DIR=$(CONFIG_DIR) distclean,)

# Will remove every built file from
# the local copy.
distclean: distclean-cops distclean-bass clean
	$(if $(configfile), \
		@rm -f $(configfile),)

#Generate LCM files
#lcm_generate: prebuild ./src/platform_support/stn8500/hardware_drivers/stn8500/memmap/memmap_logical.h ./src/platform_support/stn8500/hardware_drivers/stn8500/memmap/memmap_physical.h
#	$(LINEPREFIX) cp -f ./src/platform_support/stn8500/hardware_drivers/stn8500/memmap/memmap_logical.h	$(BUILDFOLDER)/inc/memmap_logical.h
#	$(LINEPREFIX) cp -f ./src/platform_support/stn8500/hardware_drivers/stn8500/memmap/memmap_physical.h	$(BUILDFOLDER)/inc/memmap_physical.h
#Config target needs to check for certain perl version.
#	$(LINEPREFIX) ${PERL} ./src/platform_support/stn8500/hardware_drivers/stn8500/memmap/simplify_memmap.pl $(BUILDFOLDER)/inc/memmap_logical.h $(LCM_DIR)/memmap.h $(BUILDFOLDER)/inc/ >/dev/null

coverity:
	@if [ -d "$(COV_DATA_DIR)" ]; then \
		cov-stop-gui --datadir $(COV_DATA_DIR); \
	fi
	@cov-configure --compiler $(CC) --comptype gcc
	@if [ -d "$(COV_DATA_DIR)" ]; then \
		echo coverity gui already installed; \
	else \
		cov-install-gui --password admin --datadir $(COV_DATA_DIR) --product fairbanks --domain "C/C++"; \
	fi
	@cov-build --dir $(COV_INTER_DATA_DIR) $(MAKE) build
	@cov-analyze --dir $(COV_INTER_DATA_DIR) --aggressiveness-level medium --all
	@cov-commit-defects --datadir $(COV_DATA_DIR) --product fairbanks --user admin --dir $(COV_INTER_DATA_DIR)
	@cov-start-gui --datadir $(COV_DATA_DIR) --port 1122
	@echo Go to localhost port 1122 in webbrowser and login with username admin and password admin to review result

# This prevents deletion of these files as make considers them intermediate files
.SECONDARY: $(LCM_DIR_AUTO)/loader/audit.c $(LCM_DIR_AUTO)/loader/command_marshal.c $(LCM_DIR_AUTO)/loader/custom_command_marshal.c $(LCM_DIR_AUTO)/loader/ces_commands.c

astyle:
	astyle --style=k/r --indent=spaces  --break-blocks --convert-tabs --add-brackets \
	--unpad-paren --pad-header --pad-oper --indent-col1-comments --align-pointer=name \
	-R "*.h" -R "*.c" -R "*.cpp"  --exclude=platform_support/newlib \
	--exclude=platform_support/stn8500/hardware_drivers/stn8500/platform \
	--exclude=platform_support/stn8500/hardware_drivers/stn8500/services \
	--exclude=platform_support/stn8500/hardware_drivers/stn8500/hcl \
	--exclude=src/platform_support/u5500

