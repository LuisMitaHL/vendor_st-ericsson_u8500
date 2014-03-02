PRIVATE_SIGNTOOLCLI_TOOLS_PATH := $(abspath $(TOOLS_PATH))
PRIVATE_SIGNTOOLCLI_GIT_DIR := $(PRIVATE_SIGNTOOLCLI_TOOLS_PATH)/platform/flash_kit/sign_tool_cli

ifdef TARGET_OUT_INTERMEDIATES
  PRIVATE_SIGNTOOLCLI_OUT_INTERIM:=$(abspath $(TARGET_OUT_INTERMEDIATES))/flash_kit/sign_tool_cli
  $(shell mkdir -p $(PRIVATE_SIGNTOOLCLI_OUT_INTERIM))
else
  PRIVATE_SIGNTOOLCLI_OUT_INTERIM:=$(PRIVATE_SIGNTOOLCLI_GIT_DIR)
endif

PRIVATE_SIGNTOOLCLI_FLAGS := \
 FLASH_KIT_TARGET_DIR=$(FLASHKIT_INSTALL_BASE)/flashkit \
 ANT_HOME=$(ANT_HOME) \
 TARGET_OUT_INTERMEDIATES=$(PRIVATE_SIGNTOOLCLI_OUT_INTERIM) \
 FTB_GIT_DIR=$(PRIVATE_SIGNTOOLCLI_TOOLS_PATH)/platform/flash_kit/flash_tool_backend

PRIVATE_SIGNTOOLCLI_INSTALL:=$(shell which install)
PRIVATE_SIGNTOOLCLI_LN:=$(shell which ln)
PRIVATE_SIGNTOOLCLI_MKTEMP:=$(shell which mktemp)
PRIVATE_SIGNTOOLCLI_MKDIR:=$(shell which mkdir)
PRIVATE_SIGNTOOLCLI_RM:=$(shell which rm)

PRIVATE_SIGNTOOLCLI_OTA_MODEMFS:=$(PRODUCT_OUT)/modemfs.img
PRIVATE_SIGNTOOLCLI_OTA_SCRIPT:=$(PRIVATE_SIGNTOOLCLI_GIT_DIR)/ota_script
PRIVATE_SIGNTOOLCLI_OTA_FSTAB:=$(strip $(wildcard $(TARGET_DEVICE_DIR)/recovery.fstab))

#public variables for modem packaging
TARGET_HW?=UNKNOWN
STEOTAPACKAGE:=$(PRODUCT_OUT)/signed-ste-ota.zip
STEOTAADDITIONS:=$(abspath $(PRODUCT_OUT)/ste_ota_additions)

PRIVATE_SIGNTOOLCLI_OTA_VARIABLE:=$(shell $(PRIVATE_SIGNTOOLCLI_GIT_DIR)/parse_var.sh $(TARGET_HW))
ifneq ($(PRIVATE_SIGNTOOLCLI_OTA_VARIABLE),"")
  PRIVATE_SIGNTOOLCLI_OTA_MODEMIMG:=$(subst MODEMIMG=,,$(word 1,$(PRIVATE_SIGNTOOLCLI_OTA_VARIABLE)))
  PRIVATE_SIGNTOOLCLI_OTA_SIGNPACKAGE:=$(subst SIGNPACKAGE=,,$(word 2,$(PRIVATE_SIGNTOOLCLI_OTA_VARIABLE)))
  TARGET_MODEM?=$(PRIVATE_SIGNTOOLCLI_OTA_MODEMIMG)
endif

PRIVATE_SIGNTOOLCLI_OTA_SIGNING_FLAGS := \
 LOCAL_SIGNPACKAGEROOT=$(FLASHKIT_INSTALL_BASE)/signing/signpackages \
 LOCAL_KEYROOT=$(FLASHKIT_INSTALL_BASE)/signing/signkeys \
 LOCAL_SIGN_EXECUTABLE=$(FLASHKIT_INSTALL_BASE)/flashkit/sign-tool-cli/sign-tool-$(FLASHKIT_SET_HWCONFIG).sh \
 LOCAL_SIGN_PACKAGE=$(PRIVATE_SIGNTOOLCLI_OTA_SIGNPACKAGE)


VENDOR_KEY_CERT_PAIR:=vendor/snda/security/releasekey

st-ericsson-flashkit: build_sign_tool_cli

.PHONY: build_sign_tool_cli clean_sign_tool_cli distclean_sign_tool_cli ste_otapackage

