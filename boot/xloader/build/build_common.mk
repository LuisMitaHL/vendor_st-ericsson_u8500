
SHELL= /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o .S .s


ifdef CROSS_COMPILE
GCC     = $(CROSS_COMPILE)gcc
# Don't use gold linker, no support for MEMORY regions in scripts
LD      = $(CROSS_COMPILE)ld.bfd
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
else
# User should make link to prebuilt/linux-x86/toolchain in a checked
# out repo project
TOOLCHAIN_DIR = $(BUILD_ROOT)/../toolchain
# We specify compiler version here to be able to keep track of which
# compiler is used
TOOLCHAIN_VERSION = arm-eabi-4.4.0
TOOLCHAIN = $(TOOLCHAIN_DIR)/$(TOOLCHAIN_VERSION)
GCC     = $(TOOLCHAIN)/bin/arm-eabi-gcc
LD      = $(TOOLCHAIN)/bin/arm-eabi-ld
NM      = $(TOOLCHAIN)/bin/arm-eabi-nm
OBJCOPY = $(TOOLCHAIN)/bin/arm-eabi-objcopy
OBJDUMP = $(TOOLCHAIN)/bin/arm-eabi-objdump
endif


SIGNROOT    = $(BUILD_ROOT)/../signtool
export SIGNROOT
export BUILD_ROOT

ASMTOOL     = ${SIGNROOT}/assemble_tool_cli/assemble-tool.sh
ASMTOOL_ARGS = -f -v --type ram_image \
                --configuration $(SIGNROOT)/config/bootimage.xml

# Enable this flag to use local signing.
#BUILD_USING_LOCAL_SIGNING = 1

SIGNTOOL=${SIGNROOT}/sign-tool-cli/sign-tool-u5500.sh
ifndef BUILD_USING_LOCAL_SIGNING
# Sign packages at
SIGNPACKAGES = //10.211.44.161/bmtpspc0
export SIGNSERVER=10.211.44.70:4567
export SIGNPACKAGEROOT=/tmp/signpackageroot_${USER}/
export SIGNTOOL_ARGS = -v -p U8500 -s xloader
else
export LOCAL_SIGNPACKAGEROOT=$(BUILD_ROOT)/../keys/localroot/packages
export LOCAL_KEYROOT=$(BUILD_ROOT)/../keys/localroot/keys
export SIGNTOOL_ARGS = -v -local -p U8500 -s xloader
endif

ifndef PRIVATE_OUT_DIR
PRIVATE_OUT_DIR=.
endif

OBJ_OUTPUT_BASE=$(PRIVATE_OUT_DIR)/obj/xloader/$(STE_PLATFORM)
OBJ_OUTPUT_DIR=$(OBJ_OUTPUT_BASE)/$(THIS_DIR)

# Create obj directory
ifneq ($$(MAKECMDGOALS),clean)
$(shell [ -d ${OBJ_OUTPUT_DIR} ] || mkdir -p ${OBJ_OUTPUT_DIR})
endif

ifdef XLOADER_PERFORM_AVS_CALCS
CFLAGS+=-DXLOADER_PERFORM_AVS_CALCS
endif

ifdef XLOADER_OLD_FORMAT
CFLAGS+=-DXLOADER_OLD_FORMAT
ifeq ($(PAYLOAD_TYPE), xloader)
#REMOVE THIS when changing xloader to new sign format
SIGNTOOL    = ${SHELL} ./sign_${BINARY}.sh
export SIGNTOOL_ARGS = $(CHIP)
endif
endif

ifdef ENABLE_SYSTEM_RESET
CFLAGS+=-DENABLE_SYSTEM_RESET
endif

