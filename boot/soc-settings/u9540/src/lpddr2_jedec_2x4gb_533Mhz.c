/*===========================================================================
| ST-Ericsson
| ===========================================================================
|
|  $RCSfile: /sources/lpddr2_jedec_2x4gb_533Mhz.c$
|  $Revision: 1.7$
|  $Date: Wed May 09 17:13:40 2012 GMT$
|
|  $Source: /sources/lpddr2_jedec_2x4gb_533Mhz.c$
|
|  Copyright Statement:
|  -------------------
|  The confidential and proprietary information contained in this file may
|  only be used by a person authorized under and to the extent permitted
|  by a subsisting licensing agreement from ST-Ericsson S.A.
|
|  Copyright (C) ST-Ericsson S.A. 2011. All rights reserved.
|
|  This entire notice must be reproduced on all copies of this file
|  and copies of this file may only be made by a person if such person is
|  permitted to do so under the terms of a subsisting license agreement
|  from ST-Ericsson S.A..
|
|
| Project :  AP9540
| -------
|
| Description:
| ------------
|
|
|===========================================================================
|                    Revision History
|===========================================================================
|
|  $Log: /sources/lpddr2_jedec_2x4gb_533Mhz.c$
|  
|   Revision: 1.7 Wed May 09 17:13:40 2012 GMT frq06651
|   update chip select field
|  
|   Revision: 1.6 Wed May 09 13:35:16 2012 GMT frq06651
|   Update value according ER 431656
|  
|   Revision: 1.5 Thu Apr 26 13:43:44 2012 GMT frq06447
|   Last mask register removed
|  
|   Revision: 1.4 Mon Apr 16 11:57:41 2012 GMT frq06447
|   Implementation  WP 334907 and aligned with PRCMU FW DV 3
|  
|   Revision: 1.3 Fri Apr 06 17:03:41 2012 GMT frq02595
|   Alignement with SoC Setting project: Update payload
|  
|   Revision: 1.2 Fri Feb 10 17:04:24 2012 GMT frq02595
|   Don't use the DDR autodetection. 
|   Replace section payload5, 6, 7, 8 by payload1, 2, 3, 4.
|  
|   Revision: 1.1 Fri Feb 10 15:07:03 2012 GMT frq06651
|   First version for CR413266 (SSG Memory)
|
|  $Aliases: $
|
|  $KeysEnd $
|
|
|===========================================================================*/

#include <types.h>
#include <soc_settings.h>

__attribute__((section(".type")))
const u32 type = SECTION_TYPE_DDR_DATA;

extern u32 linker_payload_length;

__attribute__((section(".length")))
const u32 payload_length = (u32)&linker_payload_length;


// ---------------------------------------------------------------------------------------------------------------
// --- DDR1 CONFIG
// ---------------------------------------------------------------------------------------------------------------
/**
 ******************************************************************************
 * @var		const u8 ddr1_id_payload[]
 * @author	MLA
 * @brief	This table is use during DDR identification after a preInit DDR
 ******************************************************************************
**/
__attribute__((section(".payload0")))
const u8 ddr1_id_payload[] = {
		/* Value,  mask */
		0x00,   0x00,   /* CS0 Device info */
		0x00,   0x00,   /* CS1 Device info */
		0x00,   0x00,   /* CS0 Basic Config-1 : Manufacturer ID */
		0x00,   0x00,   /* CS1 Basic Config-1 : Manufacturer ID */
		0x00,   0x00,   /* CS0 Basic Config-2 : Revision ID1 */
		0x00,   0x00,   /* CS1 Basic Config-2 : Revision ID1 */
		0x00,   0x00,   /* CS0 Basic Config-3 : Revision ID2 */
		0x00,   0x00,   /* CS1 Basic Config-3 : Revision ID2 */
		0x18,   0xff,   /* CS0 Basic Config-4 : I/O width + Density + Type */
		0x00,   0x00,   /* CS1 Basic Config-4 : I/O width + Density + Type */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00,   /* CS1 Not used */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00,   /* CS1 Not used */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00	/* CS1 Not used */
};

