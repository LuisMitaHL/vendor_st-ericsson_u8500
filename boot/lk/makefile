ifeq ($(MAKECMDGOALS),spotless)
spotless:
	rm -rf build-*
else

-include local.mk
include make/macros.mk

# If one of our goals (from the commandline) happens to have a
# matching project/goal.mk, then we should re-invoke make with
# that project name specified...

project-name := $(firstword $(MAKECMDGOALS))

ifneq ($(project-name),)
ifneq ($(wildcard project/$(project-name).mk),)
do-nothing := 1
$(MAKECMDGOALS) _all: make-make
make-make:
	@PROJECT=$(project-name) $(MAKE) $(filter-out $(project-name), $(MAKECMDGOALS))
endif
endif

ifeq ($(do-nothing),)

ifeq ($(PROJECT),)
$(error No project specified.  Use "make projectname" or put "PROJECT := projectname" in local.mk)
endif

DEBUG ?= 0

ifndef $(BOOTLOADER_OUT)
BOOTLOADER_OUT := .
endif

LK_TOP_DIR:= .
BUILDDIR := $(BOOTLOADER_OUT)
OUTBIN := $(BUILDDIR)/lk.bin
OUTELF := $(BUILDDIR)/lk
CONFIGHEADER := $(BUILDDIR)/config.h

LKVERSION=\"$(shell git describe --tags --always)\"
ifneq ($(BASSAPP_DIR),)
	BASSAPVERSION=\"$(shell git --git-dir $(BASSAPP_DIR)/.git describe --tags --always)\"
endif

#Initialize the command-line flag ENABLE_TRUSTZONE. Value for flag passed in at command-line will take precedence
ENABLE_TRUSTZONE := 0

ifeq ($(ENABLE_TRUSTZONE),1)
	INPUT_TZ_BIN := tzbsp/tzbsp.bin
	OUTPUT_TZ_BIN := $(BUILDDIR)/tzbsp_bin.o
endif

INCLUDES := -I$(BUILDDIR) -Iinclude
CFLAGS := -O2 -g -fno-short-enums -fno-builtin -finline -W -Wall -Wno-multichar -Wno-unused-parameter -Wno-unused-function -include $(CONFIGHEADER) \
          -DLKVERSION=$(LKVERSION) -DBASSAPVERSION=$(BASSAPVERSION)
#CFLAGS += -Werror
ifeq ($(EMMC_BOOT),1)
  CFLAGS += -D_EMMC_BOOT=1
endif

ifeq ($(SIGNED_KERNEL),1)
  CFLAGS += -D_SIGNED_KERNEL=1
endif

# When the host arch is ARM, ensure stack protection code is not emitted since
# it's not supported by the bootloader's libc
ifneq ($(shell uname -m | grep "arm.*"),)
  CFLAGS += -fno-stack-protector
endif
CPPFLAGS := -fno-exceptions -fno-rtti -fno-threadsafe-statics
#CPPFLAGS += -Weffc++
ASMFLAGS := -DASSEMBLY
LDFLAGS :=

CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -gc-sections

ifneq ($(BUILD_ENV),)
MKENV = mkenv
endif

# top level rule
all:: $(OUTBIN) $(OUTELF).lst $(OUTELF).debug.lst $(OUTELF).sym $(OUTELF).size APPSBOOTHEADER $(MKENV)
ifeq ($(ENABLE_FEATURE_BUILD_HBTS),true)
	@echo ""
	@echo "#########################################################################"
	@echo "The switchs compilation are :"
ifeq ($(ESRAM),1)
	@echo "MEMTARGET_ESRAM_HDW"
