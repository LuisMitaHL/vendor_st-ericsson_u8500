FORESTROOT_DIR	?= $(abspath $(CURDIR)/../../../../..)
TOOLCHAINPATH	?= $(FORESTROOT_DIR)/prebuilt/linux-x86/toolchain/arm-linux-androideabi-4.4.x
TOOLCHAIN	:= $(TOOLCHAINPATH)/bin
CSPSA_DIR 	:= $(FORESTROOT_DIR)/vendor/st-ericsson/storage/parameter_storage/cspsa

CROSS_PREFIX	?= arm-linux-androideabi
CROSS_COMPILE	?= $(CROSS_PREFIX)-

PATH		:= $(TOOLCHAIN):$(PATH)
export PATH

CC		:= $(CROSS_COMPILE)gcc

ifneq ($(CC), gcc)
CFLAGS		+= -mthumb -mthumb-interwork
endif


ifeq ($(DEBUG), 1)
CFLAGS		+= -DDEBUG -O0 -g
endif

ANDROID_INCLUDE ?= -I$(FORESTROOT_DIR)/bionic/libc/arch-arm/include \
		   -I$(FORESTROOT_DIR)/bionic/libc/include \
		   -I$(FORESTROOT_DIR)/bionic/libc/kernel/common \
		   -I$(FORESTROOT_DIR)/bionic/libc/kernel/arch-arm \

LD		:= $(CROSS_COMPILE)ld
LDFLAGS		:=

# This sections deserves some explanation. This makefile is normally called from
# loaders, which are currently placed in:
#   <FORESTROOT_DIR>/vendor/st-ericsson/tools/platform/flash_kit/loaders
# If they are building Android, then they have a flag CONFIG_DIR which have a
# defined value to <FORESTROOT_DIR>/out/... If that value exist, then we
# also output the osfree bass_app files to that folder. However if they are
# building for example LBP, then CONFIG_DIR isn't defined, but instead there is
# a STE_PLATFORM variable one can use to construct the path to the out-folder.
# For LBP we output the files on following path:
#   <FORESTROOT_DIR>/out/lbp/<STE_PLATFORM>/bass_app_osfree.
#
# As a fallback if a new/unknown platform is built, then we output the files to:
#   <FORESTROOT_DIR>/out/bass_app_osfree
ifeq ($(CONFIG_DIR),)
ifdef STE_PLATFORM
BASS_APP_OUT_DIR = $(FORESTROOT_DIR)/out/lbp/$(STE_PLATFORM)/bass_app_osfree
else
BASS_APP_OUT_DIR = $(FORESTROOT_DIR)/out/bass_app_osfree
$(warning No platform specified, using $(BASS_APP_OUT_DIR) as working directory!)
endif
else
BASS_APP_OUT_DIR = $(CONFIG_DIR)/bass_app_osfree
endif

# We need to create this folder here already so that the vpaths later on will add
# the folder to the vpath (it will not add folders that does not exist).
$(shell mkdir -p $(BASS_APP_OUT_DIR))

# Since it is rather compilicated to figure out where the output actually is, we
# will always print this for all builds.
$(info bass_app: BASS_APP_OUT_DIR = $(BASS_APP_OUT_DIR))

outdir = $(BASS_APP_OUT_DIR)/bin
objdir = $(BASS_APP_OUT_DIR)/obj

# This is legacy variables, but they are still used by loaders, so we cannot
# remove them (without making changes in loaders makefile also). Loaders seems
# to use these to tell use where to put files that are built by this makefile.
includedir ?=
libdir ?=

BASS_APP_DEBUG_PRINT ?= 2

