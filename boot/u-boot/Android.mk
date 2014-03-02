#Android makefile to build u-boot as a part of Android Build

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# Give other modules a nice, symbolic name to use as a dependent
# Yes, there are modules that cannot build unless uboot has
# been built. Typical (only?) example: linux kernel (needs mkimage program)
.phony: build-mkenvimg build-uboot clean-uboot

PRIVATE_UBOOT_ARGS := -C $(BOOT_PATH)/u-boot ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) O=$(abspath $(UBOOT_OUTPUT))
ifeq ($(ENABLE_FEATURE_BUILD_HBTS),true)
PRIVATE_UBOOT_ARGS += ENABLE_FEATURE_BUILD_HBTS=$(ENABLE_FEATURE_BUILD_HBTS)
endif

# The next three assignments are for building mk_envimg host tool.
# BUILD_UBOOT_ENV_IMG_OUTPUT and BUILD_UBOOT_ENV_IMG_INPUT should be set
# in BoardConfig.mk and are, if they are set, added to the cmdline.
PRIVATE_UBOOT_MK_ENVIMG_ARGS := obj=$(abspath $(HOST_OUT_EXECUTABLES))/ -C $(BOOT_PATH)/u-boot/tools/mk_envimg

ifneq ($(BUILD_UBOOT_ENV_IMG_OUTPUT),)
PRIVATE_UBOOT_MK_ENVIMG_ARGS += OUTPUT=$(abspath $(BUILD_UBOOT_ENV_IMG_OUTPUT))
endif

ifneq ($(BUILD_UBOOT_ENV_IMG_INPUT),)
PRIVATE_UBOOT_MK_ENVIMG_ARGS += INPUT=$(abspath $(BUILD_UBOOT_ENV_IMG_INPUT))
endif

PRIVATE_UBOOT_CONFIG_FILE := $(if $(UBOOT_OUTPUT),$(UBOOT_OUTPUT)/)include/config.h

# If the U-boot splash image output path or input path needs to be changed this shall
# be done by updating both the UBOOT_SPLASH_IMAGE_OUTPUT variable in BoardConfig.mk (vendor/st-ericsson/uxx00)
# and also the copying of the splash image file in ste_uxx00.mk (vendor/st-ericsson/products)

include $(CLEAR_VARS)
UBOOT_LIBPATH := $(UBOOT_OUTPUT)
LOCAL_PATH := $(UBOOT_LIBPATH)
LOCAL_SRC_FILES := u-boot.bin
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(PRODUCT_OUT)

$(UBOOT_LIBPATH)/$(LOCAL_SRC_FILES) $(UBOOT_LIBPATH)/tools/mkimage: build-uboot
include $(BUILD_PREBUILT)

ifdef BUILD_UBOOT_ENV_IMG_OUTPUT
include $(CLEAR_VARS)
ENVIMG_LIBPATH := $(dir $(BUILD_UBOOT_ENV_IMG_OUTPUT))
LOCAL_PATH := $(ENVIMG_LIBPATH)
LOCAL_SRC_FILES := $(notdir $(BUILD_UBOOT_ENV_IMG_OUTPUT))
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(PRODUCT_OUT)

$(ENVIMG_LIBPATH)/$(LOCAL_SRC_FILES): build-mkenvimg
include $(BUILD_PREBUILT)
endif

# Builds uboot. Configuration is only done if the configuration file
# is missing.
build-uboot: | $(PRIVATE_UBOOT_CONFIG_FILE)
	$(MAKE) $(PRIVATE_UBOOT_ARGS)

$(PRIVATE_UBOOT_CONFIG_FILE):
	$(MAKE) $(PRIVATE_UBOOT_ARGS) $(UBOOT_DEFCONFIG)

build-mkenvimg: build-uboot
	$(MAKE) $(PRIVATE_UBOOT_MK_ENVIMG_ARGS) all

# An Android clean removes the files built for the current HW configuration,
# such as u8500,
# while a clobber removes all built files (rm -rf $(OUT_DIR)).

clean clobber: clean-uboot clean-mk_envimg

clean-uboot:
	$(MAKE) $(PRIVATE_UBOOT_ARGS) clean

clean-mk_envimg:
	$(MAKE) $(PRIVATE_UBOOT_MK_ENVIMG_ARGS) clean

installclean: installclean-uboot

installclean-uboot:
	rm -f u-boot.bin u-boot.map u-boot.srec
