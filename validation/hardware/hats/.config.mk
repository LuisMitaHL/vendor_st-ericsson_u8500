$(if $(ANDROID_PRODUCT_OUT)$(FORESTROOT_DIR),,$(error Please run "source build/envsetup.sh" and "lunch" before or "make INSTALL_MOD_PATH=<...> FORESTROOT_DIR=<...> IMAGE_OUTPUT_BASE=<...> PRODUCT=<...>"))
$(if $(ANDROID_PRODUCT_OUT)$(INSTALL_MOD_PATH),,$(error Please run "source build/envsetup.sh" and "lunch" before or "make INSTALL_MOD_PATH=<...> FORESTROOT_DIR=<...> IMAGE_OUTPUT_BASE=<...> PRODUCT=<...>"))
$(if $(ANDROID_PRODUCT_OUT)$(IMAGE_OUTPUT_BASE),,$(error Please run "source build/envsetup.sh" and "lunch" before or "make INSTALL_MOD_PATH=<...> FORESTROOT_DIR=<...> IMAGE_OUTPUT_BASE=<...> PRODUCT=<...>"))


BUILD_TOP:=$(if $(ANDROID_BUILD_TOP),$(ANDROID_BUILD_TOP),$(FORESTROOT_DIR))
$(if $(BUILD_TOP),,$(error Please run "source build/envsetup.sh" and "lunch" before or "make INSTALL_MOD_PATH=<...> FORESTROOT_DIR=<...> IMAGE_OUTPUT_BASE=<...> PRODUCT=<...>"))

PRODUCT?=$(notdir $(ANDROID_PRODUCT_OUT))
$(if $(PRODUCT),,$(error Please run "source build/envsetup.sh" and "lunch" before or "make INSTALL_MOD_PATH=<...> FORESTROOT_DIR=<...> IMAGE_OUTPUT_BASE=<...> PRODUCT=<...>"))

CWD:=$(shell pwd)
ROOT_DIR?=
ROOT_DIR:=$(CWD)/$(ROOT_DIR)
ROOTPROJ:=$(BUILD_TOP)

TOOLCHAIN_TARGET_DIR=$(BUILD_TOP)/toolchain/arm-2009q3/arm-none-linux-gnueabi/libc
TOOLCHAIN_BIN_DIR=$(BUILD_TOP)/toolchain/arm-2009q3/bin
export PATH:=$(TOOLCHAIN_BIN_DIR):$(PATH)
export TOOLCHAIN_TARGET_DIR

#Products used
PRODUCT_U8500="u8500"
PRODUCT_U9500_21="u9500_21"
PRODUCT_U9500_100="u9500_100"
PRODUCT_UX600="ux600_svp"

# Make variables (CC, etc...)
MAKE=make
AS= $(CROSS_COMPILE)as
LD= $(CROSS_COMPILE)ld
CC= $(CROSS_COMPILE)gcc $(HATS_EXTRA_CFLAGS)
CPP= $(CC) -E
AR= $(CROSS_COMPILE)ar
NM= $(CROSS_COMPILE)nm
RANLIB= $(CROSS_COMPILE)ranlib
STRIP= $(CROSS_COMPILE)strip
OBJCOPY= $(CROSS_COMPILE)objcopy
OBJDUMP= $(CROSS_COMPILE)objdump

# Cross-compile variables
ARCH=arm
CROSS_COMPILE=arm-none-linux-gnueabi-
CROSS_COMPILE_TARGET=$(subst -gcc,,$(CROSS_COMPILE)gcc)
VARS+=ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

# Makefile commands
CP= cp
INSTALL_CMD= install
MKDIR= mkdir
RM= rm
LN= ln
ECHO= echo
MV= mv
AWK= awk
GENKSYMS= scripts/genksyms/genksyms
DEPMOD= /sbin/depmod
KALLSYMS= scripts/kallsyms
PERL= perl
CHECK= sparse
LDCONFIG= /sbin/ldconfig


# Hats shared variables
HATS_KERNEL=$(BUILD_TOP)/kernel
HATS_KERNEL_INCLUDE=$(HATS_KERNEL)/include
HATS_KERNEL_MACH_INCLUDE=$(HATS_KERNEL)/arch/arm/mach-ux500/include/mach

# Set a default value in case of standalone compilation
MULTIMEDIA_PATH?=$(BUILD_TOP)/vendor/st-ericsson/multimedia
CONNECTIVITY_PATH?=$(BUILD_TOP)/vendor/st-ericsson/connectivity
HARDWARE_PATH?=$(BUILD_TOP)/vendor/st-ericsson/hardware

