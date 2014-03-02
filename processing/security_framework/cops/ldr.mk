SHELL = /bin/sh
FORESTROOT_DIR	?= $(abspath $(CURDIR)/../../../../..)

# This sections deserves some explanation. This makefile is normally called from
# loaders, which are currently placed in:
#   <FORESTROOT_DIR>/vendor/st-ericsson/tools/platform/flash_kit/loaders
# If they are building Android, then they have a flag CONFIG_DIR which has a
# defined value to <FORESTROOT_DIR>/out/... If that value exist, then we
# also output the osfree cops files to that folder. However if they are
# building for example LBP, then CONFIG_DIR isn't defined, but instead there is
# a STE_PLATFORM variable one can use to construct the path to the out-folder.
# For LBP we output the files on following path:
#   <FORESTROOT_DIR>/out/lbp/<STE_PLATFORM>/cops_osfree.
#
# As a fallback if a new/unknown platform is built, then we output the files to:
#   <FORESTROOT_DIR>/out/cops_osfree
ifeq ($(CONFIG_DIR),)
ifdef STE_PLATFORM
COPS_OUT_DIR = $(FORESTROOT_DIR)/out/lbp/$(STE_PLATFORM)/cops_osfree
else
COPS_OUT_DIR = $(FORESTROOT_DIR)/out/cops_osfree
$(warning No platform specified, using $(COPS_OUT_DIR) as working directory!)
endif
else
COPS_OUT_DIR = $(CONFIG_DIR)/cops_osfree
endif

# We need to create this folder here already so that the vpaths later on will add
# the folder to the vpath (it will not add folders that does not exist).
$(shell mkdir -p $(COPS_OUT_DIR)/ta)

# Since it is rather compilicated to figure out where the output actually is, we
# will always print this for all builds.
$(info cops: COPS_OUT_DIR = $(COPS_OUT_DIR))

# Support verbose output, i.e. make V=1 to get verbose output.
ifeq ($(V),1)
  quiet =
  Q =
else
  quiet=quiet_
  Q = @
endif

.SUFFIXES:
.SUFFIXES: .c .o

prefix = /usr/local
localstatedir = $(prefix)/var

INSTALL_PATH ?= $(COPS_OUT_DIR)/bin
LDRINC ?=
CFLAGS ?=
CPPFLAGS ?=
DATE := `date`

# Define which tools to use
INSTALL ?= install

# From swadvice #675 (removed -Werror as it's handled differently here)
CFLAGS_HIGH := -Wall -Wbad-function-cast -Wcast-align \
	       -Wextra -Wfloat-equal \
	       -Wformat-nonliteral -Wformat-security -Wformat=2 -Winit-self \
	       -Wmissing-declarations -Wmissing-format-attribute \
	       -Wmissing-include-dirs -Wmissing-noreturn \
	       -Wmissing-prototypes -Wnested-externs -Wpointer-arith \
	       -Wshadow -Wstrict-prototypes -Wswitch-default \
	       -Wunreachable-code -Wunsafe-loop-optimizations \
	       -Wwrite-strings
#CFLAGS_HIGH += -Werror-implicit-function-declaration \

# From swadvice #675 (removed -Wswitch-enum it's harmful /EJENWIK )
CFLAGS_MEDIUM := -Waggregate-return -Wlarger-than-65500 -Wredundant-decls \
		 -fno-common

# From swadvice #675 (removed -Wpacked)
CFLAGS_LOW := -Winline -Wno-missing-field-initializers -Wno-unused-parameter \
	      -Wold-style-definition -Wstrict-aliasing=2 -Wundef \
	      -pedantic -std=c99

CFLAGS_LOADERS := -fno-short-enums \
		  -fdollars-in-identifiers

CFLAGS += $(CFLAGS_HIGH) $(CFLAGS_MEDIUM) $(CFLAGS_LOW) $(CFLAGS_LOADERS)

# CPPFLAGS sent from loaders (inc newlib link)
CPPFLAGS += $(LDRINC)

########### GENERIC TARGETS ############
.PHONY: all config build install install-cops-data clean distclean
all: build install

build: build-osfree build-test
install: build install-osfree install-test

################################################################################
# Configuration for OS FREE COPS static library                                #
################################################################################
TA_DIR := ta
TA_SRC_DIR := $(COPS_OUT_DIR)/ta
include ./ta/cops_ta.mk

# Note that we have to add and remove vpaths if we add or remove folders
# where the source code is located.
vpath
vpath %.c cops-api
vpath %.c copsd
vpath %.c copsd/srv
vpath %.c copsd/storage
vpath %.c copsd/tapp
vpath %.c shared
vpath %.c shared/ta
vpath %.c $(TA_SRC_DIR)
OS_FREE_SRC := cops_api_client_osfree.c \
	       cops_debug.c \
	       cops_log.c \
	       cops_mac.c \
	       cops_srv_common.c \
	       cops_bootpartitionhandler.c \
	       cops_storage_cspsa.c \
	       cops_tocaccess.c \
	       cops_tapp_common.c \
	       cops_data_imei.c \
	       cops_data_lock.c \
	       cops_data_parameter.c \
	       cops_data_sipc.c \
	       cops_sipc_message.c \
	       cops_msg_handler.c \
	       cops_tapp_sipc.c \
	       cops_shared_util.c

