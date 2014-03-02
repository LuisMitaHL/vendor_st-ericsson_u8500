SHELL= /bin/sh

.PHONY: all
all:

../../test_config.mk: ;
../test_config.mk: ;
test_config.mk: ;

.DEFAULT:
	@echo No rule to make \"$<\"
	@exit 1

.SUFFIXES:
.SUFFIXES: .c .o .S .s

ifeq "$(USER)" ""
USER := "amnesiac"
endif

# User should make link to prebuilt/linux-x86/toolchain in a checked
# out repo project
TOOLCHAIN_DIR     := $(abspath $(parentDir)/../toolchain)
# We specify compiler version here to be able to keep track of which
# compiler is used
TOOLCHAIN_VERSION := arm-eabi-4.4.0
TOOLCHAIN         := $(TOOLCHAIN_DIR)/$(TOOLCHAIN_VERSION)/bin
PATH              := $(TOOLCHAIN):$(PATH)
export PATH

CROSS_PREFIX      ?= arm-eabi
CROSS_COMPILE     ?= $(CROSS_PREFIX)-
export CROSS_COMPILE

CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CHMOD ?= chmod
SIGNTOOL ?= $(BUILD_ROOT)/../signtool/sign-tool-cli/sign-tool-u5500.sh

ifdef WARNS
ifndef NOWERROR
CFLAGS+= -Werror
endif
CFLAGS  += -fdiagnostics-show-option
CFLAGS_WARNS_1+= $(CFLAGS_HIGH)
CFLAGS_WARNS_2+= $(CFLAGS_WARNS_1) $(CFLAGS_MEDIUM)
CFLAGS_WARNS_3+= $(CFLAGS_WARNS_2) $(CFLAGS_LOW)

# From swadvice #675 (removed -Werror as it's handled differently here)
CFLAGS_HIGH = -Wall -Wcast-align \
              -Werror-implicit-function-declaration -Wextra -Wfloat-equal \
              -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self \
              -Wmissing-declarations -Wmissing-format-attribute \
              -Wmissing-include-dirs -Wmissing-noreturn \
              -Wmissing-prototypes -Wnested-externs -Wpointer-arith \
              -Wshadow -Wstrict-prototypes -Wswitch-default \
              -Wwrite-strings
# Remove -Wunreachable-code this warning will occur when some
# features are disabled
#CFLAGS_HIGH += -Wunreachable-code
# Remove -Wunsafe-loop-optimizations since we don't use
#CFLAGS_HIGH += -Wunsafe-loop-optimizations
# -funsafe-loop-optimizations
# Remove -Wbad-function-cast it just leads to silly workarounds
#CFLAGS_HIGH += -Wbad-function-cast

# From swadvice #675 (removed -Wswitch-enum it's harmful /EJENWIK )
# remove -Wlarger-than-65500
CFLAGS_MEDIUM = -Waggregate-return \
                -Wredundant-decls

# Replaced -std=c99 with -std=gnu99 since -std=c99 defines __STRICT_ANSI__
# while -std=gnu99 does not. /EJENWIK
CFLAGS_LOW = -Winline -Wno-missing-field-initializers -Wno-unused-parameter \
             -Wold-style-definition -Wstrict-aliasing=2 \
             -Wundef -pedantic -std=gnu99 -Wno-format-zero-length

#CFLAGS_LOW += -Wdeclaration-after-statement

# -Wpacked removed from default flags as it won't accept cops_sipc_message_t
ifndef NOWPACKED
CFLAGS_LOW += -Wpacked
endif

CFLAGS_WARNS+= $(CFLAGS_WARNS_$(WARNS))

endif #ifdef WARNS

ifdef DEBUG
CFLAGS+= -g
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

CFLAGS += -gdwarf-2 -mthumb-interwork -mlong-calls
CFLAGS += -mthumb
CFLAGS += -mno-apcs-float
CFLAGS += -fno-common
CFLAGS += -fno-short-enums

