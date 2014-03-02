#Android makefile to build hats as a part of Android Build

ifeq ($(ENABLE_FEATURE_BUILD_HATS),true)
LOCAL_PATH_RAM:= $(call my-dir)

MY_HATS_USERLAND:=$(shell if [ -d "$(LOCAL_PATH_RAM)/hats_userland" ]; then echo 1; else echo 0; fi)
ifeq ($(MY_HATS_USERLAND),1)
include $(LOCAL_PATH_RAM)/hats_userland/dthframework/dbus/dbushats.mk
endif
include $(CLEAR_VARS)

# Give other modules a nice, symbolic name to use as a dependent
# Yes, there are modules that cannot build unless the kernel has
# been built. Typical (only?) example: loadable kernel modules.
.phony: build-hats clean-hats

PRIVATE_HATS_ARGS := -C $(HATS_MAIN_PATH)/hats ARCH=arm
# Use the pathes configured in Boardconfig.mk
PRIVATE_HATS_ARGS += MULTIMEDIA_PATH=$(abspath $(MULTIMEDIA_PATH))
PRIVATE_HATS_ARGS += CONNECTIVITY_PATH=$(abspath $(CONNECTIVITY_PATH))
PRIVATE_HATS_ARGS += HARDWARE_PATH=$(abspath $(HARDWARE_PATH))
PRIVATE_HATS_ARGS_BUSYBOX := -C $(HATS_MAIN_PATH)/hats/busybox ARCH=arm install_root

HATS_CONFIG := $(TOPLEVEL)/hats
CONFIG_HATS_MK:=$(shell if test -f "$(HATS_CONFIG)/.config_hats.mk"; then echo 1; else echo 0; fi)


# Links the hats build into the Android build
LOCAL_PACKAGE_NAME := HATS
HATS_TEMPDIR := $(TARGET_OUT_INTERMEDIATES)/dummy_hats
HATS_TEMPINSTALLDIR := $(HATS_TEMPDIR)/install

# Ensures the mkimage tool is built before the miniape build tries to use it.
#build-miniape: build-uboot
HATS_LIBPATH := $(HATS_TEMPDIR)
LOCAL_PATH := $(HATS_LIBPATH)
LOCAL_SRC_FILES := HATS
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(HATS_TEMPINSTALLDIR)

# Dirty trick to build busybox into ramdisk
SYMLINKS := $(TARGET_ROOT_OUT)/hats/bin/sh
$(SYMLINKS): BUSY_BINARY := $(LOCAL_MODULE)
$(SYMLINKS): $(LOCAL_INSTALLED_MODULE) $(LOCAL_PATH_RAM)/Android.mk
	$(MAKE) $(PRIVATE_HATS_ARGS_BUSYBOX)

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS)

$(HATS_LIBPATH)/$(LOCAL_SRC_FILES): build-hats
	$(shell mkdir -p $(HATS_TEMPDIR) $(HATS_TEMPINSTALLDIR))
	@touch $@

include $(BUILD_PREBUILT) 

#PATH := $(PATH):$(CURDIR)/vendor/st-ericsson/boot/u-boot/tools
#export PATH

LOCAL_SHARED_LIBRARIES := \
	libclientgps

build-hats: build-kernel
	$(MAKE) $(PRIVATE_HATS_ARGS)

# An Android clean removes the files built for the current HW configuration,
# such as u8500,
# while a clobber removes all built files (rm -rf $(OUT_DIR)).
# Android's installclean is used when switching between different build
# variants in the same HW configuration. One of the directories removed
# is $(PRODUCT_OUT)/root - which is where Linux installs its modules.
# Thus, no modifications to the installclean targets are needed here.

clean clobber: clean-hats

clean-hats:
	$(MAKE) $(PRIVATE_HATS_ARGS) clean

endif # ENABLE_FEATURE_BUILD_HATS
