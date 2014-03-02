#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
# Author: WenHai Fang <wenhai.h.fang AT stericsson.com>,
#	  Jonas Aaberg <jonas.aberg@stericsson.com>
#   for ST-Ericsson
#
# Origin: From the file u8500/Makefile
#

# NOTE this path is android specific and points outside this directory.
# Android init command source build/envsetup.sh is needed for ANDROID_PRODUCT_OUT.

PREFIX ?= .
INSTALL_DIR := $(PREFIX)/boot/meminits/u8500_multi
BUILD_DIR := $(INSTALL_DIR)/build

SRCS :=	src/version.c \
	src/avs_calc.c \
	src/avs_calc-8420.c \
	src/avs_calc-8520.c \
	src/avs_calc-8500_boost.c \
	src/avs_calc-8500.c \
	src/memtest.c \
	src/memtest_pd.c \
	src/soc_settings_exec.c \
	src/await_battery_voltage.c \
	src/uart.c \
	bass_app/src/apply_soc_settings.c \
	bass_app/src/security.c \
	bass_app/src/tee_client_api.c

SRCS_CONFIG :=	configs/href520_avs.c \
		configs/href520.c \
		configs/href520_u8420_avs.c \
		configs/href520_u8420.c \
		configs/hrefp_8500boost_avs.c \
		configs/hrefp_8500boost.c \
		configs/hrefp_9500_1000mhz_avs_enable.c \
		configs/hrefp_avs_enable.c \
		configs/hrefp_v20_1000mhz.c \
		configs/hrefp_v20_800mhz.c \
		configs/hrefp_v20_v50_1000mhz.c \
		configs/hrefp_v20_v50_800mhz.c

# We always want to cross compile to arm. Override CROSS_COMPILE otherwise
CROSS_COMPILE ?= arm-eabi-

# Tools used
AS	:= $(CROSS_COMPILE)as
LD 	:= $(if $(shell bash -c "type $(CROSS_COMPILE)ld.bfd 2>/dev/null"), \
		$(CROSS_COMPILE)ld.bfd, $(CROSS_COMPILE)ld)
CC	:= $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
CP	:= cp
RM	:= rm
LN	:= ln
MKDIR	:= mkdir

# Use 'make V=1 target' to print true tool cmdline
ifneq ($(V),1)
	#None verbose output. Hide most of the output
	MYCC = @echo "cc\t$<" && $(CC)
	MYLD = @echo "ld\t$@" && $(LD)
	MYOBJCOPY = @echo "objcopy\t$@" && $(OBJCOPY)
	MYMKDIR = @$(MKDIR)
else
	MYCC = $(CC)
	MYLD = $(LD)
	MYOBJCOPY = $(OBJCOPY)
	MYMKDIR = $(MKDIR)
endif

# Common flags
INC := -Iinclude -Ibass_app/include

CFLAGS += -std=gnu99 -Wall -Wextra -Werror -pipe
CFLAGS += -fomit-frame-pointer -fno-stack-protector -fno-short-enums
CFLAGS += -fpie -ffreestanding
CFLAGS += -nostdlib
CFLAGS += -marm -march=armv7-a -mtune=cortex-a9
CFLAGS += -mabi=aapcs-linux -mno-thumb-interwork

LDFLAGS += -nostdlib -Bstatic

# Extra flags for linker
#  -ffunction-sections -fdata-sections will section the functions and data
#   during compilation. Needed for the linker to be able to place
#   symbols correctly
CFLAGS += -ffunction-sections -fdata-sections


# For bass_app
DEFINES := -DOS_FREE -DBOOTLOGBASEADDR=0x90000000

OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)
OBJS_CONFIG := $(SRCS_CONFIG:%.c=$(BUILD_DIR)/%.o)
OBJS_ELF := $(OBJS_CONFIG:%.o=%.elf)
OBJS_BIN := $(OBJS_ELF:%.elf=%.bin)
SRCS_RAW :=$(SRCS_CONFIG:configs/%.c=%)

# Dependency
DEPFLAGS = -Wp,-MP,-MD,$(BUILD_DIR)/$*.d,-MT$*.o

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
	CFLAGS += -g
	DEFINES += -DDEBUG
	LDFLAGS += -g
endif

# RELEASE compile
ifeq ($(BUILD),release)
	CFLAGS += -O2
# --gc-sections will remove the sections not used during linking
	LDFLAGS += --gc-sections
endif

TOOLCHAIN_DIRECTORY := $(shell dirname $$(dirname $$(which $(OBJCOPY))))
LIBGCC_A := $(wildcard $(TOOLCHAIN_DIRECTORY)/lib/gcc/$(CROSS_COMPILE:%-=%)/*/libgcc.a)

# make targets
##############

# This is the first target, it will be execute if no target is given to 'make'.
.PHONY: all

all: $(OBJS_BIN)

%.bin: %.elf
	$(MYOBJCOPY) -j .socsettings -O binary $< $@

%.elf: %.o $(OBJS)
	$(MYMKDIR) -p $(dir $@)
	$(MYLD) $(LDFLAGS) -T soc_settings_multi.ld -o $@ -Map=$@.map $< $(OBJS) $(LIBGCC_A)

${BUILD_DIR}/%.o: %.c
	$(MYMKDIR) -p $(dir $@)
	$(MYCC) $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) -c  $< -o $@

# If we want to see the ASM output. Run 'make file.s' to get the
# ASM output of file.c
${BUILD_DIR}/%.s: %.c Makefile
	$(MYMKDIR) -p $(dir $@)
	$(MYCC) $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) -S -o $@ $<

# If we want to see the preprocessor output. Run 'make file.e' to get
# the preprocessor output of file.c
$(BUILD_DIR)/%.e: %.c Makefile
	$(MYCC) $(DEPFLAGS) $(INC) $(CFLAGS) $(DEFINES) -E -o $@ $<

.SECONDARY:

.PHONY: postbuild
postbuild: all

# Clean targets
# =============

.PHONY: clean distclean
clean:
	$(RM) -rf $(BUILD_DIR)/
distclean: clean


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
	@echo "Build variables:"
	@echo "* BUILD=<release|debug>"
	@echo "    Will select release or debug build, default is 'release'"
	@echo "* V=1"
	@echo "    Will show verbose details of the build output."
	@echo ""

# Install target
install: all
	$(MYMKDIR) -p $(INSTALL_DIR)
	 @for c in $(SRCS_RAW); do \
		echo $(CP) $(BUILD_DIR)/configs/$$c.bin $(INSTALL_DIR)/soc_settings_$$c.bin ; \
		$(CP) $(BUILD_DIR)/configs/$$c.bin $(INSTALL_DIR)/soc_settings_$$c.bin ; \
	 done
