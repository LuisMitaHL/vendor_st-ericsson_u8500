#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

ifeq ($(ANDROID_BSP_ROOT),)
  $(error ANDROID_BSP_ROOT not set)
endif

## ANDROID_BSP_ROOT points to the top level of the GIT forest
## Make it absolute in case it is some link to it
ABS_ANDROID_BSP_ROOT:=$(shell cd $(ANDROID_BSP_ROOT) ; pwd -P)
export ANDROID_BSP_ROOT

# Default build (precompiled MMDSP and RVCT in source tree)
FORCEBUILD?=pack_8500_linux

ifeq ($(FORCEBUILD),pack_8500_linux)
  BUILD_MMDSP_CODE=false
  BUILD_XP70_SIA_CODE=true
  BUILD_OPT_COMPILER_CODE=false
  BUILD_GCC_CODE=true
  NO_STD_BUILD=1
  ## Default to not building a component's deps when not building all sources
  BUILD_DEPS=false
else
  BUILD_MMDSP_CODE=true
  BUILD_XP70_SIA_CODE=true
  BUILD_OPT_COMPILER_CODE=true
  BUILD_GCC_CODE=true
  NO_STD_BUILD=
  ## Default to build a component's deps when building FULL sources
  BUILD_DEPS=true
endif

BUILD_MMDSP_NMF=bin
BUILD_MMDSP_PROC=bin
NO_STD_BUILD=1
BUILD_ARM_PROC=bin
BUILD_ARM_NMF=src
BUILD_ARM_CODE=src

# Translate the legacy names into the new names
ifeq ($(BUILD_MMDSP_CODE),true)
  BUILD_MMDSP_NMF=src
  BUILD_MMDSP_PROC=src
  NO_STD_BUILD=
endif
ifeq ($(BUILD_OPT_COMPILER_CODE),true)
  BUILD_ARM_PROC=src
endif
ifeq ($(BUILD_GCC_CODE),false)
  ## When set to null then just don't do build nor install
  BUILD_ARM_CODE=
  BUILD_ARM_NMF=
endif
ifeq ($(BUILD_MMDSP_CODE),true)
  BUILD_MMDSP_ALTERNATE=true
endif

## Default to mmprobe disabeld
MMPROBE_ENABLE_FEATURE_MMPROBE?=false

ifeq ($(MMPROBE_ENABLE_FEATURE_MMPROBE),true)
  export MMPROBE_ENABLE_FEATURE_MMPROBE
endif

## Default to have new mmio i/f disabled
MMIO_ENABLE_NEW_IF?=false

ifeq ($(MMIO_ENABLE_NEW_IF),true)
  export MMIO_ENABLE_NEW_IF
endif

KERNEL_BUILD_DIR=$(ANDROID_BSP_ROOT)/kernel
KERNELDIR=$(KERNEL_BUILD_DIR)
export KERNEL_BUILD_DIR
export KERNELDIR

################################################################################
#### The environment must set for which chip we are building for

# Reset variable to ensure correct detection
PLATFORM=

include Defs.mk

################################################################################

ifeq ($(PLATFORM),)
  $(error unsupported setup : STE_PLATFORM=$(STE_PLATFORM) STE_SOFTWARE=$(STE_SOFTWARE) SW_VARIANT=$(SW_VARIANT))
else
  export PLATFORM
endif

export STE_PLATFORM
export STE_SOFTWARE
export SW_VARIANT

################################################################################

ifeq ($(findstring x86,$(STE_SOFTWARE)),x86)
  OUT_STE_PLATFORM=$(STE_PLATFORM)
  ABS_PRODUCT_OUT:=$(ABS_ANDROID_BSP_ROOT)/out/$(OUT_STE_PLATFORM)
endif