endif
	@cat  `find $(ANDROID_BUILD_TOP) \( ! -name \*~ \)|grep hbts_hard_config.opt`
	@echo
	@echo "MEMBASE=$(MEMBASE)"
	@echo
	@(maj_ver=`sed -e '/MAJOR_VERSION=\([0-9]\)\+/!d;s/MAJOR_VERSION=\([0-9]\)\+/\1/g' hard_config.sh`; \
	echo "Current HBTS major version is: $$maj_ver "; \
	if [ -f "./toolsbox/toolsbox.h" ]; then \
		min_ver=`sed -e '/^[ \t]*#[ \t]*define[ \t]\+MINOR_VERSION[ \t]\+\([0-9]\)\+/!d;s/^[ \t]*#[ \t]*define[ \t]\+MINOR_VERSION[ \t]\+\([0-9]\+\)\+/\1/g'  ./toolsbox/toolsbox.h`; \
		hbts_revision=`sed -e '/^[ \t]*#[ \t]*define[ \t]\+HBTS_REVISION[ \t]\+\([0-9]\)\+/!d;s/^[ \t]*#[ \t]*define[ \t]\+HBTS_REVISION[ \t]\+\([0-9]\)\+/\1/g'  ./toolsbox/toolsbox.h`; \
		echo "Current HBTS minor version is:" $$min_ver ; \
		if ! [ -z $$hbts_revision ]; then \
			echo "Current HBTS revision is:" $$hbts_revision ; \
		fi; \
		if [ -z $$hbts_revision ]; then \
			echo "Current HBTS version is:" $$maj_ver"."$$min_ver; \
		else \
			echo "Current HBTS version is:" $$maj_ver"."$$min_ver.$$hbts_revision; \
		fi; \
		echo "Update ../toolsbox/toolsbox.h to be in line with next baseline label !" ; \
		if [ ! -f "$(ANDROID_BUILD_TOP)/vendor/st-ericsson/validation/hardware/hbts/self_tests/rules.mk" ]; then \
			echo "The self tests have not been compiled"; \
		fi \
	else \
		echo "None minor version defined for HBTS" ; \
		echo "Current HBTS version is:" \
		$$maj_ver".xx"; \
		echo "The bring up services and self tests have not been compiled"; \
	fi )
	@if [ "$${ESRAM}" = "1" ] ; then \
		mv $(OUTBIN) $(BUILDDIR)/hbts_lk_esram.bin; \
		cp $(OUTELF) $(BUILDDIR)/hbts_lk_esram; \
	else \
		mv $(OUTBIN) $(BUILDDIR)/hbts_lk_sdram.bin; \
		cp $(OUTELF) $(BUILDDIR)/hbts_lk_sdram; \
	fi
	@echo "#########################################################################"
endif

# the following three object lists are identical except for the ordering
# which is bootobjs, kobjs, objs
BOOTOBJS :=
OBJS :=

# a linker script needs to be declared in one of the project/target/platform files
LINKER_SCRIPT :=

# anything you add here will be deleted in make clean
GENERATED := $(CONFIGHEADER)

# anything added to DEFINES will be put into $(BUILDDIR)/config.h
DEFINES := LK=1

# Anything added to SRCDEPS will become a dependency of every source file in the system.
# Useful for header files that may be included by one or more source files.
SRCDEPS := $(CONFIGHEADER)

# these need to be filled out by the project/target/platform rules.mk files
TARGET :=
PLATFORM :=
ARCH :=
ALLMODULES :=
MODULES :=

# any rules you put here will also be built by the system before considered being complete
EXTRA_BUILDDEPS :=

# any rules you put here will be depended on in clean builds
EXTRA_CLEANDEPS :=

include project/$(PROJECT).mk
include target/$(TARGET)/rules.mk
include app/env/makefile
include target/$(TARGET)/tools/makefile
include platform/$(PLATFORM)/rules.mk
include arch/$(ARCH)/rules.mk
include platform/rules.mk
include target/rules.mk
include kernel/rules.mk
include dev/rules.mk
include app/rules.mk

ifeq ($(ENABLE_FEATURE_SELFTESTS_IN_LOADER),true)
ifneq (,$(wildcard toolsbox/rules.mk))
include toolsbox/rules.mk
ifneq (,$(wildcard self_tests/rules.mk))
include self_tests/rules.mk
endif
endif
endif

# recursively include any modules in the MODULE variable, leaving a trail of included
# modules in the ALLMODULES list
include make/module.mk

# any extra top level build dependencies that someone declared
all:: $(EXTRA_BUILDDEPS)

ALLOBJS := \
	$(BOOTOBJS) \
	$(OBJS)

# add some automatic configuration defines
DEFINES += \
	BOARD=$(PROJECT) \
	PROJECT_$(PROJECT)=1 \
	TARGET_$(TARGET)=1 \
	PLATFORM_$(PLATFORM)=1 \
	ARCH_$(ARCH)=1 \
	$(addsuffix =1,$(addprefix WITH_,$(ALLMODULES)))

