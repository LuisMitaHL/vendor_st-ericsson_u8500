#
# Copyright (C) ST-Ericsson SA 2012. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
# Android makefile to build multimedia as a part of Android Build

LOCAL_PATH := $(my-dir)

ifeq ($(TARGET_USE_ST_ERICSSON_MULTIMEDIA),true)

## Default to building NOT for packaging to force intermediate to out dir
MM_BUILD_FOR_PACKAGING?=false

## Default use Modified Port Settings Change Event
MULTIMEDIA_SET_USE_OLD_PORTSETTINGSCHANGEDEVENT?=false

include $(MULTIMEDIA_PATH)/linux/build/Defs.mk

## Send all the needed variables
## We don't use export as it may impact other makefiles
PRIVATE_MM_ARGS := -C $(MULTIMEDIA_PATH)/linux/build
PRIVATE_MM_ARGS += STE_PLATFORM=$(STE_PLATFORM)
PRIVATE_MM_ARGS += MULTIMEDIA_SET_PLATFORM=$(MULTIMEDIA_SET_PLATFORM)
PRIVATE_MM_ARGS += PRODUCT_OUT=$(PRODUCT_OUT)
PRIVATE_MM_ARGS += MMD_BUILD_ENV=false
PRIVATE_MM_ARGS += ANDROID_BSP_ROOT=$(CURDIR)
PRIVATE_MM_ARGS += HARDWARE_PATH=$(realpath $(HARDWARE_PATH))
PRIVATE_MM_ARGS += GRALLOC_PATH=$(realpath $(GRALLOC_PATH))
PRIVATE_MM_ARGS += RAMDISK_COPY_FILES_TO_ANDROID=true
PRIVATE_MM_ARGS += TARGET_TOOLS_PREFIX=$(TARGET_TOOLS_PREFIX)
PRIVATE_MM_ARGS += CROSS_COMPILE=$(CROSS_COMPILE)
PRIVATE_MM_ARGS += MMPROBE_ENABLE_FEATURE_MMPROBE=$(MMPROBE_ENABLE_FEATURE_MMPROBE)
PRIVATE_MM_ARGS += CSCALL_ENABLE_SILENT_REBOOT_SUPPORT=$(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT)
PRIVATE_MM_ARGS += BUILD_WITHOUT_PV=$(BUILD_WITHOUT_PV)
PRIVATE_MM_ARGS += MM_BUILD_FOR_PACKAGING=$(MM_BUILD_FOR_PACKAGING)
PRIVATE_MM_ARGS += ENABLE_FEATURE_BUILD_HATS=$(ENABLE_FEATURE_BUILD_HATS)
PRIVATE_MM_ARGS += USE_OLD_PORTSETTINGSCHANGEDEVENT=$(MULTIMEDIA_SET_USE_OLD_PORTSETTINGSCHANGEDEVENT)
PRIVATE_MM_ARGS += TOOLS_PATH=$(realpath $(TOOLS_PATH))
PRIVATE_MM_ARGS += BASS_APP_INCLUDE_PATH=$(realpath $(PROCESSING_PATH)/security_framework/bass_app/include)

## Secure content configuration
ifneq ($(TADEVKIT),)
PRIVATE_MM_ARGS +=  TADEVKIT=$(TADEVKIT) SIGNTOOL=$(SIGNTOOL) SIGNKEYSDIR=$(SIGNKEYSDIR) SIGNPACKAGEDIR=$(SIGNPACKAGEDIR) TA_SSWNAMES="$(TA_SSWNAMES)"
endif

## Imaging sensor configuration
ifneq ($(CAMERA_SENSOR),)
  PRIVATE_MM_ARGS += CAMERA_SET_PRIMARY_SENSOR=$(CAMERA_SENSOR)
endif
ifneq ($(CAMERA_SET_PRIMARY_SENSOR),)
  PRIVATE_MM_ARGS += CAMERA_SET_PRIMARY_SENSOR=$(CAMERA_SET_PRIMARY_SENSOR)
endif
ifneq ($(CAMERA_SET_SECONDARY_SENSOR),)
  PRIVATE_MM_ARGS += CAMERA_SET_SECONDARY_SENSOR=$(CAMERA_SET_SECONDARY_SENSOR)
endif
ifneq ($(CAMERA_PRIMARY_TYPE),)
  PRIVATE_MM_ARGS += CAMERA_PRIMARY_TYPE=$(CAMERA_PRIMARY_TYPE)
endif
ifneq ($(CAMERA_SECONDARY_TYPE),)
  PRIVATE_MM_ARGS += CAMERA_SECONDARY_TYPE=$(CAMERA_SECONDARY_TYPE)