ifeq ($(STE_SOFTWARE),lbp)

  ifeq ($(TARGET_CROSS_COMPILE),)
    $(error TARGET_CROSS_COMPILE not set by environment)
  endif
  ifeq ($(CROSS_COMPILE),)
     $(error CROSS_COMPILE not set by environment)
  endif

  ## User land compiler
  export TARGET_CROSS_COMPILE
  ## Kernel compiler
  export CROSS_COMPILE

  ## Default is to make linux_ramdisk copy MM files to Android output dir
  RAMDISK_COPY_FILES_TO_ANDROID=true
  export RAMDISK_COPY_FILES_TO_ANDROID
  RAMDISK_COPY_FILES_TO_LBP_ROOTFS=true
  export RAMDISK_COPY_FILES_TO_LBP_ROOTFS

  ## Tell linux/ramdisk where to copy the MM output build
  LBP_ROOTFS_DIR:=$(realpath $(ANDROID_BSP_ROOT)/base_system/rootfs)
  export LBP_ROOTFS_DIR

  ## Allow overwrite in case of build variant
  OUT_STE_PLATFORM=$(STE_PLATFORM)

  ABS_PRODUCT_OUT:=$(ABS_ANDROID_BSP_ROOT)/out/$(OUT_STE_PLATFORM)
  ## This where generic makefiles expect to find the kernel headers !!
  ## keep align with SharedConfig.mk !!
  KERNEL_HEADER_DIR:=$(ABS_PRODUCT_OUT)/obj/mmbuildout/include/$(PLATFORM)/kernel

  KERNEL_HEADER_DIR:
	@echo $(KERNEL_HEADER_DIR)

endif

ifeq ($(STE_SOFTWARE),android)

  ABS_PRODUCT_OUT:=$(ABS_ANDROID_BSP_ROOT)/$(PRODUCT_OUT)

  ifneq ($(TARGET_TOOLS_PREFIX),)
    ## Generic makefiles needs to have the compile in the path
    PATH:=$(PATH):$(ANDROID_BSP_ROOT)/$(dir $(TARGET_TOOLS_PREFIX))

    TARGET_CROSS_COMPILE:=$(notdir $(TARGET_TOOLS_PREFIX))
    export TARGET_CROSS_COMPILE
  else
    ifeq ($(CROSS_COMPILE),)
      $(error CROSS_COMPILE not set by environment and TARGET_TOOLS_PREFIX not defined)
    endif
    TARGET_CROSS_COMPILE:=$(CROSS_COMPILE)
    export TARGET_CROSS_COMPILE
  endif

  ## Default is to make linux_ramdisk copy MM files to Android output dir
  RAMDISK_COPY_FILES_TO_ANDROID=true
  export RAMDISK_COPY_FILES_TO_ANDROID

  ## Tell linux/ramdisk where to copy the MM output build
  ANDROID_ROOTFS_DIR:=$(ABS_PRODUCT_OUT)
  export ANDROID_ROOTFS_DIR

  ## Variables needed by the included makefiles we set here as we do not want to source csh files
  ANDROID_OUT_TARGET_PRODUCT_DIRECTORY:=$(ABS_PRODUCT_OUT)
  export ANDROID_OUT_TARGET_PRODUCT_DIRECTORY

endif

################################################################################
# TOOLS variable environment declaration and path settings
################################################################################
include tools.mk

################################################################################
export MAKEFLAGS

MMROOT := $(shell cd `pwd`/../.. ; pwd)
export MMROOT

MMENV_PATH :=$(MMROOT)/shared/mmenv
export MMENV_PATH

########################################

MMSHARED :=$(MMROOT)/shared
export MMSHARED

MMLINUX :=$(MMROOT)/linux
export MMLINUX

MMAUDIO :=$(MMROOT)/audio
export MMAUDIO

MMVIDEO :=$(MMROOT)/video
export MMVIDEO

MMIMAGING :=$(MMROOT)/imaging
export MMIMAGING

