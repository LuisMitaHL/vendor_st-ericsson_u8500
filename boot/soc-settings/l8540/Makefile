#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
# Author: WenHai Fang <wenhai.h.fang AT stericsson.com>
#   for ST-Ericsson
#
# Origin: From the file u8500/Makefile

# Location and name variables
SRCDIR := src
INCDIR := include
BASS_APP_SRCDIR := bass_app/src
BASS_APP_INCDIR := bass_app/include
PRCMU_INCDIR := ../../prcmufw/L8540
NAME := soc_settings
LDSCRIPT := $(NAME).ld

######################################
# Config files used
CONFIG_H := $(INCDIR)/config.h
CONFIG_MK := config.mk

# Include config.mk generated when selecting config (if it exists)
-include $(CONFIG_MK)

COMPILE_AVS := $(if $(shell bash -c "grep compile_avs $(CONFIG_MK) 2>/dev/null"),1,0)
COMPILE_BB := $(if $(shell bash -c "grep compile_bb $(CONFIG_MK) 2>/dev/null"),1,0)
BUILD_CUST1 := $(if $(shell bash -c "grep cust1 $(CONFIG_MK) 2>/dev/null"),1,0)

# Source files used
SRCS += $(SRCDIR)/opp_init.c
SRCS += $(SRCDIR)/ab8540_init.c
SRCS += $(SRCDIR)/c2c_init.c
SRCS += $(SRCDIR)/st-ericsson_config.c
SRCS += $(SRCDIR)/customer1_config.c
SRCS += $(SRCDIR)/lpddr2_jedec_4x4gb_533Mhz.c
SRCS += $(SRCDIR)/lpddr2_jedec_2x4gb_533Mhz.c
SRCS += $(SRCDIR)/version.c
SRCS += $(SRCDIR)/imad.c
SRCS_EXEC += $(SRCDIR)/avs_calc.c
SRCS_EXEC += $(SRCDIR)/ab8540.c
ifeq ($(COMPILE_AVS),1)
SRCS_EXEC += $(SRCDIR)/avs_lib.c
else
ifeq ($(COMPILE_BB),1)
SRCS_EXEC += $(SRCDIR)/avs_lib.c
endif
endif
SRCS_EXEC += $(SRCDIR)/memtest.c
SRCS_EXEC += $(SRCDIR)/memtest_pd.c
SRCS_EXEC += $(SRCDIR)/soc_settings_exec.c
SRCS_EXEC += $(SRCDIR)/uart.c
SRCS_EXEC += $(BASS_APP_SRCDIR)/apply_soc_settings.c
SRCS_EXEC += $(BASS_APP_SRCDIR)/security.c
SRCS_EXEC += $(BASS_APP_SRCDIR)/tee_client_api.c

# We always want to cross compile to arm. Override CROSS_COMPILE otherwise
CROSS_COMPILE ?= arm-eabi-

# Tools used
AS	:= $(CROSS_COMPILE)as
LD 	:= $(if $(shell bash -c "type $(CROSS_COMPILE)ld.bfd 2>/dev/null"), \
		$(CROSS_COMPILE)ld.bfd, $(CROSS_COMPILE)ld)
CC	:= $(CROSS_COMPILE)gcc
STRIP	:= $(CROSS_COMPILE)strip
OBJCOPY := $(CROSS_COMPILE)objcopy
CP	:= cp
RM	:= rm
LN	:= ln
LS	:= ls
ECHO	:= echo
MKDIR	:= mkdir

# Use 'make V=1 target' to print true tool cmdline
ifneq ($(V),1)
	#None verbose output. Hide most of the output
	MYCC = @$(ECHO) "cc\t$<" && $(CC)
	MYLD = @$(ECHO) "ld\t$@" && $(LD)
	MYOBJCOPY = @$(ECHO) "objcopy\t$@" && $(OBJCOPY)
	MYGENVERSION = @$(ECHO) "version\t$@" && ./scripts/genversion
	MYMKDIR = @$(MKDIR)
	MYCP = @$(CP)
	MYLN = @$(LN)
	MYRM = @$(RM)
else
	MYCC = $(CC)
	MYLD = $(LD)
	MYOBJCOPY = $(OBJCOPY)
	MYGENVERSION = ./scripts/genversion
	MYMKDIR = $(MKDIR)
	MYCP = $(CP)
	MYLN = $(LN)
	MYRM = $(RM)
endif

# Common flags
INC += -I $(INCDIR)

INC += -I $(BASS_APP_INCDIR)

INC += -I $(PRCMU_INCDIR)

CFLAGS += -std=gnu99 -Wall -Wextra -Werror -pipe
CFLAGS += -fomit-frame-pointer -fno-stack-protector -fno-short-enums
CFLAGS += -fpie -ffreestanding
CFLAGS += -nostdlib
CFLAGS += -marm -march=armv7-a -mtune=cortex-a9
CFLAGS += -mabi=aapcs-linux -mno-thumb-interwork

