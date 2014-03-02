/*
 *  Copyright (C) 2010 ST-Ericsson AB
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 */

#include <ddr_settings.h>

/* will for jdec/non-jdec model memories */
void hook_dmc_266mhz_multiboot(struct ddr_init *dm_ddr)
{
	/* Write DDR settings into XP70 data memory */

	/* Copy Mask for DDR controller registers */
	dm_ddr->MaskCfg0[0] = 0x4DFFFFFF; /* no param into DENALI_CTL_25/28/29/31 */
        dm_ddr->MaskCfg0[1] = 0x9800F1FF; /* no param into DENALI_CTL_41/42/43/48to58 /61/62 */
        dm_ddr->MaskCfg0[2] = 0xFFFFF309; /* no param into DENALI_CTL_65/66/68to71/74/75 */
        dm_ddr->MaskCfg0[3] = 0xFFFFFE7F; /* no param into DENALI_CTL_103/104 */
        dm_ddr->MaskCfg0[4] = 0x0001E01F; /* no param into DENALI_CTL_133to140 */

	/* DDR settings
	 * REG_CONFIG_0 databahn registers
	 */
	dm_ddr->Cfg0[0] = 0x00000101   ;  /* DENALI_CTL_000 */
        dm_ddr->Cfg0[1] = 0x01010100   ;  /* DENALI_CTL_001 */
        dm_ddr->Cfg0[2] = 0x00000001   ;  /* DENALI_CTL_002 */
        dm_ddr->Cfg0[3] = 0x00010100   ;  /* DENALI_CTL_003 */
        dm_ddr->Cfg0[4] = 0x01010100   ;  /* DENALI_CTL_004 */
        dm_ddr->Cfg0[5] = 0x01000100   ;  /* DENALI_CTL_005 */
        dm_ddr->Cfg0[6] = 0x00000000   ;  /* DENALI_CTL_006 */
        dm_ddr->Cfg0[7] = 0x00000001   ;  /* DENALI_CTL_007 */
        dm_ddr->Cfg0[8] = 0x00000101   ;  /* DENALI_CTL_008 */
        dm_ddr->Cfg0[9] = 0x02020301   ;  /* DENALI_CTL_9 */
        dm_ddr->Cfg0[10] = 0x03030303   ; /* DENALI_CTL_10 */
        dm_ddr->Cfg0[11] = 0x01030303   ; /* DENALI_CTL_11 */
        dm_ddr->Cfg0[12] = 0x00030301   ; /* DENALI_CTL_12 */
        dm_ddr->Cfg0[13] = 0x00000000   ; /* DENALI_CTL_13 */
        dm_ddr->Cfg0[14] = 0x00030207   ; /* DENALI_CTL_14 */
        dm_ddr->Cfg0[15] = 0x03000000   ; /* DENALI_CTL_15 */
        dm_ddr->Cfg0[16] = 0x02020202   ; /* DENALI_CTL_16 */
        dm_ddr->Cfg0[17] = 0x06000f02   ; /* DENALI_CTL_17 */
        dm_ddr->Cfg0[18] = 0x00000f06   ; /* DENALI_CTL_18 */
        dm_ddr->Cfg0[19] = 0x00070000   ; /* DENALI_CTL_19 */
        dm_ddr->Cfg0[20] = 0x00020007   ; /* DENALI_CTL_20 */
        dm_ddr->Cfg0[21] = 0x00030403   ; /* DENALI_CTL_21 */
        dm_ddr->Cfg0[22] = 0x05000909   ; /* DENALI_CTL_22 */
        dm_ddr->Cfg0[23] = 0x32002e09   ; /* DENALI_CTL_23 */
        dm_ddr->Cfg0[24] = 0x23003200   ; /* DENALI_CTL_24 */
        dm_ddr->Cfg0[26] = 0x04060000   ; /* DENALI_CTL_026 */
        dm_ddr->Cfg0[27] = 0x00000012   ; /* DENALI_CTL_027 */
        dm_ddr->Cfg0[30] = 0x04080408   ;  /* DENALI_CTL_030 */
        dm_ddr->Cfg0[32] = 0xffff0000   ; /* DENALI_CTL_032 */
        dm_ddr->Cfg0[33] = 0xffffffff   ; /* DENALI_CTL_033 */
        dm_ddr->Cfg0[34] = 0x0000ffff   ; /* DENALI_CTL_034 */
        dm_ddr->Cfg0[35] = 0x00100100   ; /* DENALI_CTL_035 */
        dm_ddr->Cfg0[36] = 0x00000000   ; /* DENALI_CTL_036 */
        dm_ddr->Cfg0[37] = 0x00030000   ; /* DENALI_CTL_037 */
        dm_ddr->Cfg0[38] = 0x00141b4e   ; /* DENALI_CTL_038 */
        dm_ddr->Cfg0[39] = 0x00000015   ; /* DENALI_CTL_039 */
        dm_ddr->Cfg0[40] = 0x0000000e   ; /* DENALI_CTL_040 */
        dm_ddr->Cfg0[44] = 0x00000000   ; /* DENALI_CTL_044 */
        dm_ddr->Cfg0[45] = 0x00000000   ; /* DENALI_CTL_045 */
        dm_ddr->Cfg0[46] = 0x00000000   ; /* DENALI_CTL_046 */
        dm_ddr->Cfg0[47] = 0x00000000   ; /* DENALI_CTL_047 */
	dm_ddr->Cfg0[59] = 0x00000000   ; /* DENALI_CTL_059 */
        dm_ddr->Cfg0[60] = 0x00000000   ; /* DENALI_CTL_060 */
        dm_ddr->Cfg0[63] = 0x05000001   ; /* DENALI_CTL_063 */
        dm_ddr->Cfg0[64] = 0x00050003   ; /* DENALI_CTL_064 */
        dm_ddr->Cfg0[67] = 0x00000000   ;  /* DENALI_CTL_067 */
        dm_ddr->Cfg0[72] = 0x00000000   ; /* DENALI_CTL_072 */
        dm_ddr->Cfg0[73] = 0x00007c40   ; /* DENALI_CTL_073 */
        dm_ddr->Cfg0[76] = 0x00000100   ; /* DENALI_CTL_076 */
        dm_ddr->Cfg0[77] = 0x01000101   ; /* DENALI_CTL_077 */
        dm_ddr->Cfg0[78] = 0x02010100   ; /* DENALI_CTL_078 */
        dm_ddr->Cfg0[79] = 0x02000102   ; /* DENALI_CTL_079 */
        dm_ddr->Cfg0[80] = 0x08000100   ; /* DENALI_CTL_080 */
        dm_ddr->Cfg0[81] = 0x0001b040   ; /* DENALI_CTL_081 */
        dm_ddr->Cfg0[82] = 0x00300000   ; /* DENALI_CTL_082 */
        dm_ddr->Cfg0[83] = 0x0086000c   ; /* DENALI_CTL_083 */
        dm_ddr->Cfg0[84] = 0x00000007   ; /* DENALI_CTL_084 */
        dm_ddr->Cfg0[85] = 0x00000000   ; /* DENALI_CTL_085 */
        dm_ddr->Cfg0[86] = 0x00000000   ; /* DENALI_CTL_086 */
        dm_ddr->Cfg0[87] = 0x00220000   ; /* DENALI_CTL_087 */
        dm_ddr->Cfg0[88] = 0x00010022   ; /* DENALI_CTL_088 */
        dm_ddr->Cfg0[89] = 0x00030001   ; /* DENALI_CTL_089 */
        dm_ddr->Cfg0[90] = 0x00000003   ; /* DENALI_CTL_090 */
        dm_ddr->Cfg0[91] = 0x00000000   ; /* DENALI_CTL_091 */
        dm_ddr->Cfg0[92] = 0x000067e8   ; /* DENALI_CTL_092 */
        dm_ddr->Cfg0[93] = 0x00000086   ; /* DENALI_CTL_093 */
        dm_ddr->Cfg0[94] = 0x00000536   ; /* DENALI_CTL_094 */
        dm_ddr->Cfg0[95] = 0x00000000   ; /* DENALI_CTL_095 */
        dm_ddr->Cfg0[96] = 0x02000001   ; /* DENALI_CTL_096 */
        dm_ddr->Cfg0[97] = 0x0a030302   ; /* DENALI_CTL_097 */
        dm_ddr->Cfg0[98] = 0x0004040a   ; /* DENALI_CTL_098 */
        dm_ddr->Cfg0[99] = 0x01ff0000   ; /* DENALI_CTL_099 */
        dm_ddr->Cfg0[100]= 0x000001ff   ; /* DENALI_CTL_100 */
        dm_ddr->Cfg0[101]= 0x03020200   ; /* DENALI_CTL_101 */
        dm_ddr->Cfg0[102]= 0x00050304   ; /* DENALI_CTL_102 */
        dm_ddr->Cfg0[105]= 0x00070303   ; /* DENALI_CTL_105 */
        dm_ddr->Cfg0[106]= 0x000c0606   ; /* DENALI_CTL_106 */
        dm_ddr->Cfg0[107]= 0x25231212   ; /* DENALI_CTL_107 */
        dm_ddr->Cfg0[108]= 0x04080408   ; /* DENALI_CTL_108 */
        dm_ddr->Cfg0[109]= 0x00140818   ; /* DENALI_CTL_109 */
        dm_ddr->Cfg0[110]= 0x00270014   ; /* DENALI_CTL_110 */
	dm_ddr->Cfg0[111]= 0x00150015   ; /* DENALI_CTL_111 */
        dm_ddr->Cfg0[112]= 0x00890028    ; /* DENALI_CTL_112 */
        dm_ddr->Cfg0[113]= 0x0020c689    ; /* DENALI_CTL_113 */
        dm_ddr->Cfg0[114]= 0x0248e208    ; /* DENALI_CTL_114 */
        dm_ddr->Cfg0[115]= 0x0000c00c    ; /* DENALI_CTL_115 */
        dm_ddr->Cfg0[116]= 0x00030030    ; /* DENALI_CTL_116 */
        dm_ddr->Cfg0[117]= 0x00060018    ; /* DENALI_CTL_117 */
        dm_ddr->Cfg0[118]= 0x0e421c84    ; /* DENALI_CTL_118 */
        dm_ddr->Cfg0[119]= 0x00110022    ; /* DENALI_CTL_119 */
        dm_ddr->Cfg0[120]= 0x00210042    ; /* DENALI_CTL_120 */
        dm_ddr->Cfg0[121]= 0x00110022    ; /* DENALI_CTL_121 */
        dm_ddr->Cfg0[122]= 0x00008001    ; /* DENALI_CTL_122 */
        dm_ddr->Cfg0[123]= 0x00010002    ; /* DENALI_CTL_123 */
        dm_ddr->Cfg0[124]= 0x00008001    ; /* DENALI_CTL_124 */
        dm_ddr->Cfg0[125]= 0x02899001    ; /* DENALI_CTL_125 */
        dm_ddr->Cfg0[126]= 0x02899001    ; /* DENALI_CTL_126 */
        dm_ddr->Cfg0[127]= 0x034a3689    ; /* DENALI_CTL_127 */
        dm_ddr->Cfg0[128]= 0x011120c6    ; /* DENALI_CTL_128 */
        dm_ddr->Cfg0[129]= 0x1673a0c6    ; /* DENALI_CTL_129 */
        dm_ddr->Cfg0[130]= 0x0831a0e7    ; /* DENALI_CTL_130 */
        dm_ddr->Cfg0[131]= 0x08319c63    ; /* DENALI_CTL_131 */
        dm_ddr->Cfg0[132]= 0x02252249    ; /* DENALI_CTL_132 */
        dm_ddr->Cfg0[141]= 0x034a1001   ; /* DENALI_CTL_141 */
        dm_ddr->Cfg0[142]= 0x00018003   ; /* DENALI_CTL_142 */
        dm_ddr->Cfg0[143]= 0x00010002   ; /* DENALI_CTL_143 */
        dm_ddr->Cfg0[144]= 0x00018003   ; /* DENALI_CTL_144 */

	/* Lpddr2 phy */
	dm_ddr->Cfg0[148] = 0x02899001; /* DENALI_CTL_72 */

	/* Start DDR at fmax/2 (200/266/333 MHz) */
	IO(PRCM_DDRSUBSYS_APE_MINBW) = 0x0;
}


