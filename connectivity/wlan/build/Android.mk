#Android makefile to build wlan as a part of Android Build

ifeq ($(TARGET_USE_ST_ERICSSON_KERNEL),true)
ifeq ($(WLAN_ENABLE_OPEN_MAC_SOLUTION),false)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

ifdef WLAN_OUTPUT
#Create folder and send sources to WLAN_OUTPUT directory
$(shell mkdir -p $(WLAN_OUTPUT))
$(shell cp -ru $(CONNECTIVITY_PATH)/wlan/* $(WLAN_OUTPUT)/)
endif

# Determine name of folder where to place the LKM's
WLAN_KERNELDIR_FILE := $(WLAN_OUTPUT)/kernelrelease.mk
WLAN_DUMMY_DIR := $(TARGET_OUT_INTERMEDIATES)/dummy_kernel
$(WLAN_KERNELDIR_FILE):
	mkdir -p $(WLAN_DUMMY_DIR)
	$(MAKE) -C kernel allnoconfig O=../$(WLAN_DUMMY_DIR)
	$(MAKE) -C kernel prepare O=../$(WLAN_DUMMY_DIR)
	echo KERNELRELEASE = $$($(MAKE) -C kernel --no-print-directory kernelrelease O=../$(WLAN_DUMMY_DIR)) > $@


ifeq ($(wildcard $(KERNEL_OUTPUT_RELATIVE)),)
  temp := $(shell mkdir -p $(TARGET_OUT_INTERMEDIATES)/dummy_kernel)
  temp := $(shell cd kernel && $(MAKE) allnoconfig \
     O=../$(TARGET_OUT_INTERMEDIATES)/dummy_kernel)
  temp := $(shell cd kernel && $(MAKE) prepare \
     O=../$(TARGET_OUT_INTERMEDIATES)/dummy_kernel)
  KERNELRELEASE := $(shell cd kernel && $(MAKE) --no-print-directory \
    kernelrelease O=../$(TARGET_OUT_INTERMEDIATES)/dummy_kernel)
else
  KERNELRELEASE := $(shell $(MAKE) -C kernel --no-print-directory \
    kernelrelease O=../$(KERNEL_OUTPUT_RELATIVE))
endif

#include $(WLAN_KERNELDIR_FILE)

TARGET_OUT_LKM := $(TARGET_OUT)/lib/modules/$(KERNELRELEASE)/extra

PRIVATE_WLAN_ARGS := -C $(WLAN_OUTPUT)/build ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) ARM_ARCH_VERSION=armv7-a KERNELDIR=$(realpath kernel) KERNELOUT=$(KERNEL_OUTPUT) WLAN_SET_PLATFORM=$(WLAN_SET_PLATFORM) WLAN_SET_DUALBAND=$(WLAN_SET_DUALBAND)

# Links the wlan build into the Android build
include $(CLEAR_VARS)
WLAN_LIBPATH := $(WLAN_OUTPUT)/cw1200_wlan
LOCAL_PATH := $(WLAN_LIBPATH)
LOCAL_SRC_FILES := cw1200_wlan.ko
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := $(LOCAL_SRC_FILES)
LOCAL_MODULE_CLASS := LKM

$(WLAN_LIBPATH)/$(LOCAL_SRC_FILES): build-wlan
include $(BUILD_PREBUILT)

# Give other modules a nice, symbolic name to use as a dependent
.phony: build-wlan clean-wlan

# Builds wlan. Install is done by the Android build system.
# ifeq hack is to allow 'mm' to build the WLAN project without
# being shut down by dependencies.
ifeq ($(ONE_SHOT_MAKEFILE),)
build-wlan: uImage
	$(MAKE) $(PRIVATE_WLAN_ARGS) build-cw1200_wlan_only
else
build-wlan:
	$(MAKE) $(PRIVATE_WLAN_ARGS) build-cw1200_wlan_only
endif

clean clobber : clean-wlan

clean-wlan:
	$(MAKE) $(PRIVATE_WLAN_ARGS) clean

endif #WLAN_ENABLE_OPEN_MAC_SOLUTION
endif #TARGET_USE_ST_ERICSSON_KERNEL
