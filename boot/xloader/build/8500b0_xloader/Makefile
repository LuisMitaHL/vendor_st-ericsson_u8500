#
#             Copyright (C) ST Ericsson
#                          ALL RIGHTS RESERVED
# This program contains proprietary information and is not to be used,
# copied, nor disclosed without the written consent of ST Ericsson
#
#-------------------------------------------------------------------------------

include ../config.mk

CHIP=8500b0

# Define rom code start addr
BOOTLOGBASEADDR = 0x90000000
CPPFLAGS    += -DBOOTLOGBASEADDR=$(BOOTLOGBASEADDR)
CPPFLAGS    += -DCACHE_ON

# 8500b0
CPPFLAGS  += -D__STN_5500=0 -D__STN_8500=20


#CFLAGS += -save-temps
#CFLAGS += -ffunction-sections -fdata-sections

XLOADER_DIR = $(BOOT_ROOT)/xloader/8500b0_ste

VPATH   += $(XLOADER_DIR)/src
VPATH   += $(XLOADER_DIR)/../common
VPATH	+= $(OBJ_OUTPUT_DIR)
CPPFLAGS += -I$(XLOADER_DIR)/src

SRCS    += xl_drivers.c xl_l1_cache_mmu_mgmt.s xloader.c xloader_pl011.c

include ../xloader_common.mk

