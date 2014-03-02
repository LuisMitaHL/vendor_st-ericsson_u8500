#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

################################################################################
# Product selection

ALL_PLATFORMS=$(shell ls $(CURDIR)/Dirs.*.mk | sed 's,.*Dirs\.,  ,g' | sed 's,\.mk,,g' | awk '{ printf "%s ", $$1 }')

ifeq ($(BUILD_ALL_PLATFORMS),true)

define plat_iter
%-$(1):
	make -C $$(CURDIR) -f lbp.mk STE_PLATFORM=$(1) BUILD_ALL_PLATFORMS=false $$*
endef

$(foreach platform,$(ALL_PLATFORMS),$(eval $(call plat_iter,$(platform))))

$(MAKECMDGOALS): $(addprefix $(MAKECMDGOALS)-,$(ALL_PLATFORMS))

else

  ifeq ($(STE_PLATFORM),)
    $(error STE_PLATFORM not set !! supported values are: $(ALL_PLATFORMS))
  endif

################################################################################
MMCURDIR=$(shell pwd)
ifeq ($(ANDROID_BUILD_TOP),)
  ANDROID_BUILD_TOP=$(abspath $(MMCURDIR)/../../../../..)
endif

################################################################################
# lbp related
LBPDIR=$(abspath $(ANDROID_BUILD_TOP)/product/common)
LBPMAK=ux500.mak

################################################################################
# Get definitions from lbp makefile + extend it a bit

## Set CURDIR as expected by LBP top level makefile
CURDIR:=$(LBPDIR)
include $(LBPDIR)/$(LBPMAK)

################################################################################

STE_SOFTWARE=lbp
## Request to have all the compilation outputs (libs, exe) in the source tree
MM_BUILD_FOR_PACKAGING=false

################################################################################

PRIVATE_MM_ARGS := STE_PLATFORM=$(STE_PLATFORM)
PRIVATE_MM_ARGS += STE_SOFTWARE=$(STE_SOFTWARE)
PRIVATE_MM_ARGS += ANDROID_BSP_ROOT=$(ANDROID_BUILD_TOP)
PRIVATE_MM_ARGS += KERNEL_BUILD_DIR=$(KERNEL_OUTPUT)
## User land cross-compiler
PRIVATE_MM_ARGS += TARGET_CROSS_COMPILE=arm-none-linux-gnueabi-
## Kernel cross-compiler
PRIVATE_MM_ARGS += CROSS_COMPILE=$(CROSS_COMPILE)
PRIVATE_MM_ARGS += TOOLS_PATH=$(ANDROID_BUILD_TOP)/vendor/st-ericsson/tools
PRIVATE_MM_ARGS += MM_BUILD_FOR_PACKAGING=$(MM_BUILD_FOR_PACKAGING)

################################################################################
## Imaging sensor configuration

CAMERA_SET_PRIMARY_SENSOR:=IMX072
CAMERA_ENABLE_FEATURE_RAW_SENSOR:=true
# inter components Circular deps issues in LBP
CAMERA_ENABLE_FEATURE_YUV_CAMERA:=false

ifneq ($(CAMERA_SET_PRIMARY_SENSOR),)
  PRIVATE_MM_ARGS += CAMERA_SET_PRIMARY_SENSOR=$(CAMERA_SET_PRIMARY_SENSOR)
endif
ifneq ($(CAMERA_SET_SECONDARY_SENSOR),)
  PRIVATE_MM_ARGS += CAMERA_SET_SECONDARY_SENSOR=$(CAMERA_SET_SECONDARY_SENSOR)
endif
ifdef CAMERA_ENABLE_FEATURE_RAW_SENSOR
PRIVATE_MM_ARGS += CAMERA_ENABLE_FEATURE_RAW_SENSOR=$(CAMERA_ENABLE_FEATURE_RAW_SENSOR)
endif
ifdef CAMERA_ENABLE_FEATURE_YUV_CAMERA
PRIVATE_MM_ARGS += CAMERA_ENABLE_FEATURE_YUV_CAMERA=$(CAMERA_ENABLE_FEATURE_YUV_CAMERA)
endif

################################################################################

## Get the following variables from makefile as depends on some names used below
KERNEL_HEADER_DIR := $(shell make $(PRIVATE_MM_ARGS) --silent --no-print-directory KERNEL_HEADER_DIR)
MM_KERNEL_HEADER_INSTALL_PATH := $(ANDROID_BUILD_TOP)/out/$(STE_PLATFORM)/kernel_headers

## To make sure we always use the kernel headers install them where MM system is reading from them
.PHONY: install-linux-headers
install-linux-headers:
	$(MAKE) -C $(KERNELDIR) $(KERNEL_FLAGS) headers_install INSTALL_HDR_PATH=$(MM_KERNEL_HEADER_INSTALL_PATH)
	mkdir -p $(KERNEL_HEADER_DIR)
	cp -a $(MM_KERNEL_HEADER_INSTALL_PATH)/include/* $(KERNEL_HEADER_DIR)

clean-linux-headers:
	rm -rf $(KERNEL_HEADER_DIR)
	rm -rf $(MM_KERNEL_HEADER_INSTALL_PATH)

################################################################################

base-all:
	$(MAKE) -C $(LBPDIR) -f $(LBPMAK) config build install

base-clean:
	$(MAKE) -C $(LBPDIR) -f $(LBPMAK) clean

base-distclean:
	$(MAKE) -C $(LBPDIR) -f $(LBPMAK) distclean
	rm -rf $(KERNEL_OUTPUT)

################################################################################

mm-all: install-linux-headers
	$(MAKE) $(PRIVATE_MM_ARGS) -j $(JOBS) all

ifneq ($(DO),)
$(DO):
	$(MAKE) $(PRIVATE_MM_ARGS) -j $(JOBS) DO=$(DO) $(DO)
endif

mm-clean:
	$(MAKE) $(PRIVATE_MM_ARGS) -j1 clean

################################################################################

lbp-ramdisk:
	$(MAKE) -C $(LBPDIR) -f $(LBPMAK) image

lbp-kernel-modules:
	$(MAKE) -C $(LBPDIR) -f $(LBPMAK) build-linux-modules install-linux

lbp-kernel:
	$(MAKE) -C $(LBPDIR) -f $(LBPMAK) kernelfs

################################################################################

lbp-all: base-all install-linux-headers mm-all lbp-ramdisk
lbp-clean: base-clean clean-linux-headers mm-clean
lbp-distclean: base-distclean mm-clean
lbp-km: lbp-kernel-modules
lbp-kernel: lbp-kernel

# Backward compatibility names
all-all: lbp-all
all-clean: lbp-clean
all-distclean: lbp-distclean

endif #ifeq ($(BUILD_ALL_PLATFORMS),true)