build_sign_tool_cli: build_flash_tool_backend
	@$(PRIVATE_SIGNTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_SIGNTOOLCLI_GIT_DIR) -f Makefile
	@$(PRIVATE_SIGNTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_SIGNTOOLCLI_GIT_DIR) -f Makefile install

clean_sign_tool_cli:
	@$(PRIVATE_SIGNTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_SIGNTOOLCLI_GIT_DIR) -f Makefile clean

distclean_sign_tool_cli:
	@$(PRIVATE_SIGNTOOLCLI_FLAGS) $(MAKE) -C $(PRIVATE_SIGNTOOLCLI_GIT_DIR) -f Makefile distclean

clean: clean_sign_tool_cli

ste_otapackage: $(STEOTAPACKAGE)

# it's original deprecated target
$(STEOTAPACKAGE): PRIVATE_SIGNTOOLCLI_OTATMP:=$(shell $(PRIVATE_SIGNTOOLCLI_MKTEMP) -d)
$(STEOTAPACKAGE): otapackage build_sign_tool_cli install-issw $(PRIVATE_SIGNTOOLCLI_OTA_MODEMFS)
	@echo "---===STE OTA PACKAGE===---"
	if [ "x$(PRIVATE_SIGNTOOLCLI_OTA_VARIABLE)" = x ]; then \
		echo "ERROR: No support for TARGET_HW $(TARGET_HW)"; \
		false; \
	fi

	@$(PRIVATE_SIGNTOOLCLI_MKDIR) $(PRIVATE_SIGNTOOLCLI_OTATMP)/bin
	@$(PRIVATE_SIGNTOOLCLI_LN) -s $(abspath $(HOST_OUT_EXECUTABLES)/mkbootimg) $(PRIVATE_SIGNTOOLCLI_OTATMP)/bin/mkbootimgorig
	@$(PRIVATE_SIGNTOOLCLI_INSTALL) -m 0755 -T $(PRIVATE_SIGNTOOLCLI_OTA_SCRIPT) $(PRIVATE_SIGNTOOLCLI_OTATMP)/bin/mkbootimg
	@$(PRIVATE_SIGNTOOLCLI_LN) -s $(abspath $(HOST_OUT)/framework) $(PRIVATE_SIGNTOOLCLI_OTATMP)/framework

	cp -f $(BUILT_TARGET_FILES_PACKAGE) $(PRODUCT_OUT)/ste-target-files.zip
	mkdir -p $(STEOTAADDITIONS)/DEVICE/st-ericsson
	$(PRIVATE_SIGNTOOLCLI_GIT_DIR)/parse_fstab.sh $(PRIVATE_SIGNTOOLCLI_OTA_FSTAB) >> $(PRIVATE_SIGNTOOLCLI_OTATMP)/extra_script

# Sign modem image
	$(PRIVATE_SIGNTOOLCLI_OTA_SIGNING_FLAGS) $(FLASHKIT_INSTALL_BASE)/flashkit/sign-tool-cli/sign-tool-$(FLASHKIT_SET_HWCONFIG).sh \
		-b 1047040 -v --local -s modem -p $(PRIVATE_SIGNTOOLCLI_OTA_SIGNPACKAGE) \
		$(TARGET_MODEM) $(STEOTAADDITIONS)/DEVICE/st-ericsson/modem.fs
# modemfs
	cp -f $(PRIVATE_SIGNTOOLCLI_OTA_MODEMFS) $(STEOTAADDITIONS)/DEVICE/st-ericsson/
# Add to intermediate .zip file
# Now we've added the device/st-ericsson directory to ste-target-files.zip
	( cd $(STEOTAADDITIONS) && zip -r $(abspath $(PRODUCT_OUT))/ste-target-files.zip DEVICE/ )

	$(hide) $(PRIVATE_SIGNTOOLCLI_OTA_SIGNING_FLAGS) ./build/tools/releasetools/ota_from_target_files -v \
		-p $(PRIVATE_SIGNTOOLCLI_OTATMP) \
		-k $(VENDOR_KEY_CERT_PAIR) \
		-e $(PRIVATE_SIGNTOOLCLI_OTATMP)/extra_script \
		$(PRODUCT_OUT)/ste-target-files.zip $(STEOTAPACKAGE)

	@$(PRIVATE_SIGNTOOLCLI_RM) -rf $(PRIVATE_SIGNTOOLCLI_OTATMP)
