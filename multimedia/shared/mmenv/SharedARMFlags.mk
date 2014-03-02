#
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#

MMENV_BOARD=$(shell echo $(PLATFORM) | cut -d - -f 1)
MMENV_SYSTEM=$(shell echo $(PLATFORM) | cut -d - -f 2)

FLAGS_SET=0

################################################################################
# Some common definitions to avoid errors

## CPPFLAGS

define 8500v2-cppflags
FIXED_CPPFLAGS:=-D__STN_8500=30 -D__CUT_30 -D__STN_4500=30
endef

define 5500v1-cppflags
FIXED_CPPFLAGS:=-D__STN_5500=10 -D__CUT_10 -DHOST_ONLY
NMF_CHIP:=STn5500
endef

define 9540v1-cppflags
FIXED_CPPFLAGS:=-D__STN_9540=10 -D__CUT_10
NMF_CHIP:=STn9540
endef

define 8540v1-cppflags
FIXED_CPPFLAGS:=-D__STN_8540=10 -D__CUT_10
NMF_CHIP:=STn8540
endef

define 9600v1-cppflags
FIXED_CPPFLAGS:=-D__STN_9600=10 -D__CUT_10 -DHOST_ONLY
NMF_CHIP:=STn9600
endef


################################################################################

define rvct4-ca9
CC:=armcc
CXX:=armcc
ASM:=armasm
AR:=armar
LD_EXE:=armlink
LD_LIB:=armlink
TARGET:=rvct
OPTIMIZ_CFLAGS:=-O3 -Otime
FIXED_CFLAGS:=--cpu=Cortex-A9 --dwarf2 --no_unaligned_access 
FIXED_ASMFLAGS:=--cpu=Cortex-A9 --dwarf2 --no_unaligned_access
FIXED_LDEXEFLAGS:=--noremove --entry=Reset_Handler
FIXED_LDLIBFLAGS:=--noremove --entry=Reset_Handler
FIXED_CXXFLAGS:= $$(FIXED_CFLAGS) --no_rtti --force_new_nothrow --no_exceptions --cpp
endef

define rvct4-ca15
CC:=armcc
CXX:=armcc
ASM:=armasm
AR:=armar
LD_EXE:=armlink
LD_LIB:=armlink
TARGET:=rvct
OPTIMIZ_CFLAGS:=-O3 -Otime
FIXED_CFLAGS:=--cpu=Cortex-A15 --dwarf2 --no_unaligned_access 
FIXED_ASMFLAGS:=--cpu=Cortex-A15 --dwarf2 --no_unaligned_access
FIXED_LDEXEFLAGS:=--noremove --entry=Reset_Handler
FIXED_LDLIBFLAGS:=--noremove --entry=Reset_Handler
FIXED_CXXFLAGS:= $$(FIXED_CFLAGS) --no_rtti --force_new_nothrow --no_exceptions --cpp
endef


################################################################################
# Symbian platforms

#mop500_ed-symbian
# this platform exists only in order to generate the temporary code for NMF components running on Host, 
# since it is not possible to generate it with the symbian build system. A 
ifeq ($(PLATFORM), mop500_ed-symbian)
$(eval $(rvct4-ca9))
$(eval $(8500ed-cppflags))
FIXED_CPPFLAGS+= -D__ARM_THINK
THINK_SCATTER=8500_scatter.scf
SYMBIAN_VARIANT_NAME=8500ed
SYMBIAN_VARIANT_TAG=ST_8500ED

FLAGS_SET=1
endif

#mop500_v1-symbian
ifeq ($(PLATFORM), mop500_v1-symbian)
$(eval $(rvct4-ca9))
$(eval $(8500v1-cppflags))
FIXED_CPPFLAGS+= -D__ARM_THINK
THINK_SCATTER=8500_scatter.scf
SYMBIAN_VARIANT_NAME=8500v1
SYMBIAN_VARIANT_TAG=ST_8500V1

FLAGS_SET=1
endif