DEFINES +=

LDFLAGS += -nostdlib -Bstatic

# Extra flags for linker
#  -ffunction-sections -fdata-sections will section the functions and data
#   during compilation. Needed for the linker to be able to place
#   symbols correctly
CFLAGS += -ffunction-sections -fdata-sections

# If config.mk doesn't exist yet or for some reason
# CONFIG_NAME is missing, set it
CONFIG_NAME ?= "default"

# Add the config name to defines
DEFINES += -DCONFIG_NAME="$(CONFIG_NAME)"

# For bass_app
DEFINES += -DOS_FREE
DEFINES += -DBOOTLOGBASEADDR=0xFFFE0000

# Target is a string with the selected configuration, for file names
TARGET := _$(CONFIG_NAME)

# Derived files from source files, with variables applied
OBJS_EXEC := $(SRCS_EXEC:.c=$(TARGET).o)
OBJS_EXEC_ELF := $(SRCDIR)/soc_settings_exec$(TARGET).elf
OBJS := $(SRCS:.c=$(TARGET).o)
OBJS += $(OBJS_EXEC)
OBJS_ELF := $(SRCS:.c=$(TARGET).elf)
OBJS_ELF += $(OBJS_EXEC_ELF)
OBJS_BIN := $(OBJS_ELF:.elf=.bin)
BINFILE := $(NAME)$(TARGET).bin

# Binary files used for v20
# Do not change the Order before(include) ddr_generic_4g_d2.bin
# Memory order after ddr_generic_4g_d2.bin can be changed
# Memory ddr_generic_4g_d2.bin is the default settings if no match
SRCB_CCU += $(SRCDIR)/version$(TARGET).bin
SRCB_CCU += $(SRCDIR)/opp_init$(TARGET).bin
SRCB_CCU += $(SRCDIR)/ab8540_init$(TARGET).bin
SRCB_CCU += $(SRCDIR)/c2c_init$(TARGET).bin
SRCB_CCU += $(SRCDIR)/st-ericsson_config$(TARGET).bin
SRCB_CCU += $(SRCDIR)/imad$(TARGET).bin
SRCB_CCU += $(SRCDIR)/lpddr2_jedec_4x4gb_533Mhz$(TARGET).bin
SRCB_CCU += $(SRCDIR)/soc_settings_exec$(TARGET).bin

# Binary files used for One Die Memory
# The first bin is regarded as generic settings
SRCB_CUST1 += $(SRCDIR)/version$(TARGET).bin
SRCB_CUST1 += $(SRCDIR)/opp_init$(TARGET).bin
SRCB_CUST1 += $(SRCDIR)/ab8540_init$(TARGET).bin
SRCB_CUST1 += $(SRCDIR)/c2c_init$(TARGET).bin
SRCB_CUST1 += $(SRCDIR)/customer1_config$(TARGET).bin
SRCB_CUST1 += $(SRCDIR)/imad$(TARGET).bin
SRCB_CUST1 += $(SRCDIR)/lpddr2_jedec_2x4gb_533Mhz$(TARGET).bin
SRCB_CUST1 += $(SRCDIR)/soc_settings_exec$(TARGET).bin

# How to get all configs
CONFIGLIST = $(LS) -1 configs | sed -e 's/_config.h//'

# Dependency
DEPFLAGS = -Wp,-MP,-MD,$*$(TARGET).d,-MT$*$(TARGET).o

# As a shortcut, allow DEBUG=1 tobe the same as BUILD=debug
ifeq ($(DEBUG),1)
	BUILD := debug
endif

# If BUILD isn't selected, we assume release
ifeq ($(BUILD),)
	BUILD := release
endif

# DEBUG compile
ifeq ($(BUILD),debug)
	CFLAGS += -O2 -g
	DEFINES += -DDEBUG
	LDFLAGS += -g
endif

# RELEASE compile
ifeq ($(BUILD),release)
	CFLAGS += -O2 -gdwarf-2
	DEFINES += -UDEBUG -DNDEBUG
	LDFLAGS += --gc-sections
	# --gc-sections will remove the sections not used during linking
endif

# Short cut to override the config log level
ifneq ($(LOG),)
	DEFINES += -DCONFIG_DEBUG_LOG=$(LOG)
endif

# AVS Calculation compile
ifeq ($(COMPILE_AVS),0)
	DEFINES += -DCOMPILE_AVS=0
else
	DEFINES += -DCOMPILE_AVS=1
endif

# Body Biasing Calculation compile
ifeq ($(COMPILE_BB),0)
	DEFINES += -DCOMPILE_BB=0
