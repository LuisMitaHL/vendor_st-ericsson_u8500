
all:

BINARY  = $(THIS_DIR)
PAYLOAD_TYPE = xloader

LINK_SCRIPT = $(XLOADER_DIR)/elf_arm.x

#SRCS += public_code_header.c
SRCS += toc_mgmt.c init_gcc.S
SRCS += xloader_debug.c
CPPFLAGS += -I$(XLOADER_COMMON)
VPATH += $(XLOADER_COMMON)

ifdef XLOADER_OLD_FORMAT
# public_code_header.c must be included if signing done "the old way"
SRCS    += public_code_header.c
endif

#bass_app stuff, TODO use bass_app in forest instead
SRCS += calc_digest.c verify_signedheader.c teec_handler.c
ifdef ENABLE_SYSTEM_RESET
SRCS += reset_system.c
endif
SRCS += check_payload_hash.c handle_bootp_magic.c
CPPFLAGS += -I$(XLOADER_DIR)/../bass_app
VPATH += $(XLOADER_DIR)/../bass_app
CPPFLAGS += -DOS_FREE

ifdef XLOADER_ENABLE_PRINTF
SRCS += issw_printf.c
CPPFLAGS += -D__ENABLE_PRINTF
endif

SRCS += crtmem.s
SRCS += memset.c



VPATH   += $(DELIVERY_PATH)/../tee_client
CPPFLAGS += -I$(DELIVERY_PATH)/../tee_client
SRCS += tee_client_api.c

CPPFLAGS += -I$(HCL_PATH)
VPATH += $(HCL_PATH)
SRCS += security.c

CPPFLAGS += -I$(BOOT_ROOT)/common
CPPFLAGS += -I$(DELIVERY_PATH)/api/sla_api
CPPFLAGS += -I$(DELIVERY_PATH)/api/ssa_api
CPPFLAGS += -I$(DELIVERY_PATH)/api/boot_api
CPPFLAGS += -I$(DELIVERY_PATH)/../issw
CPPFLAGS += -I$(DELIVERY_PATH)/rom
CPPFLAGS += -I$(BUILD_ROOT)


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

ifdef XLOADER_TEE_TEST_DIR
include $(XLOADER_TEE_TEST_DIR)/test.mk
endif

ifdef XLOADER_WARNS
WARNS = $(XLOADER_WARNS)
echo "Xloader: +++++++++++++++++++++++++++++++++++++++++++ warnings turned on"
endif
ifdef XLOADER_NOWERROR
NOWERROR = $(XLOADER_NOWERROR)
endif
ifdef XLOADER_DEBUG
DEBUG = $(XLOADER_DEBUG)
endif
ifdef XLOADER_WAIT_DEBUG
WAIT_DEBUG = $(XLOADER_WAIT_DEBUG)
endif
ifdef XLOADER_BUILD_VERBOSE
BUILD_VERBOSE = $(XLOADER_BUILD_VERBOSE)
echo "Xloader: +++++++++++++++++++++++++++++++++++++++++++ verbose"
endif
ifdef XLOADER_ENABLE_LOAD_TEE
CPPFLAGS += -DENABLE_LOAD_TEE
endif
ifdef XLOADER_UART_OVER_ULPI
CPPFLAGS += -DUART_OVER_ULPI
endif
ifdef XLOADER_UART_OVER_USB
CPPFLAGS += -DUART_OVER_USB
endif
ifdef XLOADER_ACCEPT_UNSIGNED_BINARIES
ACCEPT_UNSIGNED_BINARIES = $(XLOADER_ACCEPT_UNSIGNED_BINARIES)
endif
include $(parentDir)/build_common.mk