ifdef WAIT_DEBUG
CPPFLAGS += -DWAIT_DEBUG -DWAIT_DELAY=$(WAIT_DELAY)
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
ISSW_GENERIC = $(BOOT_ROOT)/issw/generic
TEE_SHARED = $(BOOT_ROOT)/tee_shared
KTA_API_DIR = $(BUILD_ROOT)/../delivery/kta
TEE_API_DIR = $(BUILD_ROOT)/../delivery/tee
TEE_API_LEGACY_DIR = $(BUILD_ROOT)/../delivery/tee_legacy
TA_ROOT = $(BUILD_ROOT)/../ta
USER_TA_ROOT = $(BUILD_ROOT)/../user_ta
EXT_USER_TA_ROOT = $(BUILD_ROOT)/../ext_user_ta
STDLIB_DIR = $(BUILD_ROOT)/../delivery/stdlib

ifeq "$(USER_SIGNKEYSDIR)" ""
USER_SIGNKEYSDIR := $(BUILD_ROOT)/../signkeys
endif
$(eval $(shell test -d ${USER_SIGNKEYSDIR} && \
                        echo SIGNKEYSDIR=${USER_SIGNKEYSDIR}))

ifeq "$(USER_SIGNPACKAGEDIR)" ""
USER_SIGNPACKAGEDIR := $(BUILD_ROOT)/../signpackages
endif
$(eval $(shell test -d $(USER_SIGNPACKAGEDIR) && \
                        echo SIGNPACKAGEDIR=${USER_SIGNPACKAGEDIR}))

ifeq "$(SIGNKEYSDIR)" ""
ifneq "$(SIGNPACKAGEDIR)" ""
$(error USER_SIGNPACKAGEDIR available but not USER_SIGNKEYSDIR, aborting)
endif

SIGNKEYSDIR=$(BUILD_ROOT)/../keys/localroot/keys
SIGNPACKAGEDIR=$(BUILD_ROOT)/../keys/localroot/packages
else
ifeq "$(SIGNPACKAGEDIR)" ""
$(error USER_SIGNKEYSDIR, available but not USER_SIGNPACKAGEDIR, aborting)
endif

endif

export LOCAL_KEYROOT=$(SIGNKEYSDIR)
export LOCAL_SIGNPACKAGEROOT=$(SIGNPACKAGEDIR)
SIGN_PACKAGE_DB_PATH ?= $(BUILD_ROOT)
SIGN_PACKAGE_DB ?= $(SIGN_PACKAGE_DB_PATH)/sign_package_db

# TODO add check for $(CHIP)

ifneq ($(USER_TA),1)
SRCS_VPATH += $(STDLIB_DIR)
SRCS_VPATH += $(BUILD_ROOT)/../delivery/issw
endif

ifeq ($(ENABLE_C99),1)
CPPFLAGS += -I$(STDLIB_DIR)
UNPG_SRCS += c99.c
UNPG_SRCS += malloc.c
endif

ifeq ($(ENABLE_MDBG),1)
CPPFLAGS += -DENABLE_MDBG
# Smart alloc depends on assert really doing an assert of the expression
# to be able to catch some errors
ENABLE_ASSERT=1
endif

ifeq ($(ENABLE_ASSERT),1)
UNPG_SRCS += assert.c
else
CPPFLAGS += -DNDEBUG
endif


ifeq ($(ENABLE_AEABI),1)
UNPG_SRCS += aeabi_divmod.c aeabi_divmod_asm.S
endif


.PHONY: binaries signed_binaries copy_result libraries

ifdef SRCS

vpath %.c $(SRCS_VPATH)
vpath %.s $(SRCS_VPATH)
vpath %.S $(SRCS_VPATH)
vpath %.x $(X_VPATH)

C_SRCS      := $(filter %.c,$(SRCS)) $(filter %.c,$(BOOT_SRCS)) \
                $(filter %.c,$(UNPG_SRCS))
S_SRCS      := $(filter %.s,$(SRCS)) $(filter %.S,$(SRCS)) \
                $(filter %.s,$(BOOT_SRCS)) $(filter %.S,$(BOOT_SRCS)) \
                $(filter %.s,$(UNPG_SRCS)) $(filter %.S,$(UNPG_SRCS))
endif #SRCS

ifdef C_SRCS

