FORESTROOT_DIR	?= $(abspath $(CURDIR)/../../../../../..)
TOOLCHAINPATH	?= $(FORESTROOT_DIR)/toolchain/arm-2009q3
TOOLCHAIN       := $(TOOLCHAINPATH)/bin
CSPSA_DIR	:= $(FORESTROOT_DIR)/vendor/st-ericsson/storage/parameter_storage/cspsa

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

ifeq ($(DEBUG), 1)
CFLAGS          += -DDEBUG -O0 -g
endif

LD              := $(CROSS_COMPILE)ld
LDFLAGS         :=

RM              := rm -rf

OUT_DIR		?= $(FORESTROOT_DIR)/out
ifdef STE_PLATFORM
OUT_DIR		:= $(OUT_DIR)/lbp/$(STE_PLATFORM)/bass_app/libbassapp
else
OUT_DIR		:= $(OUT_DIR)/bass_app/libbassapp
endif

BASS_APP_DEBUG_PRINT ?= 2

.PHONY: all libbassapp clean show_variables

all: libbassapp copy-ta

################################################################################
# Libbassapp configuration
################################################################################
vpath %.c ../util
PACKAGE_NAME		:= libbassapp.so.1.0.0

LIBBASSAPP_SRCS		:= calc_digest.c \
			   check_payload_hash.c \
			   debug.c \
			   get_arb_table_info.c \
			   get_die_id.c \
			   get_productconfig.c \
			   get_productid.c \
			   get_ta_key_hash.c \
			   handle_bootp_magic.c \
			   imad.c \
			   smcl.c \
			   soc_settings.c \
			   teec_handler.c \
			   verify_signedheader.c \
			   verify_start_modem.c \
			   bass_app_static_ta.c \
			   drm_key.c

ifeq ($(MODEM_START_CODE),1)
LIBBASSAPP_SRCS 	+= u8500_verify_start_modem.c
endif

LIBBASSAPP_SRC_DIR	:= .
LIBBASSAPP_OBJ_DIR	:= $(OUT_DIR)
LIBBASSAPP_OBJS 	:= $(patsubst %.c,$(LIBBASSAPP_OBJ_DIR)/%.o, \
			     $(LIBBASSAPP_SRCS))
LIBBASSAPP_INCLUDES 	:= $(CURDIR)/../include \
			   $(CURDIR)/../teec/include \
			   $(CURDIR)/include \
			   $(CSPSA_DIR)

LIBBASSAPP_CFLAGS	:= $(addprefix -I, $(LIBBASSAPP_INCLUDES)) $(CFLAGS) \
			   -DDEBUGLEVEL_$(BASS_APP_DEBUG_PRINT)
LIBBASSAPP_LIBRARY	:= $(OUT_DIR)/$(PACKAGE_NAME)

libbassapp: $(LIBBASSAPP_LIBRARY)
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /usr 755 0 0
	@$(PACKAGE_DIR) /usr/lib 755 0 0
	@$(PACKAGE_FILE) /usr/lib/$(PACKAGE_NAME) $(LIBBASSAPP_LIBRARY) 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libbassapp.so.1 $(PACKAGE_NAME) 755 0 0
	@$(PACKAGE_LINK) /usr/lib/libbassapp.so $(PACKAGE_NAME) 755 0 0
	@$(PACKAGE_DIR) /lib 755 0 0
endif

ifeq ($(findstring libbassapp.so.1,$(wildcard *.so.1)), )
	@echo "creating symlink to $(OUT_DIR)/libbassapp.so.1"
	@ln -sf $(LIBBASSAPP_LIBRARY) $(OUT_DIR)/libbassapp.so.1
endif

ifeq ($(findstring libbassapp.so,$(wildcard *.so)), )
	@echo "creating symlink to $(OUT_DIR)/libbassapp.so"
	@ln -sf $(LIBBASSAPP_LIBRARY) $(OUT_DIR)/libbassapp.so
endif


$(LIBBASSAPP_LIBRARY): $(LIBBASSAPP_OBJS)
	@echo "  LD      $@"
	@$(CC) -shared -Wl,-soname,$(PACKAGE_NAME) -o $@ $+
	@echo ""

$(LIBBASSAPP_OBJ_DIR)/%.o: %.c
	@mkdir -p $(OUT_DIR)
	@echo "  CC      $<"
	@$(CC) $(LIBBASSAPP_CFLAGS) -c $< -o $@

################################################################################
# Place relevant ta files in target filesystem
################################################################################
copy-ta:
ifneq ($(SECURITY_PATH),)
	@$(PACKAGE_DIR) /lib/tee 755 0 0
	$(foreach f, \
		$(notdir $(wildcard $(CURDIR)/ta/*$(subst u,,$(STE_PLATFORM))*.ssw)), \
		$(call copy-one-ssw-file,$f))
endif

define copy-one-ssw-file
	$(PACKAGE_FILE) /lib/tee/$(1) $(CURDIR)/ta/$(1) 755 0 0;
endef

################################################################################
# Cleaning up configuration
################################################################################
clean:
	$(RM) $(OUT_DIR)