#mop500_v2-symbian
ifeq ($(PLATFORM), mop500_v2-symbian)
$(eval $(rvct4-ca9))
$(eval $(8500v2-cppflags))
FIXED_CPPFLAGS+= -D__ARM_THINK
THINK_SCATTER=8500_scatter.scf
SYMBIAN_VARIANT_NAME=8500v2
SYMBIAN_VARIANT_TAG=ST_8500V2

FLAGS_SET=1
endif

################################################################################
# Linux platforms
################################################################################

# Definition of compilation flags for using RCVT to create Linux executable

define opt-rvct-gnu-ca9
RVCT_GNU_CA9_FLAGS    := --cpu=Cortex-A9 --exceptions --dwarf3 --fpu=SoftVFP+VFPv3 --no_unaligned_access
RVCT_GNU_CA9_CXXFLAGS := --no_rtti --force_new_nothrow --no_exceptions --cpp
OPT_CC:=armcc
OPT_CXX:=armcc
OPT_ASM:=armasm
OPT_OPTIMIZ_CFLAGS:=-O3 -Otime
OPT_FIXED_CFLAGS:=$$(RVCT_GNU_CA9_FLAGS) --arm_linux --min_array_alignment=8 
OPT_FIXED_ASMFLAGS:=$$(RVCT_GNU_CA9_FLAGS)
OPT_FIXED_CXXFLAGS:=$$(OPT_FIXED_CFLAGS) $$(RVCT_GNU_CA9_CXXFLAGS)
OPT_FIXED_CPPFLAGS+= -DLINUX -D__ARM_LINUX
## Other optimization options
#  --vectorize  
endef

define linux-gnu-ca9
TARGET_CROSS_COMPILE?=arm-linux-
CC:=$$(TARGET_CROSS_COMPILE)gcc
CXX:=$$(TARGET_CROSS_COMPILE)g++
ASM:=$$(TARGET_CROSS_COMPILE)as
AR:=$$(TARGET_CROSS_COMPILE)ar
LD_EXE:=$$(TARGET_CROSS_COMPILE)g++
LD_LIB:=$$(TARGET_CROSS_COMPILE)ld
LD_SO:=$$(TARGET_CROSS_COMPILE)g++
TARGET:=armgcc
OPTIMIZ_CFLAGS:=-O3
FIXED_ARCH_FLAGS:= -march=armv7-a -mtune=cortex-a8 -mfloat-abi=softfp -mfpu=neon
FIXED_CPPFLAGS+= -DLINUX -D__ARM_LINUX $(addprefix -I,$(realpath $(ANDROID_BSP_ROOT)/base_system/tempdir/stage/usr/include))
FIXED_CCFLAGS:= $$(FIXED_ARCH_FLAGS) -pthread -fpic -Wall -fno-exceptions -fno-strict-aliasing
FIXED_CFLAGS:= $$(FIXED_CCFLAGS)
FIXED_ASMFLAGS:=
FIXED_LDEXEFLAGS:= $$(FIXED_CCFLAGS) -lrt
FIXED_LDFLAGS:=$(addprefix -L,$(realpath $(ANDROID_BSP_ROOT)/base_system/tempdir/stage/usr/lib))
FIXED_LDLIBFLAGS:=
FIXED_LDSOFLAGS:= -shared -Wl,--no-undefined -ldl
FIXED_CXXFLAGS:= -fno-rtti $$(FIXED_CCFLAGS)
endef

#svp5500_v1-linux
ifeq ($(PLATFORM), svp5500_v1-linux)
  $(eval $(5500v1-cppflags))
  $(eval $(linux-gnu-ca9))
  $(eval $(opt-rvct-gnu-ca9))

  FLAGS_SET=1
endif

#u5500_v1-linux
ifeq ($(PLATFORM), u5500_v1-linux)
  $(eval $(5500v1-cppflags))
  $(eval $(linux-gnu-ca9))
  $(eval $(opt-rvct-gnu-ca9))

  FLAGS_SET=1
endif

#svp9600_v1-linux
ifeq ($(PLATFORM), svp9600_v1-linux)
  $(eval $(9600v1-cppflags))
  $(eval $(linux-gnu-ca9))
  $(eval $(opt-rvct-gnu-ca9))

  FLAGS_SET=1
endif