## Below lines are legacy ... can be removed when shared/ostgrp/Makefile is changed to use MMSHARED and not SHARED
SHARED :=$(MMSHARED)
export SHARED

LINUX :=$(MMLINUX)
export LINUX

########################################

WIN_ROOT ?= /sdcard
export WIN_ROOT

PATH := $(PATH):$(MMROOT)/shared/makefiles:.
export PATH
export LD_LIBRARY_PATH

## Set only PERL5LIB as when defined is used instead of PERLIB environment variable
ifneq ($(PERL5LIB),)
  PERL5LIB := $(PERL5LIB):$(MMROOT)/shared/makefiles/perllibs
else
  PERL5LIB := $(MMROOT)/shared/makefiles/perllibs
endif
export PERL5LIB

## Request linux_ramdisk to cleanup before each rootfs dir build
ROOTFS_CLEANUP=true
export ROOTFS_CLEANUP

## Do not print directories or commands unless VERBOSE
ifndef VERBOSE
  MAKE_OPTIONS += --silent --no-print-directory
endif

## Request NMF tools to use 8 jobs when building
NMF_JOBS=8
export NMF_JOBS

################################################################################

## The relative path in which multimedia forest is
MULTIMEDIA_REL_PATH:=$(subst $(ABS_ANDROID_BSP_ROOT)/,,$(MMROOT))

## Directory in which to install build result
MMBUILDOUT:=$(ABS_PRODUCT_OUT)/obj/mmbuildout
export MMBUILDOUT

## Directory in which to store intermediate build result
TARGET_OUT_INTERMEDIATES:=$(ABS_PRODUCT_OUT)/obj/mmbuildint

export MM_BUILD_FOR_PACKAGING
export TARGET_OUT_INTERMEDIATES

## Top level dir to compute relative path for intermediates files
PRODUCT_TOP_DIR:=$(ABS_ANDROID_BSP_ROOT)/$(MULTIMEDIA_REL_PATH)/
export PRODUCT_TOP_DIR

## Boot start variable on makefiles
MM_MAKEFILES_DIR:=$(MMBUILDOUT)/mkfiles
export MM_MAKEFILES_DIR

## Not all components makefiles have yet been reworked
MAKEFILES_BACKWARD_COMPATIBLE:=yes
export MAKEFILES_BACKWARD_COMPATIBLE

################################################################################
## Helpers

define remove-non-valid
  $(strip $(filter-out $(BUILD_LAST_ALWAYS),$(foreach dir,$(1),$(if $(realpath $(MMROOT)/$(dir)/Makefile),$(dir),))))
endef

################################################################################
## Compute directories to install and build

SETUP_TARGET := all

ifeq ($(STE_PLATFORM),x86)
  include Dirs.x86.mk
  include Deps.x86.mk
  include Vars.x86.mk
endif

ifeq ($(STE_PLATFORM),u8500)
  include Dirs.u8500.mk
  include Deps.u8500.mk
  include Vars.u8500.mk
endif

ifeq ($(STE_PLATFORM),u5500)
  include Dirs.u5500.mk
  include Vars.u5500.mk
endif

ifeq ($(STE_PLATFORM),u9540)
  include Dirs.u9540.mk
  include Deps.u9540.mk
  include Vars.u9540.mk
endif

ifeq ($(STE_PLATFORM),l8540)
  include Dirs.l8540.mk
  include Deps.l8540.mk
  include Vars.l8540.mk
endif

################################################################################
DIRECTORIES_TO_BUILD := $(if $(DIRS_TO_BUILD),$(DIRS_TO_BUILD),$(REF_DIRS_TO_BUILD))

## Function for deps expansion
define expand-deps
  DIRECTORIES_TO_BUILD:=$$(filter $$(REF_DIRS_TO_BUILD), \
                                  $$(filter-out $$(DIRECTORIES_TO_SETUP), \
                                  $$(sort $$(strip $$(DIRECTORIES_TO_BUILD) $$(foreach dir,$$(DIRECTORIES_TO_BUILD:=-deps),$$($$(dir)))))))
