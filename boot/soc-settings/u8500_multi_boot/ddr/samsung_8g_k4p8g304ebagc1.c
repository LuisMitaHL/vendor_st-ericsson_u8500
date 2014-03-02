/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: WenHai Fang <wenhai.h.fang at stericsson.com>
 *  for ST-Ericsson.
 */

/* JEDEC 8GBits 2die 400mhz */

{
	.id_regs_and_mask = {
		/* Value,  mask */
		0x00,   0x00,   /* CS0 Device info */
		0x00,   0x00,   /* CS1 Device info */
		0x01,   0xff,   /* CS0 BC-1 : Manufacturer ID */
		0x01,   0xff,   /* CS1 BC-1 : Manufacturer ID */
		0x00,   0x00,   /* CS0 BC-2 : Revision ID1 */
		0x00,   0x00,   /* CS1 BC-2 : Revision ID1 */
		0x00,   0x00,   /* CS0 BC-3 : Revision ID2 */
		0x00,   0x00,   /* CS1 BC-3 : Revision ID2 */
		0x18,   0xff,   /* CS0 BC-4 : I/O width + Density + Type */
		0x18,   0xff,   /* CS1 BC-4 : I/O width + Density + Type */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00,   /* CS1 Not used */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00,   /* CS1 Not used */
		0x00,   0x00,   /* CS0 Not used */
		0x00,   0x00,   /* CS1 Not used */
	},
	.settings = {

		/* Mask for DDR controller registers */
		0x6DFFFFFF, /* no DENALI_CTL_25/28/31 */
		0x9800F1FF, /* no DENALI_CTL_41/42/43/48-58/61/62 */
		0xFFFFF309, /* no DENALI_CTL_65/66/68-71/74/75 */
		0xFFFFFFFF,
		0x0003E01F, /* no DENALI_CTL_133-140 */

		/*
		 * DDR settings
		 * REG_CONFIG_0 databahn registers
		 */
		0x00000101, /* DENALI_CTL_0 */
		0x01010100, /* DENALI_CTL_1 */
		0x00000001, /* DENALI_CTL_2 */
		0x00010100, /* DENALI_CTL_3 */
		0x01010100, /* DENALI_CTL_4 */
		0x01000100, /* DENALI_CTL_5 */
		0x00000000, /* DENALI_CTL_6 */
		0x00000001, /* DENALI_CTL_7 */
		0x00000101, /* DENALI_CTL_8 */
		0x02020303, /* DENALI_CTL_9 */
		0x03030303, /* DENALI_CTL_10 */
		0x01030303, /* DENALI_CTL_11 */
		0x00030301, /* DENALI_CTL_12 */
		0x00000000, /* DENALI_CTL_13 */
		0x00060207, /* DENALI_CTL_14 */
		0x03000000, /* DENALI_CTL_15 */
		0x04020202, /* DENALI_CTL_16 */
		0x0c000f03, /* DENALI_CTL_17 */
		0x00000f0d, /* DENALI_CTL_18 */
		0x000e0000, /* DENALI_CTL_19 */
		0x0002000a, /* DENALI_CTL_20 */
		0x00060803, /* DENALI_CTL_21 */
		0x05000909, /* DENALI_CTL_22 */
		0x32002e1a, /* DENALI_CTL_23 */
		0x23003200, /* DENALI_CTL_24 */
		0x08110000, /* DENALI_CTL_26 */
		0x00000034, /* DENALI_CTL_27 */
		0x06100610, /* DENALI_CTL_29 */
		0x06100610, /* DENALI_CTL_30 */
		0xffff0000, /* DENALI_CTL_32 */
		0xffffffff, /* DENALI_CTL_33 */
		0x0000ffff, /* DENALI_CTL_34 */
		0x00100100, /* DENALI_CTL_35 */
		0x00000000, /* DENALI_CTL_36 */
		0x00030000, /* DENALI_CTL_37 */
		0x00391b4e, /* DENALI_CTL_38 */
		0x0000003a, /* DENALI_CTL_39 */
		0x00000028, /* DENALI_CTL_40 */
		0x00000000, /* DENALI_CTL_44 */
		0x00000000, /* DENALI_CTL_45 */
		0x00000000, /* DENALI_CTL_46 */
		0x00000000, /* DENALI_CTL_47 */
		0x00000000, /* DENALI_CTL_59 */
		0x00000000, /* DENALI_CTL_60 */
		0x05000003, /* DENALI_CTL_63 */
		0x00050006, /* DENALI_CTL_64 */
		0x00000000, /* DENALI_CTL_67 */
		0x00000000, /* DENALI_CTL_72 */
		0x00007c00, /* DENALI_CTL_73 */
		0x01000100, /* DENALI_CTL_76 */
		0x01000103, /* DENALI_CTL_77 */
		0x02020200, /* DENALI_CTL_78 */
		0x02000103, /* DENALI_CTL_79 */
		0x14000100, /* DENALI_CTL_80 */
		0x0001b040, /* DENALI_CTL_81 */
		0x00900000, /* DENALI_CTL_82 */
		0x01900024, /* DENALI_CTL_83 */
		0x00000014, /* DENALI_CTL_84 */
		0x00000000, /* DENALI_CTL_85 */
		0x00000000, /* DENALI_CTL_86 */
		0x00820000, /* DENALI_CTL_87 */
		0x00040082, /* DENALI_CTL_88 */
		0x00010004, /* DENALI_CTL_89 */
		0x00000001, /* DENALI_CTL_90 */
		0x00000000, /* DENALI_CTL_91 */
		0x00013880, /* DENALI_CTL_92 */
		0x00000190, /* DENALI_CTL_93 */
		0x00000fa0, /* DENALI_CTL_94 */
		0x00010100, /* DENALI_CTL_95 */
		0x01000001, /* DENALI_CTL_96 */
		0x0a010101, /* DENALI_CTL_97 */
		0x0009090a, /* DENALI_CTL_98 */
		0x1fff0000, /* DENALI_CTL_99 */
		0x00001fff, /* DENALI_CTL_100 */
		0x03022000, /* DENALI_CTL_101 */
		0x00050604, /* DENALI_CTL_102 */
		0x06100610, /* DENALI_CTL_103 */
		0x00000610, /* DENALI_CTL_104 */
		0x00080403, /* DENALI_CTL_105 */
		0x00110905, /* DENALI_CTL_106 */
		0x39341a0d, /* DENALI_CTL_107 */
		0x0304017e, /* DENALI_CTL_108 */
		0x000f0610, /* DENALI_CTL_109 */
		0x0039001d, /* DENALI_CTL_110 */
		0x001e0010, /* DENALI_CTL_111 */
		0x0089003a, /* DENALI_CTL_112 */
		0x0030c6c9, /* DENALI_CTL_113 */
		0x03494288, /* DENALI_CTL_114 */
		0x00012009, /* DENALI_CTL_115 */
		0x00048024, /* DENALI_CTL_116 */
		0x00090024, /* DENALI_CTL_117 */
		0x125224a4, /* DENALI_CTL_118 */
		0x00110022, /* DENALI_CTL_119 */
		0x00410082, /* DENALI_CTL_120 */
		0x00110022, /* DENALI_CTL_121 */
		0x00008001, /* DENALI_CTL_122 */
		0x00020004, /* DENALI_CTL_123 */
		0x00008001, /* DENALI_CTL_124 */
		0x02491001, /* DENALI_CTL_125 */
		0x02ca1001, /* DENALI_CTL_126 */
		0x044b36c9, /* DENALI_CTL_127 */
		0x019130c6, /* DENALI_CTL_128 */
		0x1c7334c6, /* DENALI_CTL_129 */
		0x0c31a906, /* DENALI_CTL_130 */
		0x0c31a083, /* DENALI_CTL_131 */
		0x0201a347, /* DENALI_CTL_132 */
		0x03cb1001, /* DENALI_CTL_141 */
		0x00010003, /* DENALI_CTL_142 */
		0x00008001, /* DENALI_CTL_143 */
		0x00010003, /* DENALI_CTL_144 */
		0x00000000, /* DENALI_CTL_145 */

		/* Lpddr2 phy */
		/* cfg1 value */
		0x03cb1001, /* DENALI_CTL_72 */
	},
	/*
	 * Lpddr2 Frequency Divider
	 * This value is not part of PRCMU API
	 * It is written to address 0x80157438 by ISSW
	 */
	.frequency = 0x00000000, /* Fmax */
},