#u8500_v2-linux
ifeq ($(PLATFORM), u8500_v2-linux)
  $(eval $(8500v2-cppflags))
  $(eval $(linux-gnu-ca9))
  $(eval $(opt-rvct-gnu-ca9))

  FLAGS_SET=1
endif

#u9540_v1-linux
ifeq ($(PLATFORM), u9540_v1-linux)
  $(eval $(9540v1-cppflags))
  $(eval $(linux-gnu-ca9))
  $(eval $(opt-rvct-gnu-ca9))

  FLAGS_SET=1
endif

#u8540_v1-linux
ifeq ($(PLATFORM), l8540_v1-linux)
  $(eval $(8540v1-cppflags))
  $(eval $(linux-gnu-ca9))
  $(eval $(opt-rvct-gnu-ca9))

  FLAGS_SET=1
endif
################################################################################

# Fixup needed with ST-Linux toolchain
ifeq ($(CXX),arm-linux-g++) 
  GCCVERSION:=$(shell basename $(shell dirname `arm-linux-g++ -print-libgcc-file-name`))
  GCCTARGET:=$(shell basename $(shell dirname $(shell dirname `arm-linux-g++ -print-libgcc-file-name`)))
  GCCPATH:=$(shell dirname $(shell which arm-linux-g++))
  CXXSTDINCPATH:=$(GCCPATH)/../target/usr/include/c++/$(GCCVERSION)
  FIXED_CXXFLAGS +=-I$(CXXSTDINCPATH) -I$(CXXSTDINCPATH)/$(GCCTARGET) -I$(CXXSTDINCPATH)/backward
endif

########################################
#android

ifeq ($(MMENV_SYSTEM),android)

FLAGS_SET=1

BUILD_SYSTEM=$(ANDROID_BSP_ROOT)/build/core
TARGET_OUT_STATIC_LIBRARIES=$(ANDROID_OUT_TARGET_PRODUCT_DIRECTORY)/obj/lib

CC=$(TARGET_CROSS_COMPILE)gcc
CXX=$(TARGET_CROSS_COMPILE)g++
ASM=$(TARGET_CROSS_COMPILE)as
AR=$(TARGET_CROSS_COMPILE)ar
LD_EXE=$(TARGET_CROSS_COMPILE)g++
LD_LIB=$(TARGET_CROSS_COMPILE)ld
LD_SO=$(TARGET_CROSS_COMPILE)g++
TARGET=armgcc
OPTIMIZ_CFLAGS=-O3
FIXED_CCFLAGS=
FIXED_CFLAGS=
FIXED_CXXFLAGS=

# optimized compiler
$(eval $(opt-rvct-gnu-ca9))

ifeq ($(MMENV_BOARD),u8500_v2)
  $(eval $(8500v2-cppflags))
endif

ifeq ($(MMENV_BOARD),svp9600_v1)
  $(eval $(9600v1-cppflags))
endif

ifeq ($(MMENV_BOARD),svp5500_v1)
  $(eval $(5500v1-cppflags))
endif

ifeq ($(MMENV_BOARD),u5500_v1)
  $(eval $(5500v1-cppflags))
endif

ifeq ($(MMENV_BOARD),u9540_v1)
  $(eval $(9540v1-cppflags))
endif

ifeq ($(MMENV_BOARD),l8540_v1)
  $(eval $(8540v1-cppflags))
endif

FIXED_CPPFLAGS += -DLINUX -D__ARM_LINUX -D__ARM_ANDROID -DANDROID
FIXED_CPPFLAGS += -DENS_DONT_CHECK_STRUCT_SIZE_AND_VERSION

##### Taken from Android makefiles #####

## In case a module want to build for ARM it should set in your makefile:
## LOCAL_ARM_MODE := arm
LOCAL_ARM_MODE := $(strip $(LOCAL_ARM_MODE))
objects_mode := $(if $(LOCAL_ARM_MODE),$(LOCAL_ARM_MODE),thumb)

### Some definitions we need

combo_target := TARGET_
# $(1): os/arch
define select-android-config-h
$(ANDROID_BSP_ROOT)/system/core/include/arch/$(1)/AndroidConfig.h
endef