# debug build?
ifneq ($(DEBUG),)
DEFINES += \
	DEBUG=$(DEBUG)
endif

ifneq ($(BUILD_ENV),)
DEFINES += \
	BUILD_ENV=$(BUILD_ENV)
endif

ALLOBJS := $(addprefix $(BUILDDIR)/,$(ALLOBJS))

DEPS := $(ALLOBJS:%o=%d)

# default to no ccache
CCACHE ?=
CC := $(CCACHE) $(TOOLCHAIN_PREFIX)gcc
LD := $(TOOLCHAIN_PREFIX)ld
OBJDUMP := $(TOOLCHAIN_PREFIX)objdump
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
CPPFILT := $(TOOLCHAIN_PREFIX)c++filt
SIZE := $(TOOLCHAIN_PREFIX)size
NM := $(TOOLCHAIN_PREFIX)nm

# comment out or override if you want to see the full output of each command
NOECHO ?= @

# the logic to compile and link stuff is in here
include make/build.mk

clean: $(EXTRA_CLEANDEPS)
	rm -f $(ALLOBJS) $(DEPS) $(GENERATED) $(OUTBIN) $(OUTELF) $(OUTELF).lst
ifeq ($(ENABLE_FEATURE_SELFTESTS_IN_LOADER),true)
	@rm -rf toolsbox
	@rm -rf self_tests
	@rm -rf $(BUILDDIR)/av8100_fw.h
	@rm -rf $(BUILDDIR)/splash.h
endif

install: all
	scp $(OUTBIN) 192.168.0.4:/tftproot

# generate a config.h file with all of the DEFINES laid out in #define format
configheader:

$(CONFIGHEADER): configheader
	@$(MKDIR)
	@echo generating $@
	@rm -f $(CONFIGHEADER).tmp; \
	echo \#ifndef __CONFIG_H > $(CONFIGHEADER).tmp; \
	echo \#define __CONFIG_H >> $(CONFIGHEADER).tmp; \
	for d in `echo $(DEFINES) | tr [:lower:] [:upper:]`; do \
		echo "#define $$d" | sed "s/=/\ /g;s/-/_/g;s/\//_/g" >> $(CONFIGHEADER).tmp; \
	done; \
	if [ "$${ENABLE_FEATURE_BUILD_HBTS}" = "true" ] ; then \
	echo \#define  ENABLE_FEATURE_BUILD_HBTS >> $(CONFIGHEADER).tmp; \
	fi; \
	if [ "$${ESRAM}" = "1" ] ; then \
	echo \#define MEMTARGET_ESRAM_HDW  >> $(CONFIGHEADER).tmp; \
	fi; \
	if [ "$${ENABLE_FEATURE_SELFTESTS_IN_LOADER}" = "true" ] ; then \
		echo \#define ENABLE_FEATURE_SELFTESTS_IN_LOADER  >> $(CONFIGHEADER).tmp; \
		./hard_config.sh $(CONFIGHEADER).tmp $(ANDROID_BUILD_TOP); \
	fi; \
	echo \#endif >> $(CONFIGHEADER).tmp; \
	if [ -f "$(CONFIGHEADER)" ]; then \
		if cmp "$(CONFIGHEADER).tmp" "$(CONFIGHEADER)"; then \
			rm -f $(CONFIGHEADER).tmp; \
		else \
			mv $(CONFIGHEADER).tmp $(CONFIGHEADER); \
		fi \
	else \
		mv $(CONFIGHEADER).tmp $(CONFIGHEADER); \
	fi
	@if [ "$${ENABLE_FEATURE_SELFTESTS_IN_LOADER}" = "true" ] ; then \
		if [ -f toolsbox/av8100_fw_v3.bin ] ; then \
			./toolsbox/gen_fw_header.py ./toolsbox/av8100_fw_v3.bin  >$(BUILDDIR)/av8100_fw.h; \
		fi; \
	fi

# Empty rule for the .d files. The above rules will build .d files as a side
# effect. Only works on gcc 3.x and above, however.
%.d:

ifeq ($(filter $(MAKECMDGOALS), clean), )
-include $(DEPS)
endif

.PHONY: configheader
endif

endif # make spotless