/**
 ******************************************************************************
 * @var		const u32 ddr1_settings[]
 * @author	MLA
 * @brief	This table is the DDR configuration.
 * 			It used for the ddr init prcmu fw service
 ******************************************************************************
**/
__attribute__((section(".payload1")))
const u32 ddr1_settings[] = {
		// --- Mask for DDR controller registers -----------------
		0x6DFFFFFF, /* no DENALI_CTL_25/28/31 */
		0x9800F1FF, /* no DENALI_CTL_41/42/43/48-58/61/62 */
		0xFFFFF309, /* no DENALI_CTL_65/66/68-71/74/75 */
		0xFFFFFFFF,
		0x0001E01F, /* no DENALI_CTL_133-140/146/147 */

		// --- DDR Setting ---------------------------------------
		0x00000101, /* DENALI_CTL_0 */
		0x01010100, /* DENALI_CTL_1 */
		0x00010001, /* DENALI_CTL_2 */
		0x00010100, /* DENALI_CTL_3 */
		0x01010100, /* DENALI_CTL_4 */
		0x01000100, /* DENALI_CTL_5 */
		0x00000000, /* DENALI_CTL_6 */
		0x00000001, /* DENALI_CTL_7 */
		0x00000101, /* DENALI_CTL_8 */
		0x02020304, /* DENALI_CTL_9 */
		0x03030303, /* DENALI_CTL_10 */
		0x01030303, /* DENALI_CTL_11 */
		0x00030101, /* DENALI_CTL_12 */
		0x00000000, /* DENALI_CTL_13 */
		0x00000207, /* DENALI_CTL_14 */
		0x03000000, /* DENALI_CTL_15 */
		0x06020202, /* DENALI_CTL_16 */
		0x10000f04, /* DENALI_CTL_17 */
		0x00000f12, /* DENALI_CTL_18 */
		0x00120000, /* DENALI_CTL_19 */
		0x0002000a, /* DENALI_CTL_20 */
		0x00080a03, /* DENALI_CTL_21 */
		0x05000909, /* DENALI_CTL_22 */
		0x32002e22, /* DENALI_CTL_23 */
		0x23003200, /* DENALI_CTL_24 */
		0x0a170000, /* DENALI_CTL_26 */
		0x00000046, /* DENALI_CTL_27 */
		0x08130813, /* DENALI_CTL_29 */
		0x08130813, /* DENALI_CTL_30 */
		0xffff0000, /* DENALI_CTL_32 */
		0xffffffff, /* DENALI_CTL_33 */
		0x0000ffff, /* DENALI_CTL_34 */
		0x00100100, /* DENALI_CTL_35 */
		0x00000000, /* DENALI_CTL_36 */
		0x00040000, /* DENALI_CTL_37 */
		0x004c1b4e, /* DENALI_CTL_38 */
		0x0000004d, /* DENALI_CTL_39 */
		0x00000036, /* DENALI_CTL_40 */
		0x00000000, /* DENALI_CTL_44 */
		0x00000000, /* DENALI_CTL_45 */
		0x00000000, /* DENALI_CTL_46 */
		0x00000000, /* DENALI_CTL_47 */
		0x00000000, /* DENALI_CTL_59 */
		0x00000000, /* DENALI_CTL_60 */
		0x05000004, /* DENALI_CTL_63 */
		0x00050009, /* DENALI_CTL_64 */
		0x00000000, /* DENALI_CTL_67 */
		0x00000000, /* DENALI_CTL_72 */
		0x00017c00, /* DENALI_CTL_73 */
		0x01000100, /* DENALI_CTL_76 */
		0x01000103, /* DENALI_CTL_77 */
		0x02030300, /* DENALI_CTL_78 */
		0x02000104, /* DENALI_CTL_79 */
		0x1b000100, /* DENALI_CTL_80 */
		0x0001b040, /* DENALI_CTL_81 */
		0x00c00000, /* DENALI_CTL_82 */
		0x02140030, /* DENALI_CTL_83 */
		0x0000001b, /* DENALI_CTL_84 */
		0x00000000, /* DENALI_CTL_85 */
		0x00000000, /* DENALI_CTL_86 */
		0x00c20000, /* DENALI_CTL_87 */
		0x000600c2, /* DENALI_CTL_88 */
		0x00010006, /* DENALI_CTL_89 */
		0x00000001, /* DENALI_CTL_90 */
		0x00000000, /* DENALI_CTL_91 */
		0x00019f8f, /* DENALI_CTL_92 */
		0x00000214, /* DENALI_CTL_93 */
		0x000014c8, /* DENALI_CTL_94 */
		0x00010100, /* DENALI_CTL_95 */
		0x01000001, /* DENALI_CTL_96 */
		0x0a010101, /* DENALI_CTL_97 */
		0x000c0c0a, /* DENALI_CTL_98 */
		0x1fff0000, /* DENALI_CTL_99 */
		0x0000ffff, /* DENALI_CTL_100 */
		0x0302ffff, /* DENALI_CTL_101 */
		0x00050804, /* DENALI_CTL_102 */
		0x08130813, /* DENALI_CTL_103 */
		0x00000813, /* DENALI_CTL_104 */
		0x000a0503, /* DENALI_CTL_105 */
		0x00170c06, /* DENALI_CTL_106 */
		0x39462312, /* DENALI_CTL_107 */
		0x04080200, /* DENALI_CTL_108 */
		0x00140813, /* DENALI_CTL_109 */
		0x004c0027, /* DENALI_CTL_110 */
		0x00280015, /* DENALI_CTL_111 */
		0x00d1004d, /* DENALI_CTL_112 */
		0x00490711, /* DENALI_CTL_113 */
		0x0449b388, /* DENALI_CTL_114 */
		0x0001800c, /* DENALI_CTL_115 */
		0x00060030, /* DENALI_CTL_116 */
		0x000c0030, /* DENALI_CTL_117 */
		0x186230c4, /* DENALI_CTL_118 */
		0x00210022, /* DENALI_CTL_119 */
		0x006100c2, /* DENALI_CTL_120 */
		0x00210022, /* DENALI_CTL_121 */
		0x00010001, /* DENALI_CTL_122 */
		0x00030006, /* DENALI_CTL_123 */
		0x00010001, /* DENALI_CTL_124 */
		0x02899001, /* DENALI_CTL_125 */
		0x034a1001, /* DENALI_CTL_126 */
		0x06684711, /* DENALI_CTL_127 */
		0x02114106, /* DENALI_CTL_128 */
		0x24934906, /* DENALI_CTL_129 */
		0x1041a907, /* DENALI_CTL_130 */
		0x1041a8a3, /* DENALI_CTL_131 */
		0x03462449, /* DENALI_CTL_132 */
		0x060b1001, /* DENALI_CTL_141 */
		0x00010003, /* DENALI_CTL_142 */
		0x00008001, /* DENALI_CTL_143 */
		0x00010003, /* DENALI_CTL_144 */

		// --- LpDDR2 phy register direct access -----------------
		0x060b1001, /* DENALI_CTL_72 */

		// --- LpDDR2 Frequency divider --------------------------
		// This value is not part of PRCMU fw API
		// It is written by ISSW
		0x00000000  /* Fmax */
};


