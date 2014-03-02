#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

################################################################################
# Product selection

ALL_STE_SOFTWARE=x86-linux x86_mmdsp-linux x86_cortexA9-linux

BUILD_ALL_PLATFORMS=true
STE_PLATFORM=x86
FORCEBUILD=src
BUILD_LAST_ALWAYS=
STANDALONE_COMPILATION=true
MM_BUILD_FOR_PACKAGING=false
# Not we force JOB to be1 for clean/realclean target
JOBS ?= $(shell awk "BEGIN { print `cat /proc/cpuinfo | grep processor | wc -l`+1 }")


ifeq ($(BUILD_ALL_PLATFORMS),true)

define plat_iter
%-$(1):
	make -C $$(CURDIR) -f x86.mk STE_SOFTWARE=$(1) BUILD_ALL_PLATFORMS=false $$*
endef

$(foreach platform,$(ALL_STE_SOFTWARE),$(eval $(call plat_iter,$(platform))))

$(MAKECMDGOALS): $(addprefix $(MAKECMDGOALS)-,$(ALL_STE_SOFTWARE))

else

ifeq ($(STE_PLATFORM),)
	$(error STE_PLATFORM not set !! supported values are: $(ALL_STE_SOFTWARE))
endif

################################################################################
MMCURDIR=$(shell pwd)
ifeq ($(ANDROID_BUILD_TOP),)
  ANDROID_BUILD_TOP=$(abspath $(MMCURDIR)/../../../../..)
endif


################################################################################

STE_SOFTWARE=x86-linux
## Request to have all the compilation outputs (libs, exe) in the source tree

################################################################################

PRIVATE_MM_ARGS := STE_PLATFORM=$(STE_PLATFORM)
PRIVATE_MM_ARGS += STE_SOFTWARE=$(STE_SOFTWARE)
PRIVATE_MM_ARGS += ANDROID_BSP_ROOT=$(ANDROID_BUILD_TOP)
PRIVATE_MM_ARGS += TOOLS_PATH=$(ANDROID_BUILD_TOP)/vendor/st-ericsson/tools
PRIVATE_MM_ARGS += MM_BUILD_FOR_PACKAGING=$(MM_BUILD_FOR_PACKAGING)
PRIVATE_MM_ARGS += ENABLE_FEATURE_BUILD_HATS=$(ENABLE_FEATURE_BUILD_HATS)
PRIVATE_MM_ARGS += BUILD_LAST_ALWAYS=$(BUILD_LAST_ALWAYS)
PRIVATE_MM_ARGS += FORCEBUILD=$(FORCEBUILD)
PRIVATE_MM_ARGS += STANDALONE_COMPILATION=$(STANDALONE_COMPILATION)


################################################################################
all: mm-all
mm-all:
	$(MAKE) $(PRIVATE_MM_ARGS) -j $(JOBS) all

ifneq ($(DO),)
$(DO):
	$(MAKE) $(PRIVATE_MM_ARGS) -j $(JOBS) DO=$(DO) $(DO)
endif

mm-clean:
	$(MAKE) $(PRIVATE_MM_ARGS) -j1 clean

mm-realclean:
	$(MAKE) $(PRIVATE_MM_ARGS) -j1 realclean
endif
################################################################################




