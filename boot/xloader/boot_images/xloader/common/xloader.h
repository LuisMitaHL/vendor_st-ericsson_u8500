/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __INC_XLOADER_H
#define __INC_XLOADER_H

#include "security.h"
#include "boot_types.h"
#include "boot_api.h"
#include "boot_error.h"
#include "public_code_header.h"
#include "nomadik_mapping.h"
#include "nomadik_registers.h"
#include "xloader_debug.h"
#include "xloader_common.h"
#include "xloader_header.h"
#include "public_rom_api.h"
#include "mmc_mgmt.h"
#include "issw_printf.h"

#include "hcl_defs.h"
#include "toc_mgmt.h"

#define rom_shared_load_image(a, b, c, d, e, f, g, h)	\
	ROM_SHARED_LOAD_IMAGE(a, b, c, d, e, f, g, h)

/* Timeout definitions when loading the different parts of the boot image */
#define MEM_INIT_LOADING_TIMEOUT	3000	/* 3 seconds */
#define NORMAL_APPL_LOADING_TIMEOUT	3000	/* 3 seconds */
#define TEE_LOADING_TIMEOUT		3000	/* 3 seconds */

/* address in backup RAM where to store Enginnering Info (12 bytes) */
#define ENGINEERING_ID_ADDRESS	0x80151F70

#define PRCM_XP70_DATA_COMM	0x8015FFFC

#define XL_SUCCESS 0

#define ALIGNMENT_IS_OK(p, type) \
	(((t_uint32)p & (__assert_alignof__(type) - 1)) == 0)

/*
 * TODO detect if compiler supports __alignof__
 */
#define __assert_alignof__(type) __alignof__(type)

/* Round up the even multiple of size, size has to be a multiple of 2 */
#define ROUNDUP(v, size) (((v) + (size - 1)) & ~(size - 1))


/*
 * Use local version of PUTS to disable debug prints if Xloader
 * was loaded of UART by the flash loader
 * */
#define XLPRINTF_INIT(_x_) \
	do {		   \
		switch (_x_) {		   \
		case IF_UART:	   \
			break;	   \
		case IF_USB:	   \
		default:			  \
			boot_print_enable = TRUE; \
			PRINTF_INIT;		  \
			break;			  \
		}				  \
	} while (0)

#define XLPUTS(_x_) if (boot_print_enable) PUTS(_x_)
#define XLPRINTF(...) if (boot_print_enable) PRINTF(__VA_ARGS__)

typedef t_uint32 (*t_mem_init_func)(t_uint32 boot_stage, t_uint32 boot_status,
					t_uint8 *avs_params, void *avs_data);

struct data_collection {
	t_address *rom_cut_id;
	void (**jmp_table) (void);
	t_pub_rom_loader_info *loader_info;
	t_boot_trace_info *boot_trace;
	t_uint8 boot_if;
	t_secure_rom_boot_info boot_info;
	t_boot_indication indication;
	t_uint32 prcmufw_status;
	t_uint32 boot_status;
        t_mem_init_func mem_init_func;
};

struct toc_data {
	t_boot_toc_item xloader_toc_item;
	t_uint32 *write_address;
	t_address *toc_esram;	/* Pointer to TOC located in eSRAM  */
	t_address *subtoc_esram; /* Pointer to SUBTOC located in eSRAM  */
	t_uint32 subtoc_size;
	t_public_code_header *cert_header;
};

/*
 * Keep compatibility with xloader_debug.c shared with older ASIC revisions
 * after update of xloader.c to ST-Ericsson coding standards
 */
#define xldbg_send_trace XLDBG_SendTrace
#define toc_search_item_in_toc TOC_SearchItemInToc

extern t_bool boot_print_enable;


t_boot_error load_toc_item(struct data_collection *data,
			   struct toc_data *toc_data,
			   t_address read_address,
			   t_uint32 *write_address,
			   t_boot_toc_item *toc_item,
			   t_uint32 token, t_uint32 timeout_value);

void update_caches_after_load(void *start_addr, void *end_addr,
			      t_bool invalidate_icache);

#endif /*__INC_XLOADER_H*/