void hook_dmc_266mhz_twodie(struct ddr_init *dm_ddr)
{
	/* Write DDR settings into XP70 data memory */

        /* Copy Mask for DDR controller registers */
        dm_ddr->MaskCfg0[0] = 0x4DFFFFFF; /* no param into DENALI_CTL_25/28/29/31 */
        dm_ddr->MaskCfg0[1] = 0x9800F1FF; /* no param into DENALI_CTL_41/42/43/48to58 /61/62 */
        dm_ddr->MaskCfg0[2] = 0xFFFFF309; /* no param into DENALI_CTL_65/66/68to71/74/75 */
        dm_ddr->MaskCfg0[3] = 0xFFFFFE7F; /* no param into DENALI_CTL_103/104 */
        dm_ddr->MaskCfg0[4] = 0x0001E01F; /* no param into DENALI_CTL_133to140 */

	/* DDR settings
	 * REG_CONFIG_0 databahn registers
	 */
        dm_ddr->Cfg0[0] =0x00000101   ;  /* DENALI_CTL_000 */
        dm_ddr->Cfg0[1] =0x01010100   ;  /* DENALI_CTL_001 */
        dm_ddr->Cfg0[2] =0x00000001   ;  /* DENALI_CTL_002 */
        dm_ddr->Cfg0[3] =0x00010100   ;  /* DENALI_CTL_003 */
        dm_ddr->Cfg0[4] =0x01010100   ;  /* DENALI_CTL_004 */
        dm_ddr->Cfg0[5] =0x01000100   ;  /* DENALI_CTL_005 */
        dm_ddr->Cfg0[6] =0x00000000   ;  /* DENALI_CTL_006 */
        dm_ddr->Cfg0[7] =0x00000001   ;  /* DENALI_CTL_007 */
        dm_ddr->Cfg0[8] =0x00000101   ;  /* DENALI_CTL_008 */
        dm_ddr->Cfg0[9] =0x02020302   ;  /* DENALI_CTL_9 */
        dm_ddr->Cfg0[10] = 0x03030303   ; /* DENALI_CTL_10 */
        dm_ddr->Cfg0[11] = 0x01030303   ; /* DENALI_CTL_11 */
        dm_ddr->Cfg0[12] = 0x00030301   ; /* DENALI_CTL_12 */
        dm_ddr->Cfg0[13] = 0x00000000   ; /* DENALI_CTL_13 */
        dm_ddr->Cfg0[14] = 0x00040207   ; /* DENALI_CTL_14 */
        dm_ddr->Cfg0[15] = 0x03000000   ; /* DENALI_CTL_15 */
        dm_ddr->Cfg0[16] = 0x03020202   ; /* DENALI_CTL_16 */
        dm_ddr->Cfg0[17] = 0x08000f02   ; /* DENALI_CTL_17 */
        dm_ddr->Cfg0[18] = 0x00000f08   ; /* DENALI_CTL_18 */
        dm_ddr->Cfg0[19] = 0x00090000   ; /* DENALI_CTL_19 */
        dm_ddr->Cfg0[20] = 0x00020008   ; /* DENALI_CTL_20 */
        dm_ddr->Cfg0[21] = 0x00040503   ; /* DENALI_CTL_21 */
        dm_ddr->Cfg0[22] = 0x05000909   ; /* DENALI_CTL_22 */
        dm_ddr->Cfg0[23] = 0x32002e11   ; /* DENALI_CTL_23 */
        dm_ddr->Cfg0[24] = 0x23003200   ; /* DENALI_CTL_24 */
        dm_ddr->Cfg0[26] = 0x050c0000   ; /* DENALI_CTL_026 */
        dm_ddr->Cfg0[27] = 0x00000023   ; /* DENALI_CTL_027 */
        dm_ddr->Cfg0[30] = 0x04080408   ;  /* DENALI_CTL_030 */
        dm_ddr->Cfg0[32] = 0xffff0000   ; /* DENALI_CTL_032 */
        dm_ddr->Cfg0[33] = 0xffffffff   ; /* DENALI_CTL_033 */
        dm_ddr->Cfg0[34] = 0x0000ffff   ; /* DENALI_CTL_034 */
        dm_ddr->Cfg0[35] = 0x00100100   ; /* DENALI_CTL_035 */
        dm_ddr->Cfg0[36] = 0x00000000   ; /* DENALI_CTL_036 */
        dm_ddr->Cfg0[37] = 0x00030000   ; /* DENALI_CTL_037 */
        dm_ddr->Cfg0[38] = 0x00271b4e   ; /* DENALI_CTL_038 */
        dm_ddr->Cfg0[39] = 0x00000028   ; /* DENALI_CTL_039 */
        dm_ddr->Cfg0[40] = 0x0000001b   ; /* DENALI_CTL_040 */
        dm_ddr->Cfg0[44] = 0x00000000   ; /* DENALI_CTL_044 */
        dm_ddr->Cfg0[45] = 0x00000000   ; /* DENALI_CTL_045 */
        dm_ddr->Cfg0[46] = 0x00000000   ; /* DENALI_CTL_046 */
        dm_ddr->Cfg0[47] = 0x00000000   ; /* DENALI_CTL_047 */
        dm_ddr->Cfg0[59] = 0x00000000   ; /* DENALI_CTL_059 */
        dm_ddr->Cfg0[60] = 0x00000000   ; /* DENALI_CTL_060 */
        dm_ddr->Cfg0[63] = 0x05000002   ; /* DENALI_CTL_063 */
        dm_ddr->Cfg0[64] = 0x00050004   ; /* DENALI_CTL_064 */
        dm_ddr->Cfg0[67] = 0x00000000   ;  /* DENALI_CTL_067 */
        dm_ddr->Cfg0[72] = 0x00000000   ; /* DENALI_CTL_072 */
        dm_ddr->Cfg0[73] = 0x00007c00   ; /* DENALI_CTL_073 */
        dm_ddr->Cfg0[76] = 0x01000100   ; /* DENALI_CTL_076 */
        dm_ddr->Cfg0[77] = 0x01000102   ; /* DENALI_CTL_077 */
        dm_ddr->Cfg0[78] = 0x02020200   ; /* DENALI_CTL_078 */
        dm_ddr->Cfg0[79] = 0x02000102   ; /* DENALI_CTL_079 */
        dm_ddr->Cfg0[80] = 0x0e000100   ; /* DENALI_CTL_080 */
        dm_ddr->Cfg0[81] = 0x0001b040   ; /* DENALI_CTL_081 */
        dm_ddr->Cfg0[82] = 0x00600000   ; /* DENALI_CTL_082 */
        dm_ddr->Cfg0[83] = 0x010b0018   ; /* DENALI_CTL_083 */
        dm_ddr->Cfg0[84] = 0x0000000e   ; /* DENALI_CTL_084 */
        dm_ddr->Cfg0[85] = 0x00000000   ; /* DENALI_CTL_085 */
        dm_ddr->Cfg0[86] = 0x00000000   ; /* DENALI_CTL_086 */
        dm_ddr->Cfg0[87] = 0x00420000   ; /* DENALI_CTL_087 */
        dm_ddr->Cfg0[88] = 0x00020042   ; /* DENALI_CTL_088 */
        dm_ddr->Cfg0[89] = 0x00020002   ; /* DENALI_CTL_089 */
        dm_ddr->Cfg0[90] = 0x00000002   ; /* DENALI_CTL_090 */
        dm_ddr->Cfg0[91] = 0x00000000   ; /* DENALI_CTL_091 */
        dm_ddr->Cfg0[92] = 0x0000cfd0   ; /* DENALI_CTL_092 */
        dm_ddr->Cfg0[93] = 0x0000010b   ; /* DENALI_CTL_093 */
        dm_ddr->Cfg0[94] = 0x00000a6b   ; /* DENALI_CTL_094 */
        dm_ddr->Cfg0[95] = 0x00010100   ; /* DENALI_CTL_095 */
        dm_ddr->Cfg0[96] = 0x01000001   ; /* DENALI_CTL_096 */
        dm_ddr->Cfg0[97] = 0x0a020201   ; /* DENALI_CTL_097 */
        dm_ddr->Cfg0[98] = 0x0006060a   ; /* DENALI_CTL_098 */
        dm_ddr->Cfg0[99] = 0x0fff0000   ; /* DENALI_CTL_099 */
        dm_ddr->Cfg0[100]= 0x00000fff   ; /* DENALI_CTL_100 */
        dm_ddr->Cfg0[101]= 0x03021000   ; /* DENALI_CTL_101 */
        dm_ddr->Cfg0[102]= 0x00050404   ; /* DENALI_CTL_102 */
        dm_ddr->Cfg0[105]= 0x00050303   ; /* DENALI_CTL_105 */
        dm_ddr->Cfg0[106]= 0x000c0606   ; /* DENALI_CTL_106 */
        dm_ddr->Cfg0[107]= 0x25231212   ; /* DENALI_CTL_107 */
        dm_ddr->Cfg0[108]= 0x02000200   ; /* DENALI_CTL_108 */
        dm_ddr->Cfg0[109]= 0x00140408   ; /* DENALI_CTL_109 */
        dm_ddr->Cfg0[110]= 0x00270014   ; /* DENALI_CTL_110 */
        dm_ddr->Cfg0[111]= 0x00150015   ; /* DENALI_CTL_111 */
        dm_ddr->Cfg0[112]= 0x00890028   ;   /* DENALI_CTL_112 */
        dm_ddr->Cfg0[113]= 0x0020c689   ;   /* DENALI_CTL_113 */
        dm_ddr->Cfg0[114]= 0x0248e208   ;   /* DENALI_CTL_114 */
        dm_ddr->Cfg0[115]= 0x0000c00c   ;   /* DENALI_CTL_115 */
        dm_ddr->Cfg0[116]= 0x00030030   ;   /* DENALI_CTL_116 */
        dm_ddr->Cfg0[117]= 0x00060018   ;   /* DENALI_CTL_117 */
        dm_ddr->Cfg0[118]= 0x0c421884   ;   /* DENALI_CTL_118 */
        dm_ddr->Cfg0[119]= 0x00110022   ;   /* DENALI_CTL_119 */
        dm_ddr->Cfg0[120]= 0x00210042   ;   /* DENALI_CTL_120 */
        dm_ddr->Cfg0[121]= 0x00110022   ;   /* DENALI_CTL_121 */
        dm_ddr->Cfg0[122]= 0x00008001   ;   /* DENALI_CTL_122 */
        dm_ddr->Cfg0[123]= 0x00010002   ;   /* DENALI_CTL_123 */
        dm_ddr->Cfg0[124]= 0x00008001   ;   /* DENALI_CTL_124 */
        dm_ddr->Cfg0[125]= 0x02899001   ;   /* DENALI_CTL_125 */
        dm_ddr->Cfg0[126]= 0x02899001   ;   /* DENALI_CTL_126 */
        dm_ddr->Cfg0[127]= 0x034a3689   ;   /* DENALI_CTL_127 */
        dm_ddr->Cfg0[128]= 0x011120c6   ;   /* DENALI_CTL_128 */
        dm_ddr->Cfg0[129]= 0x126320c6   ;   /* DENALI_CTL_129 */
        dm_ddr->Cfg0[130]= 0x0831a0e7   ;   /* DENALI_CTL_130 */
        dm_ddr->Cfg0[131]= 0x08319463   ;   /* DENALI_CTL_131 */
        dm_ddr->Cfg0[132]= 0x02251249   ;   /* DENALI_CTL_132 */
        dm_ddr->Cfg0[141]= 0x034a1001   ; /* DENALI_CTL_141 */
        dm_ddr->Cfg0[142]= 0x00018003   ; /* DENALI_CTL_142 */
        dm_ddr->Cfg0[143]= 0x00010002   ; /* DENALI_CTL_143 */
        dm_ddr->Cfg0[144]= 0x00018003   ; /* DENALI_CTL_144 */

        /* Lpddr2 phy */
        dm_ddr->Cfg0[148] = 0x034a1001; /* DENALI_CTL_72 */

	/* Start DDR at fmax/2 (200/266/333 MHz) */
	IO(PRCM_DDRSUBSYS_APE_MINBW) = 0x0;
}

