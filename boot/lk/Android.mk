#Android makefile to build Little Kernel as a part of Android Build

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

BOOTLOADER_OUT := $(abspath $(LK_OUTPUT))
TOOLCHAIN_PREFIX := arm-eabi-
PRIVATE_LK_ARGS := -C $(abspath $(BOOT_PATH))/lk \
	-f $(abspath $(BOOT_PATH))/lk/makefile \
	BOOTLOADER_OUT=$(BOOTLOADER_OUT) \
	TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) \
	DEBUG=1 EMMC_BOOT=1 OPTIMIZED_TIME=0 $(BUILD_LK_TARGET)

BASSAPP_DIR := $(abspath $(PROCESSING_PATH)/security_framework/bass_app/)
PRIVATE_LK_OUT_DIR = $(abspath $(TOP))/$(TARGET_OUT_INTERMEDIATES)/lk
ifneq (,$(findstring u8500, ${TARGET_PRODUCT}))
MODEM_START_CODE := 1
else
MODEM_START_CODE := 0
endif

PRIVATE_LK_ARGS += BASSAPP_DIR=$(BASSAPP_DIR)
PRIVATE_LK_ARGS += PRIVATE_LK_OUT_DIR=$(PRIVATE_LK_OUT_DIR)

ifeq ($(ENABLE_FEATURE_BUILD_HBTS),true)
PRIVATE_LK_ARGS += ENABLE_FEATURE_BUILD_HBTS=$(ENABLE_FEATURE_BUILD_HBTS)
PRIVATE_LK_ARGS += ENABLE_FEATURE_SELFTESTS_IN_LOADER=true
PRIVATE_LKESRAM_ARGS = $(PRIVATE_LK_ARGS)
PRIVATE_LKESRAM_ARGS += ESRAM=1
PRIVATE_LKSDRAM_ARGS = $(PRIVATE_LK_ARGS)
PRIVATE_LKSDRAM_ARGS += SDRAM=1
endif

# The next assignments are for building lk environment binary.
# BUILD_LK_ENV_IMG_OUTPUT and BUILD_LK_ENV_IMG_INPUT should be set
# in BoardConfig.mk and are, if they are set, added to the cmdline.
PRIVATE_LK_ENV_ARGS := -C $(abspath $(BOOT_PATH))/lk \
	-f $(abspath $(BOOT_PATH))/lk/app/env/makefile \
	BOOTLOADER_OUT=$(BOOTLOADER_OUT) \
	TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) \
	BUILD_ENV=1

ifneq ($(BUILD_LK_ENV_IMG_OUTPUT),)
PRIVATE_LK_ENV_ARGS += BUILD_LK_ENV_IMG_OUTPUT=$(abspath $(BUILD_LK_ENV_IMG_OUTPUT))
endif

ifneq ($(BUILD_LK_ENV_IMG_INPUT),)
PRIVATE_LK_ENV_ARGS += BUILD_LK_ENV_IMG_INPUT=$(abspath $(BUILD_LK_ENV_IMG_INPUT))
endif

LK_LIBPATH := $(LK_OUTPUT)
LOCAL_PATH := $(LK_LIBPATH)
LOCAL_SRC_FILES := lk.bin
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(PRODUCT_OUT)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
ENVIMG_LIBPATH := $(dir $(BUILD_LK_ENV_IMG_OUTPUT))
LOCAL_PATH := $(ENVIMG_LIBPATH:%/=%)
LOCAL_SRC_FILES := $(notdir $(BUILD_LK_ENV_IMG_OUTPUT))
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(PRODUCT_OUT)

include $(BUILD_PREBUILT)

.PHONY: lk-prereq

lk-prereq:
	mkdir -p $(PRIVATE_LK_OUT_DIR)
	$(MAKE) -C $(BASSAPP_DIR) \
		build-OSFREE MODEM_START_CODE=$(MODEM_START_CODE) \
		CONFIG_DIR=$(PRIVATE_LK_OUT_DIR)

ifeq ($(ENABLE_FEATURE_BUILD_HBTS),true)
	$(BOOT_PATH)/lk/hbts_env.sh $(BOOT_PATH)/lk $(ANDROID_BUILD_TOP) $(LK_OUTPUT)

hbts-lk-sdram: lk-prereq
	$(MAKE) $(PRIVATE_LKSDRAM_ARGS)
hbts-lk-esram: lk-prereq
	$(MAKE) $(PRIVATE_LKESRAM_ARGS)
else

$(LK_OUTPUT)/lk.bin: lk-prereq FORCE
	$(MAKE) $(PRIVATE_LK_ARGS)
endif

$(BUILD_LK_ENV_IMG_OUTPUT): FORCE
	$(MAKE) $(PRIVATE_LK_ENV_ARGS)

# An Android clean removes the files built for the current HW configuration,
# while a clobber removes all built files (rm -rf $(OUT_DIR)).

.PHONY: clean-lk clean-lkenv clean clobber

clean clobber: clean-lk clean-lkenv

clean-lk:
ifeq ($(ENABLE_FEATURE_BUILD_HBTS),true)
	$(BOOT_PATH)/lk/hbts_env.sh $(BOOT_PATH)/lk $(ANDROID_BUILD_TOP) clean-lk
endif
	$(MAKE) $(PRIVATE_LK_ARGS) clean

clean-lkenv:
	$(MAKE) $(PRIVATE_LK_ENV_ARGS) clean