endef

ifeq ($(BUILD_DEPS),true)
  ## Expand deps - no way to make recursive function so use a foreach
  ## 10 recursive expansion allows to expand all deps
  $(foreach i,0 1 2 3 4 5 6 7 8 9 10,$(eval $(call expand-deps,$(i))))
endif

## Component to always build and build them as last ones
DO_LAST_ALWAYS:=$(if $(DO),$(DO),all)
ifneq ($(findstring $(DO_LAST_ALWAYS),all install),)
  BUILD_LAST_ALWAYS:=linux/ramdisk
endif

DIRECTORIES_TO_BUILD   := $(call remove-non-valid,$(DIRECTORIES_TO_BUILD)) $(BUILD_LAST_ALWAYS)


################################################################################

################################################################################

ifeq ($(MMENVSET),true)
  $(info ============================================)
  $(info STE_PLATFORM=$(STE_PLATFORM))
  $(info STE_SOFTWARE=$(STE_SOFTWARE))
  $(info SW_VARIANT=$(SW_VARIANT))
  $(info PLATFORM=$(PLATFORM))
  $(info )
  $(info BUILD_SHARED=$(BUILD_SHARED))
  $(info BUILD_AUDIO=$(BUILD_AUDIO))
  $(info BUILD_IMAGING=$(BUILD_IMAGING))
  $(info CAMERA_ENABLE_FEATURE_RAW_SENSOR=$(CAMERA_ENABLE_FEATURE_RAW_SENSOR))
  $(info CAMERA_ENABLE_FEATURE_YUV_CAMERA=$(CAMERA_ENABLE_FEATURE_YUV_CAMERA))
  $(info CAMERA_SET_PRIMARY_SENSOR=$(CAMERA_SET_PRIMARY_SENSOR))
  $(info CAMERA_SET_SECONDARY_SENSOR=$(CAMERA_SET_SECONDARY_SENSOR))
  $(info MMIO_ENABLE_NEW_IF=$(MMIO_ENABLE_NEW_IF))
  $(info BUILD_VIDEO=$(BUILD_VIDEO))
  $(info BUILD_MALI=$(BUILD_MALI))
  $(info BUILD_VALID=$(BUILD_VALID))
  $(info ============================================)
endif

################################################################################

default: all

setupenv: $(addsuffix -$(SETUP_TARGET),$(DIRECTORIES_TO_SETUP))
ifdef VERBOSE
	@echo "\n################################################################################\n"
	@echo "MMROOT      =$(MMROOT)"
	@echo "MMBUILDOUT  =$(MMBUILDOUT)"
	@echo
	@echo "ANDROID_BSP_ROOT    =$(ANDROID_BSP_ROOT)"
	@echo "ABS_ANDROID_BSP_ROOT=$(ABS_ANDROID_BSP_ROOT)"
	@echo "MULTIMEDIA_REL_PATH =$(MULTIMEDIA_REL_PATH)"
	@echo "PATH                =$(PATH)"
	@echo "ARMLMD_LICENSE_FILE=$(ARMLMD_LICENSE_FILE)"
	@echo
	@echo "DIRS_AS_BINARY=$(DIRS_AS_BINARY)"
	@echo "DIRS_AS_FILTERED_SOURCE =$(DIRS_AS_FILTERED_SOURCE)"
	@echo
	@echo "DIRECTORIES_TO_BUILD  =$(DIRECTORIES_TO_BUILD)"
	@echo "\n################################################################################\n"
endif

RM:=rm -rf

reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))
DIRECTORIES_TO_UNSETUP := $(strip $(call reverse,$(DIRECTORIES_TO_SETUP)))
UNSETUP_TARGET := uninstall