### Taken from android/build/core/config.mk
COMMON_GLOBAL_CFLAGS:= -DANDROID -fmessage-length=0 -W -Wall -Wno-unused -Winit-self -Wpointer-arith
COMMON_RELEASE_CFLAGS:= -DNDEBUG -UDEBUG

COMMON_GLOBAL_CPPFLAGS:= $(COMMON_GLOBAL_CFLAGS) -Wsign-promo
COMMON_RELEASE_CPPFLAGS:= $(COMMON_RELEASE_CFLAGS)

### Taken from android/build/core/combo/select.mk
$(combo_target)GLOBAL_CFLAGS := -fno-exceptions -Wno-multichar -frecord-gcc-switches
$(combo_target)RELEASE_CFLAGS := -g -fno-strict-aliasing

### Taken from android/build/core/combo/arch/arm/armv7-a.mk

arch_variant_cflags := \
    -march=armv7-a \
    -mfloat-abi=softfp \
    -mfpu=neon

arch_variant_ldflags := \
        -Wl,--fix-cortex-a8

### Taken from android/build/core/combo/linux-arm.mk

TARGET_arm_CFLAGS :=    -fomit-frame-pointer \
                        -fstrict-aliasing    \
                        -funswitch-loops     \
                        -finline-limit=300

TARGET_thumb_CFLAGS :=  -mthumb \
                        -fomit-frame-pointer \
                        -fno-strict-aliasing \
                        -finline-limit=64

ifeq ($(FORCE_ARM_DEBUGGING),true)
  TARGET_arm_CFLAGS += -fno-omit-frame-pointer -fno-strict-aliasing
  TARGET_thumb_CFLAGS += -marm -fno-omit-frame-pointer
endif

android_config_h := $(call select-android-config-h,linux-arm)
arch_include_dir := $(dir $(android_config_h))

$(combo_target)GLOBAL_CFLAGS += \
			-msoft-float -fpic \
			-ffunction-sections \
			-funwind-tables \
			-fstack-protector \
			-fno-short-enums \
			$(arch_variant_cflags) \
			-include $(android_config_h) \
			-I $(arch_include_dir)

$(combo_target)GLOBAL_LDFLAGS += \
			$(arch_variant_ldflags)

$(combo_target)GLOBAL_CFLAGS +=	-mthumb-interwork

$(combo_target)GLOBAL_CPPFLAGS += -fvisibility-inlines-hidden


$(combo_target)RELEASE_CFLAGS := \
			-DSK_RELEASE -DNDEBUG \
			-g \
			-Wstrict-aliasing=2 \
			-finline-functions \
			-fno-inline-functions-called-once \
			-fgcse-after-reload \
			-frerun-cse-after-loop \
			-frename-registers


FIXED_CCFLAGS  += $(COMMON_GLOBAL_CFLAGS) $(TARGET_GLOBAL_CFLAGS) $(TARGET_RELEASE_CFLAGS) $(TARGET_$(objects_mode)_CFLAGS)

libc_root := $(ANDROID_BSP_ROOT)/bionic/libc
libm_root := $(ANDROID_BSP_ROOT)/bionic/libm
libstdc++_root := $(ANDROID_BSP_ROOT)/bionic/libstdc++
libthread_db_root := $(ANDROID_BSP_ROOT)/bionic/libthread_db

# We compile with the global cflags to ensure that 
# any flags which affect libgcc are correctly taken
# into account.
TARGET_LIBGCC := $(shell $(CC) $(FIXED_CCFLAGS) -print-libgcc-file-name)

# We are taking the ones in bionic for now
KERNEL_HEADERS_COMMON := $(libc_root)/kernel/common
KERNEL_HEADERS_ARCH   := $(libc_root)/kernel/arch-arm
KERNEL_HEADERS := $(KERNEL_HEADERS_COMMON) $(KERNEL_HEADERS_ARCH)

TARGET_CRTBEGIN_STATIC_O := $(TARGET_OUT_STATIC_LIBRARIES)/crtbegin_static.o
TARGET_CRTBEGIN_DYNAMIC_O := $(TARGET_OUT_STATIC_LIBRARIES)/crtbegin_dynamic.o
TARGET_CRTBEGIN_O := $(TARGET_CRTBEGIN_DYNAMIC_O)
TARGET_CRTEND_O := $(TARGET_LIBGCC) $(TARGET_OUT_STATIC_LIBRARIES)/crtend_android.o

