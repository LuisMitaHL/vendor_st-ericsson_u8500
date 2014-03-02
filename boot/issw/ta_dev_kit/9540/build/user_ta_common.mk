
ifndef SSWNAMES
SSWNAMES := $(DEFAULT_TA_SSWNAMES)
endif

USER_TA = 1

PAYLOAD_TYPE = usta

USER_TA_HEADER_DIR = $(USER_TA_ROOT)/header

OBJCOPY_FLAGS = -j .ta_head -j .ta_func_head -j .text -j .rodata \
                -j .rel.dyn -j .data

LINK_SCRIPT = $(USER_TA_HEADER_DIR)/elf_arm.x

FIX_BINARY = $(BUILD_ROOT)/fix_ta_binary

CFLAGS += -fpie
#CFLAGS += -mword-relocations
CPPFLAGS += -I$(USER_TA_ROOT)/include
CPPFLAGS += -I$(TEE_API_DIR)
CPPFLAGS += -I$(DELIVERY_PATH)/ta

ifdef BINARY

SRCS_VPATH += $(USER_TA_HEADER_DIR)
SRCS += user_ta_header.c
CPPFLAGS += -I$(USER_TA_HEADER_DIR)


#libutee
LDADD += -L$(BUILD_ROOT)/user_ta/libutee -lutee
DPADD += $(BUILD_ROOT)/user_ta/libutee/libutee.a

#libmpa
LDADD += -L$(BUILD_ROOT)/user_ta/libmpa -lmpa
DPADD += $(BUILD_ROOT)/user_ta/libmpa/libmpa.a

LDADD += -lgcc
LDFLAGS += -Wl,-pie
#LDFLAGS += -Wl,--emit-relocs
#LDFLAGS += -pie
#LDFLAGS += --emit-relocs

endif #BINARY

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

ifneq ($(USER_TA_DISABLE_PRINT_A_),1)
CPPFLAGS += -DPRINT_A_
endif
ifneq ($(USER_TA_DISABLE_PRINT_B_),1)
CPPFLAGS += -DPRINT_B_
endif
ifeq ($(ISSW_PRINT_C),1)
CPPFLAGS += -DPRINT_C_
endif

ifdef USER_TA_WARNS
WARNS = $(USER_TA_WARNS)
endif
ifdef USER_TA_NOWERROR
NOWERROR = $(USER_TA_NOWERROR)
endif
ifdef USER_TA_DEBUG
DEBUG = $(USER_TA_DEBUG)
endif
ifdef USER_TA_WAIT_DEBUG
WAIT_DEBUG = $(USER_TA_WAIT_DEBUG)
endif
ifdef USER_TA_BUILD_VERBOSE
BUILD_VERBOSE = $(USER_TA_BUILD_VERBOSE)
endif
include $(parentDir)/build_common.mk
