FORESTROOT_DIR	?= $(abspath $(CURDIR)/../../../../../..)
TOOLCHAINPATH	?= $(FORESTROOT_DIR)/toolchain/arm-2009q3
TOOLCHAIN       := $(TOOLCHAINPATH)/bin
CSPSA_LIB_PATH	:= $(FORESTROOT_DIR)/out/obj/base_system/cspsa/target

CROSS_PREFIX    ?= arm-none-linux-gnueabi
CROSS_COMPILE   ?= $(CROSS_PREFIX)-

PATH            := $(TOOLCHAIN):$(PATH)
export PATH

CC              := $(CROSS_COMPILE)gcc

CFLAGS          := -Wall -Wbad-function-cast -Wcast-align \
		   -Werror-implicit-function-declaration -Wextra \
		   -Wfloat-equal -Wformat-nonliteral -Wformat-security \
		   -Wformat=2 -Winit-self -Wmissing-declarations \
		   -Wmissing-format-attribute -Wmissing-include-dirs \
		   -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs \
		   -Wpointer-arith -Wshadow -Wstrict-prototypes \
		   -Wswitch-default -Wunsafe-loop-optimizations \
		   -Wwrite-strings
CFLAGS          += -c -fPIC

ifneq ($(CC), gcc)
CFLAGS          += -mthumb -mthumb-interwork
endif

DEBUG		?= 0
ifeq ($(DEBUG), 1)
CFLAGS          += -DDEBUG -O0 -g
endif

LD              := $(CROSS_COMPILE)ld
LDFLAGS         :=

RM              := rm -rf

OUT_DIR		?= $(FORESTROOT_DIR)/out
ifdef STE_PLATFORM
OUT_DIR		:= $(OUT_DIR)/lbp/$(STE_PLATFORM)/bass_app/test
else
STE_PLATFORM    ?= U8500
OUT_DIR		:= $(OUT_DIR)/bass_app/test
endif

BASS_APP_DEBUG_PRINT ?= 2

.PHONY: all bass_app_testsuite clean

all: bass_app_testsuite
################################################################################
# Build bass_app_testsuite - Bass app's main tests                            #
################################################################################
vpath %.c ../util
PACKAGE_NAME	:= bass_app_testsuite

BAT_SRCS := arb_table_info_test.c \
	    bass_app_test.c \
	    calc_digest_test.c \
	    check_payload_hash_test.c \
	    debug.c \
	    product_id_test.c \
	    product_config_test.c \
	    stress_test.c \
	    verify_signedheader_test.c \
	    test_tee_static_uuid.c \
	    nw_to_tee_test_data.c \
	    nw_to_tee_test.c
BAT_SRC_DIR	:= .
BAT_OBJ_DIR	:= $(OUT_DIR)
BAT_OBJS	:= $(addprefix $(BAT_OBJ_DIR)/, \
		     $(patsubst %.c, %.o, $(BAT_SRCS)))
BAT_INCLUDES := $(CURDIR)/../include \
		$(CURDIR)/../libbassapp/include \
		$(CURDIR)/../teec/include \
		$(CURDIR)/include \
		$(CURDIR)/tee/include

BAT_CFLAGS	:= $(addprefix -I, $(BAT_INCLUDES)) $(CFLAGS) \
		   -D_GNU_SOURCE \
		   -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT) \
		   -DBINARY_PREFIX=\"TEST\" \
		   -DSTE_PLATFORM_$(shell echo $(STE_PLATFORM) | tr a-z A-Z) \
		   -DLBP

BAT_LFLAGS	:= -lpthread \
		   -L$(OUT_DIR)/../teec -ltee \
		   -L$(OUT_DIR)/../libbassapp -lbassapp \
		   -L$(CSPSA_LIB_PATH) -lcspsa

BAT_BINARY	:= $(OUT_DIR)/$(PACKAGE_NAME)

bass_app_testsuite: $(BAT_BINARY)
ifeq ($(DEBUG), 1)
	@echo "PACKAGE_DIR = $(PACKAGE_DIR)"
	@echo "PACKAGE_FILE = $(PACKAGE_FILE)"
	@echo "PACKAGE_LINK = $(PACKAGE_LINK)"
endif
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/bin 755 0 0
	@$(PACKAGE_FILE) /usr/bin/$(PACKAGE_NAME) $(BAT_BINARY) 755 0 0
	@$(PACKAGE_DIR) /lib 755 0 0
	@$(PACKAGE_DIR) /lib/tee 755 0 0
	$(foreach f, \
		$(notdir $(wildcard $(CURDIR)/ta/*.ssw)), \
		$(call copy-one-ssw-file,$f))
endif

define copy-one-ssw-file
	$(PACKAGE_FILE) /lib/tee/$(1) $(CURDIR)/ta/$(1) 755 0 0;
endef

$(BAT_BINARY): $(BAT_OBJS)
	@echo "  LD      $@"
	$(CC) -o $@ $^ $(BAT_LFLAGS)
	@echo ""

$(BAT_OBJ_DIR)/%.o: $(BAT_SRC_DIR)/%.c
	@mkdir -p $(BAT_OBJ_DIR)
	@echo "  CC      $<"
	@$(CC) $(BAT_CFLAGS) -c $< -o $@

################################################################################
# Cleaning up configuration
################################################################################
clean:
	$(RM) $(OUT_DIR)