FIXED_C_INCLUDES := \
	$(libc_root)/arch-arm/include \
	$(libc_root)/include \
	$(libstdc++_root)/include \
	$(KERNEL_HEADERS) \
	$(libm_root)/include \
	$(libthread_db_root)/include \
	$(ANDROID_BSP_ROOT)/system/core/include \
	$(ANDROID_BSP_ROOT)/frameworks/base/include

# This directory is used by android build system but does not exist for now...
#	$(libm_root)/include/arch/arm \

FIXED_CPPFLAGS += $(foreach incdir, $(FIXED_C_INCLUDES), -I$(incdir))
FIXED_CFLAGS   += $(FIXED_CCFLAGS)
FIXED_CXXFLAGS += -fno-rtti $(TARGET_GLOBAL_CPPFLAGS) $(COMMON_GLOBAL_CPPFLAGS) $(COMMON_RELEASE_CPPFLAGS) $(FIXED_CCFLAGS)

FIXED_LDEXEFLAGS= \
	-nostdlib -Bdynamic \
	-Wl,-z,relro \
	-Wl,-z,now \
	-Wl,-dynamic-linker,/system/bin/linker \
	-Wl,--gc-sections \
	-Wl,-z,nocopyreloc \
	-Wl,--no-undefined \
	-L $(TARGET_OUT_STATIC_LIBRARIES) -Wl,-rpath-link=$(TARGET_OUT_STATIC_LIBRARIES) -lc -lm -lcutils -lstdc++
FIXED_LDLIBFLAGS=
FIXED_LDSOFLAGS= \
	-nostdlib -Wl,-soname,$(notdir $@) \
	-Wl,-z,relro \
	-Wl,-z,now \
	-Wl,--gc-sections \
	-Wl,-shared,-Bsymbolic -Wl,--no-undefined \
	-L $(TARGET_OUT_STATIC_LIBRARIES) -lc -lm -ldl -llog -lcutils -lstdc++

endif

################################################################################

ifeq ($(SW_VARIANT),android)
  FIXED_CPPFLAGS += -DSW_VARIANT_ANDROID
endif

################################################################################

#x86*-linux
ifeq ($(findstring x86,$(MMENV_BOARD)),x86)
ifeq ($(MMENV_SYSTEM),linux)

CC=gcc
CXX=g++
ASM=as
ASMFLAGS=-g 
AR=ar
LD_EXE=g++
LD_LIB=ld
LD_SO=g++
TARGET=unix
OPTIMIZ_CFLAGS=-O0
FIXED_CFLAGS=-m32 -pthread -Wall -fno-strict-aliasing
FIXED_ASMFLAGS=
FIXED_CPPFLAGS=-DWORKSTATION -DLINUX -DHOST_ONLY
FIXED_LDEXEFLAGS=$(FIXED_CFLAGS) -lrt
FIXED_LDLIBFLAGS=-m elf_i386
FIXED_LDSOFLAGS=$(FIXED_CFLAGS) -shared -Wl,--no-undefined
FIXED_CXXFLAGS=$(FIXED_CFLAGS)
NMF_CHIP=

ifeq ($(MMENV_BOARD),x86)
MMDSPCORE=x86
FLAGS_SET=1
endif
ifeq ($(MMENV_BOARD),x86_mmdsp)
MMDSPCORE=x86_mmdsp
FLAGS_SET=1
endif
ifeq ($(MMENV_BOARD),x86_cortexA9)
MMDSPCORE=x86_cortexA9
FLAGS_SET=1
endif
ifeq ($(MMENV_BOARD), x86_8500ed)
MMDSPCORE=mmdsp_8500ed
FLAGS_SET=1
endif
ifeq ($(MMENV_BOARD), x86_8500)
MMDSPCORE=mmdsp_8500
FLAGS_SET=1
endif

endif
endif

ifeq ($(FLAGS_SET),0)
$(error Unknown target platform: PLATFORM='$(PLATFORM)')
endif