endif
ifdef CAMERA_ENABLE_FEATURE_RAW_SENSOR
PRIVATE_MM_ARGS += CAMERA_ENABLE_FEATURE_RAW_SENSOR=$(CAMERA_ENABLE_FEATURE_RAW_SENSOR)
endif
ifdef CAMERA_ENABLE_FEATURE_YUV_CAMERA
PRIVATE_MM_ARGS += CAMERA_ENABLE_FEATURE_YUV_CAMERA=$(CAMERA_ENABLE_FEATURE_YUV_CAMERA)
endif
ifdef MMIO_ENABLE_NEW_IF
PRIVATE_MM_ARGS += MMIO_ENABLE_NEW_IF=$(MMIO_ENABLE_NEW_IF)
endif

## KERNEL_OUTPUT is directory where kernel output files have been generated when specified
ifneq ($(KERNEL_OUTPUT),)
  PRIVATE_MM_ARGS += KERNEL_BUILD_DIR=$(KERNEL_OUTPUT)
endif

ifneq ($(SHOW_COMMANDS),)
  PRIVATE_MM_ARGS += VERBOSE=4
endif

ifneq ($(DIRS_TO_BUILD),)
  PRIVATE_MM_ARGS += DIRS_TO_BUILD="$(DIRS_TO_BUILD)"
endif

ifdef VERBOSE
  $(info $(LOCAL_PATH)/$(lastword $(MAKEFILE_LIST)) : MAKECMDGOALS=$(MAKECMDGOALS))
  $(info $(LOCAL_PATH)/$(lastword $(MAKEFILE_LIST)) : ONE_SHOT_MAKEFILE=$(ONE_SHOT_MAKEFILE))
  HIDE :=
else
  PRIVATE_MM_ARGS += -s
  HIDE := $(hide)
endif

################################################################################

include $(CLEAR_VARS)

# Include the multimedia in the Android build system.
# To be able to use the Android build system for
# multimedia a dummy package is created as multimedia
# uses its own make system and not Androids.

LOCAL_MODULE := st-ericsson-multimedia-package
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC

LOCAL_SRC_FILES := none
LOCAL_PATH := $(TARGET_OUT_INTERMEDIATES)/st-ericsson-multimedia
LOCAL_MODULE_PATH := $(LOCAL_PATH)

MM_DO?=build-multimedia

$(LOCAL_PATH)/$(LOCAL_SRC_FILES): $(MM_DO)
	$(hide) mkdir -p `dirname $@`
	$(hide) touch $@

include $(BUILD_PREBUILT)

################################################################################

ifeq ($(ONE_SHOT_MAKEFILE),)

STE_MM_BUILD_DEPS := libc liblog libutils libcutils libui libandroid_runtime libstlport libalsactrl
STE_MM_BUILD_DEPS += libbassapp libhardware libm libmmprobe uImage libtinyalsa
ifeq ($(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT), true)
  ifneq ($(MULTIMEDIA_SET_PLATFORM),u5500)
    # u5500 does not depend on shmnetlink library
    STE_MM_BUILD_DEPS += libshmnetlnk
  endif
endif
ifeq ($(MULTIMEDIA_SET_PLATFORM),u9540)
  STE_MM_BUILD_DEPS += libteemm
else ifeq ($(MULTIMEDIA_SET_PLATFORM),l8540)
  STE_MM_BUILD_DEPS += libteemm
endif
STE_MM_BUILD_DEPS += $(TARGET_CRTBEGIN_DYNAMIC_O) $(TARGET_CRTEND_O)
STE_MM_BUILD_DEPS += libblt_hw
# VENDOR_DEPS is to be defined by STE vendors in to explicit any extra dep MM build may have
# This macro should not be used by STE to define any dependencies.
STE_MM_BUILD_DEPS += $(VENDOR_DEPS)

else #ifeq ($(ONE_SHOT_MAKEFILE),)
STE_MM_BUILD_DEPS :=
endif #ifeq ($(ONE_SHOT_MAKEFILE),)

.PHONY: build-multimedia clean-multimedia

# Builds and installs multimedia.
build-multimedia: $(STE_MM_BUILD_DEPS)
	$(HIDE) $(MAKE) $(PRIVATE_MM_ARGS) all

# Make sure we perform/finish multimedia build before creating data partition
patch-userdataimage: build-multimedia

STE_MM_SHARED_LIBRARY_CLEAN := $(foreach lib,$(STE_MM_SHARED_LIBRARIES),$(addprefix clean-,$(basename $(notdir $(lib)))))

clean-multimedia: $(STE_MM_SHARED_LIBRARY_CLEAN)
	$(HIDE) $(MAKE) $(PRIVATE_MM_ARGS) clean
ifeq ($(ONE_SHOT_MAKEFILE),)
	$(HIDE) $(MAKE) $(PRIVATE_MM_ARGS) distclean
endif

ifneq ($(DO),)
$(DO):
	$(HIDE) $(MAKE) $(PRIVATE_MM_ARGS) DO=$(DO) $(DO)
endif

include $(MULTIMEDIA_PATH)/linux/build/android_dependency.mk

clean: clean-multimedia

endif #ifeq ($(TARGET_USE_ST_ERICSSON_MULTIMEDIA),true)