ifdef WARNS
ifndef NOWERROR
CFLAGS_WARNS+= -Werror
endif
CFLAGS_WARNS_1+= -Wall
CFLAGS_WARNS_2+= $(CFLAGS_WARNS_1)
#CFLAGS_WARNS_2+= -Wstrict-prototypes -Wold-style-definition
#CFLAGS_WARNS_2+= -Wmissing-prototypes -Wmissing-declarations -Wpointer-arith
#CFLAGS_WARNS_2+= -Wcast-qual -Wwrite-strings -Wcast-align -Wshadow
CFLAGS_WARNS_2+= $(CFLAGS_HIGH) $(CFLAGS_MEDIUM) $(CFLAGS_LOW)
CFLAGS_WARNS_3+= $(CFLAGS_WARNS_2) 

# From swadvice #675 (removed -Werror as it's handled differently here)
CFLAGS_HIGH = -Wall -Wbad-function-cast -Wcast-align \
              -Werror-implicit-function-declaration -Wextra -Wfloat-equal \
              -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self \
              -Wmissing-declarations -Wmissing-format-attribute \
              -Wmissing-include-dirs -Wmissing-noreturn \
              -Wmissing-prototypes -Wnested-externs -Wpointer-arith \
              -Wshadow -Wstrict-prototypes -Wswitch-default \
              -Wunreachable-code -Wunsafe-loop-optimizations \
              -Wwrite-strings 

# From swadvice #675 (removed -Wswitch-enum it's harmful /EJENWIK )
# Removed -Wredundant-decls consider putting it back when find_service_addr
# conflict in isswapi.h and sla.h is resolved
CFLAGS_MEDIUM = -Waggregate-return -Wlarger-than-65500 \
                -fno-common

# From swadvice #675 (removed -Wpacked)
# removed -std=c99 for nomadik_registers.h
#             -pedantic 
CFLAGS_LOW = -Winline -Wno-missing-field-initializers -Wno-unused-parameter \
             -Wold-style-definition -Wstrict-aliasing=2 -Wundef \
             -std=c99


CFLAGS_WARNS+= $(CFLAGS_WARNS_$(WARNS))

endif #ifdef WARNS

ifdef DEBUG
CFLAGS+= -g
CPPFLAGS+= -DBOOT_DEBUG=$(DEBUG)
CFLAGS+= -O0
else
CFLAGS+= -Os
endif

ifeq ($(ACCEPT_UNSIGNED_BINARIES),1)
CPPFLAGS += -DACCEPT_UNSIGNED_BINARIES
endif

CFLAGS+= -pipe

CPUARCH = a9
CFLAGS  += -mcpu=cortex-$(CPUARCH)
SFLAGS  += -mcpu=cortex-$(CPUARCH)

CFLAGS += -gdwarf-2 -mlong-calls -mthumb-interwork
CFLAGS += -mthumb
CFLAGS += -fno-builtin-memmove
CFLAGS += -mfloat-abi=soft -mfpu=vfpv3

ifdef CHIP
CPPFLAGS += -D_$(CHIP)
endif

ifdef WAIT_DEBUG
CPPFLAGS += -DWAIT_DEBUG
endif

#---------------------------------------------------------
# Find the parent directory of this file.
# Requires make version 3.80 or later.

