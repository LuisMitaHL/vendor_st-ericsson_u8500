#=============================================================================
# Copyright (C) ST-Ericsson SA 2010
# Author: Dheeraj Gaur <dheeraj.gaur@stericsson.com>
# License terms: GNU General Public License (GPL), version 2.
#============================================================================

CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
LD = $(CROSS_COMPILE)ld
ARCH ?= arm
ARCH_VERSION ?= armv7-a


INCLUDES = -I ./mod_umi/int/umi/ -I ./mod_umi/export/ -I ./proj_host_umac/int/ -I ./mod_hif/export/ -I ./mod_umc/int/adapt_umc/ -I ./mod_umc/export/ \
           -I ./mod_umc/int/adapt_umc/ -I ./mod_umc/int/imports/ -I ./mod_umc/int/sys_umc/ -I ./mod_umc/int/umc/ -I ./ext_inc/
INCLUDES += -I $(KLIB_BUILD)/include -I $(KLIB_BUILD)/arch/$(ARCH)/include
INCLUDES += -I $(KERNEL_OUTPUT)/usr/include
CFLAGS = -D_LINUX \
	-DUSE_UMAC -DLITTLE_ENDIAN_PLATFORM -DENABLE_UMAC_DEBUG=1 -DWBF00003238_SW_WORKAROUND=1 -DIOCTL_INTERFACE -DW0 -DUSE_LINUX=1 -DUSE_SET_BSS_PARAMS_FLAGS \
	-DFT_SUPPORT=1 -DDOT11K_SUPPORT=1 -DWAPI_SUPPORT=1 -DMGMT_FRAME_PROTECTION=1 -DP2P_MINIAP_SUPPORT=1 \
	-DALLOC_SMALL_MEM_CHUNK \
	-mlittle-endian -Iarch/arm/mach-ux500/include -Iarch/arm/plat-nomadik/include -Wall -Wundef -Wstrict-prototypes -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -D__KERNEL__ -fconserve-stack -Wno-trigraphs -fno-inline-functions-called-once

CFLAGS += $(INCLUDES)

SRCS  = ./mod_umi/int/umi/UMI_ScanList.c ./mod_umi/int/umi/UMI_Utility.c ./mod_umi/int/umi/UMI_Queue.c ./mod_umi/int/umi/UMI_DebugTrace.c \
        ./mod_umi/int/umi/UMI_UmacDil.c ./mod_umi/int/umi/UMI_ProcUmacMsg.c ./mod_umi/int/umi/UMI_Api.c ./mod_umc/int/umc/umac_task.c \
        ./mod_umc/int/umc/umac_utils.c ./mod_umc/int/umc/umac_dev_list.c ./mod_umc/int/umc/umac_dbg.c ./mod_umc/int/umc/umac_proc_lmac_frames.c \
        ./mod_umc/int/umc/umac_sm.c ./mod_umc/int/umc/umac_data_handler.c ./mod_umc/int/umc/umac_2_ul.c ./mod_umc/int/umc/umac_frames.c \
        ./mod_umc/int/umc/umac_mib.c ./mod_umc/int/umc/umac_2_lmac.c ./mod_umc/int/umc/umac_internal.c ./mod_umc/int/adapt_umc/umac_hi.c \
        ./mod_umc/int/adapt_umc/lmac_callbacks.c ./mod_umc/int/sys_umc/umac_mem.c

OBJS := $(patsubst %.c,%.o,$(SRCS))


LIB   = proj_host_umac.lib

all:	$(SRCS) $(OBJS) $(LIB)

%.o: %.c
	@echo "	[CC] $<"
	$(HIDE)$(CC) $(CFLAGS) -c $< -o $@

%.lib: $(OBJS)
	$(LD) -r -o $(LIB) $(OBJS)

clean:
	rm -f $(OBJS) $(LIB) .depfiles/*.d

mrproper: clean