OS_FREE_HEADERS := cops-api/include/cops.h \
		   cops-api/include/cops_types.h

CPPFLAGS += -I./../../../storage/parameter_storage/cspsa \
	    -I./cops-api \
	    -I./cops-api/include \
	    -I./copsd \
	    -I./copsd/srv \
	    -I./copsd/storage \
	    -I./copsd/tapp \
	    -I./shared \
	    -I./shared/ta \
	    -I./ta \
	    -DCOPS_IN_LOADERS \
	    -DCOPS_STORAGE_DIR=\"\" \
	    -DCOPS_TAPP_TEST_PATH=\"./\" \

ifeq ($(COPS_PLATFORM_TYPE), 8500)
CPPFLAGS += -DCOPS_TOCACCESS_SUPPORT \
	    -I./../../../storage/boot_area/toc/
COPS_USE_GP_TEEV0_17 ?= true
endif

ifdef ENABLE_STUBBED_SECURE_WORLD
OS_FREE_SRC += cops_tapp_stubbed.c
CPPFLAGS += -DCOPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
else
OS_FREE_SRC += cops_tapp_secwrld.c \
	       $(TA_SRC)

CPPFLAGS += -I./../bass_app/include \
           -I./../bass_app/teec/include

ifeq ($(COPS_USE_GP_TEEV0_17), true)
CPPFLAGS += -DCOPS_USE_GP_TEEV0_17
endif
endif

OS_FREE_OBJ_DIR = $(COPS_OUT_DIR)/osfree
OS_FREE_OBJS := $(patsubst %.c,$(OS_FREE_OBJ_DIR)/%.o,$(OS_FREE_SRC))
OS_FREE_STATIC_LIB := libcops.a

build-osfree: $(COPS_OUT_DIR)/$(OS_FREE_STATIC_LIB)

quiet_cmd_cc_o_c_osfree = "  CC      $<"
cmd_cc_o_c_osfree = $(CC) -c $(CPPFLAGS) $(CFLAGS) $(CFLAGS_WARNS) -o $@ $<
$(OS_FREE_OBJ_DIR)/%.o : %.c
	$(Q)mkdir -p $(OS_FREE_OBJ_DIR)
	@echo $($(quiet)cmd_cc_o_c_osfree)
	@$(cmd_cc_o_c_osfree)

quiet_cmd_ar_osfree = "  AR      $@"
cmd_ar_osfree = $(AR) rcs $@ $^
$(COPS_OUT_DIR)/$(OS_FREE_STATIC_LIB): $(OS_FREE_OBJS)
	$(Q)mkdir -p $(COPS_OUT_DIR)
	@echo $($(quiet)cmd_ar_osfree)
	@$(cmd_ar_osfree)

install-osfree: $(COPS_OUT_DIR)/$(OS_FREE_STATIC_LIB)
	$(Q)mkdir -p $(INSTALL_PATH)
	$(Q)$(INSTALL) -d $(INSTALL_PATH)
	$(Q)$(INSTALL) -m 644 $(COPS_OUT_DIR)/$(OS_FREE_STATIC_LIB) $(INSTALL_PATH)
	$(Q)$(INSTALL) -m 644 $(OS_FREE_HEADERS) $(INSTALL_PATH)

################################################################################
# Configuration for OS FREE static test library                                #
################################################################################
# TODO : Add source code files to TEST_SRC                                     #
#        Add header files to TEST_HEADERS                                      #
#        Add (update) CPPFLAGS as needed                                       #
################################################################################
TEST_SRC :=
TEST_OBJ := $(TEST_SRC:.c=.o)
CPPFLAGS +=
TEST_HEADERS +=
TEST_LIB_NAME := libcops_test.a

build-test: $(COPS_OUT_DIR)/$(TEST_LIB_NAME)

quiet_cmd_ar_test = "  AR      $@"
cmd_ar_test = $(AR) rcs $@ $^
$(COPS_OUT_DIR)/$(TEST_LIB_NAME) : $(TEST_OBJ)
	$(Q)mkdir -p $(COPS_OUT_DIR)
	@echo $($(quiet)cmd_ar_test)
	@$(cmd_ar_test)

install-test: build
	$(Q)mkdir -p $(INSTALL_PATH)
	$(Q)$(INSTALL) -d $(INSTALL_PATH)
	$(Q)$(INSTALL) -m 644 $(COPS_OUT_DIR)/$(TEST_LIB_NAME) $(INSTALL_PATH)

################################################################################
# Cops data file handling                                                      #
################################################################################
enable_cops_data_rx_diversity_support ?= false
CopsDataFile ?= copsd/COPS_DATA_0.csd
ifdef EXTENDED_SIPC_MSGS
CopsDataFile := copsd/COPS_DATA_0.csd.ext
else ifeq ($(enable_cops_data_rx_diversity_support),true)
CopsDataFile := copsd/COPS_DATA_0.csd.rx_on
endif

install-cops-data:
	$(Q)$(INSTALL) -d $(localstatedir)
	$(Q)$(INSTALL) -m 644 $(CopsDataFile) $(localstatedir)

################################################################################
# Clean and other makefile targets                                             #
################################################################################
# clean-ta is included in cops_ta.mk
clean: clean-ta
	rm -rf $(COPS_OUT_DIR)

distclean : clean