# Get the pathname of this file
this := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
# Extract the directory part of that path.
ifeq (/,$(findstring /,$(this)))
# Remove the last slash and everything after it.
parentDir := $(shell x=$(this) && echo $${x%/*})
else
parentDir := .
endif
#--------------------------------------------------------- 
BUILD_ROOT := $(parentDir)
BOOT_ROOT = $(BUILD_ROOT)/../boot_images
DELIVERY_PATH = $(BUILD_ROOT)/../delivery/$(CHIP)
HCL_PATH = $(BUILD_ROOT)/../delivery/hcl
ISSW_COMMON = $(BOOT_ROOT)/issw/common
XLOADER_COMMON = $(BOOT_ROOT)/xloader/common
TA_ROOT = $(BUILD_ROOT)/../ta
COPS_ROOT = $(BUILD_ROOT)/../cops

# TODO add check for $(CHIP)

.PHONY: $(VERSION_FILE) $(DATE_FILE) all

all:


ifeq ($(ENABLE_C99),1)
VPATH += $(BUILD_ROOT)/../delivery/c99
CPPFLAGS += -I$(BUILD_ROOT)/../delivery/c99
SRCS += c99.c
endif

ifdef BINARY

ifndef LINK_SCRIPT
    $(error LINK_SCRIPT not defined)
endif

BIN_ELF_NAME=$(OBJ_OUTPUT_DIR)/$(BINARY).axf

BIN_NAME=$(OBJ_OUTPUT_DIR)/$(BINARY).bin

SSW_NAME=$(BINARY).ssw

DMP_NAME=$(BINARY).dmp

VERSION_FILE=gitversion.c
DATE_FILE = build_date.c

all: $(BIN_NAME)

all: $(DMP_NAME)

$(DATE_FILE):
	@date +'const char __attribute__((used)) build_date[]="%a %b %d %X %Y";' > $@

$(VERSION_FILE):
	${SHELL} $(BUILD_ROOT)/getgitversion.sh > $@

SRCS += $(VERSION_FILE) $(DATE_FILE)

ifdef SRCS
C_SRCS      := $(filter %.c,$(SRCS)) $(filter %.c,$(BOOT_SRCS))
S_SRCS      := $(filter %.s,$(SRCS)) $(filter %.S,$(SRCS)) \
                $(filter %.s,$(BOOT_SRCS)) $(filter %.S,$(BOOT_SRCS))
endif #SRCS

ifdef C_SRCS

define C_SRCS_template
CFLAGS_$1       += $$(CFLAGS) $$(CFLAGS_WARNS)
CPPFLAGS_$1     += $$(CPPFLAGS)
OBJS += $$(basename $1).o
DEPS += $$(basename $1).d
$(basename $1).o: $1 $(BUILD_ROOT)/config.mk $(BUILD_ROOT)/build_common.mk $(BUILD_ROOT)/xloader_common.mk
ifdef BUILD_VERBOSE
	$$(GCC) $$(filter-out $$(CFLAGS_REMOVE_$1), $$(CFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $(OBJ_OUTPUT_DIR)/$$@
else
	@echo Compiling $$<
	@$$(GCC) $$(filter-out $$(CFLAGS_REMOVE_$1), $$(CFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $(OBJ_OUTPUT_DIR)/$$@
endif
	@if test x$$(findstring $$(notdir $$<),$$(BOOT_SRCS)) != x ; then \
            echo Renaming sections for $$(notdir $$<) ;\
            (set -e; mv $$@ $$@.$$$$ ; \
	    $$(OBJCOPY)  $$(RENAME_SECTIONS) $$@.$$$$ $$@ ); rm -f $$@.$$$$ ; \
	fi

$(basename $1).d: $1
	@echo Creating/updating $$@
	@(set -e; rm -f $$@; \
	$$(GCC) -M $$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
                $$< > $(OBJ_OUTPUT_DIR)/$$@.$$$$; \
	sed 's,\($$*\)\.o[ :]*,\1.o $$@ : ,g' < $(OBJ_OUTPUT_DIR)/$$@.$$$$ > $(OBJ_OUTPUT_DIR)/$$@);\
	rm -f $(OBJ_OUTPUT_DIR)/$$@.$$$$

ifneq ($$(MAKECMDGOALS),clean)
-include $(basename $1).d
endif

endef

$(foreach f, $(C_SRCS), $(eval $(call C_SRCS_template,$(f))))

endif #C_SRCS

ifdef S_SRCS

define S_SRCS_template
SFLAGS_$1       += $$(SFLAGS)
CPPFLAGS_$1     += $$(CPPFLAGS)
OBJS += $$(basename $1).o
DEPS += $$(basename $1).d
$(basename $1).o: $1  $(BUILD_ROOT)/config.mk
ifdef BUILD_VERBOSE
	$$(GCC) $$(filter-out $$(SFLAGS_REMOVE_$1), $$(SFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $(OBJ_OUTPUT_DIR)/$$@
else
	@echo Assembling $<
	@$$(GCC) $$(filter-out $$(SFLAGS_REMOVE_$1), $$(SFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $(OBJ_OUTPUT_DIR)/$$@
endif
	@if test x$$(findstring $$(notdir $$<),$$(BOOT_SRCS)) != x ; then \
            echo Renaming sections for $$(notdir $$<) ;\
            (set -e; mv $$@ $$@.$$$$ ; \
	    $$(OBJCOPY)  $$(RENAME_SECTIONS) $$@.$$$$ $$@ ); rm -f $$@.$$$$ ; \
	fi

$(basename $1).d: $1
	@echo Creating/updating $$@
	@(set -e; rm -f $$@; \
	$$(GCC) -M $$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
                $$< > $(OBJ_OUTPUT_DIR)/$$@.$$$$; \
	sed 's,\($$*\)\.o[ :]*,\1.o $$@ : ,g' < $(OBJ_OUTPUT_DIR)/$$@.$$$$ > $(OBJ_OUTPUT_DIR)/$$@);\
	rm -f $(OBJ_OUTPUT_DIR)/$$@.$$$$

ifneq ($$(MAKECMDGOALS),clean)
-include $(basename $1).d
endif
endef

$(foreach f, $(S_SRCS), $(eval $(call S_SRCS_template,$(f))))

endif #S_SRCS


CLEANFILES += $(BIN_ELF_NAME) $(BIN_NAME) $(SSW_NAME) $(BINARY).map $(OBJS) \
                $(BOOT_OBJS) $(DEPS) $(DMP_NAME) $(VERSION_FILE)

RENAME_SECTIONS += --rename-section .text=.text_boot 
RENAME_SECTIONS += --rename-section .rodata=.rodata_boot
RENAME_SECTIONS += --rename-section .rodata.str1.1=.rodata_boot
RENAME_SECTIONS += --rename-section .data=.data_boot
RENAME_SECTIONS += --rename-section .bss=.bss_boot

$(BIN_ELF_NAME): $(OBJS) $(VERSION_FILE)
ifdef BUILD_VERBOSE
	@echo Linking $@
	$(GCC)  -nostdlib -Wl,--print-gc-sections,-T,$(LINK_SCRIPT),-o,$@,-Map=$(BINARY).map -Xlinker $(addprefix $(OBJ_OUTPUT_DIR)/,$(OBJS)) -Wl,-lgcc
else
	@echo Linking $@
	$(GCC)  -nostdlib -Wl,--print-gc-sections,-T,$(LINK_SCRIPT),--verbose,-o,$@,-Map=$(BINARY).map -Xlinker $(addprefix $(OBJ_OUTPUT_DIR)/,$(OBJS)) -Wl,-lgcc
endif
ifdef DISPLAY_SYMBOL_PREFIX
	@echo Sympols
	@$(NM) $@ | grep "$(DISPLAY_SYMBOL_PREFIX)"
endif

$(BIN_NAME): $(BIN_ELF_NAME)
ifdef BUILD_VERBOSE
	@echo Dumping binary $@
	$(OBJCOPY) $(OBJCOPY_FLAGS) -O binary $< $@
	rm -f $(DATE_FILE) $(VERSION_FILE)
else
	@echo Dumping binary $@
	@$(OBJCOPY) $(OBJCOPY_FLAGS) -O binary $< $@
	rm -f $(DATE_FILE) $(VERSION_FILE)
endif

$(DMP_NAME): $(BIN_ELF_NAME)
	$(OBJDUMP) -l -x -d $< > $@


CFLOW_FLAGS+= --symbol __attribute__:wrapper
CFLOW_FLAGS+= --symbol section:wrapper
CFLOW_FLAGS+= --format=posix --omit-arguments
CFLOW_FLAGS+= --level-indent='0=\t' --level-indent='1=\t' \
              --level-indent=start='\t'

CLEANFILES += ${BINARY}.cflow
$(BINARY).cflow: $(filter %.c, $(SRCS))
	cflow --cpp -o$@ $(CFLOW_FLAGS) $(CPPFLAGS) $^

CLEANFILES += $(BINARY).gc.dot
$(BINARY).gc.dot: $(BINARY).cflow
	cflow2dot < $< | sed 's,../../boot_images/,,' > $@

CLEANFILES += $(BINARY).gc.vcg
$(BINARY).gc.vcg: $(BINARY).cflow
	cflow2vcg < $< > $@

CLEANFILES += $(BINARY).gc.pdf
$(BINARY).gc.pdf: $(BINARY).gc.dot
	dot -Gratio=compress -Gsize="8.5,11" -Grankdir=LR -Tpdf $^ -o $@

endif #BINARY

ifdef remove_stuff
%.d: %.c
	@echo Creating/updating $@
	@(set -e; rm -f $@; \
	$(GCC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@);\
	rm -f $@.$$$$

%.d: %.S
	@echo Creating/updating $@
	@(set -e; rm -f $@; \
	$(GCC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@);\
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif
endif

ifdef DIRS

all: recursive_all

.PHONY: recursive_all
recursive_all:
	@set -e; \
        for d in $(DIRS); do \
		$(MAKE) -C $$d THIS_DIR=$$d all; \
	done

clean: recursive_clean

.PHONY: recursive_clean
recursive_clean:
	@set -e; \
	for d in $(DIRS); do \
		$(MAKE) -C $$d THIS_DIR=$$d clean; \
	done

endif #DIRS


ifdef BOOT_IMAGES
CLEANFILES += $(addsuffix _ste_boot_image.bin,$(BOOT_IMAGES))
CLEANFILES += $(addsuffix _boot_image_filelist.txt,$(BOOT_IMAGES))

.PHONY: boot_images
boot_images:
	@set -e; \
	for b in $(BOOT_IMAGES); do \
	    echo Assembling $$b ; \
            echo "issw=$${b}_ste_issw/issw.ssw" > \
                $${b}_bootimage_filelist.txt ; \
	    echo "xloader=$${b}_ste_xloader/xloader.ssw" >> \
                $${b}_bootimage_filelist.txt ; \
            sh $(ASMTOOL) $(ASMTOOL_ARGS) \
                --file_list $${b}_bootimage_filelist.txt \
                $${b}_ste_boot_image.bin ; \
	    rm -f $${b}_bootimage_filelist.txt ; \
	done

endif #BOOT_IMAGES

.PHONY: clean
clean:
ifdef CLEANFILES
	rm -f $(addprefix $(OBJ_OUTPUT_DIR)/,$(CLEANFILES))
endif #CLEANFILES

.PHONY: mount_signroot
mount_signroot:
	${BUILD_ROOT}/setup_signserver.sh

.PHONY: umount_signroot
umount_signroot:
	${BUILD_ROOT}/setup_signserver.sh -u

.PHONY: install
install: XLOADERBINPATH := $(PRIVATE_OUT_DIR)/boot/bootimages
install:
	@set -e; \
	for b in $(BOOT_IMAGES); do \
	    echo "Installing $$b" ; \
	    echo "Copy to $(XLOADERBINPATH)/$${b}_xloader.bin" ; \
	    $(if $(wildcard $(XLOADERBINPATH)),,mkdir -p $(XLOADERBINPATH);) \
	    cp -f $(OBJ_OUTPUT_BASE)/$${b}_xloader/$${b}_xloader.bin $(XLOADERBINPATH)/$${b}_xloader.bin ; \
	done

.PHONY: sign
sign:
	@set -e; \
	for b in $(BOOT_IMAGES); do \
	   echo "Signing $$b"; \
	   echo $(SIGNTOOL) $(SIGNTOOL_ARGS) $(OBJ_OUTPUT_DIR)/$${b}_xloader/xloader.bin \
		$(OBJ_OUTPUT_DIR)/$${b}_xloader/xloader.ssw; \
		$(SIGNTOOL) $(SIGNTOOL_ARGS) $(OBJ_OUTPUT_DIR)/$${b}_xloader/xloader.bin \
		$${b}_ste_xloader/xloader.ssw; \
	done

.PHONY: distclean config

config:

distclean: clean