# Support verbose output, i.e. make V=1 to get verbose output.
ifeq ($(V),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

CFLAGS_BASE := -Wall -Wbad-function-cast -Wcast-align \
	       -Werror-implicit-function-declaration -Wextra \
	       -Wfloat-equal -Wformat-nonliteral -Wformat-security \
	       -Wformat=2 -Winit-self -Wmissing-declarations \
	       -Wmissing-format-attribute -Wmissing-include-dirs \
	       -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs \
	       -Wpointer-arith -Wshadow -Wstrict-prototypes \
	       -Wswitch-default -Wunsafe-loop-optimizations \
	       -Wwrite-strings

# This flag is to disable to "uses variable-size enums yet the output is to use
# 32-bit enums" warning that loaders get using their flags and newlib.
CFLAGS_BASE := -fno-short-enums

CPPFLAGS_BASE :=
INSTALL ?= install

.PHONY: all build install build-OSFREE build-TEST install-OSFREE install-TEST \
		clean distclean
default: build

all: build

build: build-OSFREE build-TEST

install: install-OSFREE install-TEST

################################################################################
# Configuration for OSFREE static library                                      #
################################################################################
vpath %.c ./libbassapp
vpath %.c ./teec
vpath %.c ./util
OSFREE_SRCS := calc_digest.c \
	       tee_client_api_osfree.c \
	       teec_hcl.c \
	       check_payload_hash.c \
	       verify_signedheader.c \
	       verify_start_modem.c \
	       get_productconfig.c \
	       get_productid.c \
	       get_ta_key_hash.c \
	       teec_handler.c \
	       soc_settings.c \
	       imad.c \
	       get_die_id.c \
	       get_arb_table_info.c \
	       drm_key.c \
	       bass_app_static_ta.c \
	       debug.c

ifeq ($(MODEM_START_CODE),1)
OSFREE_SRCS += u8500_verify_start_modem.c
endif

OSFREE_OBJDIR := $(objdir)/osfree
OSFREE_OBJS := $(patsubst %.c,$(OSFREE_OBJDIR)/%.o,$(OSFREE_SRCS))
OSFREE_CFLAGS := -I. \
		 -I./include \
		 -I./teec/include \
		 -I./test/include \
		 -I./libbassapp/include \
		 $(ANDROID_INCLUDE) \
		 -I$(CSPSA_DIR) \
		 -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		 $(CFLAGS_BASE) \
		 $(CFLAGS)

OSFREE_CPPFLAGS := -DOS_FREE \
		   $(CPPFLAGS_BASE) \
		   $(CPPFLAGS)

OSFREE_LIB = libbassapp.a

quiet_cmd_cc_o_c_osfree = "  CC      $<"
cmd_cc_o_c_osfree = $(CC) $(OSFREE_CPPFLAGS) $(OSFREE_CFLAGS) -c -o $@ $<
$(OSFREE_OBJDIR)/%.o : %.c
	$(Q)mkdir -p $(OSFREE_OBJDIR)
	@echo $($(quiet)cmd_cc_o_c_osfree)
	@$(cmd_cc_o_c_osfree)

build-OSFREE: $(outdir)/$(OSFREE_LIB)

quiet_cmd_ar_osfree = "  AR      $@"
cmd_ar_osfree = $(AR) rcs $@ $^
$(outdir)/$(OSFREE_LIB): $(OSFREE_OBJS)
	$(Q)mkdir -p $(outdir)
	@echo $($(quiet)cmd_ar_osfree)
	@$(cmd_ar_osfree)

install-OSFREE: $(outdir)/$(OSFREE_LIB) include/bass_app.h
ifdef libdir
	@echo "bass_app: Install to dir: $(libdir), $(includedir)"
	$(Q)$(INSTALL) -d $(libdir)
	$(Q)$(INSTALL) -d $(includedir)
	$(Q)$(INSTALL) -m 644 $(outdir)/$(OSFREE_LIB) $(libdir)/$(OSFREE_LIB)
	$(Q)$(INSTALL) -m 644 include/bass_app.h $(includedir)/bass_app.h
else
	@echo "bass_app: Nothing to install (target $@)"
endif

################################################################################
# Configuration for OSFREE static test library                                 #
################################################################################
vpath %.c test
vpath %.c test/tee
vpath %.c util
vpath %.ssw test/ta
vpath %.c $(BASS_APP_OUT_DIR)
TEST_TA_BIN := sha_aes_ta.ssw
TEST_TA_SRC := $(TEST_TA_BIN:.ssw=.c)
TEST_TA_SRC_WITH_PATH := $(BASS_APP_OUT_DIR)/$(TEST_TA_SRC)
TEST_TA_OBJ := $(BASS_APP_OUT_DIR)/$(TEST_TA_BIN:.ssw=.o)

TEST_SRCS := bass_app_test.c \
	     calc_digest_test.c \
	     tee_test.c \
	     tee_test_data.c \
	     verify_signedheader_test.c \
	     product_config_test.c \
	     product_id_test.c \
	     arb_table_info_test.c \
	     drm_key_test.c \
	     debug.c \
	     $(TEST_TA_SRC)

TEST_OBJDIR = $(objdir)/osfree-test
TEST_OBJ := $(patsubst %.c,$(TEST_OBJDIR)/%.o,$(TEST_SRCS))

TEST_CFLAGS := -I./include \
	       -I./teec/include \
	       -I./test/include \
	       -I./test/tee/include \
	       -I./libbassapp/include \
	       $(ANDROID_INCLUDE) \
	       -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
	       $(CFLAGS_BASE) \
	       $(CFLAGS)

TEST_CPPFLAGS := -DOS_FREE \
		 $(CPPFLAGS_BASE) \
		 $(CPPFLAGS)

TEST_LIB := libbassapp_test.a

quiet_cmd_cc_o_c_test = "  CC      $<"
cmd_cc_o_c_test = $(CC) $(TEST_CPPFLAGS) $(TEST_CFLAGS) -c -o $@ $<
$(TEST_OBJDIR)/%.o : %.c
	$(Q)mkdir -p $(TEST_OBJDIR)
	@echo $($(quiet)cmd_cc_o_c_test)
	@$(cmd_cc_o_c_test)

quiet_cmd_ar_test = "  AR      $(outdir)/$(TEST_LIB)"
cmd_ar_test = $(AR) rcs $(outdir)/$(TEST_LIB) $^
build-TEST: $(TEST_OBJ)
	$(Q)mkdir -p $(outdir)
	@echo $($(quiet)cmd_ar_test)
	@$(cmd_ar_test)

install-TEST: build-TEST
ifdef libdir
	@echo "bass_app: Install to dir: $(libdir), $(includedir)"
	$(Q)$(INSTALL) -d $(libdir)
	$(Q)$(INSTALL) -d $(includedir)
	$(Q)$(INSTALL) -m 644 $(outdir)/$(TEST_LIB) $(libdir)/$(TEST_LIB)
	$(Q)$(INSTALL) -m 644 test/include/bass_app_test.h $(includedir)/bass_app_test.h
else
	@echo "bass_app: Nothing to install (target $@)"
endif

################################################################################
# Clean and other makefile targets                                             #
################################################################################
clean:
	@echo "bass_app: Cleaning ..."
	$(Q)rm -rf $(BASS_APP_OUT_DIR)

distclean: clean

DATE := $(shell date +"%D at %T")

$(TEST_TA_SRC_WITH_PATH): $(TEST_TA_BIN)
	@rm -f $@
	@echo "/*************************************************************************"> $@
	@echo " * Copyright ST-Ericsson 2010" >> $@
	@echo " *" >> $@
	@echo " * Autogenerated on $(DATE)." >> $@
	@echo " *" >> $@
	@echo " * DO NOT EDIT THIS FILE!" >> $@
	@echo " ************************************************************************/" >> $@
	@echo "#include \"tee_test_ta.h\"" >> $@
	@echo "const char test_ta[] = {" >> $@
	@perl ./tools/hexdump.pl $< >> $@
	@echo "};" >> $@
	@echo "const unsigned int test_ta_size = sizeof(test_ta);" >> $@