// ---------------------------------------------------------------------------------------------------------------
// --- DDR2 CONFIG
// ---------------------------------------------------------------------------------------------------------------

/**
 ******************************************************************************
 * @var		const u8 ddr2_id_payload[]
 * @author	MLA
 * @brief	This table is use during DDR identification after a preInit DDR
 ******************************************************************************
**/
__attribute__((section(".payload2")))
const u8 ddr2_id_payload[] = {
		/* Value,  mask */
		0x00,   0x00,   /* CS0 Device info */
		0x00,   0x00,   /* CS1 Device info */
		0x00,   0x00,   /* CS0 Basic Config-1 : Manufacturer ID */
		0x00,   0x00,   /* CS1 Basic Config-1 : Manufacturer ID */
		0x00,   0x00,   /* CS0 Basic Config-2 : Revision ID1 */
		0x00,   0x00,   /* CS1 Basic Config-2 : Revision ID1 */
		0x00,   0x00,   /* CS0 Basic Config-3 : Revision ID2 */
		0x00,   0x00,   /* CS1 Basic Config-3 : Revision ID2 */
		0x18,   0xff,   /* CS0 Basic Config-4 : I/O width + Density + Type */
		0x00,   0x00,   /* CS1 Basic Config-4 : I/O width + Density + Type */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00,   /* CS1 Not used */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00,   /* CS1 Not used */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00	/* CS1 Not used */
};


