##########################################################
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
##########################################################
# config.mk
#

######################################################################################################################
#                                 Build/Shell commands - Edit at your own risk		                             #
######################################################################################################################

SHELL := /bin/sh

TOPDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../)

include $(TOPDIR)/config/platform.mk

# setup build
AR := $(PLATFORM_BIN)$(CROSS_COMPILE)ar
CC := $(PLATFORM_BIN)$(CROSS_COMPILE)gcc

ifeq ($(CROSS_COMPILE),arm-eabi-)
LD := $(PLATFORM_BIN)$(CROSS_COMPILE)gcc
else
LD := $(PLATFORM_BIN)$(CROSS_COMPILE)ld
endif
ifeq ($(CROSS_COMPILE),arm-linux-gnueabi-)
LD := $(PLATFORM_BIN)$(CROSS_COMPILE)gcc
endif

OBJDUMP := $(PLATFORM_BIN)$(CROSS_COMPILE)objdump

INSTALL := install

# shell commands
RM := rm -rf
MV := mv
CP := cp -rf
MAKE := make
STRIP := strip

CFLAGS += -Wall

ifeq ($(DEBUG),on)
CFLAGS += -g3
endif

# Project Include paths
LBS_INCLUDES := -I"$(TOPDIR)/lbsd/libagpsosa/incl" \
		-I"$(TOPDIR)/lbsd/gnsi/incl" \
		-I"$(TOPDIR)/lbsd/include" \
		-I"$(TOPDIR)/include"

LBS_INCLUDES += $(PLATFORM_INC)

######################################################################################################################
#                                                Project/Feature flags		                                     #
######################################################################################################################

LBS_FLAGS := -DAGPS_LINUX_FTR -DAGPS_DONOT_USE_OS_QUEUE \
	-DSUPL_DO_NOT_USE_ALMANAC -DAGPS_GENERIC_SOLUTION_FTR -DAGPS_TEST_MODE_FTR

ifeq ($(GENERIC_SOLUTION),true)
LBS_FLAGS += -UAGPS_TRACE_TO_FILE -DAGPS_TRACE_TO_LOG \
	-UCMCC_LOGGING_ENABLE -UAGPS_TEST_MODE_FTR
endif

ifeq ($(ENABLE_FREQ_AIDING),true)
LBS_FLAGS += -DGPS_FREQ_AID_FTR
endif

ifeq ($(ENABLE_CP),true)
LBS_FLAGS += -DAGPS_FTR -DAGPS_CP_SUSPEND_ON_AID_REQ -DNO_REQ_NAV_REF_FTR
endif

ifeq ($(ENABLE_UP),true)
LBS_FLAGS += -DAGPS_UP_FTR
endif

ifeq ($(DEBUG),on)
LBS_FLAGS += -DDEBUG_LOG -UDEBUG_LOG_LEV2
endif

ifneq ($(CROSS_COMPILE),arm-eabi-)
LBS_FLAGS += -DARM_LINUX_AGPS_FTR
endif

ifeq ($(ENABLE_SBEE),true)
LBS_FLAGS += -DAGPS_SBEE_FTR
LBS_FLAGS += -UCGPS_STRAIGHT_TO_OFF_FTR
LBS_FLAGS += -UAGPS_HISTORICAL_DATA_FTR
endif

ifeq ($(LBS_UBUNTU),true)
LBS_FLAGS += -DGPS_OVER_HCI -DLBS_LINUX_UBUNTU_FTR -DAGPS_LINUX_BARE_FTR
else
ifeq ($(UBUNTU_X86),true)
LBS_FLAGS += -DX86_64_UBUNTU_FTR -DENABLE_ARM_FPGA_STANDALONE -DHALGPS_DONOT_USE_RTC -DGPS_OVER_UART -DLBS_LINUX_UBUNTU_FTR -DAGPS_LINUX_BARE_FTR
else
LBS_FLAGS += -DGPS_OVER_HCI
endif
endif
