##########################################################
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
##########################################################
# platform.mk
#

# Default values for Command line options
ARCH ?= armv7el

#CROSS_COMPILE ?= arm-linux-

######################################################################################################################
#                                               Feature Enable/Disable                                               #
######################################################################################################################
LBS_UBUNTU ?= false
UBUNTU_X86 ?= false

# Select GNSS Chip version in use.
# Set to CG2900 for CG2900 Chip versions.
# Set to CG2910 for CG2910 & CG2905 Chip versions.
GNSSCHIP_VER ?= CG2900

ifeq ($(LBS_UBUNTU),true)
COMPILE_SUPL_ASN1C ?= false
ENABLE_CP ?= false
ENABLE_UP ?= false
ENABLE_FREQ_AIDING ?= false
GENERIC_SOLUTION ?= false
ENABLE_SBEE ?= false
else
ifeq ($(UBUNTU_X86),true)
COMPILE_SUPL_ASN1C ?= false
ENABLE_CP ?= false
ENABLE_UP ?= false
ENABLE_FREQ_AIDING ?= false
GENERIC_SOLUTION ?= false
ENABLE_SBEE ?= false
else
COMPILE_SUPL_ASN1C ?= true
ENABLE_CP ?= true
ENABLE_UP ?= true
ENABLE_FREQ_AIDING ?= true
GENERIC_SOLUTION ?= true
ENABLE_SBEE ?= true
endif
endif
DEBUG ?= off
LBS_UNIT_TEST_FTR ?= false

######################################################################################################################
#                     Compiler Specific Options - Add/Modify for any new compiler/environment                        #
######################################################################################################################

# Add PLATFORM_LIB_PATH below when changing the USE_DEFUALT_PATH to false or export the lib path as well

USE_DEFAULT_PATH ?= true

#
# Compiler Options
#

# arm-eabi-gcc
ifeq ($(CROSS_COMPILE),arm-eabi-)
PLATFORM_LIB := -lc -lm -lgcc -lcutils -lutils -ldl -llog -lstdc++ -lz
CFLAGS := -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -mthumb -mfpu=vfp -mabi=aapcs \
	-fno-exceptions -fpic -ffunction-sections -funwind-tables -fno-short-enums -fmessage-length=0 \
	-finline-functions -fgcse-after-reload -frerun-cse-after-loop -frename-registers -fomit-frame-pointer \
	-ffreestanding
ARFLAGS := crs
LDFLAGS := -Wl, -nostdlib -entry=main $(PLATFORM_LIB) 
endif

# arm-linux-gcc
ifeq ($(CROSS_COMPILE),arm-linux-)
PLATFORM_LIB := -lc -lm -lutil  -lpthread -lssl -ldl -lcrypto -lgcc_s
CFLAGS := -march=armv5te -mtune=xscale -msoft-float -mthumb-interwork -mthumb -mfpu=vfp -mabi=aapcs \
	-fno-exceptions -fpic -ffunction-sections -funwind-tables -fno-short-enums -fmessage-length=0 \
	-finline-functions -fgcse-after-reload -frerun-cse-after-loop -frename-registers -fomit-frame-pointer \
	-ffreestanding
ARFLAGS := crs
LDFLAGS := -nostdlib -entry=main $(PLATFORM_LIB) 
endif

ifeq ($(CROSS_COMPILE), )
PLATFORM_LIB := -lc -lgcc -lm -lpthread 
ifeq ($(UBUNTU_X86),false)
CFLAGS += -fPIC -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -fmessage-length=0 -mlittle-endian -march=armv7-a -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp -Wall
LDFLAGS := -nostdlib -entry=main $(PLATFORM_LIB)
endif
ifeq ($(UBUNTU_X86),true)
PLATFORM_LIB += -lrt
CFLAGS += -fPIC -O0 -g3 -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -fmessage-length=0 -Wall
LDFLAGS := -g3 -nostdlib -entry=main  $(PLATFORM_LIB)
endif
ARFLAGS := crs
endif
# arm-linux-gnueabi
ifeq ($(CROSS_COMPILE),arm-linux-gnueabi-)
PLATFORM_LIB := -lm -lrt
CFLAGS += -pthread -fPIC -O2 -Os -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -DRENAME_RX_TYPES -fexceptions -fstack-protector --param=ssp-buffer-size=4 -fmessage-length=0 -mlittle-endian -march=armv7-a -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp -Wall
ARFLAGS := crs
LDFLAGS := -pthread -entry=main
LDLIBS  := $(PLATFORM_LIB)
endif

#
# Environment Options
#

ifeq ($(USE_DEFAULT_PATH),true)

# arm-eabi-gcc
ifeq ($(CROSS_COMPILE),arm-eabi-)
PLATFORM_ROOT ?= /psc/proj/gpsandroidb/manju/u8500-2.2_r1.1_v0.0.15_nonMilou
PLATFORM_BIN := $(PLATFORM_ROOT)/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/bin/
PLATFORM_INC := -I${PLATFORM_ROOT}/bionic/libc/include \
	-I${PLATFORM_ROOT}/bionic/libc/stdlib \
	-I${PLATFORM_ROOT}/bionic/libc/stdio \
	-I${PLATFORM_ROOT}/bionic/libc/arch-arm/include \
	-I${PLATFORM_ROOT}/bionic/libc/arch-arm/include/machine \
	-I${PLATFORM_ROOT}/bionic/libc/kernel/common \
	-I${PLATFORM_ROOT}/bionic/libc/kernel/arch-arm \
	-I${PLATFORM_ROOT}/bionic/libm/include 
PLATFORM_LIB_PATH := -L${PLATFORM_ROOT}/prebuilt/linux-x86/toolchain/arm-eabi-4.2.1/lib/gcc/arm-eabi/4.2.1/interwork \
	-L$(TOPDIR)/../arm-eabi-stdlib
LDFLAGS += $(PLATFORM_LIB_PATH)
endif

# arm-linux-gcc
ifeq ($(CROSS_COMPILE),arm-linux-)
PLATFORM_ROOT ?= /data/ccase11/ALL_TOOLS_MBL_LSP/tools/distro/cortex/STLinux-2.3/devkit/arm
PLATFORM_BIN := $(PLATFORM_ROOT)/bin/
PLATFORM_INC := -I${PLATFORM_ROOT}/include -I${PLATFORM_ROOT}/target/usr/include/
PLATFORM_LIB_PATH := -L${PLATFORM_ROOT}/target/usr/lib -L${PLATFORM_ROOT}/target/lib \
	-L${PLATFORM_ROOT}/arm-cortex-linux-gnueabi/lib/
LDFLAGS += $(PLATFORM_LIB_PATH)
endif

# arm-linux-gnueabi
ifeq ($(CROSS_COMPILE),arm-linux-gnueabi-)
PLATFORM_ROOT ?= /usr
PLATFORM_BIN := $(PLATFORM_ROOT)/bin/
endif
endif