/**
 ******************************************************************************
 * @var		const u32 ddr2_settings[]
 * @author	MLA
 * @brief	This table is the DDR configuration.
 * 			It used for the ddr init prcmu fw service
 ******************************************************************************
**/
__attribute__((section(".payload3")))
const u32 ddr2_settings[] = {
		// --- Mask for DDR controller registers -----------------
		0x6DFFFFFF, /* no DENALI_CTL_25/28/31 */
		0x9800F1FF, /* no DENALI_CTL_41/42/43/48-58/61/62 */
		0xFFFFF309, /* no DENALI_CTL_65/66/68-71/74/75 */
		0xFFFFFFFF,
		0x0001E01F, /* no DENALI_CTL_133-140/146/147 */

		// --- DDR Setting ---------------------------------------
		0x00000101, /* DENALI_CTL_0 */
		0x01010100, /* DENALI_CTL_1 */
		0x00010001, /* DENALI_CTL_2 */
		0x00010100, /* DENALI_CTL_3 */
		0x01010100, /* DENALI_CTL_4 */
		0x01000100, /* DENALI_CTL_5 */
		0x00000000, /* DENALI_CTL_6 */
		0x00000001, /* DENALI_CTL_7 */
		0x00000101, /* DENALI_CTL_8 */
		0x02020304, /* DENALI_CTL_9 */
		0x03030303, /* DENALI_CTL_10 */
		0x03030303, /* DENALI_CTL_11 */
		0x00030103, /* DENALI_CTL_12 */
		0x00000000, /* DENALI_CTL_13 */
		0x00000207, /* DENALI_CTL_14 */
		0x03000000, /* DENALI_CTL_15 */
		0x06020202, /* DENALI_CTL_16 */
		0x10000f04, /* DENALI_CTL_17 */
		0x00000f12, /* DENALI_CTL_18 */
		0x00120000, /* DENALI_CTL_19 */
		0x0002000a, /* DENALI_CTL_20 */
		0x00080a03, /* DENALI_CTL_21 */
		0x05000909, /* DENALI_CTL_22 */
		0x32002e22, /* DENALI_CTL_23 */
		0x23003200, /* DENALI_CTL_24 */
		0x0a170000, /* DENALI_CTL_26 */
		0x00000046, /* DENALI_CTL_27 */
		0x08130813, /* DENALI_CTL_29 */
		0x08130813, /* DENALI_CTL_30 */
		0xffff0000, /* DENALI_CTL_32 */
		0xffffffff, /* DENALI_CTL_33 */
		0x0000ffff, /* DENALI_CTL_34 */
		0x00100100, /* DENALI_CTL_35 */
		0x00000000, /* DENALI_CTL_36 */
		0x00040000, /* DENALI_CTL_37 */
		0x004c1b4e, /* DENALI_CTL_38 */
		0x0000004d, /* DENALI_CTL_39 */
		0x00000036, /* DENALI_CTL_40 */
		0x00000000, /* DENALI_CTL_44 */
		0x00000000, /* DENALI_CTL_45 */
		0x00000000, /* DENALI_CTL_46 */
		0x00000000, /* DENALI_CTL_47 */
		0x00000000, /* DENALI_CTL_59 */
		0x00000000, /* DENALI_CTL_60 */
		0x05000004, /* DENALI_CTL_63 */
		0x00050009, /* DENALI_CTL_64 */
		0x00000000, /* DENALI_CTL_67 */
		0x00000000, /* DENALI_CTL_72 */
		0x00017c00, /* DENALI_CTL_73 */
		0x01000100, /* DENALI_CTL_76 */
		0x01000103, /* DENALI_CTL_77 */
		0x02030300, /* DENALI_CTL_78 */
		0x02000104, /* DENALI_CTL_79 */
		0x1b000100, /* DENALI_CTL_80 */
		0x0001b040, /* DENALI_CTL_81 */
		0x00c00000, /* DENALI_CTL_82 */
		0x02140030, /* DENALI_CTL_83 */
		0x0000001b, /* DENALI_CTL_84 */
		0x00000000, /* DENALI_CTL_85 */
		0x00000000, /* DENALI_CTL_86 */
		0x00c20000, /* DENALI_CTL_87 */
		0x000600c2, /* DENALI_CTL_88 */
		0x00010006, /* DENALI_CTL_89 */
		0x00000001, /* DENALI_CTL_90 */
		0x00000000, /* DENALI_CTL_91 */
		0x00019f8f, /* DENALI_CTL_92 */
		0x00000214, /* DENALI_CTL_93 */
		0x000014c8, /* DENALI_CTL_94 */
		0x00010100, /* DENALI_CTL_95 */
		0x01000001, /* DENALI_CTL_96 */
		0x0a010101, /* DENALI_CTL_97 */
		0x000c0c0a, /* DENALI_CTL_98 */
		0x1fff0000, /* DENALI_CTL_99 */
		0x0000ffff, /* DENALI_CTL_100 */
		0x0302ffff, /* DENALI_CTL_101 */
		0x00050804, /* DENALI_CTL_102 */
		0x08130813, /* DENALI_CTL_103 */
		0x00000813, /* DENALI_CTL_104 */
		0x000a0503, /* DENALI_CTL_105 */
		0x00170c06, /* DENALI_CTL_106 */
		0x39462312, /* DENALI_CTL_107 */
		0x04080200, /* DENALI_CTL_108 */
		0x00140813, /* DENALI_CTL_109 */
		0x004c0027, /* DENALI_CTL_110 */
		0x00280015, /* DENALI_CTL_111 */
		0x00d1004d, /* DENALI_CTL_112 */
		0x00490711, /* DENALI_CTL_113 */
		0x0449b388, /* DENALI_CTL_114 */
		0x0001800c, /* DENALI_CTL_115 */
		0x00060030, /* DENALI_CTL_116 */
		0x000c0030, /* DENALI_CTL_117 */
		0x186230c4, /* DENALI_CTL_118 */
		0x00210022, /* DENALI_CTL_119 */
		0x006100c2, /* DENALI_CTL_120 */
		0x00210022, /* DENALI_CTL_121 */
		0x00010001, /* DENALI_CTL_122 */
		0x00030006, /* DENALI_CTL_123 */
		0x00010001, /* DENALI_CTL_124 */
		0x02899001, /* DENALI_CTL_125 */
		0x034a1001, /* DENALI_CTL_126 */
		0x06684711, /* DENALI_CTL_127 */
		0x02114106, /* DENALI_CTL_128 */
		0x24934906, /* DENALI_CTL_129 */
		0x1041a907, /* DENALI_CTL_130 */
		0x1041a8a3, /* DENALI_CTL_131 */
		0x03462449, /* DENALI_CTL_132 */
		0x060b1001, /* DENALI_CTL_141 */
		0x00010003, /* DENALI_CTL_142 */
		0x00008001, /* DENALI_CTL_143 */
		0x00010003, /* DENALI_CTL_144 */

		// --- LpDDR2 phy register direct access -----------------
		0x060b1001, /* DENALI_CTL_72 */

		// --- LpDDR2 Frequency divider --------------------------
		// This value is not part of PRCMU fw API
		// It is written by ISSW
		0x00000000  /* Fmax */
};