void hook_dmc_266mhz_onedie(struct ddr_init *dm_ddr)
{
	/* Write DDR settings into XP70 data memory */

	/* Copy Mask for DDR controller registers */
	dm_ddr->MaskCfg0[0] = 0x4DFFFFFF; /* no param into DENALI_CTL_25/28/29/31 */
        dm_ddr->MaskCfg0[1] = 0x9800F1FF; /* no param into DENALI_CTL_41/42/43/48to58 /61/62 */
        dm_ddr->MaskCfg0[2] = 0xFFFFF309; /* no param into DENALI_CTL_65/66/68to71/74/75 */
        dm_ddr->MaskCfg0[3] = 0xFFFFFE7F; /* no param into DENALI_CTL_103/104 */
        dm_ddr->MaskCfg0[4] = 0x0001E01F; /* no param into DENALI_CTL_133to140 */

	/* DDR settings
	 * REG_CONFIG_0 databahn registers
	 */
	dm_ddr->Cfg0[0] =  0x00000101   ;  /* DENALI_CTL_000 */
        dm_ddr->Cfg0[1] =  0x01010100   ;  /* DENALI_CTL_001 */
        dm_ddr->Cfg0[2] =  0x00000001   ;  /* DENALI_CTL_002 */
        dm_ddr->Cfg0[3] =  0x00010100   ;  /* DENALI_CTL_003 */
        dm_ddr->Cfg0[4] =  0x01010100   ;  /* DENALI_CTL_004 */
        dm_ddr->Cfg0[5] =  0x01000100   ;  /* DENALI_CTL_005 */
        dm_ddr->Cfg0[6] =  0x00000000   ;  /* DENALI_CTL_006 */
        dm_ddr->Cfg0[7] =  0x00000001   ;  /* DENALI_CTL_007 */
        dm_ddr->Cfg0[8] =  0x00000101   ;  /* DENALI_CTL_008 */
        dm_ddr->Cfg0[9] =  0x02020302   ;  /* DENALI_CTL_9 */
        dm_ddr->Cfg0[10] = 0x03030303   ; /* DENALI_CTL_10 */
        dm_ddr->Cfg0[11] = 0x01030303   ; /* DENALI_CTL_11 */
        dm_ddr->Cfg0[12] = 0x00030301   ; /* DENALI_CTL_12 */
        dm_ddr->Cfg0[13] = 0x00000000   ; /* DENALI_CTL_13 */
        dm_ddr->Cfg0[14] = 0x00040207   ; /* DENALI_CTL_14 */
        dm_ddr->Cfg0[15] = 0x03000000   ; /* DENALI_CTL_15 */
        dm_ddr->Cfg0[16] = 0x03020202   ; /* DENALI_CTL_16 */
        dm_ddr->Cfg0[17] = 0x08000f02   ; /* DENALI_CTL_17 */
        dm_ddr->Cfg0[18] = 0x00000f08   ; /* DENALI_CTL_18 */
        dm_ddr->Cfg0[19] = 0x00090000   ; /* DENALI_CTL_19 */
        dm_ddr->Cfg0[20] = 0x00020008   ; /* DENALI_CTL_20 */
        dm_ddr->Cfg0[21] = 0x00040503   ; /* DENALI_CTL_21 */
        dm_ddr->Cfg0[22] = 0x05000909   ; /* DENALI_CTL_22 */
        dm_ddr->Cfg0[23] = 0x32002e11   ; /* DENALI_CTL_23 */
        dm_ddr->Cfg0[24] = 0x23003200   ; /* DENALI_CTL_24 */
        dm_ddr->Cfg0[26] = 0x050c0000   ; /* DENALI_CTL_026 */
        dm_ddr->Cfg0[27] = 0x00000023   ; /* DENALI_CTL_027 */
	dm_ddr->Cfg0[30] = 0x04080408     ;  /* DENALI_CTL_030 */
	dm_ddr->Cfg0[32] = 0xffff0000   ; /* DENALI_CTL_032 */
        dm_ddr->Cfg0[33] = 0xffffffff   ; /* DENALI_CTL_033 */
        dm_ddr->Cfg0[34] = 0x0000ffff   ; /* DENALI_CTL_034 */
        dm_ddr->Cfg0[35] = 0x00100100   ; /* DENALI_CTL_035 */
        dm_ddr->Cfg0[36] = 0x00000000   ; /* DENALI_CTL_036 */
        dm_ddr->Cfg0[37] = 0x00030000   ; /* DENALI_CTL_037 */
        dm_ddr->Cfg0[38] = 0x00271b4e   ; /* DENALI_CTL_038 */
        dm_ddr->Cfg0[39] = 0x00000028   ; /* DENALI_CTL_039 */
        dm_ddr->Cfg0[40] = 0x0000001b   ; /* DENALI_CTL_040 */
        dm_ddr->Cfg0[44] = 0x00000000   ; /* DENALI_CTL_044 */
        dm_ddr->Cfg0[45] = 0x00000000   ; /* DENALI_CTL_045 */
        dm_ddr->Cfg0[46] = 0x00000000   ; /* DENALI_CTL_046 */
        dm_ddr->Cfg0[47] = 0x00000000   ; /* DENALI_CTL_047 */
        dm_ddr->Cfg0[59] = 0x00000000    ; /* DENALI_CTL_059 */
        dm_ddr->Cfg0[60] = 0x00000000   ; /* DENALI_CTL_060 */
        dm_ddr->Cfg0[63] = 0x05000002   ; /* DENALI_CTL_063 */
        dm_ddr->Cfg0[64] = 0x00050004   ; /* DENALI_CTL_064 */
        dm_ddr->Cfg0[67] = 0x00000000   ; /* DENALI_CTL_067 */
        dm_ddr->Cfg0[72] = 0x00000000   ; /* DENALI_CTL_072 */
        dm_ddr->Cfg0[73] = 0x00007c00   ; /* DENALI_CTL_073 */
        dm_ddr->Cfg0[76] = 0x01000100   ; /* DENALI_CTL_076 */
        dm_ddr->Cfg0[77] = 0x01000102   ; /* DENALI_CTL_077 */
        dm_ddr->Cfg0[78] = 0x02020200   ; /* DENALI_CTL_078 */
        dm_ddr->Cfg0[79] = 0x02000102   ; /* DENALI_CTL_079 */
        dm_ddr->Cfg0[80] = 0x0e000100   ; /* DENALI_CTL_080 */
        dm_ddr->Cfg0[81] = 0x0001b040   ; /* DENALI_CTL_081 */
        dm_ddr->Cfg0[82] = 0x00600000   ; /* DENALI_CTL_082 */
        dm_ddr->Cfg0[83] = 0x010b0018   ; /* DENALI_CTL_083 */
        dm_ddr->Cfg0[84] = 0x0000000e   ; /* DENALI_CTL_084 */
        dm_ddr->Cfg0[85] = 0x00000000   ; /* DENALI_CTL_085 */
        dm_ddr->Cfg0[86] = 0x00000000   ; /* DENALI_CTL_086 */
        dm_ddr->Cfg0[87] = 0x00420000   ; /* DENALI_CTL_087 */
        dm_ddr->Cfg0[88] = 0x00020042   ; /* DENALI_CTL_088 */
        dm_ddr->Cfg0[89] = 0x00020002   ; /* DENALI_CTL_089 */
        dm_ddr->Cfg0[90] = 0x00000002   ; /* DENALI_CTL_090 */
        dm_ddr->Cfg0[91] = 0x00000000   ; /* DENALI_CTL_091 */
        dm_ddr->Cfg0[92] = 0x0000cfd0   ; /* DENALI_CTL_092 */
        dm_ddr->Cfg0[93] = 0x0000010b   ; /* DENALI_CTL_093 */
        dm_ddr->Cfg0[94] = 0x00000a6b   ; /* DENALI_CTL_094 */
        dm_ddr->Cfg0[95] = 0x00010100   ; /* DENALI_CTL_095 */
        dm_ddr->Cfg0[96] = 0x01000001   ; /* DENALI_CTL_096 */
        dm_ddr->Cfg0[97] = 0x0a010101   ; /* DENALI_CTL_097 */
        dm_ddr->Cfg0[98] = 0x0006060a   ; /* DENALI_CTL_098 */
        dm_ddr->Cfg0[99] = 0x1fff0000   ; /* DENALI_CTL_099 */
        dm_ddr->Cfg0[100]= 0x0000ffff   ; /* DENALI_CTL_100 */
        dm_ddr->Cfg0[101]= 0x0302ffff   ; /* DENALI_CTL_101 */
        dm_ddr->Cfg0[102]= 0x00000404   ; /* DENALI_CTL_102 */
        dm_ddr->Cfg0[105]= 0x00050303   ; /* DENALI_CTL_105 */
        dm_ddr->Cfg0[106]= 0x000c0606   ; /* DENALI_CTL_106 */
        dm_ddr->Cfg0[107]= 0x25231212   ; /* DENALI_CTL_107 */
        dm_ddr->Cfg0[108]= 0x02000200   ; /* DENALI_CTL_108 */
        dm_ddr->Cfg0[109]= 0x00140408   ; /* DENALI_CTL_109 */
        dm_ddr->Cfg0[110]= 0x00270014   ; /* DENALI_CTL_110 */
	dm_ddr->Cfg0[111]= 0x00150015   ; /* DENALI_CTL_111 */
        dm_ddr->Cfg0[112]= 0x00890028   ; /* DENALI_CTL_112 */
        dm_ddr->Cfg0[113]= 0x0020c689   ; /* DENALI_CTL_113 */
        dm_ddr->Cfg0[114]= 0x0248e208   ; /* DENALI_CTL_114 */
        dm_ddr->Cfg0[115]= 0x0000c00c   ; /* DENALI_CTL_115 */
        dm_ddr->Cfg0[116]= 0x00030030   ; /* DENALI_CTL_116 */
        dm_ddr->Cfg0[117]= 0x00060018   ; /* DENALI_CTL_117 */
        dm_ddr->Cfg0[118]= 0x0c421884   ; /* DENALI_CTL_118 */
        dm_ddr->Cfg0[119]= 0x00110022   ; /* DENALI_CTL_119 */
        dm_ddr->Cfg0[120]= 0x00210042   ; /* DENALI_CTL_120 */
        dm_ddr->Cfg0[121]= 0x00110022   ; /* DENALI_CTL_121 */
        dm_ddr->Cfg0[122]= 0x00008001   ; /* DENALI_CTL_122 */
        dm_ddr->Cfg0[123]= 0x00010002   ; /* DENALI_CTL_123 */
        dm_ddr->Cfg0[124]= 0x00008001   ; /* DENALI_CTL_124 */
        dm_ddr->Cfg0[125]= 0x02899001   ; /* DENALI_CTL_125 */
        dm_ddr->Cfg0[126]= 0x02899001   ; /* DENALI_CTL_126 */
        dm_ddr->Cfg0[127]= 0x034a3689   ; /* DENALI_CTL_127 */
        dm_ddr->Cfg0[128]= 0x011120c6   ; /* DENALI_CTL_128 */
        dm_ddr->Cfg0[129]= 0x126320c6   ; /* DENALI_CTL_129 */
        dm_ddr->Cfg0[130]= 0x0831a0e7   ; /* DENALI_CTL_130 */
        dm_ddr->Cfg0[131]= 0x08319463   ; /* DENALI_CTL_131 */
        dm_ddr->Cfg0[132]= 0x02251249   ; /* DENALI_CTL_132 */
	dm_ddr->Cfg0[141]= 0x034a1001   ; /* DENALI_CTL_141 */
        dm_ddr->Cfg0[142]= 0x00018003   ; /* DENALI_CTL_142 */
        dm_ddr->Cfg0[143]= 0x00010002   ; /* DENALI_CTL_143 */
        dm_ddr->Cfg0[144]= 0x00018003   ; /* DENALI_CTL_144 */

	/* Lpddr2 phy */
	dm_ddr->Cfg0[148] = 0x034a1001; /* DENALI_CTL_72 */

	/* Start DDR at fmax/2 (200/266/333 MHz) */
	IO(PRCM_DDRSUBSYS_APE_MINBW) = 0x0;
}