define C_SRCS_template
CFLAGS_REMOVE_$1 := $$(CFLAGS_REMOVE) $$(CFLAGS_REMOVE_$1)
CPPFLAGS_REMOVE_$1 := $$(CPPFLAGS_REMOVE) $$(CPPFLAGS_REMOVE_$1))
CFLAGS_$1       := $$(CFLAGS) $$(CFLAGS_WARNS) $$(CFLAGS_$1)
CPPFLAGS_$1     := $$(CPPFLAGS) $$(CPPFLAGS_$1)  -D__FILENAME__=\"$(notdir $1)\"
OBJS += $$(basename $1).o
DEPS += $$(basename $1).d*
$(basename $1).o: $(1)
ifdef BUILD_VERBOSE
	$$(CC) $$(filter-out $$(CFLAGS_REMOVE_$1), $$(CFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $$@
else
	@echo Compiling $$(notdir $$<)
	@$$(CC) $$(filter-out $$(CFLAGS_REMOVE_$1), $$(CFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $$@
endif
ifneq "$(RENAME_BOOT_SECTIONS)" ""
	@if test x$$(findstring $$(notdir $$<),$$(BOOT_SRCS)) != x ; then \
            echo Renaming boot sections for $$(notdir $$<) ;\
            (set -e; mv $$@ $$@.$$$$ ; \
	    $$(OBJCOPY)  $$(RENAME_BOOT_SECTIONS) $$@.$$$$ $$@ ); \
	    rm -f $$@.$$$$ ; \
	fi
endif
ifneq "$(RENAME_UNPG_SECTIONS)" ""
	@if test x$$(findstring $$(notdir $$<),$$(UNPG_SRCS)) != x ; then \
            echo Renaming unpaged sections for $$(notdir $$<) ;\
            (set -e; mv $$@ $$@.$$$$ ; \
	    $$(OBJCOPY)  $$(RENAME_UNPG_SECTIONS) $$@.$$$$ $$@ ); \
	    rm -f $$@.$$$$ ; \
	fi
endif

$(basename $1).d: $1
	@echo Creating/updating $$@
	@(set -e; rm -f $$@; \
	$$(CC) -M $$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
                $$< > $$@.$$$$; \
	sed 's,\($$*\)\.o[ :]*,\1.o $$@ : ,g' < $$@.$$$$ > $$@);\
	rm -f $$@.$$$$

ifneq ($$(MAKECMDGOALS),clean)
-include $(basename $1).d
endif

endef

$(foreach f, $(C_SRCS), $(eval $(call C_SRCS_template,$(f))))

endif #C_SRCS

ifdef S_SRCS

define S_SRCS_template
SFLAGS_REMOVE_$1 := $$(SFLAGS_REMOVE) $$(SFLAGS_REMOVE_$1)
CPPFLAGS_REMOVE_$1 := $$(CPPFLAGS_REMOVE) $$(CPPFLAGS_REMOVE_$1))
SFLAGS_$1       := $$(SFLAGS) $$(SFLAGS_$1)
CPPFLAGS_$1     := $$(CPPFLAGS) $$(CPPFLAGS_$1)
OBJS += $$(basename $1).o
DEPS += $$(basename $1).d*
$(basename $1).o: $1
ifdef BUILD_VERBOSE
	$$(CC) $$(filter-out $$(SFLAGS_REMOVE_$1), $$(SFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $$@
else
	@echo Assembling $$(notdir $$<)
	@$$(CC) $$(filter-out $$(SFLAGS_REMOVE_$1), $$(SFLAGS_$1)) \
		$$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
		-c $$< -o $$@
endif
ifneq "$(RENAME_BOOT_SECTIONS)" ""
	@if test x$$(findstring $$(notdir $$<),$$(BOOT_SRCS)) != x ; then \
            echo Renaming boot sections for $$(notdir $$<) ;\
            (set -e; mv $$@ $$@.$$$$ ; \
	    $$(OBJCOPY)  $$(RENAME_BOOT_SECTIONS) $$@.$$$$ $$@ ); \
	    rm -f $$@.$$$$ ; \
	fi
endif
ifneq "$(RENAME_UNPG_SECTIONS)" ""
	@if test x$$(findstring $$(notdir $$<),$$(UNPG_SRCS)) != x ; then \
            echo Renaming unpaged sections for $$(notdir $$<) ;\
            (set -e; mv $$@ $$@.$$$$ ; \
	    $$(OBJCOPY)  $$(RENAME_UNPG_SECTIONS) $$@.$$$$ $$@ ); \
	    rm -f $$@.$$$$ ; \
	fi
endif

$(basename $1).d: $1
	@echo Creating/updating $$@
	@(set -e; rm -f $$@; \
	$$(CC) -M $$(filter-out $$(CPPFLAGS_REMOVE_$1), $$(CPPFLAGS_$(1))) \
                $$< > $$@.$$$$; \
	sed 's,\($$*\)\.o[ :]*,\1.o $$@ : ,g' < $$@.$$$$ > $$@);\
	rm -f $$@.$$$$

ifneq ($$(MAKECMDGOALS),clean)
-include $(basename $1).d
endif
endef

$(foreach f, $(S_SRCS), $(eval $(call S_SRCS_template,$(f))))

endif #S_SRCS

ifdef LIBRARY
ifdef BINARY
    $(error BINARY and LIBRARY are mutually exclusive)
endif

LIB_NAME = lib$(LIBRARY).a

all: libraries

libraries: $(LIB_NAME)

CLEANFILES += $(LIB_NAME) $(OBJS) $(DEPS) $(SW_VERSION_FILE)

ARCHIVE_COMMAND = $(AR) rcs

$(LIB_NAME): $(OBJS)
ifdef BUILD_VERBOSE
	@echo $(ARCHIVE_COMMAND) $@ $(filter %.o,$^)
else
	@echo Creating library $@
endif
	@$(ARCHIVE_COMMAND) $@ $(filter %.o,$^)

endif #LIBRARY


ifdef BINARY
ifdef LIBRARY
    $(error BINARY and LIBRARY are mutually exclusive)
endif

ifndef LINK_SCRIPT
    $(error LINK_SCRIPT not defined)
endif

BIN_ELF_NAME=$(BINARY).axf

BIN_NAME=$(BINARY).bin

DMP_NAME=$(BINARY).dmp

binaries: $(BIN_NAME) $(DMP_NAME)

all: binaries


CLEANFILES += $(BIN_ELF_NAME) $(BIN_NAME) $(BINARY).map $(OBJS) \
                $(DEPS) $(DMP_NAME) \
                $(SIGN_PACKAGE_DB) $(SIGN_PACKAGE_DB).tmp \
                $(SIGN_PACKAGE_DB).lock

LINK_COMMAND = $(CC) -nostartfiles -nodefaultlibs -Wl,--print-gc-sections \
                $(LDFLAGS) -Wl,--warn-section-align -Wl,--fatal-warnings \
                -Wl,-T $(LINK_SCRIPT) -Wl,-Map=$(BINARY).map

ifdef SW_VERSION_FILE
SW_VERSION_FILE_O=$(patsubst %.c,%.o,$(SW_VERSION_FILE))
CLEANFILES += $(SW_VERSION_FILE) $(SW_VERSION_FILE_O) $(SW_VERSION_FILE_O).*
endif

$(BIN_ELF_NAME): $(OBJS) $(LINK_SCRIPT) $(DPADD)
ifdef SW_VERSION_FILE
	@${SHELL} $(BUILD_ROOT)/get_git_sw_version.sh $(BUILD_ROOT) > \
		$(SW_VERSION_FILE)
	@$(CC) $(filter-out $(CFLAGS_REMOVE), $(CFLAGS)) \
		-c $(SW_VERSION_FILE) -o $(SW_VERSION_FILE_O)
ifneq "$(RENAME_BOOT_SECTIONS)" ""
	@echo Renaming boot sections for $(notdir $(SW_VERSION_FILE)) ; \
	(set -e; mv $(SW_VERSION_FILE_O) $(SW_VERSION_FILE_O).$$$$ ; \
	$(OBJCOPY)  $(RENAME_BOOT_SECTIONS) $(SW_VERSION_FILE_O).$$$$ \
		$(SW_VERSION_FILE_O)) ; \
	rm -f $(SW_VERSION_FILE_O).$$$$
endif
endif #SW_VERSION_FILE
ifdef BUILD_VERBOSE
	@echo $(LINK_COMMAND) -o $@ $(filter %.o,$^) \
		$(SW_VERSION_FILE_O) $(LDADD)
else
	@echo Linking $@
endif
	@$(LINK_COMMAND) -o $@ $(filter %.o,$^) $(SW_VERSION_FILE_O) $(LDADD)
	@$(CHMOD) -x $@
ifdef DISPLAY_SYMBOL_PREFIX
	@echo Sympols
	@$(NM) $@ | grep "$(DISPLAY_SYMBOL_PREFIX)"
endif

$(BIN_NAME): $(BIN_ELF_NAME)
ifdef BUILD_VERBOSE
	$(OBJCOPY) $(OBJCOPY_FLAGS) -O binary $< $@
	$(CHMOD) -x $@
ifneq "$(FIX_BINARY)" ""
	$(FIX_BINARY) -v $< $@
endif
else
	@echo Dumping binary $@
	@$(OBJCOPY) $(OBJCOPY_FLAGS) -O binary $< $@
	@$(CHMOD) -x $@
ifneq "$(FIX_BINARY)" ""
	@echo Fixing binary $@
	@$(FIX_BINARY) -v $< $@
endif
endif #BINARY



space :=
space +=
SIGN_FNAME_SUFFIX := $(subst $(space),_,$(SIGN_FNAME_SUFFIX))
ifneq "$(SIGN_FNAME_SUFFIX)" ""
export SIGN_FNAME_SUFFIX := _$(SIGN_FNAME_SUFFIX)
endif

# $1 : Sign package alias
# $2 : Part of signed filename, which will be "$(BINARY)_$(2).ssw"
# $3 : Root key hash type
define SignSSW
CLEANFILES += $(BINARY)$(SIGN_FNAME_SUFFIX)_$(2).ssw
$(BINARY)$(SIGN_FNAME_SUFFIX)_$(2).ssw: $(BIN_NAME) $(SIGN_PACKAGE_DB)
ifdef BUILD_VERBOSE
	@if test "`grep -wc $(1) $(SIGN_PACKAGE_DB)`" = 1 ; then \
		echo Signing $$< as $$@ \
			with sign args \"$(PAYLOAD_TYPE)\", \"$(1)\" \
			$(if $(3), and root key hash type \"$(3)\");\
		$(SIGNTOOL) -v -local -p $(1) $(if $(3), --root-key-hash-type $(3)) \
					-s $(PAYLOAD_TYPE) $$< $$@ ; \
	else \
		echo Signpackage $(1) is not available. Available packages: `cat $(SIGN_PACKAGE_DB)` ;\
	fi
else
	@if test "`grep -wc $(1) $(SIGN_PACKAGE_DB)`" = 1 ; then \
		echo Signing $$< as $$@ \
			with sign args \"$(PAYLOAD_TYPE)\", \"$(1)\" \
			$(if $(3), and root key hash type \"$(3)\");\
		$(SIGNTOOL) -local -p $(1) $(if $(3), --root-key-hash-type $(3)) \
					-s $(PAYLOAD_TYPE) $$< $$@ ; \
	else \
		echo Signpackage $(1) is not available. ;\
	fi
endif

signed_binaries: $(BINARY)$(SIGN_FNAME_SUFFIX)_$(2).ssw
endef

# The SSWNAMES is an array of Alias, OutputFilename, and optional RootKeyHash
SSWARG1=$(word 1,$(subst :, ,$(n)))
SSWARG2=$(word 2,$(subst :, ,$(n)))
SSWARG3=$(if $(word 3,$(subst :, ,$(n))),$(word 3,$(subst :, ,$(n))))
$(foreach n, $(SSWNAMES),$(eval $(call SignSSW,$(SSWARG1),$(SSWARG2),$(SSWARG3))))

$(DMP_NAME): $(BIN_ELF_NAME)
ifdef BUILD_VERBOSE
	$(OBJDUMP) -l -x -d $< > $@
else
	@echo Dumping $@
	@$(OBJDUMP) -l -x -d $< > $@
endif

copy_result: binaries signed_binaries
ifdef DEST_DIR
	@echo Copy result to $(DEST_DIR)
	@sh ${BUILD_ROOT}/copy_result.sh
endif
all: copy_result

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

%.d: %.c
	@echo Creating/updating $@
	@(set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@);\
	rm -f $@.$$$$

%.d: %.S
	@echo Creating/updating $@
	@(set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@);\
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJS:.o=.d)
endif

ifdef DIRS1

all: check_sign_package_db recursive_all_dirs1

.PHONY: recursive_all_dirs1 $(DIRS1)
recursive_all_dirs1: $(DIRS1)

$(DIRS1):
	$(MAKE) -C $@ all

clean: recursive_clean_dirs1

.PHONY: recursive_clean_dirs1
recursive_clean_dirs1:
	@set -e; \
	for d in $(DIRS1); do \
		$(MAKE) -C $$d clean; \
	done

endif #DIRS1

ifdef DIRS2

all: check_sign_package_db recursive_all_dirs2

.PHONY: recursive_all_dirs2 $(DIRS2)
recursive_all_dirs2: $(DIRS2)

$(DIRS2): $(DIRS1)
	$(MAKE) -C $@ all

clean: recursive_clean_dirs2

.PHONY: recursive_clean_dirs2
recursive_clean_dirs2:
	@set -e; \
	for d in $(DIRS2); do \
		$(MAKE) -C $$d clean; \
	done

endif #DIRS2

ifdef DIRS3

all: check_sign_package_db recursive_all_dirs3

.PHONY: recursive_all_dirs3 $(DIRS3)
recursive_all_dirs3: $(DIRS3)

$(DIRS3): $(DIRS2)
	$(MAKE) -C $@ all

clean: recursive_clean_dirs3

.PHONY: recursive_clean_dirs3
recursive_clean_dirs3:
	@set -e; \
	for d in $(DIRS3); do \
		$(MAKE) -C $$d clean; \
	done

endif #DIRS3



.PHONY: clean
clean:
ifdef CLEANFILES
	rm -f $(CLEANFILES)
endif #CLEANFILES

# Get the sha1 sum of the sign package directory
DB_FINGERPRINT=`ls -lR $(SIGNPACKAGEDIR) | grep .pkg | sort | sha1sum | \
				sed 's/\([0-9a-fA-F]*\).*/\1/'`

# $1 : Set to 1 if DB file is to be deleted
define Check_Fingerprint_DB
	@echo Locking $(SIGN_PACKAGE_DB).lock
	@dotlockfile $(SIGN_PACKAGE_DB).lock
	@if [ "$(1)" = 1 ] ; then \
		rm -f $(SIGN_PACKAGE_DB); \
	fi
	@if [ "`grep -s "[0-9a-fA-F]\{40\}" $(SIGN_PACKAGE_DB)`" != \
			"$(DB_FINGERPRINT)" ] ; then \
		rm -f $(SIGN_PACKAGE_DB); \
		echo Creating sign package DB file $(SIGN_PACKAGE_DB); \
		echo $(DB_FINGERPRINT) > $(SIGN_PACKAGE_DB).tmp; \
		echo $(SIGNPACKAGEDIR) >> $(SIGN_PACKAGE_DB).tmp; \
		$(SIGNTOOL) -local -l >> $(SIGN_PACKAGE_DB).tmp; \
		mv $(SIGN_PACKAGE_DB).tmp $(SIGN_PACKAGE_DB); \
	fi
	@echo Unlocking $(SIGN_PACKAGE_DB).lock
	@dotlockfile -u $(SIGN_PACKAGE_DB).lock
endef

.PHONY: create_sign_package_db_dir
create_sign_package_db_dir:
	mkdir -p $(SIGN_PACKAGE_DB_PATH)

.PHONY: create_sign_package_db
create_sign_package_db:
ifdef BUILD_VERBOSE
	@echo Fingerprint of $(SIGNPACKAGEDIR) is \"$(DB_FINGERPRINT)\"
endif
	$(call Check_Fingerprint_DB,1)

.PHONY: check_sign_package_db
$(SIGN_PACKAGE_DB) check_sign_package_db: create_sign_package_db_dir
ifdef BUILD_VERBOSE
	@echo Fingerprint of $(SIGNPACKAGEDIR) is \"$(DB_FINGERPRINT)\"
	@if [ -f $(SIGN_PACKAGE_DB) ] ; then \
		echo Checking sign package DB file $(SIGN_PACKAGE_DB); \
	fi
endif
	$(call Check_Fingerprint_DB,0)