else
	DEFINES += -DCOMPILE_BB=1
endif

TOOLCHAIN_DIRECTORY := $(shell dirname $$(dirname $$(which $(OBJCOPY))))
LIBGCC_A := $(wildcard $(TOOLCHAIN_DIRECTORY)/lib/gcc/$(CROSS_COMPILE:%-=%)/*/libgcc.a)

# make targets
##############

# This is the first target, it will be execute if no target is given to 'make'.
.PHONY: all
all: $(BINFILE)

# Simple target to force a rebuild of everything
.PHONY: rebuild
rebuild: | clean all

# I always want the size of meminit.bin printed after compilation
.PHONY: size
size: $(BINFILE)
	$(LS) -l $(BINFILE)

# This target will both configure and build for a selected config
%_avs_bb: %_avs_bb_config
	@$(MAKE) --no-print-directory
%_bb: %_bb_config
	@$(MAKE) --no-print-directory
%_avs_bb_cust1: %_avs_bb_cust1_config
	@$(MAKE) --no-print-directory
%_bb_cust1: %_bb_cust1_config
	@$(MAKE) --no-print-directory
%_cust1: %_cust1_config
	@$(MAKE) --no-print-directory
%: %_config
	@$(MAKE) --no-print-directory

# Prevention rules for the above catch all to not match if the include
# files are not generated yet
$(CONFIG_MK): ;
$(SRCDIR)/%.d: ;

# Select configuration
%_avs_bb_config:
	@$(ECHO) "config\tconfigs/$@.h"
	@[ -f configs/$@.h ] || \
		{ $(ECHO) "ERROR: configs/$@.h does not exist"; exit 1; }
	@$(LN) -sf ../configs/$@.h $(CONFIG_H)
	@$(ECHO) "compile_avs=1" > $(CONFIG_MK)
	@$(ECHO) "CONFIG_NAME=$*_avs_bb" >> $(CONFIG_MK)

%_bb_config:
	@$(ECHO) "config\tconfigs/$@.h"
	@[ -f configs/$@.h ] || \
		{ $(ECHO) "ERROR: configs/$@.h does not exist"; exit 1; }
	@$(LN) -sf ../configs/$@.h $(CONFIG_H)
	@$(ECHO) "compile_bb=1" > $(CONFIG_MK)
	@$(ECHO) "CONFIG_NAME=$*_bb" >> $(CONFIG_MK)

%_avs_bb_cust1_config:
	@$(ECHO) "config\tconfigs/$@.h"
	@[ -f configs/$@.h ] || \
		{ $(ECHO) "ERROR: configs/$@.h does not exist"; exit 1; }
	@$(LN) -sf ../configs/$@.h $(CONFIG_H)
	@$(ECHO) "cust1=1" > $(CONFIG_MK)
	@$(ECHO) "compile_avs=1" >> $(CONFIG_MK)
	@$(ECHO) "CONFIG_NAME=$*_avs_bb_cust1" >> $(CONFIG_MK)

%_bb_cust1_config:
	@$(ECHO) "config\tconfigs/$@.h"
	@[ -f configs/$@.h ] || \
		{ $(ECHO) "ERROR: configs/$@.h does not exist"; exit 1; }
	@$(LN) -sf ../configs/$@.h $(CONFIG_H)
	@$(ECHO) "cust1=1" > $(CONFIG_MK)
	@$(ECHO) "compile_bb=1" >> $(CONFIG_MK)
	@$(ECHO) "CONFIG_NAME=$*_bb_cust1" >> $(CONFIG_MK)

%_cust1_config:
	@$(ECHO) "config\tconfigs/$@.h"
	@[ -f configs/$@.h ] || \
		{ $(ECHO) "ERROR: configs/$@.h does not exist"; exit 1; }
	@$(LN) -sf ../configs/$@.h $(CONFIG_H)
	@$(ECHO) "cust1=1" > $(CONFIG_MK)
	@$(ECHO) "CONFIG_NAME=$*_cust1" >> $(CONFIG_MK)

%_config:
	@$(ECHO) "config\tconfigs/$@.h"
	@[ -f configs/$@.h ] || \
		{ $(ECHO) "ERROR: configs/$@.h does not exist"; exit 1; }
	@$(LN) -sf ../configs/$@.h $(CONFIG_H)
	@$(ECHO) "CONFIG_NAME=$*" > $(CONFIG_MK)

# Will list all available configs:
.PHONY: list
list:
	@$(CONFIGLIST)

# Dummy target to force another target to always rebuild
.PHONY: force
force:

# Main linker
# -----------

# Create binary
$(BINFILE): $(OBJS) $(OBJS_ELF) $(OBJS_BIN)
ifeq ($(BUILD_CUST1),0)
	cat $(SRCB_CCU) > $@
else
	cat $(SRCB_CUST1) > $@
endif

# -------------------------------------

# Target to tell that include/config.h doesn't exist yet.
$(CONFIG_H):
	@$(ECHO) "ERROR: Please run 'make <selected>_config' first."
	@exit 1

# Normal compile target. It will compile a file from file.elf to file.bin
%$(TARGET).bin: %$(TARGET).elf Makefile $(CONFIG_H)
	$(MYOBJCOPY) -j .socsettings -O binary $< $@

# Compiling ELF for executable part
$(OBJS_EXEC_ELF): $(OBJS_EXEC) Makefile $(CONFIG_H) $(LIBGCC_A)
	$(MYLD) $(LDFLAGS) -T $(LDSCRIPT) -o $@ -Map=$@.map $(OBJS_EXEC) $(LIBGCC_A)

# Normal compile target. It will compile a file from file.o to file.elf
%$(TARGET).elf: %$(TARGET).o Makefile $(CONFIG_H) $(OBJS)
	$(MYLD) $(LDFLAGS) -T $(LDSCRIPT) -o $@ -Map=$@.map $<

# Normal compile target. It will compile a file from file.c to file.o
%$(TARGET).o: %.c Makefile $(CONFIG_H)
	$(MYCC) $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) -c  $< -o $@

# If we want to see the ASM output. Run 'make file.s' to get the
# ASM output of file.c
%$(TARGET).s: %.c Makefile $(CONFIG_H)
	$(MYCC) $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) -S -o $@ $<

# If we want to see the preprocessor output. Run 'make file.e' to get
# the preprocessor output of file.c
%$(TARGET).e: %.c Makefile $(CONFIG_H)
	$(MYCC) $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) -E -o $@ $<

# Build all configs
.PHONY: everything
# Can't run our self in parallel. Since config.h can only be linked to
# one config at the time.
.NOTPARALLEL: everything
everything:
	@for c in $$($(CONFIGLIST)); do \
		$(MAKE) --no-print-directory $$c || exit $$?; \
	done

.PHONY: postbuild
postbuild: everything

# Clean targets
# =============

.PHONY: clean distclean
clean:
	-$(RM) -f $(SRCDIR)/*.e $(SRCDIR)/*.s $(SRCDIR)/*.d $(SRCDIR)/*.o
	-$(RM) -f $(BASS_APP_SRCDIR)/*.e $(BASS_APP_SRCDIR)/*.s
	-$(RM) -f $(BASS_APP_SRCDIR)/*.d $(BASS_APP_SRCDIR)/*.o
	-$(RM) -f $(SRCDIR)/*.elf $(SRCDIR)/*.map $(SRCDIR)/*.bin
	-$(RM) -f *.bin *.elf *.map

distclean: clean
	-$(RM) -f $(CONFIG_H) $(CONFIG_MK)

# Other code checking tools
# =========================
.PHONY: check variants cppcheck clang pmccabe

check: pmccabe cppcheck clang

# Outputs an analyzis of which function are the biggest/most complex ones
# The first pmccabe will print the header of the columns
pmccabe:
	pmccabe -v -c /dev/null; pmccabe -v -c $(SRCS) | sort -nr | head -20

cppcheck:
	cppcheck -a -s -v --unused-functions $(INC) $(SRCS)

clang:
	clang -fsyntax-only $(INC) $(CFLAGS) $(DEFINES) $(SRCS)
	clang --analyze $(INC) $(CFLAGS) $(DEFINES) $(SRCS)


help:
	@$(ECHO) "Build variables:"
	@$(ECHO) "* BUILD=<release|debug>"
	@$(ECHO) "    Will select release or debug build, default is 'release'"
	@$(ECHO) "* V=1"
	@$(ECHO) "    Will show verbose details of the build output."
	@$(ECHO) ""
	@$(ECHO) "Select a config:"
	@$(ECHO) "  make <config name>_config"
	@$(ECHO) ""
	@$(ECHO) "Build the selected config:"
	@$(ECHO) "  make"
	@$(ECHO) ""
	@$(ECHO) "Select config and build it at the same time:"
	@$(ECHO) "  make <config name>"
	@$(ECHO) ""
	@$(ECHO) "To list all configs:"
	@$(ECHO) "  make list"
	@$(ECHO) ""
	@$(ECHO) "To build all configs:"
	@$(ECHO) "  make everything"
	@$(ECHO) ""

# Install target
install: SOCSETTINGSPATH := $(PREFIX)/boot/soc-settings/l8540/
install:
	@$(ECHO) SOCSETTINGSPATH: $(SOCSETTINGSPATH)
	@$(if $(wildcard $(SOCSETTINGSPATH)),,mkdir -p $(SOCSETTINGSPATH))
	$(MYCP) -f *.bin $(SOCSETTINGSPATH)