# HATS "basic" paths (almost defines HATS tree structure)
# DO NOT USE THESE DEFINES DIRECTELY, USE the "on-target" or "build time" ones
HATS_BIN_BASE=usr/bin
HATS_LOCALBIN_BASE=usr/local/bin
HATS_ETC_BASE=etc
HATS_LIB_BASE=usr/lib
HATS_LOCALLIB_BASE=usr/local/lib
HATS_SHARE_BASE=usr/share
HATS_VARLOCAL_BASE=var/local
HATS_OPT_BASE=opt
HATS_LOG_BASE=var/log
HATS_MODEMFS_BASE=modemfs
HATS_FIRMWARE_BASE=lib/firmware

# HATS "on-target" paths (were to find bin,lib,... during execution)
HATS_ROOT_PATH=
HATS_BIN_PATH=$(HATS_ROOT_PATH)/$(HATS_BIN_BASE)
HATS_LOCALBIN_PATH=$(HATS_ROOT_PATH)/$(HATS_LOCALBIN_BASE)
HATS_ETC_PATH=$(HATS_ROOT_PATH)/$(HATS_ETC_BASE)
HATS_LIB_PATH=$(HATS_ROOT_PATH)/$(HATS_LIB_BASE)
HATS_LOCALLIB_PATH=$(HATS_ROOT_PATH)/$(HATS_LOCALLIB_BASE)
HATS_SHARE_PATH=$(HATS_ROOT_PATH)/$(HATS_SHARE_BASE)
HATS_VARLOCAL_PATH=$(HATS_ROOT_PATH)/$(HATS_VARLOCAL_BASE)
HATS_OPT_PATH=$(HATS_ROOT_PATH)/$(HATS_OPT_BASE)
HATS_LOG_PATH=$(HATS_ROOT_PATH)/$(HATS_LOG_BASE)
HATS_MODEMFS_PATH=$(HATS_ROOT_PATH)/$(HATS_MODEMFS_BASE)
HATS_FIRMWARE_PATH=$(HATS_ROOT_PATH)/$(HATS_FIRMWARE_BASE)

# HATS "build time" paths (were to store bin,lib,... during build)

HATS_OBJ = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/obj/hats,$(IMAGE_OUTPUT_BASE)/obj/hats)
HATS_SYM = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/symbols/hats,$(IMAGE_OUTPUT_BASE)/symbols/hats)
HATS_INC = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/obj/hats/include,$(IMAGE_OUTPUT_BASE)/obj/hats/include)
HATS_SYSTEMFS = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/system/hats,$(INSTALL_MOD_PATH))
HATS_ROOTFS = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/root/hats,$(INSTALL_MOD_PATH))
HATS_MODEMFS = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/modemfs,$(INSTALL_MOD_PATH)/modemfs)
HATS_MODEMDIR = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/system/hats/data/ste/modem/)
HATS_ANDROID_MODEMDIR = $(if $(ANDROID_PRODUCT_OUT),$(ANDROID_PRODUCT_OUT)/data/ste/modem/Phone_FS)

HATS_BIN = $(HATS_SYSTEMFS)/$(HATS_BIN_BASE)
HATS_LOCALBIN = $(HATS_SYSTEMFS)/$(HATS_LOCALBIN_BASE)
HATS_ETC = $(HATS_SYSTEMFS)/$(HATS_ETC_BASE)
HATS_LIB = $(HATS_SYSTEMFS)/$(HATS_LIB_BASE)
HATS_LOCALLIB = $(HATS_SYSTEMFS)/$(HATS_LOCALLIB_BASE)
HATS_SHARE = $(HATS_SYSTEMFS)/$(HATS_SHARE_BASE)
HATS_VARLOCAL = $(HATS_SYSTEMFS)/$(HATS_VARLOCAL_BASE)
HATS_OPT = $(HATS_SYSTEMFS)/$(HATS_OPT_BASE)
HATS_LOG = $(HATS_SYSTEMFS)/$(HATS_LOG_BASE)
HATS_FIRMWARE=$(HATS_SYSTEMFS)/$(HATS_FIRMWARE_BASE)

# HATS configuration generated file
HATS_CONFIG = $(HATS_OBJ)/.config

# Many libraries are compiled as shared
LDFLAGS+= -L$(HATS_LIB)
LIBFLAGS= -shared
EXEFLAGS=
