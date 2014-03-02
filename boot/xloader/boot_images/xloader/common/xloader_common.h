/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------   */
/*                             Wireless Multimedia                           */
/*                           ROM code and Boot Image Team                    */
/*------------------------------------------------------------------------   */
/* This module provides template xloader.c file implementation               */
/* Specification release related to this implementation:                     */
/*------------------------------------------------------------------------   */
/*                                                                           */
/*****************************************************************************/
#ifndef __INC_XLOADER_COMMON_H
#define __INC_XLOADER_COMMON_H

/*-----------------------------------------------------------------------------
 * Types definition
 *---------------------------------------------------------------------------*/
typedef t_uint32 (*t_pInitMemFct) (t_uint32 param, t_uint32 status);
typedef void (*t_pApplication) (t_pub_rom_loader_info *p_loaderInfo,
				t_uint32 param_r1, t_uint32 param_r2);

/*-----------------------------------------------------------------------------
 * Registers definition
 *---------------------------------------------------------------------------*/
/* SSP registers */
typedef struct {
/*	Type			Name	 Offset */
	t_uint32		cr0;	/* 0x00 */
	t_uint32		cr1;	/* 0x04 */
	t_uint32		dr;	/* 0x08 */
	t_uint32		sr;	/* 0x0c */
	t_uint32		cpsr;	/* 0x10 */
	t_uint32		imsc;	/* 0x14 */
	t_uint32		ris;	/* 0x18 */
	t_uint32		mis;	/* 0x1C */
	t_uint32		icr;	/* 0x20 */
	t_uint32		dmacr;	/* 0x124 */
} t_ssp_registers;

typedef union {
	struct {
		t_bitfield  data		: 8;
		const t_bitfield reserved_1	: 2;
		t_bitfield  reg_address		: 8;
		t_bitfield  block_address	: 5;
		t_bitfield  rw			: 1;
	} Bit;
	t_uint32 Reg;
} t_ssp_dr_reg;

typedef union {
	struct {
		t_bitfield  rwb			: 1;
		t_bitfield  a7			: 7;
		t_bitfield  tsize		: 2;
		t_bitfield  hwgcen		: 1;
		t_bitfield  stopen		: 1;
		const t_bitfield reserved_1	: 4;
		t_bitfield  reg_add		: 8;
		t_bitfield  reg_data		: 8;
	} Bit;
	t_uint32 Reg;
} t_i2c_cmd_reg;

/*-----------------------------------------------------------------------------
 * Addresses
 *---------------------------------------------------------------------------*/
#if (__STN_9540 == 10)
#define ROM_CUT_ID_ADDRESS  0xFFFFDBF4
#elif (__STN_8500 == 20)
#define ROM_CUT_ID_ADDRESS	0x9001DBF4
#else
#error "Unknown build config, can't set ROM_CUT_ID_ADDRESS"
#endif

/*-----------------------------------------------------------------------------
 * Other defines
 *---------------------------------------------------------------------------*/
/* code which define the active peripheral stored in the backup RAM */
#define UART_CODE	0x54524155
#define USB_CODE	0x42535520

#define CUT_ID_4500_ED		0x00
#define CUT_ID_4500_V1		0x10
#define CUT_ID_4500_V1_1	0x11

/*
 * Add Secure service identification (missing in security.h
 * enum "t_sec_rom_service_id" )
 */
#define SEC_ROM_ROOT_KEY_HASH_READ_APPL		0x0F
#define SEC_ROM_KEY_IMPORT_APPL			0x10
#define SEC_ROM_RD_CERT_IMPORT_APPL		0x11
#define SEC_ROM_EFUSE_VERSION_CHECK_APPL	0x1C
#define SEC_ROM_BOOT_CONFIG_READ_APPL		0x1D
#define SEC_ROM_SW_DEBUG_CONTROL_GET_APPL	0x1E

/* Add Secure mode environment setting bit masks (missing in security.h) */
#define SEC_ROM_RNG_CLOCK_ENABLE_MASK		0x0020
#define SEC_ROM_HASH_CLOCK_ENABLE_MASK		0x0040
#define SEC_ROM_CRYP_CLOCK_ENABLE_MASK		0x0080
#define SEC_ROM_CLOCKS_CONTROL_MASK		0x0100

typedef enum {
	SEC_SERVICE_LEVEL_1_RESULT_OK = 0,
	SEC_SERVICE_LEVEL_1_RESULT_FAIL = 1
} t_sec_service_level_1_result;

typedef enum {
	SEC_SERVICE_LEVEL_2_RESULT_OK = 0,
	SEC_SERVICE_LEVEL_2_RESULT_FAIL = 1
} t_sec_service_level_2_result;


/* sec_headers.h define : */
#define SEC_ROM_ROOT_KEY_LENGTH			256
#define SEC_ROM_SECURE_KEY_LENGTH		256
#define SEC_ROM_OEM1_KEY_LENGTH			128
#define SEC_ROM_OEM2_KEY_LENGTH			128
#define SEC_ROM_RD_DOMAIN_KEY_LENGTH		128
#define SEC_ROM_RD_SW_KEY_LENGTH		128
#define SEC_ROM_RD_PROT_APP_KEY_LENGTH		128
 /* (offsetof(PUBLIC_KEY_STR,n)) */
#define SEC_ROM_PK_HEADER_SIZE			8

#define SEC_ROM_KEY_SIGNATURE_LENGTH		256
#define SEC_ROM_PUBLIC_ID_LENGTH		20
#define SEC_ROM_HEADER_SIGNATURE_LENGTH		128
#define SEC_ROM_HEADER_SIGNATURE_		2048     256
#define SEC_ROM_OPERATOR_KEY_ID_LENGTH		20
#define SEC_ROM_CONTROL_PARAM_LENGTH		4
#define SEC_ROM_LANGUAGE_LIST_LENGTH		20
#define SEC_ROM_SW_VERSION_LENGTH		20
#define SEC_ROM_ASIC_COUNT			2
#define SEC_ROM_IMEI_LENGTH			32
#define SEC_ROM_MANUFACTURING_LOCATION_LENGTH	4
#define SEC_ROM_ROOT_KEY_HASH_LENGTH		16
#define SEC_ROM_SHA1_DIGEST_LENGTH		20
#define SEC_ROM_MD5_DIGEST_LENGTH		16

#define SEC_IMEI_CERTIFICATE_MAGIC		0x3F67FCCBUL
#define SEC_VARIANT_CERTIFICATE_MAGIC		0xA5437035UL
#define SEC_PRIM_APPL_CERTIFICATE_MAGIC		0x5A991032UL
#define SEC_PUBLIC_KEY_HEADER_MAGIC		0x337D3C19UL
#define SEC_RD_CERTIFICATE_MAGIC		0xB7A44297UL
#define SEC_RD_DOMAIN_CERTIFICATE_MAGIC		0x6A22B7FAUL
#define SEC_PA_CERTIFICATE_MAGIC		0x83B2662FUL

#define DDR_TRACKER_REG				0x80157E08

#endif /*__INC_XLOADER_COMMON_H*/