ifdef VERBOSE
  $(info DIRECTORIES_TO_UNSETUP=$(DIRECTORIES_TO_UNSETUP))
  $(info DIRECTORIES_TO_SETUP=$(DIRECTORIES_TO_SETUP))
endif

unsetenv: $(addsuffix -$(UNSETUP_TARGET),$(DIRECTORIES_TO_UNSETUP))

################################################################################

ifneq ($(HAS_DEFINED_DEPS),true)
  # In case inter components dependencies have not been explicited then force sequential build
  INTER_PARALLEL_BUILD_ARG := -j1
endif

# Within a component parallel build cannot be yet enabled
INTRA_PARALLEL_BUILD_ARG := -j1

# If defined to true full source compilation is enabled only if component/component.xml does exist in the component
ENABLE_COMPONENT_FULL_SRC_DETECTION?=false

################################################################################
## For Variable definition
define variables_in_dir

  # MMDSP build option
  BUILD_MMDSP_NMF_$(1) = $(BUILD_MMDSP_NMF)
  BUILD_MMDSP_PROC_$(1) = $(BUILD_MMDSP_PROC)
  NO_STD_BUILD_$(1)= $(NO_STD_BUILD)

  # RVCT build option
  BUILD_ARM_PROC_$(1) = $(BUILD_ARM_PROC)

  # GCC build option
  BUILD_ARM_CODE_$(1) = $(BUILD_ARM_CODE)
  BUILD_ARM_NMF_$(1) = $(BUILD_ARM_NMF)

  # XP70 SIA build option
  BUILD_XP70_SIA_CODE_$(1) = $(BUILD_XP70_SIA_CODE)

  # Include the packed entities definition ( in case it exist it will overwrite the above variables)
  -include $(MMROOT)/$(1)/BuildSpec.mk

endef #define variables_in_dir

## The MM rules expander macro
define make_in_dir

ifeq ($(filter $(1),$(DIRECTORIES_TO_SETUP)),)
  $(1)-$(2): setupenv
endif

ifeq ($(filter %clean %uninstall,$(2)),)
  ifneq ($(filter-out $(DIRECTORIES_TO_SETUP),$(filter $($(1)-deps),$(DIRECTORIES_TO_BUILD))),)
    $(1)-$(2): $(addsuffix -all,$(filter-out $(DIRECTORIES_TO_SETUP),$(filter $($(1)-deps),$(DIRECTORIES_TO_BUILD))))
  endif
  ifneq ($(1),$(BUILD_LAST_ALWAYS))
    $(BUILD_LAST_ALWAYS)-all: $(1)-$(2)
  endif
else
  $(1)-$(2): $(addsuffix -$(2),$(filter-out $(DIRECTORIES_TO_SETUP),$(filter $($(1)-deps),$(DIRECTORIES_TO_BUILD))))
endif

$(1)-$(2):
	$(MAKE) $(INTRA_PARALLEL_BUILD_ARG) $(MAKE_OPTIONS) \
	  -C $(MMROOT)/$(1) \
	  $$($$(addprefix MKFLAGS_,$$(subst /,_,$(1)))) \
	  SW_VARIANT=$(SW_VARIANT) \
	  BUILD_MMDSP_NMF=$$(BUILD_MMDSP_NMF_$(1)) \
	  BUILD_MMDSP_PROC=$$(BUILD_MMDSP_PROC_$(1)) \
	  BUILD_ARM_CODE=$$(BUILD_ARM_CODE_$(1))\
	  BUILD_ARM_NMF=$$(BUILD_ARM_NMF_$(1)) \
	  BUILD_ARM_PROC=$$(BUILD_ARM_PROC_$(1)) \
	  BUILD_XP70_SIA_CODE=$$(BUILD_XP70_SIA_CODE_$(1)) \
	  NO_STD_BUILD=$$(NO_STD_BUILD_$(1)) \
	  $(2)

endef #define make_in_dir

