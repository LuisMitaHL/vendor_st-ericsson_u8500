# SOC_SETTINGS_SET_PLATFORM comes from BoardConfig.mk
ifeq ($(SOC_SETTINGS_SET_PLATFORM), u8500)

.PHONY: build-socsettings-multi install-socsettings-multi clean-socsettings-multi distclean-socsettings-multi

#Source location
PRIVATE_SOCSETTINGS_MULTI_DIR := $(abspath $(BOOT_PATH)/soc-settings/u8500_multi_boot)

#Output
# FLASHKIT_INSTALL_BASE comes from BoardConfig.mk
PRIVATE_SOCSETTINGS_MULTI_OUT := $(FLASHKIT_INSTALL_BASE)/

#Other flags
PRIVATE_SOCSETTINGS_MULTI_PREFIX := $(PRIVATE_SOCSETTINGS_MULTI_OUT)/

#PREFIX is passed to Makefile
PRIVATE_SOCSETTINGS_MULTI_FLAGS := PREFIX=$(PRIVATE_SOCSETTINGS_MULTI_PREFIX)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
# Pretend that the link script file is an Executable file to make Android
# build system happy.
LOCAL_SRC_FILES:= soc_settings_multi.ld
LOCAL_MODULE := soc_settings
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(PRIVATE_SOCSETTINGS_MULTI_PREFIX)/boot/meminits/u8500_multi/
include $(BUILD_PREBUILT)

# Use the link script as an excuse to call the external Makefile
.PHONY: soc_settings_multi.ld
$(LOCAL_PATH)/soc_settings_multi.ld: build-socsettings-multi install-socsettings-multi

build-socsettings-multi:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_MULTI_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_MULTI_FLAGS) postbuild

install-socsettings-multi: build-socsettings-multi
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_MULTI_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_MULTI_FLAGS) install

clean-socsettings-multi:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_MULTI_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_MULTI_FLAGS) clean

distclean-socsettings-multi:
	$(MAKE) -C $(PRIVATE_SOCSETTINGS_MULTI_DIR) -f Makefile $(PRIVATE_SOCSETTINGS_MULTI_FLAGS) distclean

clean clobber: clean-socsettings-multi

st-ericsson-flashkit: build-socsettings-multi install-socsettings-multi

endif