## Expend the build specifiers variables for each component
$(foreach dir,$(sort $(DIRECTORIES_TO_SETUP) $(DIRECTORIES_TO_BUILD)), \
  $(eval $(call variables_in_dir,$(dir))))

## Expend for each component the 'all' and 'realclean' targets
$(foreach dir,$(sort $(DIRECTORIES_TO_SETUP) $(DIRECTORIES_TO_BUILD)), \
  $(foreach target,all realclean,$(eval $(call make_in_dir,$(dir),$(target)))))

## Expend for each 'setup' components  the 'uninstall' targets
$(foreach dir,$(DIRECTORIES_TO_UNSETUP), $(eval $(call make_in_dir,$(dir),uninstall)))

TARGETS := all install clean

ifneq ($(DO),)
  TARGETS += $(DO)
  $(foreach dir,$(DIRECTORIES_TO_BUILD),$(eval $(call make_in_dir,$(dir),$(DO))))
endif

################################################################################

## 'all' top level target is calling 'all' on each component
## variables created by `variables_in_dir` macro will control the actual work 'all'
## will perform: install, stubbing or normal build of sources
all_envset: setupenv $(addsuffix -all,$(DIRECTORIES_TO_BUILD))

################################################################################

## 'empty-dirs is called to check if (TARGET_OUT_INTERMEDIATES)/$(dir) for each
##  media audio,video,shared,linux,imaging (as there are empty directories
## components in video and nmf in shared inside (TARGET_OUT_INTERMEDIATES)
## If all is empty that means clean is called for whole multimedia. Hence remove 
## mmbuildint and mmbuildout
dir_empty?=0

define empty-dirs
is_dir_empty=$(shell ls $(TARGET_OUT_INTERMEDIATES)/$(dir))
ifeq ($(is_dir_empty),)
    dir_empty+=1
else
    dir_empty=0
endif
endef

## 'clean' top level target is calling 'realclean' on each component
clean_envset: setupenv $(addsuffix -realclean,$(DIRECTORIES_TO_BUILD)) unsetenv
	$(foreach dir, $(strip $(filter-out linux/ramdisk,$(DIRECTORIES_TO_BUILD))),$(RM) $(TARGET_OUT_INTERMEDIATES)/$(dir))
	$(foreach dir, audio video/components imaging shared/nmf linux,$(eval $(call empty-dirs,$(dir))))
ifeq ($(dir_empty),0 1 1 1 1 1)
	$(RM) $(TARGET_OUT_INTERMEDIATES) $(MMBUILDOUT)
endif

ifneq ($(DO),)
  $(DO)_envset: setupenv $(addsuffix -$(DO),$(DIRECTORIES_TO_BUILD))
endif

distclean:

ifneq ($(PLATFORM),)

define build-alternate
    for p in $(BUILD_MMDSP_ALTERNATE_STE_PLATFORMS) ; do \
      $(MAKE) $(MAKE_OPTIONS) $@ STE_PLATFORM=$$p OUT_STE_PLATFORM=$(STE_PLATFORM) \
        BUILD_MMDSP_ALTERNATE=false BUILD_GCC_CODE=false BUILD_MALI=false BUILD_LAST_ALWAYS= ; \
      done
endef

ifeq ($(filter clean,$(MAKECMDGOALS)),clean)
  ## No deps management in case of clean
  INTER_PARALLEL_BUILD_ARG := -j1
endif

$(TARGETS):
	$(MAKE) $(MAKE_OPTIONS) $(INTER_PARALLEL_BUILD_ARG) $@_envset MMENVSET=true
  ifeq ($(BUILD_MMDSP_ALTERNATE),true)
	$(build-alternate)
  endif

else

all $(DO):
	$(warning unsupported setup : STE_PLATFORM=$(STE_PLATFORM) SW_VARIANT=$(SW_VARIANT) doing nothing for `$@`)

clean: distclean

endif
